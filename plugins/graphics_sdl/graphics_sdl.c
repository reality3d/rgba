#ifdef LINUX

#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <epopeia/epopeia.h>
#include "graphics_sdl.h"
#include "keyboard_sdl.h"

int  Graphics_Sdl_Init(TGraphicsSdl* self, TResolution* RestrictedTo, int Windowed);
void Graphics_Sdl_End(TGraphicsSdl* self);
TGraphicsContext* Graphics_Sdl_FrameStart(TGraphicsSdl* self);
void Graphics_Sdl_FrameEnd(TGraphicsSdl* self);
void Graphics_Sdl_SetGlobalFade(TGraphicsSdl* self, float r, float g, float b, float a);
void Graphics_Sdl_ToggleWindowed(TGraphicsSdl* self);
void Graphics_Sdl_SetZBuffer(TGraphicsSdl* self, int bits);
void Graphics_Sdl_SetStencilBuffer(TGraphicsSdl* self, int bits);
TKeyboard* Graphics_Sdl_GetKeyboardSubsystem(TGraphicsSdl* self);
void Graphics_Sdl_Delete(TGraphicsSdl* self);


//-------------------------------------------------------------------
TGraphicsSdl* Graphics_Sdl_New(void)
{
    TGraphicsSdl* Sdl;

    Sdl = malloc(sizeof(TGraphicsSdl));
    assert(Sdl != NULL);

    Sdl->Init = Graphics_Sdl_Init;
    Sdl->End  = Graphics_Sdl_End;
    Sdl->Delete  = Graphics_Sdl_Delete;
    Sdl->FrameStart = Graphics_Sdl_FrameStart;
    Sdl->FrameEnd   = Graphics_Sdl_FrameEnd;
    Sdl->SetGlobalFade    = Graphics_Sdl_SetGlobalFade;
    Sdl->ToggleWindowed   = Graphics_Sdl_ToggleWindowed;
    Sdl->SetZBuffer       = Graphics_Sdl_SetZBuffer;
    Sdl->SetStencilBuffer = Graphics_Sdl_SetStencilBuffer;
    Sdl->GetKeyboardSubsystem = Graphics_Sdl_GetKeyboardSubsystem;

    Sdl->surface = NULL;
    Sdl->zbuffer = 24;
    Sdl->stencil = 8;

    return Sdl;
}
//-------------------------------------------------------------------
void Graphics_Sdl_Delete(TGraphicsSdl* self)
{
    free(self);
}
//-------------------------------------------------------------------
int Graphics_Sdl_Init(TGraphicsSdl* self, TResolution* res, int Windowed)
{
    const SDL_VideoInfo* info = NULL;

    /* Flags we will pass into SDL_SetVideoMode. */
    int flags = 0;
    assert(res != NULL);


    /* First, initialize SDL's video subsystem. */
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE ) < 0 ) {
        /* Failed, exit. */
        fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError( ) );
        return -1;
    }
    /* Let's get some video information. */
    info = SDL_GetVideoInfo( );

    if( !info ) {
        /* This should probably never happen. */
        fprintf( stderr, "Video query failed: %s\n", SDL_GetError());
        return -2;
    }
    /*
     * Now, we want to setup our requested
     * window attributes for our OpenGL window.
     * We want *at least* 5 bits of red, green
     * and blue. We also want at least a 16-bit
     * depth buffer.
     *
     * The last thing we do is request a double
     * buffered window. '1' turns on double
     * buffering, '0' turns it off.
     *
     * Note that we do not use SDL_DOUBLEBUF in
     * the flags to SDL_SetVideoMode. That does
     * not affect the GL attribute state, only
     * the standard 2D blitting setup.
     */
//    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5);
//    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5);
//    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, res->Bpp);
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, self->zbuffer);
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, self->stencil);
    /*
     * We want to request that SDL provide us
     * with an OpenGL window, in a fullscreen
     * video mode.
     */
    flags = (Windowed ? SDL_OPENGL : SDL_OPENGL | SDL_FULLSCREEN);

    /*
     * Set the video mode
     */
    self->surface = SDL_SetVideoMode(res->Width, res->Height, res->Bpp, flags);
    if(self->surface == NULL) {
        /* 
         * This could happen for a variety of reasons,
         * including DISPLAY not being set, the specified
         * resolution not being available, etc.
         */
        fprintf( stderr, "Video mode set failed: %s\n",
                 SDL_GetError( ) );
        fprintf( stderr, "%dx%dx%d\n", res->Width, res->Height, res->Bpp);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        self->surface = SDL_SetVideoMode(res->Width, res->Height, res->Bpp, flags);
	if(self->surface == NULL)
	{
	    fprintf( stderr, "Video mode set failed (with recovery-correction): %s\n",
		    SDL_GetError( ) );
	    fprintf( stderr, "%dx%dx%d\n", res->Width, res->Height, res->Bpp);

	    return -3;
	}
    }
    self->RC.GraphicsType = rtSdl;
    self->RC.Width  = res->Width;
    self->RC.Height = res->Height;
    self->RC.Bpp    = res->Bpp;
//    self->RC.Pitch  = RestrictedTo->Width;
//    self->RC.BytesPerPixel = (RestrictedTo->Bpp+7)/8;
    self->RC.FrameBuffer   = NULL;
    self->RC.BytesPerPixel = 0;
    self->fade_a = 0.0f;
    
    SDL_ShowCursor(SDL_DISABLE);
    SDL_WM_SetCaption(Epopeia_GetDemoName(), Epopeia_GetDemoName());
    return 0;
}
//-------------------------------------------------------------------
void Graphics_Sdl_End(TGraphicsSdl* self)
{
    SDL_ShowCursor(SDL_ENABLE);
    SDL_Quit();
}
//-------------------------------------------------------------------
TGraphicsContext* Graphics_Sdl_FrameStart(TGraphicsSdl* self)
{
    return &self->RC;
}
//-------------------------------------------------------------------
void Graphics_Sdl_FrameEnd(TGraphicsSdl* self)
{
    if (self->fade_a != 0.0f)
    {
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ( );
        gluOrtho2D (0.0, 1.0, 0.0, 1.0);
        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity ( );

        glDisable (GL_TEXTURE_2D);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f (self->fade_r, self->fade_g, self->fade_b, self->fade_a);
        glRecti (0, 0, 1, 1);

        glDisable (GL_BLEND);
    }
    SDL_GL_SwapBuffers( );
}
//-------------------------------------------------------------------
void Graphics_Sdl_SetGlobalFade(TGraphicsSdl* self, float r, float g, float b, float a)
{
    if(r>1.0)
	self->fade_r = 1.0;
    else
	self->fade_r = r;
    if(g>1.0)
	self->fade_g = 1.0;
    else
	self->fade_g = g;
    if(b>1.0)
	self->fade_b = 1.0;
    else
	self->fade_b = b;
    if(a>1.0)
	self->fade_a = 1.0;
    else
	self->fade_a = a;

}
//-------------------------------------------------------------------
void Graphics_Sdl_ToggleWindowed(TGraphicsSdl* self)
{
    SDL_WM_ToggleFullScreen(self->surface);
}
//-------------------------------------------------------------------
void Graphics_Sdl_SetZBuffer(TGraphicsSdl* self, int bits)
{
    self->zbuffer = bits;
}
//-------------------------------------------------------------------
void Graphics_Sdl_SetStencilBuffer(TGraphicsSdl* self, int bits)
{
    self->stencil = bits;
}
//-------------------------------------------------------------------
TKeyboard* Graphics_Sdl_GetKeyboardSubsystem(TGraphicsSdl* self)
{
    return Keyboard_Sdl_New();
}

#endif //LINUX

