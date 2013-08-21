///////////////////////////////////////////////////////////////////////
// Blur
///////////////////////////////////////////////////////////////////////
#ifndef __BLUR_H__
#define __BLUR_H__

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <glt/glt.h>
#include "interp.h"

typedef struct
{
    TTexture *texture;        // Texture
    GLint     x0, y0, x1, y1; // Texture coords
    GLfloat   u0, v0, u1, v1; // Texture mapping coords
    GLint     viewport[4];    // Last active viewport

    float     blur_level;     // 0..100
    int       linear;         // 0: nearest, 1: linear

    TInterp  *it;
} TBlur;

TBlur *BLUR_New    (void);
void   BLUR_Do     (TBlur *self);
void   BLUR_Delete (TBlur *self);


#endif // __BLUR_H__
