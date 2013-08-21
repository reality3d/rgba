///////////////////////////////////////////////////////////////////////
// COLOR BUFFER DEFORM  -  epsilum
///////////////////////////////////////////////////////////////////////

//
// NOTAS:
//  Para la deformacion, tenemos un espacio definido en el rango
//   [0, 2] x [0, 2]   con z = +1 (defecto)  (z=0 es la pantalla)
//
//  Para la proxima version, seguramente cambiara a
//   [-1, -1] x [+1, +1]
//  de forma que tengamos dentro un circulo de radio 1 perfecto.
//

#ifdef WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <glt/glt.h>
#include "deform.h"
#include "interp.h"
#include "expr/expr.h"
#include "vector.h"
#include "wave.h"

#define PI 3.141592f
#define SQR(a) ((a)*(a))

// Matrix 4x4
static float m[4*4];


// Devuelve el menor numero que sea mayor o igual a "n" y que sea potencia de 2
static int BiggerPow2 (int n)
{
    int i;

    for (i=0; i<31; i++)
        if ((1 << i) >= n)
            break;

    return (1 << i);
}

//
// 1. Genera una textura en potencia de 2 que contenga al viewport actual
// 2. Actualiza las coordenadas de los vertices y textura maximos y minimos
// 3. Genera los nuevos datos de la malla
//
static int DEFORM_DynamicInit (TDeform *self, GLint *viewport)
{
    int width_p2, height_p2; // width and height (power of 2)

    int i, j;
    TGrid *g;

    // Find width and height (power of 2)
    width_p2  = BiggerPow2 (viewport[2]);
    height_p2 = BiggerPow2 (viewport[3]);

    // Existe textura con dimensiones inferiores a las requeridas, o no existe textura => Crear una nueva
    if ((self->texture && ((self->texture->width < width_p2) || (self->texture->height < height_p2))) || (!self->texture))
    {
        char texname[64];

        // Delete texture (if exists)
        if (self->texture)
            CACHE_FreeTexture (self->texture);

        // Create the symbolic name of the texture
        // RT2_<width>x<height>x<format>
        sprintf (texname, "R2T_%ix%ix%s", width_p2, height_p2, "RGBA");

        // Try to load from cache
        self->texture = CACHE_LoadTexture (texname, TEX_BUILD_MIPMAPS);
        if (!self->texture)
        {
            // Create new one
            // NOTA: Aunque no usamos mipmaps, como se va a compartir la textura,
            //       mejor los creamos por si otro plugin los necesita.
            //self->texture = TEX_New (width_p2, height_p2, GL_RGBA, NULL, TEX_BUILD_MIPMAPS);
            self->texture = TEX_New (width_p2, height_p2, 32, NULL, TEX_BUILD_MIPMAPS);
            if (!self->texture)
                return 0;

            if (!CACHE_InsertTexture (texname, self->texture))
                return 0;
        }
    }

    // Set texture parameters
    TEX_Bind (self->texture);
    TEX_SetFilter (GL_LINEAR, GL_LINEAR);
    TEX_SetWrap (GL_CLAMP, GL_CLAMP);
    TEX_SetEnvironment (GL_MODULATE);

    // Init texture mapping coords
    self->u0 = (GLfloat) 0.0f;
    self->v0 = (GLfloat) 0.0f;
    //self->u1 = (GLfloat) viewport[2] / (GLfloat) width_p2;
    //self->v1 = (GLfloat) viewport[3] / (GLfloat) height_p2;
    self->u1 = (GLfloat) viewport[2] / (GLfloat) self->texture->width;
    self->v1 = (GLfloat) viewport[3] / (GLfloat) self->texture->height;

    // Init texture coords
    self->x0 = (GLfloat) viewport[0];
    self->y0 = (GLfloat) viewport[1];
    self->x1 = (GLfloat) (viewport[2] + self->x0);
    self->y1 = (GLfloat) (viewport[3] + self->y0);

    // Fill grid
    g = self->grid;
    for (j=0; j<self->grid_ysize; j++)
    {
        for (i=0; i<self->grid_xsize; i++, g++)
        {
            g->src_x = g->x = 2.0f * (float) i / (float) (self->grid_xsize - 1.0f);
            g->src_y = g->y = 2.0f - 2.0f * (float) j / (float) (self->grid_ysize - 1.0f);

            g->src_z = g->z = +1.0f;
            g->src_u = g->u = self->u1 * (float) i / (float) (self->grid_xsize - 1.0f);
            g->src_v = g->v = self->v1 * (float) j / (float) (self->grid_ysize - 1.0f);

            // Distancia al centro
            g->d = sqrt (SQR (g->x - 1.0f) + SQR (g->y - 1.0f));
        }
    }

    return 1;
}

// Matriz de proyeccion con rango [0,2]x[0,2]
// Si z=1 => Se rellena todo el viewport
// Cuando lo pasemos a OpenGL puro, sera con z=-1
static void GL_SetProyection (void)
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ( );
    glLoadMatrixf ((float *) m);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ( );
}

static void CreateMatrix (void)
{
    float w  = +4.0f / (float) 2.0f;
    float h  = -4.0f / (float) 2.0f;
    float dw = -2.0f;
    float dh = +2.0f;

    float mfar  = 2.0f;
    float mnear = 0.001f;
    float q     = mfar / (mfar - mnear);

    m[0x0]=w;    m[0x1]=0.0f; m[0x2]=0.0f;     m[0x3]=0.0f;
    m[0x4]=0.0f; m[0x5]=h;    m[0x6]=0.0f;     m[0x7]=0.0f;
    m[0x8]=0.0f; m[0x9]=0.0f; m[0xA]=q;        m[0xB]=1.0f;
    m[0xC]=dw;   m[0xD]=dh;   m[0xE]=-q*mnear; m[0xF]=1.0f;
}


TDeform *DEFORM_New (int grid_w, int grid_h)
{
    TDeform *self;

    self = (TDeform *) malloc (sizeof (TDeform));
    if (!self)
        return 0;
    memset (self, 0, sizeof (TDeform));

    // Init viewport to unused state
    self->viewport[0] = -1; self->viewport[1] = -1;
    self->viewport[2] = -1; self->viewport[3] = -1;

    // No texture (La creamos dinamicamente)
    self->texture = NULL;

    // Init grid
    self->grid_xsize = grid_w;
    self->grid_ysize = grid_h;
    self->grid       = (TGrid *) malloc (self->grid_xsize * self->grid_ysize * sizeof (TGrid));
    assert (self->grid != NULL);
    CreateMatrix ( );

    // Default parameters
    self->mode       = DEFORM_MODE_WAVES;
    self->amplitude  = 2.0f;
    self->frequency1 = 15.0f;
    self->frequency2 = 15.0f;
    self->num_waves  = 1.0f;

    self->draw_grid     = 0;
    self->wire_width    = 2.0f;
    self->wire_color[3] = 1.0f; // Alpha
    self->wire_color[0] = self->wire_color[1] = self->wire_color[2] = 1.0f;

    self->ilumination = ILUMINATION_NONE;
    Set4f (self->light_ambient,  0.5f, 0.5f, 0.5f, 1.0f);
    Set4f (self->light_diffuse,  1.0f, 1.0f, 1.0f, 1.0f);
    Set4f (self->light_position, 1.0f, 1.0f, -4.0f, 1.0f);
    //Set4f (self->light_specular, 0.793277f, 0.383193f, 0.258824f, 1.0f);
    Set4f (self->light_specular, 1.0f, 1.0f, 1.0f, 1.0f);
    Set4f (self->mat_emission,   0.0f, 0.0f, 0.0f, 0.0f);
    //Set4f (self->mat_specular,   0.793277f, 0.383193f, 0.258824f, 1.0f);
    Set4f (self->mat_specular,   1.0f, 1.0f, 1.0f, 1.0f);
    Set4f (self->mat_ambient,    0.1f, 0.1f, 0.1f, 1.0f);
    Set4f (self->mat_diffuse,    1.0f, 1.0f, 1.0f, 1.0f);
    self->mat_shininess = 22.0f;
    // Interpoladores
    self->i_amplitude = INTERP_New ( );
    self->i_freq1     = INTERP_New ( );
    self->i_freq2     = INTERP_New ( );
    self->i_nwaves    = INTERP_New ( );

    self->draw_grid   = 0;

    TURB_Init (0);
    // Expression
    VL_Init ( );
    VL_AddVar ("x");
    VL_AddVar ("y");
    VL_AddVar ("z");
    VL_AddVar ("d");
    VL_AddVar ("t");
    VL_AddVar ("ampl");
    VL_AddVar ("freq1");
    VL_AddVar ("freq2");
    VL_AddVar ("nwaves");
    VL_AddVar ("u");
    VL_AddVar ("v");

    self->expr_x = EXPR_New ("x");
    self->expr_y = EXPR_New ("y");
    self->expr_z = EXPR_New ("1.0");
    self->expr_u = EXPR_New ("u");
    self->expr_v = EXPR_New ("v");

    // Water
    WAVE_Init ( );

    self->viscosity = 0.98f;

    return self;
}

static float frand (void)
{
    return (rand ( ) & 0xFF) / 255.0f;
}


static void CalcVertexNormals (TDeform *self)
{
    int i, j, w, h;
    TVector vv1, vv2;
    TGrid *g1, *g2;

    // Clear vertex normals
    i  = self->grid_xsize * self->grid_ysize;
    g1 = self->grid;
    while (i --)
    {
        g1->nv[0] = g1->nv[1] = g1->nv[2] = 0.0f;
        g1 ++;
    }

    w = self->grid_xsize - 1;
    h = self->grid_ysize - 1;
    g1 = self->grid;
    g2 = &self->grid[self->grid_xsize];
    for (j=0; j<h; j++)
    {
        for (i=0; i<w; i++)
        {
            VECT_Sub (&vv1, (TVector *) &g1[i+1].x, (TVector *) &g1[i].x);
            VECT_Sub (&vv2, (TVector *) &g2[i].x,   (TVector *) &g1[i].x);
            VECT_CrossProduct ((TVector *) g1[i].n, &vv1, &vv2);
            //VECT_Normalize ((TVector *) g1[i].n);

            VECT_Inc ((TVector *) g1[i].nv,   (TVector *) g1[i].n);
            VECT_Inc ((TVector *) g1[i+1].nv, (TVector *) g1[i].n);
            VECT_Inc ((TVector *) g2[i].nv,   (TVector *) g1[i].n);
            VECT_Inc ((TVector *) g2[i+1].nv, (TVector *) g1[i].n);
            //VECT_Add ((TVector *) g1[i].nv,   (TVector *) g1[i].nv,   (TVector *) g1[i].n);
            //VECT_Add ((TVector *) g1[i+1].nv, (TVector *) g1[i+1].nv, (TVector *) g1[i].n);
            //VECT_Add ((TVector *) g2[i].nv,   (TVector *) g2[i].nv,   (TVector *) g1[i].n);
            //VECT_Add ((TVector *) g2[i+1].nv, (TVector *) g2[i+1].nv, (TVector *) g1[i].n);
        }

        g1 += self->grid_xsize;
        g2 += self->grid_xsize;
    }

        /*
    // Old code
    for (j=0; j<self->grid_ysize-1; j++)
        for (i=0; i<self->grid_xsize-1; i++)
        {
            VECT_Sub (&vv1,
                      (TVector *) &self->grid[(i+1) + self->grid_xsize * j].x,
                      (TVector *) &self->grid[i     + self->grid_xsize * j].x);

            VECT_Sub (&vv2,
                      (TVector *) &self->grid[i + self->grid_xsize * (j+1)].x,
                      (TVector *) &self->grid[i + self->grid_xsize * j].x);

            VECT_CrossProduct ((TVector *) self->grid[i + self->grid_xsize * j].n, &vv1, &vv2);
            //VECT_Normalize ((TVector *) self->grid[i+self->grid_xsize*j].n);

            VECT_Add ((TVector *) self->grid[i+self->grid_xsize*j].nv,
                      (TVector *) self->grid[i+self->grid_xsize*j].nv,
                      (TVector *) self->grid[i+self->grid_xsize*j].n);
            VECT_Add ((TVector *) self->grid[i+1+self->grid_xsize*j].nv,
                      (TVector *) self->grid[i+1+self->grid_xsize*j].nv,
                      (TVector *) self->grid[i+self->grid_xsize*j].n);
            VECT_Add ((TVector *) self->grid[i+self->grid_xsize*(j+1)].nv,
                      (TVector *) self->grid[i+self->grid_xsize*(j+1)].nv,
                      (TVector *) self->grid[i+self->grid_xsize*j].n);
            VECT_Add ((TVector *) self->grid[i+1+self->grid_xsize*(j+1)].nv,
                      (TVector *) self->grid[i+1+self->grid_xsize*(j+1)].nv,
                      (TVector *) self->grid[i+self->grid_xsize*j].n);
        }
*/

    /*
    for (j=0; j<self->grid_ysize; j++)
        for (i=0; i<self->grid_xsize; i++)
            VECT_Normalize ((TVector *) self->grid[i+self->grid_xsize*j].nv);
   */
}


static void CalcQuadNormals (TDeform *self)
{
    int i, j, w, h;
    TVector vv1, vv2;
    TGrid  *g1, *g2;

    w = self->grid_xsize - 1;
    h = self->grid_ysize - 1;

    g1 = self->grid;
    g2 = &self->grid[self->grid_xsize];

    for (j=0; j<h; j++)
    {
        for (i=0; i<w; i++)
        {
            VECT_Sub (&vv1, (TVector *) &g1[i+1].x, (TVector *) &g1[i].x);
            VECT_Sub (&vv2, (TVector *) &g2[i].x,   (TVector *) &g1[i].x);
            VECT_CrossProduct ((TVector *) g1[i].n, &vv1, &vv2);
            //VECT_Normalize ((TVector *) g1[i].n);
        }

        g1 += self->grid_xsize;
        g2 += self->grid_xsize;
    }


    /*
    // Old code
    for (j=0; j<self->grid_ysize-1; j++)
        for (i=0; i<self->grid_xsize-1; i++)
        {
            VECT_Sub (&vv1,
                      (TVector *) &self->grid[(i+1) + self->grid_xsize * j].x,
                      (TVector *) &self->grid[i     + self->grid_xsize * j].x);

            VECT_Sub (&vv2,
                      (TVector *) &self->grid[i + self->grid_xsize * (j+1)].x,
                      (TVector *) &self->grid[i + self->grid_xsize * j].x);

            VECT_CrossProduct ((TVector *) self->grid[i + self->grid_xsize * j].n, &vv1, &vv2);
            //VECT_Normalize ((TVector *) grid[i][j].n);
        }
    */
}


// --------------------------------------------------------------------
//                       D E F O R M A C I O N E S
// --------------------------------------------------------------------
static void GRID_Rand (TDeform *self, float amplitude)
{
    int i, j;
    float dx, dy;
    TGrid *g;

    dx = 1.0f / (float) self->grid_xsize;
    dy = 1.0f / (float) self->grid_ysize;

    g = &self->grid[ self->grid_xsize ];
    for (j=1; j<self->grid_ysize-1; j++)
    {
        for (i=1; i<self->grid_xsize-1; i++)
            g[i].z = +1.0f + amplitude * (frand ( ) - 0.5f);

        g += self->grid_xsize;
    }
}

static void GRID_Sphere (TDeform *self, float amplitude)
{
    int i, j;
    float dx, dy, x, y, d;
    TGrid *g;

    dx = 2.0f / (float) self->grid_xsize;
    dy = 2.0f / (float) self->grid_ysize;

    g = &self->grid[ self->grid_xsize ];
    y = -1.0f + dy;
    for (j=1; j<self->grid_ysize-1; j++)
    {
        x = -1.0f + dx;
        for (i=1; i<self->grid_xsize-1; i++)
        {
            //d = sqrt (x*x + y*y);
            //d = x*x + y*y;   // fast method => less quality
            d = g[i].d;
            if (d <= 1.0f)
                g[i].z = 1.0f + (1.0f - d) * amplitude;
            else
                g[i].z = 1.0f;

            x += dx;
        }

        y += dy;
        g += self->grid_xsize;
    }
}

static void GRID_Waves (TDeform *self, float t,
                        float nwaves,
                        float amplitude,
                        float frequency)
{
    int i, j;
    float dx, dy, x, y, d, a;
    TGrid *g;

    nwaves    = nwaves * PI;
    frequency = 1.0f / frequency;

    dx = 2.0f / (float) self->grid_xsize;
    dy = 2.0f / (float) self->grid_ysize;

    g = &self->grid[ self->grid_xsize ];
    y = -1.0f + dy;
    for (j=1; j<self->grid_ysize-1; j++)
    {
        x = -1.0f + dx;
        for (i=1; i<self->grid_xsize-1; i++)
        {
            //d = sqrt (x*x + y*y);
            //d = x*x + y*y;   // fast method => less quality
            d = g[i].d;

            a = 1.0f + amplitude * cos (d*nwaves + t*frequency);
            g[i].z = 1.0f + (1.0f - d) * a;

            x += dx;
        }

        y += dy;
        g += self->grid_xsize;
    }
}

static void GRID_Sombrero (TDeform *self, float t,
                           float amplitude,
                           float freq)
{
    int i, j;
    float dx, dy, x, y, d, a;
    TGrid *g;

    freq = 1.0f / freq;

    dx = 2.0f / (float) self->grid_xsize;
    dy = 2.0f / (float) self->grid_ysize;

    g = &self->grid[ self->grid_xsize ];
    y = -1.0f + dy;
    for (j=1; j<self->grid_ysize-1; j++)
    {
        x = -1.0f + dx;
        for (i=1; i<self->grid_xsize-1; i++)
        {
            d = amplitude * g[i].d;

            g[i].z = 1.0f + (sin (d + t*freq) / d);
            if (g[i].z <= 0.001f)
                g[i].z = 0.001f;

            x += dx;
        }

        y += dy;
        g += self->grid_xsize;
    }
}

static void GRID_F1 (TDeform *self, float t,
                     float amplitude,
                     float freq1,
                     float freq2)
{
    int i, j;
    float dx, dy, x, y;
    TGrid *g;

    freq1 = 1.0f / freq1;
    freq2 = 1.0f / freq2;

    dx = 2.0f / (float) self->grid_xsize;
    dy = 2.0f / (float) self->grid_ysize;

    g = &self->grid[ self->grid_xsize ];
    y = -1.0f + dy;
    for (j=1; j<self->grid_ysize-1; j++)
    {
        x = -1.0f + dx;
        for (i=1; i<self->grid_xsize-1; i++)
        {
            g[i].z = 1.0f + amplitude * cos(x+t*freq1 + PI * sin(y+t*freq2));
            x += dx;
        }

        y += dy;
        g += self->grid_xsize;
    }
}

static void GRID_F2 (TDeform *self, float t,
                     float amplitude,
                     float freq1,
                     float freq2)
{
    int i, j;
    float dx, dy, x, y, d;
    TGrid *g;

    freq1 = 1.0f / freq1;
    freq2 = 1.0f / freq2;

    dx = 2.0f / (float) self->grid_xsize;
    dy = 2.0f / (float) self->grid_ysize;

    g = &self->grid[ self->grid_xsize ];
    y = -1.0f + dy;
    for (j=1; j<self->grid_ysize-1; j++)
    {
        x = -1.0f + dx;
        for (i=1; i<self->grid_xsize-1; i++)
        {
            g[i].z = 1.0f + amplitude * ((cos(PI*x+t*freq1) + sin(PI*y+t*freq2))*0.5f);
            if (g[i].z < 0.001f)
                g[i].z = 0.001f;
            x += dx;
        }

        y += dy;
        g += self->grid_xsize;
    }
}


static void GRID_Expr (TDeform *self, float t)
{
    int i, j;
    float dx, dy, x, y, d;
    TGrid *g;

    VL_Set ("t", t);
    VL_Set ("ampl",   self->amplitude);
    VL_Set ("freq1",  self->frequency1);
    VL_Set ("freq2",  self->frequency2);
    VL_Set ("nwaves", self->num_waves);

    dx = 2.0f / (float) self->grid_xsize;
    dy = 2.0f / (float) self->grid_ysize;

    /*
    g = &self->grid[ self->grid_xsize ];
    y = -1.0f + dy;
    for (j=1; j<self->grid_ysize-1; j++)
    {
        x = -1.0f + dx;
        for (i=1; i<self->grid_xsize-1; i++)
        {
            VL_Set ("x", (double) g[i].src_x);
            VL_Set ("y", (double) g[i].src_y);
            VL_Set ("u", (double) g[i].src_u);
            VL_Set ("v", (double) g[i].src_v);
            VL_Set ("d", (double) g[i].d);

            g[i].x = (float) EXPR_Eval (self->expr_x);
            g[i].y = (float) EXPR_Eval (self->expr_y);
            g[i].z = (float) EXPR_Eval (self->expr);

            g[i].u = (float) EXPR_Eval (self->expr_u);
            g[i].v = (float) EXPR_Eval (self->expr_v);

            x += dx;
        }

        y += dy;
        g += self->grid_xsize;
    }
    */

    g = &self->grid[0];
    for (j=0; j<self->grid_ysize; j++)
    {
        for (i=0; i<self->grid_xsize; i++)
        {
            VL_Set ("x", (double) g[i].src_x);
            VL_Set ("y", (double) g[i].src_y);
            VL_Set ("z", (double) g[i].src_z);
            VL_Set ("u", (double) g[i].src_u);
            VL_Set ("v", (double) g[i].src_v);
            VL_Set ("d", (double) g[i].d);

            g[i].x = (float) EXPR_Eval (self->expr_x);
            g[i].y = (float) EXPR_Eval (self->expr_y);
            g[i].u = (float) EXPR_Eval (self->expr_u);
            g[i].v = (float) EXPR_Eval (self->expr_v);

            if ((j!=0)&&(j!=(self->grid_ysize-1))&&
                (i!=0)&&(i!=(self->grid_xsize-1)))
                g[i].z = (float) EXPR_Eval (self->expr_z);

            // clamp u
            // Le restamos 0.001f para que pille 2 texel (y pueda hacer filtro bilineal) ¨?
            if (g[i].u > g[self->grid_xsize - 1].src_u - 0.001f)
                g[i].u = g[self->grid_xsize - 1].src_u - 0.001f;
            if (g[i].u < g[0].src_u)
                g[i].u = g[0].src_u;

            // clamp v
            if (g[i].v > self->v1 - 0.001f)
                g[i].v = self->v1 - 0.001f;
            if (g[i].v < self->v0)
                g[i].v = self->v0;

            // clamp y
            if (j == (self->grid_ysize - 1))
                if (g[i].y > 0.0f) g[i].y = 0.0f;
            if (j == 0)
                if (g[i].y < 2.0f) g[i].y = 2.0f;
        }

        // clamp x
        if (g[self->grid_xsize-1].x < 2.0f)
            g[self->grid_xsize-1].x = 2.0f;
        if (g[0].x > 0.0f)
            g[0].x = 0.0f;

        g += self->grid_xsize;
    }
}

// No se calculan los bordes!!
static void GRID_ExprClamp (TDeform *self, float t)
{
    int i, j;
    float dx, dy, x, y, d;
    TGrid *g;

    VL_Set ("t", t);
    VL_Set ("ampl",   self->amplitude);
    VL_Set ("freq1",  self->frequency1);
    VL_Set ("freq2",  self->frequency2);
    VL_Set ("nwaves", self->num_waves);

    dx = 2.0f / (float) self->grid_xsize;
    dy = 2.0f / (float) self->grid_ysize;

    g = &self->grid[ self->grid_xsize ];
    y = -1.0f + dy;
    for (j=1; j<self->grid_ysize-1; j++)
    {
        x = -1.0f + dx;
        for (i=1; i<self->grid_xsize-1; i++)
        {
            VL_Set ("x", (double) g[i].src_x);
            VL_Set ("y", (double) g[i].src_y);
            VL_Set ("z", (double) g[i].src_z);
            VL_Set ("u", (double) g[i].src_u);
            VL_Set ("v", (double) g[i].src_v);
            VL_Set ("d", (double) g[i].d);

            g[i].x = (float) EXPR_Eval (self->expr_x);
            g[i].y = (float) EXPR_Eval (self->expr_y);
            g[i].z = (float) EXPR_Eval (self->expr_z);

            g[i].u = (float) EXPR_Eval (self->expr_u);
            g[i].v = (float) EXPR_Eval (self->expr_v);

            x += dx;
        }

        y += dy;
        g += self->grid_xsize;
    }
}

///////////////////////////////////////////////////////////////////////
// W A T E R                                                         //
///////////////////////////////////////////////////////////////////////

void WATER_CreateRipple (TDeform *self, float x, float y,
                         int width, float height)
{
    int   i, j, ix, iy;
    float fx, fy, d, dk;

    x *= (float) self->grid_xsize;
    y *= (float) self->grid_ysize;

    // Ball
    dk = sqrt (width*width);
    if (width <= 0) dk = 1.0f;
    for (j=-width; j<=width; j++)
        for (i=-width; i<=width; i++)
        {
            ix = i + x;
            iy = j + y;
            if ((ix >= 0) && (ix < self->grid_xsize) &&
                (iy >= 0) && (iy < self->grid_ysize))
            {
                d = (dk - sqrt (i*i + j*j)) / dk;
                if (d < 0.0f) d = 0.0f;

                self->grid[ix+self->grid_xsize*iy].h0 += height * d;
            }
        }
}

void WATER_CreateBox (TDeform *self, float x, float y,
                      float width, float height)
{
    int   i, j, ix, iy;
    float fx, fy, d, dk;

    x *= (float) self->grid_xsize;
    y *= (float) self->grid_ysize;

    // Box
    for (j=-width; j<=width; j++)
        for (i=-width; i<=width; i++)
        {
            ix = i + x;
            iy = j + y;
            if ((ix >= 0) && (ix < self->grid_xsize) &&
                (iy >= 0) && (iy < self->grid_ysize))
                self->grid[ix+self->grid_xsize*iy].h0 += height;
        }
}

void WATER_ResetGrid (TDeform *self)
{
    int i, l;

    l = self->grid_xsize * self->grid_ysize;
    for (i=0; i<l; i++)
    {
        self->grid[i].z  = 1.0f;
        self->grid[i].h0 = self->grid[i].h1 = 0.0f;
    }
}


static void GRID_Water (TDeform *self)
{
    int i, j;
    float a, b;
    TGrid *g;

    // Smooth water
    g = &self->grid[ self->grid_xsize ];
    for (j=1; j<self->grid_ysize-1; j++)
    {
        for (i=1; i<self->grid_xsize-1; i++)
        {
            a = 4.0f * (g[i-1].h1 + g[i+1].h1 + g[i-self->grid_xsize].h1 + g[i+self->grid_xsize].h1);
            b = a + g[i-1-self->grid_xsize].h1 + g[i+1-self->grid_xsize].h1 + g[i-1+self->grid_xsize].h1 + g[i+1+self->grid_xsize].h1;
            g[i].h0 += (g[i].h1 - b*0.04f) / 7.0f;
        }

        g += self->grid_xsize;
    }

    // Calc new 'z' values
    g = &self->grid[ self->grid_xsize ];
    for (j=1; j<self->grid_ysize-1; j++)
    {
        for (i=1; i<self->grid_xsize-1; i++)
        {
            g[i].h1 = g[i].h1 - g[i].h0;
            g[i].h0 = g[i].h0 * self->viscosity;

            g[i].z = 1.0f + g[i].h1;

        }

        g += self->grid_xsize;
    }

    /*
    g = &self->grid[ self->grid_xsize ];
    for (j=1; j<self->grid_ysize-1; j++)
    {
        for (i=1; i<self->grid_xsize-1; i++)
        {
            // Vertex normals
            g[i].nv[0] = g[i+1].z - g[i-1].z;
            g[i].nv[2] = g[i+self->grid_xsize].z - g[i-self->grid_xsize].z;
            g[i].nv[1] = 1.0f;
        }

        g += self->grid_xsize;
    }
    */
}

static void GRID_Water2 (TDeform *self)
{
    int i, j;
    float a, b;
    TGrid *g;

    // Smooth water
    g = &self->grid[ self->grid_xsize ];
    for (j=1; j<self->grid_ysize-1; j++)
    {
        for (i=1; i<self->grid_xsize-1; i++)
        {
            a = (g[i-self->grid_xsize-1].h1 +
                 g[i-self->grid_xsize].h1 +
                 g[i-self->grid_xsize+1].h1 +
                 g[i-1].h1 +
                 g[i+1].h1 +
                 g[i+self->grid_xsize-1].h1 +
                 g[i+self->grid_xsize].h1 +
                 g[i+self->grid_xsize+1].h1) / 8.0f;
            g[i].h0 += (g[i].h1 - a) * 1.0f;
        }

        g += self->grid_xsize;
    }

    // Calc new 'z' values
    g = &self->grid[ self->grid_xsize ];
    for (j=1; j<self->grid_ysize-1; j++)
    {
        for (i=1; i<self->grid_xsize-1; i++)
        {
            g[i].h1 = g[i].h1 - g[i].h0;
            g[i].h0 = g[i].h0 * self->viscosity;

            g[i].z = 1.0f + g[i].h1;
        }

        g += self->grid_xsize;
    }
}



// --------------------------------------------------------------------
//                          GRID DRAWERS
// --------------------------------------------------------------------

static void GRID_Draw (TGrid *grid, int grid_w, int grid_h)
{
    int    i, j;
    TGrid *g1, *g2;

    g1 = grid;
    g2 = &grid[grid_w];
    for (j=0; j<grid_h-1; j++)
    {
        glBegin (GL_QUAD_STRIP);
        for (i=0; i<grid_w; i++, g1++, g2++)
        {
            glTexCoord2fv ((GLfloat *) &g2->u); glVertex3fv ((GLfloat *) &g2->x);
            glTexCoord2fv ((GLfloat *) &g1->u); glVertex3fv ((GLfloat *) &g1->x);
        }
        glEnd ( );
    }

    /*
    // Sin quad-strips
    g1 = self->grid;
    g2 = &self->grid[self->grid_xsize];
    for (j=0; j<self->grid_ysize-1; j++)
    {
        for (i=0; i<self->grid_xsize-1; i++)
        {
            glBegin (GL_QUADS);
             //glNormal3fv ((GLfloat *) &g1[i].nx);
             glTexCoord2fv ((GLfloat *) &g1[i].u);   glVertex3fv ((GLfloat *) &g1[i].x);
             glTexCoord2fv ((GLfloat *) &g2[i].u);   glVertex3fv ((GLfloat *) &g2[i].x);
             glTexCoord2fv ((GLfloat *) &g2[i+1].u); glVertex3fv ((GLfloat *) &g2[i+1].x);
             glTexCoord2fv ((GLfloat *) &g1[i+1].u); glVertex3fv ((GLfloat *) &g1[i+1].x);
            glEnd ( );
        }

        g1 += self->grid_xsize;
        g2 += self->grid_xsize;
    }
    */
}

static void GRID_DrawFlat (TGrid *grid, int grid_w, int grid_h)
{
    int    i, j;
    TGrid *g1, *g2;

    g1 = grid;
    g2 = &grid[grid_w];
    for (j=0; j<grid_h-1; j++)
    {
        glBegin (GL_QUAD_STRIP);
        for (i=0; i<grid_w; i++, g1++, g2++)
        {
            if (!(i & 1))
                glNormal3fv ((GLfloat *) g1->n);

            glTexCoord2fv ((GLfloat *) &g2->u); glVertex3fv ((GLfloat *) &g2->x);
            glTexCoord2fv ((GLfloat *) &g1->u); glVertex3fv ((GLfloat *) &g1->x);
        }
        glEnd ( );
    }
}

static void GRID_DrawSmooth (TGrid *grid, int grid_w, int grid_h)
{
    int    i, j;
    TGrid *g1, *g2;

    g1 = grid;
    g2 = &grid[grid_w];
    for (j=0; j<grid_h-1; j++)
    {
        glBegin (GL_QUAD_STRIP);
        for (i=0; i<grid_w; i++, g1++, g2++)
        {
            glNormal3fv ((GLfloat *) g1->nv);
            glTexCoord2fv ((GLfloat *) &g1->u); glVertex3fv ((GLfloat *) &g1->x);

            glNormal3fv ((GLfloat *) g2->nv);
            glTexCoord2fv ((GLfloat *) &g2->u); glVertex3fv ((GLfloat *) &g2->x);
        }
        glEnd ( );
    }
}

static void GRID_DrawWire (TGrid *grid, int grid_w, int grid_h)
{
    int    i, j;
    TGrid *g1, *g2;

    g1 = grid;
    g2 = &grid[grid_w];

    glBegin (GL_LINES);
    for (j=0; j<grid_h-1; j++)
    {
        for (i=0; i<grid_w-1; i++)
        {
            glVertex3fv ((GLfloat *) &g1[i].x);
            glVertex3fv ((GLfloat *) &g2[i].x);

            glVertex3fv ((GLfloat *) &g1[i].x);
            glVertex3fv ((GLfloat *) &g1[i+1].x);
        }

        g1 += grid_w;
        g2 += grid_w;
    }
    glEnd ( );
}



void DEFORM_Do (TDeform *self, float t)
{
    int i, j;
    GLint  viewport[4];
    TGrid *g1, *g2, *gsrc;
    printf("Do 0\n");
    // Get viewport dimensions
    glGetIntegerv (GL_VIEWPORT, viewport);
    if ((viewport[0] != self->viewport[0]) || (viewport[1] != self->viewport[1]) ||
        (viewport[2] != self->viewport[2]) || (viewport[3] != self->viewport[3]))
    {
        // Update viewport
        self->viewport[0] = viewport[0]; self->viewport[1] = viewport[1];
        self->viewport[2] = viewport[2]; self->viewport[3] = viewport[3];
        assert (DEFORM_DynamicInit (self, viewport) != 0);
    }

    // ZBuffer
    glClear (GL_DEPTH_BUFFER_BIT);
    glClearDepth (1.0);
    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    // Update texture
    glEnable (GL_TEXTURE_2D);
    TEX_Bind (self->texture);
    glCopyTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, self->x0, self->y0,
                         self->x1 - self->x0, self->y1 - self->y0);
    TEX_SetFilter (GL_LINEAR, GL_LINEAR);
    TEX_SetWrap (GL_CLAMP, GL_CLAMP);

    // Set Matrix mode
    GL_SetProyection ( );

    // Por si acaso
    glDisable (GL_BLEND);
    glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
    printf("Do 1\n");
    // Deform grid
    switch (self->mode)
    {
    case DEFORM_MODE_RAND:
        GRID_Rand (self, self->amplitude);
        break;

    case DEFORM_MODE_SPHERE:
        GRID_Sphere (self, self->amplitude);
        break;

    case DEFORM_MODE_WAVES:
        GRID_Waves (self, t, self->num_waves, self->amplitude, self->frequency1);
        break;

    case DEFORM_MODE_SOMBRERO:
        GRID_Sombrero (self, t, self->amplitude, self->frequency1);
        break;

    case DEFORM_MODE_F1:
        GRID_F1 (self, t, self->amplitude, self->frequency1, self->frequency2);
        break;

    case DEFORM_MODE_F2:
        GRID_F2 (self, t, self->amplitude, self->frequency1, self->frequency2);
        break;

    case DEFORM_MODE_WATER:
        GRID_Water (self);
        break;

    case DEFORM_MODE_WATER2:
        GRID_Water2 (self);
        break;

    case DEFORM_MODE_EXPRESSION:
        GRID_Expr (self, t);
        //GRID_ExprClamp (self, t);
        break;
    }

    glEnable (GL_NORMALIZE);

    // Setup ilumination
    if (self->ilumination)
    {
        glEnable (GL_LIGHT1);
        glEnable (GL_LIGHTING);

        // Light
        glLightfv (GL_LIGHT1, GL_POSITION, self->light_position);
        glLightfv (GL_LIGHT1, GL_AMBIENT,  self->light_ambient);
        glLightfv (GL_LIGHT1, GL_DIFFUSE,  self->light_diffuse);
        glLightfv (GL_LIGHT1, GL_SPECULAR, self->light_specular);

        // Material
        glMaterialfv (GL_FRONT, GL_SPECULAR, self->mat_specular);
        glMaterialfv (GL_FRONT, GL_EMISSION, self->mat_emission);
        glMaterialf  (GL_FRONT_AND_BACK, GL_SHININESS, self->mat_shininess);
        glMaterialfv (GL_FRONT, GL_AMBIENT, self->mat_ambient);
        glMaterialfv (GL_FRONT, GL_DIFFUSE, self->mat_diffuse);
    }

    printf("Do 2\n");
    // Draw grid
    switch (self->ilumination)
    {
    case ILUMINATION_NONE:
        GRID_Draw (self->grid, self->grid_xsize, self->grid_ysize);
        break;

    case ILUMINATION_QUAD:
        glShadeModel (GL_FLAT);
        CalcQuadNormals (self);
        GRID_DrawFlat (self->grid, self->grid_xsize, self->grid_ysize);
        break;

    case ILUMINATION_VERTEX:
        glShadeModel (GL_SMOOTH);
        CalcVertexNormals (self);
        GRID_DrawSmooth (self->grid, self->grid_xsize, self->grid_ysize);
        break;
    }

    glDisable (GL_LIGHTING);
    glDisable (GL_LIGHT1);
    glDisable (GL_NORMALIZE);

    glDisable (GL_DEPTH_TEST);
    glDisable (GL_TEXTURE_2D);


    // Draw grid lines
    if (self->draw_grid)
    {
        glEnable (GL_BLEND);
        //glColor4f (1.0f, 1.0f, 1.0f, self->wire_alpha);
        glColor4fv (self->wire_color);
        glLineWidth (self->wire_width);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glShadeModel (GL_SMOOTH);
        glEnable (GL_LINE_SMOOTH);
        glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        GRID_DrawWire (self->grid, self->grid_xsize, self->grid_ysize);

        glDisable (GL_LINE_SMOOTH);
        glDisable (GL_BLEND);
    }
}

void DEFORM_Delete (TDeform *self)
{
    if (self->texture)
        CACHE_FreeTexture (self->texture);

    free (self->grid);

    free (self);
}
