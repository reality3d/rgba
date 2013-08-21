#ifndef __CANALP_H__
#define __CANALP_H__

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
    int    vp_x0, vp_y0, vp_x1, vp_y1;
    int    width, height;

    // Quadlist (sub-textures)
    int    num_quads;
    TQuad *quadlist;

    // Array con los desplazamientos horizontales (0..height)
    GLint *pos_y;       // Posicion de cada linea horizontal

    int    max_despl;  // Maximo desplazamiento vertical (en lineas)
    int    num_lines;  // Numero de lineas que se desplazan (en porcentage)

    int    t0;
    int    deform;     // 0 - Canal +
                       // 1 - Desplazar la imagen verticalmente (wrap repeat)

    float  speed;      // En deform=1 => Indica velocidad de desplazamiento
} TCanalPlus;


TCanalPlus *CANALP_Init (void);
void        CANALP_Do   (TCanalPlus *self);
void        CANALP_Kill (TCanalPlus *self);


#endif // __CANALP_H__
