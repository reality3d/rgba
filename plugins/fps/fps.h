#ifndef __FPS_H__
#define __FPS_H__

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "fonts.h"

typedef struct
{
    int    t0;
    int    frames;

    TFont *font;
} TFPS;

#endif // __FPS_H__
