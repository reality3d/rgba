#ifndef __MBLUR_H__
#define __MBLUR_H__

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <glt/glt.h>

typedef struct
{
    TTexture *texture;
    GLint     x0, y0, x1, y1;
} TQuad;

typedef struct
{
    // Viewport
    int    width, height;
    int    vp_x0, vp_y0, vp_x1, vp_y1;

    // Quad list
    int    num_quads;
    TQuad *quadlist;

    // Motion blur alpha [0..1]
    float  alpha;

    // Blend
    GLenum blend_src_factor;
    GLenum blend_dst_factor;

    int    clear_old_frames;
} TMBlur;


TMBlur *MBLUR_Init  (void);
void    MBLUR_Do    (TMBlur *self);
void    MBLUR_Kill  (TMBlur *self);
void    MBLUR_Clear (TMBlur *self);

#endif // __MBLUR_H__
