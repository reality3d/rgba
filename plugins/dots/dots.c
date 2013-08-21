// --------------------------------------------------------------------
// DOTS . epsilum . March 2003
// --------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <glt/image.h>
#include <glt/glt.h>
#include "dots.h"
#include "interp.h"


typedef unsigned int color32;


void DOTS_SetDefault (TDots *dots)
{
    dots->xscale = 1.0f;
    dots->yscale = 1.0f;
    dots->xpos   = 50.0f;
    dots->ypos   = 50.0f;
    dots->blend  = DOTS_BLEND_ADD;
    dots->mode   = DOTS_DYNAMIC;
    dots->alpha  = 0xFF;

    DOTS_SetWindow (dots, 100.0f, 100.0f);
}

// NEAREST image resize
static void IMG_Resize (TImage src, TImage dst)
{
    int w, h, x, y;
    float dx, dy, i, j;
    color32 *psrc, *pdst;

    w = dst->width;
    h = dst->height;
    pdst = dst->data;
    psrc = src->data;

    dx = (float)src->width  / (float)w;
    dy = (float)src->height / (float)h;

    j = 0.0f;
    for (y=0; y<h; y++)
    {
        i = 0.0f;
        for (x=0; x<w; x++)
        {
            *(pdst ++) = psrc[(int)i+src->width*(int)j];

            i += dx;
        }

        j += dy;
    }
}

TDots *DOTS_New (char *dotfile, char *imgfile, int num_xdots, int num_ydots)
{
    TImage  imgtmp;
    TDots  *dots;

    dots = (TDots *) malloc (sizeof (TDots));
    assert (dots != NULL);

    imgtmp = IMG_Read (imgfile);
    assert (imgtmp != NULL);

    dots->num_xdots = num_xdots;
    dots->num_ydots = num_ydots;
    dots->srcimg_w = imgtmp->width;
    dots->srcimg_h = imgtmp->height;

    IMG_Convert (imgtmp, 32);
    dots->img = IMG_New (num_xdots, num_ydots, 32);
    assert (dots->img != NULL);

    //gluScaleImage (GL_RGBA, imgtmp->width, imgtmp->height, GL_UNSIGNED_BYTE, imgtmp->data,
    //               num_xdots, num_ydots, GL_UNSIGNED_BYTE, dots->img->data);
    IMG_Resize (imgtmp, dots->img);

    IMG_Delete (imgtmp);

    dots->texture = TEX_Load (dotfile, TEX_BUILD_MIPMAPS);
    assert (dots->texture != NULL);

    TEX_Bind (dots->texture);
    TEX_SetFilter (GL_LINEAR, GL_LINEAR);
    TEX_SetWrap (GL_CLAMP, GL_CLAMP);

    DOTS_SetDefault (dots);

    // Interpolador
    dots->it_alpha = INTERP_New ( );

    return dots;
}

void DOTS_SetFilter (TDots *dots, int filter)
{
    glEnable (GL_TEXTURE_2D);
    TEX_Bind (dots->texture);

    switch (filter)
    {
    case DOTS_FILTER_NEAREST:
        TEX_SetFilter (GL_NEAREST, GL_NEAREST);
        break;

    case DOTS_FILTER_LINEAR:
        TEX_SetFilter (GL_LINEAR, GL_LINEAR);
        break;

    case DOTS_FILTER_MIPMAP:
        TEX_SetFilter (GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        break;
    }

    glDisable (GL_TEXTURE_2D);
}

void DOTS_SetWindow (void *self, float w, float h)
{
    float ax, ay, max;

    TDots *d = (TDots *) self;

    ax = (float) d->srcimg_w;
    ay = (float) d->srcimg_h;
    max = (ax > ay) ? 1.0f / ax : 1.0f / ay;

    d->wnd_width  = w * ax * max;
    d->wnd_height = h * ay * max;
}

static void DrawDot (float x, float y, float w, float h)
{
    float w2 = 0.5f * w;
    float h2 = 0.5f * h;

    glBegin (GL_QUADS);
     glTexCoord2i (0, 1); glVertex2f (x-w2, y+h2);
     glTexCoord2i (1, 1); glVertex2f (x+w2, y+h2);
     glTexCoord2i (1, 0); glVertex2f (x+w2, y-h2);
     glTexCoord2i (0, 0); glVertex2f (x-w2, y-h2);
    glEnd ( );
}


static void GL_SetProjectionMatrix (double left, double right, double bottom, double top)
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ( );
    gluOrtho2D (left, right, bottom, top);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ( );
}


// Dynamic: Se modifica el escalado base con una sinusoidal
void DOTS_DoDynamic (TDots *dots, float t)
{
    color32 color, *p;
    int   w, h, i, j, a;
    float px, py, kx, ky;
    float dot_dx, dot_dy;
    float d;

    p = (color32 *) IMG_GetData (dots->img);
    w = IMG_GetWidth (dots->img);
    h = IMG_GetHeight (dots->img);

    dot_dx = dots->xscale * dots->wnd_width  / dots->num_xdots;
    dot_dy = dots->yscale * dots->wnd_height / dots->num_ydots;
    kx     = (float)dots->wnd_width  / (float)(w - 1.0f);
    ky     = (float)dots->wnd_height / (float)(h - 1.0f);

    py = dots->ypos + 0.5f * (dot_dy - dots->wnd_height);
    for (j=0; j<h; j++)
    {
        px = dots->xpos + 0.5f * (dot_dx - dots->wnd_width);
        for (i=0; i<w; i++)
        {
            color = *(p ++);
            if (color & 0xFF000000)
            {
                // NOTA: Para que el movimiento sea igual en todas las imagenes,
                // usar unas nuevas variables ii y jj que vayan siempre de 0 a 1
                // ii = i / w
                // jj = j / h

                // Escalado de cada dot
                // (Modificar la formula para distintos movimientos)
                //d = (float) (0.5f * cos ((t+i+j) / 31.0f) * sin ((t+2.0f*i*j)/59.0f) * sin ((t+2.0f*i*j)/59.0f) + 0.5f);
                d = (float) (0.5f * cos ((1.3*t+i+j) / 31.0f) * sin ((t+2.0f*i*j)/59.0f) * sin ((0.2*t+2.0f*i*j)/59.0f) + 0.5f);
                d *= 2.5f;

                // global alpha
                a = (color & 0xFF000000) >> 24;
                a = (a * dots->alpha) >> 8;
                color = (color & 0x00FFFFFF) | (a << 24);
                glColor4ubv ((unsigned char *) &color);

                // Se podria modificar tambien la posicion de cada dot
                // cambiando (px, py)
                DrawDot (px,
                         py,
                         dot_dx * d,
                         dot_dy * d);

            }

            px += kx;
        }

        py += ky;
    }
}

// Dynamic: Se modifica el escalado y posiciones base con una sinusoidal
void DOTS_DoDynamic2 (TDots *dots, float t)
{
    color32 color, *p;
    int   w, h, i, j, a;
    float px, py, kx, ky;
    float dot_dx, dot_dy;
    float d;

    p = (color32 *) IMG_GetData (dots->img);
    w = IMG_GetWidth (dots->img);
    h = IMG_GetHeight (dots->img);

    dot_dx = dots->xscale * dots->wnd_width  / dots->num_xdots;
    dot_dy = dots->yscale * dots->wnd_height / dots->num_ydots;
    kx     = (float)dots->wnd_width  / (float)(w - 1.0f);
    ky     = (float)dots->wnd_height / (float)(h - 1.0f);

    py = dots->ypos + 0.5f * (dot_dy - dots->wnd_height);
    for (j=0; j<h; j++)
    {
        px = dots->xpos + 0.5f * (dot_dx - dots->wnd_width);
        for (i=0; i<w; i++)
        {
            color = *(p ++);
            if (color & 0xFF000000)
            {
                // NOTA: Para que el movimiento sea igual en todas las imagenes,
                // usar unas nuevas variables ii y jj que vayan siempre de 0 a 1
                // ii = i / w
                // jj = j / h

                // Escalado de cada dot
                // (Modificar la formula para distintos movimientos)
                d = (float) (0.5f * cos ((2*t+i+j) / 31.0f) * sin ((2*t+2.0f*i*j)/59.0f) * sin ((2*t+2.0f*i*j)/59.0f) + 0.5f);
				d *= 2.5f;

                // global alpha
                a = (color & 0xFF000000) >> 24;
                a = (a * dots->alpha) >> 8;
                color = (color & 0x00FFFFFF) | (a << 24);
                glColor4ubv ((unsigned char *) &color);

                // Se podria modificar tambien la posicion de cada dot
                // cambiando (px, py)
                DrawDot (px+d*2,
                         py+d*2,
                         dot_dx * d,
                         dot_dy * d);

            }

            px += kx;
        }

        py += ky;
    }
}

void DOTS_DoRand (TDots *dots, float t)
{
    static unsigned int myseed = 0;
    color32 color, *p;
    int   w, h, i, j, a;
    float px, py, kx, ky;
    float dot_dx, dot_dy;
    float d, dx, dy;

    p = (color32 *) IMG_GetData (dots->img);
    w = IMG_GetWidth (dots->img);
    h = IMG_GetHeight (dots->img);

    dot_dx = dots->xscale * dots->wnd_width  / dots->num_xdots;
    dot_dy = dots->yscale * dots->wnd_height / dots->num_ydots;
    kx     = (float)dots->wnd_width  / (float)(w - 1.0f);
    ky     = (float)dots->wnd_height / (float)(h - 1.0f);

    py = dots->ypos + 0.5f * (dot_dy - dots->wnd_height);
    for (j=0; j<h; j++)
    {
        px = dots->xpos + 0.5f * (dot_dx - dots->wnd_width);
        for (i=0; i<w; i++)
        {
            color = *(p ++);
            if (color & 0xFF000000)
            {
                // Random
                myseed = 0x269EC3 + myseed * 0x343FD;
                dx = (float) (((myseed >> 16) & 0xFF) / 255.0f);

                // Random
                myseed = 0x269EC3 + myseed * 0x343FD;
                dy = (float) (((myseed >> 16) & 0xFF) / 255.0f);

                // global alpha
                a = (color & 0xFF000000) >> 24;
                a = (a * dots->alpha) >> 8;
                color = (color & 0x00FFFFFF) | (a << 24);

                glColor4ubv ((unsigned char *) &color);

                // Se podria modificar tambien la posicion de cada dot
                // cambiando (px, py)
                DrawDot (px + dx,
                         py + dy,
                         dot_dx,
                         dot_dy);

            }

            px += kx;
        }

        py += ky;
    }
}

void DOTS_DoStatic (TDots *dots, float t)
{
    color32 color, *p;
    int   i,j, w, h, a;
    float px, py, kx, ky;
    float dot_dx, dot_dy;

    p = (color32 *) IMG_GetData (dots->img);
    w = IMG_GetWidth (dots->img);
    h = IMG_GetHeight (dots->img);

    dot_dx = dots->xscale * dots->wnd_width  / (float)dots->num_xdots;
    dot_dy = dots->yscale * dots->wnd_height / (float)dots->num_ydots;

    kx = (float)dots->wnd_width  / (float)(w - 1.0f);
    ky = (float)dots->wnd_height / (float)(h - 1.0f);

    py = dots->ypos + 0.5f * (dot_dy - dots->wnd_height);
    for (j=0; j<h; j++)
    {
        px = dots->xpos + 0.5f * (dot_dx - dots->wnd_width);
        for (i=0; i<w; i++)
        {
            color = *(p ++);
            if (color & 0xFF000000)
            {
                // global alpha
                a = (color & 0xFF000000) >> 24;
                a = (a * dots->alpha) >> 8;
                color = (color & 0x00FFFFFF) | (a << 24);
                glColor4ubv ((unsigned char *) &color);

                DrawDot (px,
                         py,
                         dot_dx,
                         dot_dy);
            }

            px += kx;
        }

        py += ky;
    }
}


void DOTS_Do (TDots *dots, float t)
{
    GL_SetProjectionMatrix (0.0, 100.0, 100.0, 0.0);

    glEnable (GL_TEXTURE_2D);
    glEnable (GL_BLEND);


    // Set blend
    switch (dots->blend)
    {
    case DOTS_BLEND_NORMAL:
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;

    case DOTS_BLEND_ADD:
        glBlendFunc (GL_SRC_ALPHA, GL_ONE);
        break;
    }


    TEX_Bind (dots->texture);

    // Dibujar los dots dependiendo del modo
    switch (dots->mode)
    {
    case DOTS_STATIC:
        DOTS_DoStatic (dots, t);
        break;

    case DOTS_RAND:
        DOTS_DoRand (dots, t);
        break;

	case DOTS_DYNAMIC2:
	    DOTS_DoDynamic2 (dots, t);
        break;
        
    case DOTS_DYNAMIC:    
    default:
        DOTS_DoDynamic (dots, t);
        break;
    }

    glDisable (GL_TEXTURE_2D);
    glDisable (GL_BLEND);
}

void DOTS_Delete (TDots *dots)
{
    TEX_Delete (dots->texture);
    IMG_Delete (dots->img);
    free (dots);
}
