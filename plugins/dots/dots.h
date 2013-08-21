// --------------------------------------------------------------------
// DOTS . epsilum . March 2003
// --------------------------------------------------------------------
#ifndef __DOTS_H__
#define __DOTS_H__

#include "glt/image.h"
#include "glt/glt.h"
#include "interp.h"


// -- FILTER --
#define DOTS_FILTER_NEAREST 0
#define DOTS_FILTER_LINEAR  1
#define DOTS_FILTER_MIPMAP  2

// -- BLEND --
#define DOTS_BLEND_NORMAL 0
#define DOTS_BLEND_ADD    1

// -- MODE --
#define DOTS_STATIC  0
#define DOTS_DYNAMIC 1
#define DOTS_RAND    2
#define DOTS_DYNAMIC2 3


typedef struct
{
    TTexture *texture;        // Dot texture
    TImage    img;            // Dotted image
    int       srcimg_w, srcimg_h;

    int       xpos, ypos;     // Position
    int       num_xdots;      // Number of horizontal dots
    int       num_ydots;      // Number of vertical dots
    float     wnd_width, wnd_height;
    float     xscale, yscale; // Horizontal and vertical dot scale
    int       alpha;          // Transparencia global para los dots [0..0xFF]

    int       mode;           // Modo de dibujado (static, dynamic, ...)
    int       blend;          // Normal, Add

    // Interpolador
    TInterp  *it_alpha;
} TDots;


TDots *DOTS_New       (char *dotfile, char *imgfile, int num_xdots, int num_ydots);
void   DOTS_Do        (TDots *dots, float t);
void   DOTS_Delete    (TDots *dots);
void   DOTS_SetFilter (TDots *dots, int filter);
void   DOTS_SetWindow (void *self, float w, float h);

#endif // __DOTS_H__
