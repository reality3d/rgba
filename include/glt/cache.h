///////////////////////////////////////////////////////////////////////
//
// CACHE
//
///////////////////////////////////////////////////////////////////////
#ifndef __CACHE_H__
#define __CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include <windows.h>
//#include <GL/gl.h>
#include "glt.h"


// Maximo espacio reservado para texturas en la cache
#define CACHE_MAX_TEXTURES 256



//
// Constructor / Destructor
//
GLT_API void CACHE_Init (void);
GLT_API void CACHE_Kill (void);


//
// Manejar texturas (upload / download)
//

// RET: 1 => OK!
//      0 => No se ha podido insertar la textura (Cache llena o id invalido)
//
// NOTA: Una vez insertada la textura en la cache, para liberarla NO se
//       debe usar TEX_Delete(), sino CACHE_FreeTexture()
GLT_API int CACHE_InsertTexture (char *id, TTexture *texture);


// RET: NULL   => No se ha podido cargar la textura
//                Posibles razones: Cache llena, no se encuentra el fichero,
//                                  falta de memoria...
//      !=NULL => Puntero valido a una textura
GLT_API TTexture *CACHE_LoadTexture (char *id, int tex_flags);


//   Quita una referencia a la textura, y si no hay mas referencias, la
// borra de la cache.
GLT_API void      CACHE_FreeTexture (TTexture *texture);



#ifdef __cplusplus
};
#endif

#endif // __CACHE_H__
