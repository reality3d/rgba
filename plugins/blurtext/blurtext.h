///////////////////////////////////////////////////////////////////////
// BLURRING TEXTURE :: epsilum
///////////////////////////////////////////////////////////////////////
#ifndef __BLUR_TEXT_H__
#define __BLUR_TEXT_H__

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <glt/glt.h>
#include "interp.h"

#define BLUR_HORIZONTAL    0
#define BLUR_VERTICAL      1
#define BLUR_ALL           2
#define BLUR_HORIZONTAL2X  3
#define BLUR_VERTICAL2X    4
#define BLUR_ALL2          5
#define BLUR_BOXHORIZONTAL 6
#define BLUR_HARDWARE      9

typedef struct
{
    TTexture *texture;    // Texture

    int       w, h;
    void     *data_src;
    void     *data_dst;

    int       step;       // 1, 2, 3, ...
    float     level;      // Blur level (significado depende del modo)
    int       mode;

    TInterp  *it;
} TBlurText;

TBlurText *BT_New    (char *texture);
void       BT_Do     (TBlurText *self);
void       BT_Delete (TBlurText *self);

#endif // __BLUR_TEXT_H__
