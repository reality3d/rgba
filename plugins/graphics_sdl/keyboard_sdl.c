#ifdef LINUX

#include <assert.h>
#include <malloc.h>
#include <SDL/SDL.h>
#include <string.h>
#include "keyboard_sdl.h"
#define DEBUG_MODULE "keyboard_sdl"
#define DEBUG_VAR 0
#include "../../epopeia/debug.h"

int  Keyboard_Sdl_Init       (struct TKeyboardSdl_t* self);
void Keyboard_Sdl_End        (struct TKeyboardSdl_t* self);
int  Keyboard_Sdl_ExitPressed(struct TKeyboardSdl_t* self);
int  Keyboard_Sdl_KeyPressed (struct TKeyboardSdl_t* self); // <0 no, >0 keycode
int  Keyboard_Sdl_Pressed    (struct TKeyboardSdl_t* self, int KeyCode);
void Keyboard_Sdl_Delete     (struct TKeyboardSdl_t* self);


//-------------------------------------------------------------------
TKeyboard* Keyboard_Sdl_New(void)
{
    TKeyboardSdl* self;

    self = malloc(sizeof(TKeyboardSdl));
    assert(self != NULL);
    memset(self, 0, sizeof(TKeyboardSdl));
    
    self->Init = Keyboard_Sdl_Init;
    self->End  = Keyboard_Sdl_End;
    self->ExitPressed = Keyboard_Sdl_ExitPressed;
    self->KeyPressed  = Keyboard_Sdl_KeyPressed;
    self->Pressed     = Keyboard_Sdl_Pressed;
    self->Delete      = Keyboard_Sdl_Delete;
    self->exit_req    = 0;

    return (TKeyboard*)self;
}

//-------------------------------------------------------------------
int  Keyboard_Sdl_Init(struct TKeyboardSdl_t* self)
{
    return 0;
}

//-------------------------------------------------------------------
void Keyboard_Sdl_End        (struct TKeyboardSdl_t* self)
{
    // nothing
}

//-------------------------------------------------------------------
int Keyboard_Sdl_ProcessEvents(struct TKeyboardSdl_t* self)
{
    /* Our SDL event placeholder. */
    SDL_Event event;

    /* Grab all the events off the queue. */
    while( SDL_PollEvent( &event ) ) {

        switch( event.type ) {
        case SDL_KEYUP:
	    self->key = -1;
	    break;

	case SDL_KEYDOWN:
            /* Handle key presses. */
	    switch(event.key.keysym.sym)
	    {
	    case SDLK_ESCAPE:
		self->exit_req += 1;
		break;
	    case SDLK_LEFT:
                self->key = KEY_LEFT;
                DEBUG_msg("KEY_LEFT");
                break;
	    case SDLK_RIGHT:
                DEBUG_msg("KEY_RIGHT");
		self->key = KEY_RIGHT;
		break;
	    case SDLK_LCTRL:
	    case SDLK_RCTRL:
		self->key = KEY_CTRL;
                break;
            case SDLK_SPACE:
                self->key = KEY_SPACE;
	    default:
                break;
	    }
	    break;
	case SDL_QUIT:
            self->exit_req = 1;
            /* Handle quit requests (like Ctrl-c). */
	    break;
	default:
            break;
        }

    }
    return 0;
}

//-------------------------------------------------------------------
int  Keyboard_Sdl_ExitPressed(struct TKeyboardSdl_t* self)
{
    Keyboard_Sdl_ProcessEvents(self);
    return self->exit_req;
}

//-------------------------------------------------------------------
int  Keyboard_Sdl_KeyPressed (struct TKeyboardSdl_t* self)  // <0 no, >0 keycode
{
    Keyboard_Sdl_ProcessEvents(self);
    return self->key;
}
//-------------------------------------------------------------------
int  Keyboard_Sdl_Pressed (struct TKeyboardSdl_t* self, int KeyCode)
{
    //    Keyboard_Sdl_ProcessEvents(self);
    
    return (KeyCode == self->key);
}
//-------------------------------------------------------------------
void Keyboard_Sdl_Delete    (struct TKeyboardSdl_t* self)
{
	free(self);
}
//-------------------------------------------------------------------

#endif //LINUX

