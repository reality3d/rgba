////////////////////////////////////////////////////////////////////////
//
// ZOOMER
//
////////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <malloc.h>
#include <glt/glt.h>
#include "zoomer.h"


static int ZOOMER_DynamicInit (TZoomer *self, GLint *viewport)
{
    int width_p2, height_p2; // width and height (power of 2)
    int i;

    // Find width
    width_p2 = 0;
    for (i=0; i<31; i++)
    {
        if ((1 << i) >= viewport[2])
        {
            width_p2 = 1 << i;
            break;
        }
    }

    // Find height
    height_p2 = 0;
    for (i=0; i<31; i++)
    {
        if ((1 << i) >= viewport[3])
        {
            height_p2 = 1 << i;
            break;
        }
    }

    // Delete texture (if exists)
    if (self->texture_old)
        TEX_Delete (self->texture_old);

    if (self->texture_new)
        TEX_Delete (self->texture_new);

    // Create new one
    self->texture_old = TEX_New (width_p2, height_p2, 32, NULL, 0);
    if (!self->texture_old)
        return 0;

    // Set texture parameters
    TEX_SetFilter (GL_LINEAR, GL_LINEAR);
    TEX_SetWrap (GL_CLAMP, GL_CLAMP);
    TEX_SetEnvironment (GL_MODULATE);


    self->texture_new = TEX_New (width_p2, height_p2, 32, NULL, 0);
    if (!self->texture_new)
        return 0;

    // Set texture parameters
    TEX_SetFilter (GL_LINEAR, GL_LINEAR);
    TEX_SetWrap (GL_CLAMP, GL_CLAMP);
    TEX_SetEnvironment (GL_MODULATE);

    // Init texture mapping coords
    self->u0 = (GLfloat) 0.0f;
    self->v0 = (GLfloat) 0.0f;
    self->u1 = (GLfloat) viewport[2] / (GLfloat) width_p2;
    self->v1 = (GLfloat) viewport[3] / (GLfloat) height_p2;

    // Init texture coords
    self->x0 = (GLfloat) viewport[0];
    self->y0 = (GLfloat) viewport[1];
    self->x1 = (GLfloat) (viewport[2] + self->x0);
    self->y1 = (GLfloat) (viewport[3] + self->y0);

    return 1;
}


TZoomer *ZOOMER_Init (void)
{
    TZoomer *self;

    self = (TZoomer *) malloc (sizeof (TZoomer));
    if (!self)
        return 0;

    // Init viewport to unused state
    self->viewport[0] = -1;
    self->viewport[1] = -1;
    self->viewport[2] = -1;
    self->viewport[3] = -1;

    // No initial textures
    self->texture_old = NULL;
    self->texture_new = NULL;

    // Default scales
    self->scale_x_old = 1.03f;
    self->scale_y_old = 1.03f;
    self->scale_x_new = 1.0f;
    self->scale_y_new = 1.0f;

    self->alpha_old   = 0.9f;
    self->alpha_new   = 0.2f;

    return self;
}


void ZOOMER_Do (TZoomer *self, float t)
{
    GLint viewport[4];

    // Get viewport dimensions
    glGetIntegerv (GL_VIEWPORT, viewport);
    if ((viewport[0] != self->viewport[0]) ||
        (viewport[1] != self->viewport[1]) ||
        (viewport[2] != self->viewport[2]) ||
        (viewport[3] != self->viewport[3]))
    {
        // Update viewport
        self->viewport[0] = viewport[0];
        self->viewport[1] = viewport[1];
        self->viewport[2] = viewport[2];
        self->viewport[3] = viewport[3];

        if (!ZOOMER_DynamicInit (self, viewport))
        {
            printf ("ZOOMER: Dynamic init failed!\n");
            exit (1);
        }
    }

    glEnable (GL_TEXTURE_2D);
    glEnable (GL_BLEND);

    // Update NEW texture
    TEX_Bind (self->texture_new);
    glCopyTexSubImage2D (GL_TEXTURE_2D, 0,
                         0, 0,
                         self->x0,
                         self->y0,
                         self->x1 - self->x0,
                         self->y1 - self->y0);
    TEX_SetFilter (GL_LINEAR, GL_LINEAR);
    TEX_SetWrap (GL_CLAMP, GL_CLAMP);

    // Set Matrix mode
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ( );
    gluOrtho2D (-1.0, +1.0, -1.0, +1.0);
    glMatrixMode (GL_MODELVIEW);

        // Paint OLD texture

    TEX_Bind (self->texture_old);
    glBlendFunc (GL_SRC_ALPHA, GL_ZERO);
    glColor4f (1.0f, 1.0f, 1.0f, self->alpha_old);
    glLoadIdentity ( );
    glScalef (self->scale_x_old, self->scale_y_old, 1.0);
    glBegin (GL_QUADS);
     glTexCoord2f (self->u0, self->v1); glVertex2f (-1.0, +1.0);
     glTexCoord2f (self->u1, self->v1); glVertex2f (+1.0, +1.0);
     glTexCoord2f (self->u1, self->v0); glVertex2f (+1.0, -1.0);
     glTexCoord2f (self->u0, self->v0); glVertex2f (-1.0, -1.0);
    glEnd ( );

    // Paint NEW texture
    TEX_Bind (self->texture_new);
//    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f (1.0f, 1.0f, 1.0f, self->alpha_new);
    glLoadIdentity ( );
    glScalef (self->scale_x_new, self->scale_y_new, 1.0);
    glBegin (GL_QUADS);
     glTexCoord2f (self->u0, self->v1); glVertex2f (-1.0, +1.0);
     glTexCoord2f (self->u1, self->v1); glVertex2f (+1.0, +1.0);
     glTexCoord2f (self->u1, self->v0); glVertex2f (+1.0, -1.0);
     glTexCoord2f (self->u0, self->v0); glVertex2f (-1.0, -1.0);
    glEnd ( );

    // Update OLD texture
    TEX_Bind (self->texture_old);
    glCopyTexSubImage2D (GL_TEXTURE_2D, 0,
                         0, 0,
                         self->x0,
                         self->y0,
                         self->x1 - self->x0,
                         self->y1 - self->y0);
    TEX_SetFilter (GL_LINEAR, GL_LINEAR);
    TEX_SetWrap (GL_CLAMP, GL_CLAMP);

    glDisable (GL_TEXTURE_2D);
    glDisable (GL_BLEND);
}

void ZOOMER_Kill (TZoomer *self)
{
    if (self->texture_old) TEX_Delete (self->texture_old);
    if (self->texture_new) TEX_Delete (self->texture_new);

    free (self);
}
