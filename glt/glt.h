///////////////////////////////////////////////////////////////////////
//
// GLT - OpenGL Texture
//
//  >> Image   : Read image formats (png and jpeg)
//  >> Texture : Setup gl texture
//  >> Cache   : Manage GL textures
//
///////////////////////////////////////////////////////////////////////
#ifndef __GLT_CONFIG__
#define __GLT_CONFIG__

//  GLT_DLL    GLT_EXPORT   |  Info
// ----------------------------------------------------------------
//  0          0            => Static library for compile and link
//  0          1            => Not defined
//  1          0            => Dynamic library for link (use)
//  1          1            => Dynamic library for compile

#ifdef GLT_DLL
 #ifdef GLT_EXPORT
  #define GLT_API __declspec(dllexport)
 #else
  #define GLT_API __declspec(dllimport)
 #endif
#else
 #define GLT_API
#endif

//#include "image.h"
#include "texture.h"
#include "cache.h"


#endif // __GLT_CONFIG__
