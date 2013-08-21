///////////////////////////////////////////////////////////////////////
//
// OPENGL TEXTURE
//
///////////////////////////////////////////////////////////////////////
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include "glt.h"


//
// Texture flags
//
#define TEX_BUILD_MIPMAPS 0x0001  // (1<<0)
#define TEX_SWAP_RB       0x0002  // (1<<1)
#define TEX_SET_BORDER    0x0004  // (1<<2)
#define TEX_CUBEMAP       0x0008  // (1<<3)
#define TEX_CUBEMAP_POSX  0x0010  // (1<<4)
#define TEX_CUBEMAP_NEGX  0x0020  // (1<<5)
#define TEX_CUBEMAP_POSY  0x0040  // (1<<6)
#define TEX_CUBEMAP_NEGY  0x0080  // (1<<7)
#define TEX_CUBEMAP_POSZ  0x0100  // (1<<8)
#define TEX_CUBEMAP_NEGZ  0x0200  // (1<<9)




typedef struct
{
    GLuint id;               // OpenGL texture id
    int    width, height;    // Texture resolution
    int    bits_per_pixel;   // 8/16/24/32

    GLenum format;           // GL texture format
    int    flags;            // Texture flags
} TTexture;


//
// Carga una textura directamente con sus datos
//
GLT_API TTexture *TEX_New (int width, int height, int bits_per_pixel, void *data,
                           int flags);

//
// Actualiza la textura (para usarlo con texturas dinamicas)
//
GLT_API int TEX_UpdateData (TTexture *texture, void *data, int flags);

//
// Carga una textura desde fichero
//
GLT_API TTexture *TEX_Load        (char *filename, int flags);
GLT_API TTexture *TEX_LoadCubeMap (char *filename, int flags);


//
// Elimina la textura
//
GLT_API void TEX_Delete (TTexture *texture);

//
// Establece la textura para usar en opengl
//
GLT_API void TEX_Bind (TTexture *texture);



///////////////////////////////////////////////////////////////////////
//
// PARAMETROS DE LA TEXTURA:
//
//    Se pueden usar estas rutinas para establecer los parametros de
// la textura, o bien usar directamente la API de OpenGL haciendo
// previamente un bind de la textura
//
///////////////////////////////////////////////////////////////////////


// filter:
//  - GL_NEAREST
//  - GL_LINEAR
//  - GL_NEAREST_MIPMAP_NEAREST
//  - GL_LINEAR_MIPMAP_NEAREST
//  - GL_NEAREST_MIPMAP_LINEAR
//  - GL_LINEAR_MIPMAP_LINEAR
GLT_API void TEX_SetFilter (GLenum min_filter, GLenum mag_filter);
GLT_API void TEX_SetCubeMapFilter (GLenum min_filter, GLenum mag_filter);


// wrap:
//  - GL_CLAMP
//  - GL_REPEAT
//  - GL_CLAMP_TO_EDGE
GLT_API void TEX_SetWrap (GLenum wrap_s, GLenum wrap_t);
GLT_API void TEX_SetCubeMapWrap (GLenum wrap_s, GLenum wrap_t, GLenum wrap_r);


// envi:
//   - GL_MOVULATE
//   - GL_DECAL
//   - GL_BLEND
GLT_API void TEX_SetEnvironment (GLint envi);



#ifdef __cplusplus
};
#endif

#endif // __TEXTURE_H__
