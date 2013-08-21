//
// Simple OpenGL FX
//

#ifdef WIN32
    #include <windows.h>
    #include <winuser.h>
#endif

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include "fx.h"
// Epopeia mesh END
#define DEBUG_VAR 1
#define DEBUG_MODULE "fx"
#include <epopeia/debug.h>


    const int texture_width  = 256;
    const int texture_height = 256;

//
// Funciones locales
//
static GLuint make_and_upload_texture (void)
{
    int x, y;
    int c;
    unsigned int *data = (unsigned int *) malloc (texture_width * texture_height * 4);
    GLuint texture_id;

    if (!data)
        return 0;

    for (y=0; y<texture_height; y++)
        for (x=0; x<texture_width; x++)
        {
            c = x^y;
            data[x+texture_width*y] = (c<<24) | (c<<16) | (c<<8) | c;
        }

    glGenTextures (1, &texture_id);
    glBindTexture (GL_TEXTURE_2D, texture_id);
    glTexImage2D  (GL_TEXTURE_2D, 0, 4, texture_width, texture_height, 0,
                   GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    free (data);

    return texture_id;
}

static void DrawCube (void)
{
    GLfloat umax = 1.0;

    glBegin(GL_QUADS);
//glBegin(GL_LINES);
     // Front Face
     glNormal3f( 0.0f, 0.0f, 1.0f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
     glTexCoord2f(umax, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
     glTexCoord2f(umax, umax); glVertex3f( 1.0f,  1.0f,  1.0f);
     glTexCoord2f(0.0f, umax); glVertex3f(-1.0f,  1.0f,  1.0f);

     // Back Face
     glNormal3f( 0.0f, 0.0f,-1.0f);
     glTexCoord2f(umax, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
     glTexCoord2f(umax, umax); glVertex3f(-1.0f,  1.0f, -1.0f);
     glTexCoord2f(0.0f, umax); glVertex3f( 1.0f,  1.0f, -1.0f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);

     // Top Face
     glNormal3f( 0.0f, 1.0f, 0.0f);
     glTexCoord2f(0.0f, umax); glVertex3f(-1.0f,  1.0f, -1.0f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
     glTexCoord2f(umax, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
     glTexCoord2f(umax, umax); glVertex3f( 1.0f,  1.0f, -1.0f);

     // Bottom Face
     glNormal3f( 0.0f,-1.0f, 0.0f);
     glTexCoord2f(umax, umax); glVertex3f(-1.0f, -1.0f, -1.0f);
     glTexCoord2f(0.0f, umax); glVertex3f( 1.0f, -1.0f, -1.0f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
     glTexCoord2f(umax, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);

     // Right face
     glNormal3f( 1.0f, 0.0f, 0.0f);
     glTexCoord2f(umax, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
     glTexCoord2f(umax, umax); glVertex3f( 1.0f,  1.0f, -1.0f);
     glTexCoord2f(0.0f, umax); glVertex3f( 1.0f,  1.0f,  1.0f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);

     // Left Face
     glNormal3f(-1.0f, 0.0f, 0.0f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
     glTexCoord2f(umax, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
     glTexCoord2f(umax, umax); glVertex3f(-1.0f,  1.0f,  1.0f);
     glTexCoord2f(0.0f, umax); glVertex3f(-1.0f,  1.0f, -1.0f);
    glEnd();
}


//
// API
//
TFx* fx_init (int RES_X, int RES_Y)
{
    TFx *self;
//    printf("res: %d, %d\n", RES_X, RES_Y);
    self = malloc(sizeof(TFx));
    //assert(self != NULL);

    self->RES_X = RES_X;
    self->RES_Y = RES_Y;

    // Init opengl code
    glViewport (0, 0, RES_X, RES_Y);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ( );
    gluPerspective (140.0f, (GLfloat)RES_X/(GLfloat)RES_Y, 0.1f, 100.0f);
    glMatrixMode (GL_MODELVIEW);

    self->texture_id = make_and_upload_texture();
    if(!self->texture_id)
    {
        free(self);
        return NULL;
    }

    return self;
}

int fx_do(TFx * self, float t)
{
    // Clear screen
//    float c = (t / 100.0) - (int)(t / 100.0);
//    glClearColor (c, 0.5, 0.0, 0.0);
    //glClear (GL_COLOR_BUFFER_BIT);

//    printf("Texture ID en FX: %d\n", self->texture_id);

  // Init opengl code
    glViewport (0, 0, self->RES_X, self->RES_Y);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ( );
    gluPerspective (140.0f, (GLfloat)self->RES_X/(GLfloat)self->RES_Y, 0.1f, 100.0f);
    glMatrixMode (GL_MODELVIEW);

    // Configure frame
    glEnable (GL_TEXTURE_2D);
    glEnable (GL_BLEND);
    glBindTexture (GL_TEXTURE_2D, self->texture_id);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc (GL_SRC_ALPHA, GL_ZERO);

    // Cube (RED)
    glColor4f (1.0, 0.0, 0.0, 0.5*cos(t/15.0)+0.5);
    glLoadIdentity ( );
    glTranslatef (0.3*cos(t/91.0),
                  0.3*cos(t/51.0),
                  0.3*cos(t/45.0));
    glRotatef (t, 0.8, 0.9, 1.0);
    DrawCube ( );

    // Cube (GREEN)
    glColor4f (0.0, 1.0, 0.0, 0.5*cos(t/35.0)+0.5);
    glLoadIdentity ( );
    glTranslatef (0.3*cos((t+50.0)/91.0),
                  0.3*cos((t+50.0)/51.0),
                  0.3*cos((t+50.0)/45.0));
    glRotatef (t+50.0, 0.8, 0.9, 1.0);
    DrawCube ( );

    // Cube (BLUE)
    glColor4f (0.0, 0.0, 1.0, 0.5*cos(t/25.0)+0.5);
    glLoadIdentity ( );
    glTranslatef (0.3*cos((t+100.0)/91.0),
                  0.3*cos((t+100.0)/51.0),
                  0.3*cos((t+100.0)/45.0));
    glRotatef (t+100.0, 0.8, 0.9, 1.0);
    DrawCube ( );

    glDisable (GL_BLEND);
    glDisable (GL_TEXTURE_2D);

    return 1;
}

int fx_kill (TFx* self)
{
    // nothing to do
    return 1;
}
