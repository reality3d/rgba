///////////////////////////////////////////////////////////////////////
// COLOR BUFFER DEFORM  -  epsilum
///////////////////////////////////////////////////////////////////////
#ifndef __DEFORM_H__
#define __DEFORM_H__

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <glt/glt.h>
#include "expr/expr.h"
#include "interp.h"

// ----- DEFORM MODES -----
#define DEFORM_MODE_RAND     0  // z = 1 + amplitude * rnd
#define DEFORM_MODE_SPHERE   1  // z = 1 + amplitude * (1 - d)
#define DEFORM_MODE_WAVES    2  // z = 1 + (1 - d) * (1 + amplitude * cos (d*num_waves + t/freq1))
#define DEFORM_MODE_SOMBRERO 3  // z = 1 + sin (amplitude * d + t*freq1) / (amplitude * d)
#define DEFORM_MODE_F1       4  // z = 1 + amplitude * cos(x+t/freq1 + PI * sin(y+t/freq2))
#define DEFORM_MODE_F2       5  // z = 1 + amplitude * ((cos(PI*x+t/freq1) + sin(PI*y+t/freq2))*0.5)
#define DEFORM_MODE_EXPRESSION 6
#define DEFORM_MODE_WATER    7
#define DEFORM_MODE_WATER2   8

// ----- ILUMINATION ------
#define ILUMINATION_NONE   0   // Must be zero!!
#define ILUMINATION_QUAD   1
#define ILUMINATION_VERTEX 2


typedef struct
{
    float x, y, z;  // Vertex coords
    float u, v;     // Mapping coords

    float nv[3];    // Vertex normal
    float n[3];     // Quad normal

    float src_x, src_y, src_z; // Valores originales (sin deformar)
    float src_u, src_v;

    float d;  // Distancia euclidea al centro del "grid"

    float h0, h1; // Water height
} TGrid;

typedef struct
{
    TTexture *texture;        // Texture
    GLint     x0, y0, x1, y1; // Texture coords
    GLfloat   u0, v0, u1, v1; // Texture mapping coords
    GLint     viewport[4];    // Last active viewport

    int       grid_xsize;
    int       grid_ysize;
    TGrid    *grid;           // Malla original y deformada (se calcula cada frame)

    int       mode;

    // Specific mode parameters
    float     amplitude;
    float     frequency1, frequency2;
    float     num_waves;

    // Interpoladores
    TInterp  *i_amplitude;
    TInterp  *i_freq1, *i_freq2;
    TInterp  *i_nwaves;

    // Expression
    TExpr    *expr_x, *expr_y, *expr_z;
    TExpr    *expr_u, *expr_v;

    // Grid
    int       draw_grid;     // Draw wireframe grid?
    float     wire_width;    // Line width
    float     wire_color[4]; // color + alpha

    // Ilumination
    int       ilumination;      // Ilumination mode (0, 1, 2)
    GLfloat   light_ambient[4];
    GLfloat   light_diffuse[4];
    GLfloat   light_position[4];
    GLfloat   light_specular[4];
    GLfloat   mat_emission[4];
    GLfloat   mat_specular[4];
    GLfloat   mat_ambient[4];
    GLfloat   mat_diffuse[4];
    GLfloat   mat_shininess;

    // Water
    float viscosity;
} TDeform;


TDeform *DEFORM_New    (int grid_w, int grid_h);
void     DEFORM_Do     (TDeform *self, float t);
void     DEFORM_Delete (TDeform *self);


void WATER_CreateRipple (TDeform *self, float x, float y, int width, float height);
void WATER_CreateBox (TDeform *self, float x, float y, float width, float height);
void WATER_ResetGrid (TDeform *self);

// Macro to set 4 floats
#define Set4f(p,a,b,c,d) \
    { \
    p[0] = a; \
    p[1] = b; \
    p[2] = c; \
    p[3] = d; \
    }

#endif // __DEFORM_H__
