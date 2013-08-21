///////////////////////////////////////////////////////////////////////
// BLURRING TEXTURE :: epsilum
///////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <glt/glt.h>
#include "blurtext.h"

TBlurText *BT_New (char *texture)
{
    TBlurText *self;

    self = (TBlurText *) malloc (sizeof (TBlurText));
    assert (self != NULL);

    self->texture = CACHE_LoadTexture (texture, TEX_BUILD_MIPMAPS);
    assert (self->texture != NULL);
    TEX_Bind (self->texture);

    self->w    = self->texture->width;
    self->h    = self->texture->height;
    self->data_src = malloc (self->w * self->h * 4);
    self->data_dst = malloc (self->w * self->h * 4);
    //self->data_src = malloc (Epopeia_GetResX ( ) * Epopeia_GetResY ( ) * 4);
    //self->data_dst = malloc (Epopeia_GetResX ( ) * Epopeia_GetResY ( ) * 4);
    assert (self->data_src != NULL);
    assert (self->data_dst != NULL);

    glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, self->data_src);
    //glGetTexImage (GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, GL_UNSIGNED_BYTE, self->data_src);
    //glGetTexImage (GL_TEXTURE_2D, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, self->data_src);
    //glGetTexImage (GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, self->data_src);

    self->step  = 0;
    self->mode  = BLUR_ALL;
    self->level = 0.0f;

    self->it     = INTERP_New ( );

    return self;
}

#if defined(LINUX) || defined(MINGW32)
#define BLURMMX_Horizontal BLUR_Horizontal
#define BLURMMX_Horizontal2x BLUR_Horizontal2x
#define BLURMMX_Vertical BLUR_Vertical
#define BLURMMX_Vertical2x BLUR_Vertical2x
#define BLURMMX_All BLUR_All
#define BLURMMX_All2 BLUR_All2


static void BLUR_Horizontal (int w, int h, unsigned char *src, unsigned char *dst)
{
    int i;
    int bytes = 4 * w * h;

    for (i=4; i<bytes-4; i++)
        dst[i] = (src[i-4] + src[i+4]) >> 1;
}

static void BLUR_Horizontal2x (int w, int h, unsigned char *src, unsigned char *dst)
{
    int i;
    int bytes = 4 * w * h;

    for (i=8; i<bytes-8; i++)
        dst[i] = (src[i-8] + src[i-4] + src[i+4] + src[i+8]) >> 2;
}


static void BLUR_Vertical (int w, int h, unsigned char *src, unsigned char *dst)
{
    int i;
    int bytes = 4 * w * h;
    int pitch = 4 * w;

    for (i=pitch; i<bytes-pitch; i++)
        dst[i] = (src[i-pitch] + src[i+pitch]) >> 1;
}

static void BLUR_Vertical2x (int w, int h, unsigned char *src, unsigned char *dst)
{
    int i;
    int bytes = 4 * w * h;
    int pitch = 4 * w;

    for (i=2*pitch; i<bytes-2*pitch; i++)
        dst[i] = (src[i-2*pitch] + src[i+2*pitch] + src[i-pitch] + src[i+pitch]) >> 2;
}


static void BLUR_All (int w, int h, unsigned char *src, unsigned char *dst)
{
    int i;
    int bytes = 4 * w * h;
    int pitch = 4 * w;

    for (i=4+pitch; i<bytes-4-pitch; i++)
        dst[i] = (src[i+pitch] + src[i-pitch] + src[i-4] + src[i+4]) >> 2;
}

static void BLUR_All2 (int w, int h, unsigned char *src, unsigned char *dst)
{
    int i;
    int bytes = 4 * w * h;
    int pitch = 4 * w;

    for (i=4+pitch; i<bytes-4-pitch; i++)
        dst[i] = (src[i+pitch] + src[i-pitch] + src[i-8] + src[i+8]) >> 2;
}

#else //def LINUX


static void BLURMMX_Horizontal (int w, int h, unsigned char *src, unsigned char *dst)
{
    int pixels = w * h - 2;

    __asm
    {
        mov       esi, src       // esi = src
        mov       edi, dst       // edi = dst
        mov       ecx, pixels    // ecs = pixels
        add       esi, 4         // Skip first pixel
        add       edi, 4         // Skip first pixel
        pxor      mm7, mm7       // mm7 = 0
    next_pixel:
        movd      mm0, [esi-4]   // mm0 <- left pixel
        movd      mm1, [esi+4]   // mm1 <- right pixel
        punpcklbw mm0, mm7       // Unpack mm0 to words
        punpcklbw mm1, mm7       // Unpack mm1 to words
        paddusw   mm0, mm1       // Add mm1 to mm0
        psrlw     mm0, 1         // >> 1
        packuswb  mm0, mm0       // Pack to DWORD
        movd      [edi], mm0     // Save dst
        add       esi, 4
        add       edi, 4
        dec       ecx
        jnz       next_pixel
        emms
    }
}

static void BLURMMX_Horizontal2x (int w, int h, unsigned char *src, unsigned char *dst)
{
    int pixels = w * h - 4;

    __asm
    {
        mov       esi, src       // esi = src
        mov       edi, dst       // edi = dst
        mov       ecx, pixels    // ecs = pixels
        add       esi, 8         // Skip first pixel
        add       edi, 8         // Skip first pixel
        pxor      mm7, mm7       // mm7 = 0
    next_pixel:
        movd      mm0, [esi-8]   // mm0 <- leftmost pixel
        movd      mm1, [esi-4]   // mm1 <- left pixel
        movd      mm2, [esi+4]   // mm2 <- right pixel
        movd      mm3, [esi+8]   // mm3 <- rightmost pixel
        punpcklbw mm0, mm7       // Unpack mm0 to words
        punpcklbw mm1, mm7       // Unpack mm1 to words
        punpcklbw mm2, mm7       // Unpack mm2 to words
        punpcklbw mm3, mm7       // Unpack mm3 to words
        paddusw   mm0, mm1       // Add mm1 to mm0
        paddusw   mm0, mm2       // Add mm2 to mm0
        paddusw   mm0, mm3       // Add mm3 to mm0
        psrlw     mm0, 2         // >> 2
        packuswb  mm0, mm0       // Pack to DWORD
        movd      [edi], mm0     // Save dst
        add       esi, 4
        add       edi, 4
        dec       ecx
        jnz       next_pixel
        emms
    }
}


//
//    | ebx |        // esi = src + pitch
// ---|-----|---     // edi = dst
//    | edi |        // ebx = src - pitch
// ---|-----|---
//    | esi |
//
static void BLURMMX_Vertical (int w, int h, unsigned char *src, unsigned char *dst)
{
    int pitch  = 4 * w;
    int pitch2 = 2 * pitch;
    int pixels = w * h - 2 * pitch;

    __asm
    {
        mov       esi, src
        mov       ebx, esi
        mov       edi, dst
        mov       ecx, pixels
        add       esi, pitch2
        add       edi, pitch
        pxor      mm7, mm7
    next_pixel:
        movd      mm0, [ebx]
        movd      mm1, [esi]
        punpcklbw mm0, mm7
        punpcklbw mm1, mm7
        paddusw   mm0, mm1
        psrlw     mm0, 1
        packuswb  mm0, mm0
        movd      [edi], mm0
        add       ebx, 4
        add       esi, 4
        add       edi, 4
        dec       ecx
        jnz       next_pixel
        emms
    }
}

//    | eax |
// ---|-----|---     // eax = src - 2*pitch
//    | ebx |        // ebi = src -   pitch
// ---|-----|---     // edi = dst
//    | edi |        // edx = src +   pitch
// ---|-----|---     // esi = src + 2*pitch
//    | edx |
// ---|-----|---
//    | esi |
static void BLURMMX_Vertical2x (int w, int h, unsigned char *src, unsigned char *dst)
{
    int pitch  = 4 * w;
    int pitch2 = 2 * pitch;
    int pixels = w * h - 4 * pitch;

    __asm
    {
        mov       esi, src
        mov       eax, esi
        add       esi, pitch
        mov       ebx, esi
        add       esi, pitch2
        mov       edx, esi
        add       esi, pitch
        mov       edi, dst
        add       edi, pitch2
        mov       ecx, pixels
        pxor      mm7, mm7
    next_pixel:
        movd      mm0, [eax]
        movd      mm1, [ebx]
        movd      mm2, [edx]
        movd      mm3, [esi]
        punpcklbw mm0, mm7
        punpcklbw mm1, mm7
        punpcklbw mm2, mm7
        punpcklbw mm3, mm7
        paddusw   mm0, mm1
        paddusw   mm0, mm2
        paddusw   mm0, mm3
        psrlw     mm0, 2
        packuswb  mm0, mm0
        movd      [edi], mm0
        add       eax, 4
        add       ebx, 4
        add       edx, 4
        add       esi, 4
        add       edi, 4
        dec       ecx
        jnz       next_pixel
        emms
    }
}


//
//        |   eax   |              // esi = src
// -------|---------|-------       // edi = dst
//  esi-4 | esi/edi | esi+4        // eax = src - pitch  (pixel arriba)
// -------|---------|-------       // ebx = src + pitch  (pixel abajo)
//        |   ebx   |
//
static void BLURMMX_All (int w, int h, unsigned char *src, unsigned char *dst)
{
    int pitch  = 4 * w;
    int pitch2 = 2 * pitch;
    int pixels = w * h - 2 * w; // no hacemos la 1¦ y ultima fila

    __asm
    {
        mov esi, src
        mov eax, esi
        mov edi, dst
        add edi, pitch
        mov ecx, pixels
        add esi, pitch2
        mov ebx, esi
        sub esi, pitch

        pxor mm7, mm7        // mm7 = 0

    next_pixel:
        // Cogemos los 4 pixels
        movd mm0, [esi-4]
        movd mm1, [esi+4]
        movd mm2, [eax]
        movd mm3, [ebx]

        // Desempaquetamos en 4 words
        punpcklbw mm0, mm7
        punpcklbw mm1, mm7
        punpcklbw mm2, mm7
        punpcklbw mm3, mm7

        // Sumamos
        paddusw mm0, mm1
        paddusw mm0, mm2
        paddusw mm0, mm3

        // Dividimos entre 4
        psrlw mm0, 2

        // Empaquetamos el resultado en un DWORD y lo llevamos a edi
        packuswb mm0, mm0
        movd [edi], mm0

        add edi, 4
        add esi, 4
        add eax, 4
        add ebx, 4
        dec ecx
        jnz next_pixel
        emms
    }
}

//
//        |   eax   |              // esi = src
// -------|---------|-------       // edi = dst
//  esi-8 | esi/edi | esi+8        // eax = src - pitch  (pixel arriba)
// -------|---------|-------       // ebx = src + pitch  (pixel abajo)
//        |   ebx   |
//
static void BLURMMX_All2 (int w, int h, unsigned char *src, unsigned char *dst)
{
    int pitch  = 4 * w;
    int pitch2 = 2 * pitch;
    int pixels = w * h - 2 * w; // no hacemos la 1¦ y ultima fila

    __asm
    {
        mov esi, src
        mov eax, esi
        mov edi, dst
        add edi, pitch
        mov ecx, pixels
        add esi, pitch2
        mov ebx, esi
        sub esi, pitch

        pxor mm7, mm7        // mm7 = 0

    next_pixel:
        // Cogemos los 4 pixels
        movd mm0, [esi-8]
        movd mm1, [esi+8]
        movd mm2, [eax]
        movd mm3, [ebx]

        // Desempaquetamos en 4 words
        punpcklbw mm0, mm7
        punpcklbw mm1, mm7
        punpcklbw mm2, mm7
        punpcklbw mm3, mm7

        // Sumamos
        paddusw mm0, mm1
        paddusw mm0, mm2
        paddusw mm0, mm3

        // Dividimos entre 4
        psrlw mm0, 2

        // Empaquetamos el resultado en un DWORD y lo llevamos a edi
        packuswb mm0, mm0
        movd [edi], mm0

        add edi, 4
        add esi, 4
        add eax, 4
        add ebx, 4
        dec ecx
        jnz next_pixel
        emms
    }
}
#endif // LINUX

//
// Horizontal box blur
//
//  [1 ... 1 1 1 ... 1]  => box size = 2*d+1
//    \.../     \.../
//      d         d
//
static void BLUR_BoxHorizontal (unsigned char *src,
                                unsigned char *dst,
                                int w, int h, int d)
{
    int i, box = 2 * d + 1;  // box pixels
    unsigned short acc[4];   // rgba color accumulator
    unsigned char *src_l, *src_r;
    int cte_div = (int) (65536.0f / (float) box);

    // Caso basico
    if (d <= 0)
    {
        memcpy (dst, src, w * h * 4);
        return;
    }

    // Fill accumulator
    acc[0] = acc[1] = acc[2] = acc[3] = 0;
    for (i=0; i<4*box; i++)
        acc[i&3] += src[i];

    // Adjust pointers
    src_l = src;
    src_r = src + 4*box;
    dst  += 4 * (d + 1);

    //memset (src, 0xFF, 4*box);
    //memset (src, 0xFF, 4*box);

    // Pixels
    i = w * h - 4 * (box - d + 1);
#ifdef _MSC_VER
    __asm
    {
        mov  esi, src_l
        mov  ebx, src_r
        mov  edi, dst
        pxor mm7, mm7
        movq mm0, [acc]         // Load accumulator in mm0
        mov  ecx, i
    next_pixel:
        movd mm1, [esi]
        movd mm2, [ebx]
        punpcklbw mm1, mm7
        punpcklbw mm2, mm7
        psubusw mm0, mm1
        paddusw mm0, mm2
        movq [acc], mm0

        // Divide a
        mov  eax, DWORD PTR [acc+0]
        and  eax, 65535
        imul eax, cte_div
        sar  eax, 16
        mov  WORD PTR [acc+0], ax

        // Divide b
        mov  eax, DWORD PTR [acc+2]
        and  eax, 65535
        imul eax, cte_div
        sar  eax, 16
        mov  WORD PTR [acc+2], ax

        // Divide g
        mov  eax, DWORD PTR [acc+4]
        and  eax, 65535
        imul eax, cte_div
        sar  eax, 16
        mov  WORD PTR [acc+4], ax

        // Divide r
        mov  eax, DWORD PTR [acc+6]
        and  eax, 65535
        imul eax, cte_div
        sar  eax, 16
        mov  WORD PTR [acc+6], ax

        // Pack to dword
        movq mm1, [acc]
        packuswb mm1, mm7
        movd [edi], mm1

        // Next pixel
        add  esi, 4
        add  ebx, 4
        add  edi, 4
        dec  ecx
        jnz  next_pixel
        emms
    }
#else
    while (i --)
    {
        // Sub left pixel
        acc[0] -= src_l[0];
        acc[1] -= src_l[1];
        acc[2] -= src_l[2];
        acc[3] -= src_l[3];

        // Add right pixel
        acc[0] += src_r[0];
        acc[1] += src_r[1];
        acc[2] += src_r[2];
        acc[3] += src_r[3];

        // Average
        /*
        dst[0] = acc[0] / box;
        dst[1] = acc[1] / box;
        dst[2] = acc[2] / box;
        dst[3] = acc[3] / box;*/
        dst[0] = (acc[0] * cte_div) >> 16;
        dst[1] = (acc[1] * cte_div) >> 16;
        dst[2] = (acc[2] * cte_div) >> 16;
        dst[3] = (acc[3] * cte_div) >> 16;

        // Next pixel
        src_l += 4;
        src_r += 4;
        dst   += 4;
    }
#endif
}

/*
// h+v (working...)
void box_blur2 (unsigned char *src,
                unsigned char *dst,
                int w, int h, int d)
{
    int i, j, box = 2 * d + 1;   // box pixels
    unsigned short acc[4];      // rgba accumulator
    unsigned char  *src_l, *src_r;
    int div = (int) (65536.0f / (float) (box * box));

    // Fill accumulator
    acc[0] = acc[1] = acc[2] = acc[3] = 0;
    for (j=0; j<box; j++)
        for (i=0; i<4*box; i++)
        {
            acc[0] += src[i+4*w*j];
            acc[1] += src[i+4*w*j];
            acc[2] += src[i+4*w*j];
            acc[3] += src[i+4*w*j];
        }


    src_l = src;
    src_r = src + 4*box;
    dst  += 4 * w * (d + 1) + 4 * (d + 1);

    i = (w * h - 4 * w * (box*box + d + 1) - 4 * (box + d + 1)) - 4*w;
    while (i --)
    {
        // Sub left pixel
        for (j=0; j<4*box*w; j+=4*w)
        {
            acc[0] -= src_l[j+0];
            acc[1] -= src_l[j+1];
            acc[2] -= src_l[j+2];
            acc[3] -= src_l[j+3];
        }

        // Add right pixel
        for (j=0; j<4*box*w; j+=4*w)
        {
            acc[0] += src_r[j+0];
            acc[1] += src_r[j+1];
            acc[2] += src_r[j+2];
            acc[3] += src_r[j+3];
        }

        // Average
        dst[0] = (acc[0] * div) >> 16;
        dst[1] = (acc[1] * div) >> 16;
        dst[2] = (acc[2] * div) >> 16;
        dst[3] = (acc[3] * div) >> 16;

        // Next pixel
        src_l += 4;
        src_r += 4;
        dst   += 4;
    }
}
*/


void BT_Do (TBlurText *self)
{
    void *psrc, *pdst;

    if (self->step & 1)
    {
        psrc = self->data_dst;
        pdst = self->data_src;
    }
    else
    {
        psrc = self->data_src;
        pdst = self->data_dst;
    }

    self->step ++;

    glEnable (GL_TEXTURE_2D);
    switch (self->mode)
    {
    case BLUR_HORIZONTAL:
        BLURMMX_Horizontal (self->w, self->h, psrc, pdst);
        TEX_UpdateData (self->texture, pdst, TEX_BUILD_MIPMAPS);
        break;

    case BLUR_HORIZONTAL2X:
        BLURMMX_Horizontal2x (self->w, self->h, psrc, pdst);
        TEX_UpdateData (self->texture, pdst, TEX_BUILD_MIPMAPS);
        break;

    case BLUR_VERTICAL:
        BLURMMX_Vertical (self->w, self->h, psrc, pdst);
        TEX_UpdateData (self->texture, pdst, TEX_BUILD_MIPMAPS);
        break;

    case BLUR_VERTICAL2X:
        BLURMMX_Vertical2x (self->w, self->h, psrc, pdst);
        TEX_UpdateData (self->texture, pdst, TEX_BUILD_MIPMAPS);
        break;

    case BLUR_ALL:
        BLURMMX_All (self->w, self->h, psrc, pdst);
        TEX_UpdateData (self->texture, pdst, TEX_BUILD_MIPMAPS);
        break;

    case BLUR_ALL2:
        BLURMMX_All2 (self->w, self->h, psrc, pdst);
        TEX_UpdateData (self->texture, pdst, TEX_BUILD_MIPMAPS);
        break;

    case BLUR_HARDWARE:
        TEX_Bind (self->texture);
        // Set bias: 0..10
        glTexEnvf (GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, self->level / 10.0f);
        break;

    case BLUR_BOXHORIZONTAL:
        BLUR_BoxHorizontal (self->data_src, self->data_dst, self->w, self->h, (int)self->level);
        TEX_UpdateData (self->texture, self->data_dst, TEX_BUILD_MIPMAPS);
        break;

    default:
        BLUR_BoxHorizontal (self->data_src, self->data_dst, self->w, self->h, (int) (128.0f * (0.5f + 0.5f * cos (Epopeia_GetTimeMs ( ) / 360.0f))));
        TEX_UpdateData (self->texture, self->data_dst, TEX_BUILD_MIPMAPS);

        /*
        glDisable (GL_TEXTURE_2D);
        glDisable (GL_BLEND);
        glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
        glReadPixels (0, 0, Epopeia_GetResX ( ), Epopeia_GetResY ( ), GL_RGBA, GL_UNSIGNED_BYTE, psrc);
        BLUR_BoxHorizontal (psrc, pdst, Epopeia_GetResX ( ), Epopeia_GetResY ( ), (int) (512.0f * (0.5f + 0.5f * cos (Epopeia_GetTimeMs ( ) / 360.0f))));
        glDrawPixels (Epopeia_GetResX ( ), Epopeia_GetResY ( ), GL_RGBA, GL_UNSIGNED_BYTE, pdst);
        */
            break;
    }

    glDisable (GL_TEXTURE_2D);


    // TEST
/*    {
    glEnable (GL_TEXTURE_2D);
    TEX_Bind (self->texture);
    TEX_SetFilter (GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f (1.0f, 1.0f, 1.0f, 1.0f);

    // Set Matrix mode
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ( );
    gluOrtho2D (0, Epopeia_GetResX ( ), 0, Epopeia_GetResY ( ));
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ( );

    glBegin (GL_QUADS);
     glTexCoord2f (0, 0); glVertex2f (0.0,                  self->texture->height);
     glTexCoord2f (1, 0); glVertex2f (self->texture->width, self->texture->height);
     glTexCoord2f (1, 1); glVertex2f (self->texture->width, 0.0);
     glTexCoord2f (0, 1); glVertex2f (0.0,                  0.0);
    glEnd ( );

    glDisable (GL_BLEND);
    }*/
}

void BT_Delete (TBlurText *self)
{
    CACHE_FreeTexture (self->texture);
    free (self->data_src);
    free (self->data_dst);

    free (self);
}
