////////////////////////////////////////////////////////////////////////
//
// ZOOMER
//
////////////////////////////////////////////////////////////////////////
#ifndef __ZOOMER_H__
#define __ZOOMER_H__

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <glt/glt.h>


typedef struct
{
    TTexture *texture_old;
    TTexture *texture_new;

    GLint     x0, y0, x1, y1; // Texture coords
    GLfloat   u0, v0, u1, v1; // Texture mapping coords
    GLint     viewport[4];    // Last active viewport

    GLfloat   scale_x_old, scale_y_old;
    GLfloat   scale_x_new, scale_y_new;
    GLfloat   alpha_old, alpha_new;
} TZoomer;

TZoomer *ZOOMER_Init (void);
void     ZOOMER_Do   (TZoomer *self, float t);
void     ZOOMER_Kill (TZoomer *self);


#endif // __ZOOMER_H__
