#ifndef __FX_H__
#define __FX_H__

#ifdef WIN32
  #include <windows.h>
#endif // WIN32
#include <GL/gl.h>

typedef struct TFx_t
{
    GLuint texture_id;
    int RES_X;
    int RES_Y;
} TFx;

TFx* fx_init(int width, int height);
int  fx_do  (TFx* self, float t);
int  fx_kill(TFx* self);

#endif // __FX_H__
