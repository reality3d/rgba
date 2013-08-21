///////////////////////////////////////////////////////////////////////
//
// IMAGE LIB v2.0
//
//  >> Link with /D_NDEBUG for NO DEBUG!!
//
///////////////////////////////////////////////////////////////////////
#ifndef __IMAGE_H__
#define __IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>


#ifdef IMG_DLL
 #ifdef IMG_EXPORT
  #define IMG_API __declspec(dllexport)
 #else
  #define IMG_API __declspec(dllimport)
 #endif
#else
 #define IMG_API
#endif


///////////////////////////////////////////////////////////////////////
// MACROS
// NOTA: Las dejo aqui por compatibilidad con versiones anteriores, pero
// no es una buena practica usar estas macros, mucho mejor las nuevas
// rutinas IMG_GetWidth(), IMG_GetHeight(), ...
///////////////////////////////////////////////////////////////////////
#define IMG_bpp(img)   img->bits_per_pixel
#define IMG_data(img)  img->data
#define IMG_w(img)     img->width
#define IMG_h(img)     img->height
#define IMG_size(img)  (img->width * img->height * img->bits_per_pixel / 8)


///////////////////////////////////////////////////////////////////////
// TYPES
///////////////////////////////////////////////////////////////////////
typedef struct
{
    int   width, height;
    int   bits_per_pixel;
    void *data;
} image_ptr_t;


typedef image_ptr_t * TImage;
typedef image_ptr_t * image_t;



///////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////

// RET: NULL  - Error
//      image - OK
IMG_API image_t IMG_New (int width, int height, int bpp);


// RET: NULL  - Error
//      image - OK
IMG_API image_t IMG_Read (char *filename);


// RET: 0 - Error
//      1 - OK
IMG_API int IMG_Delete (image_t img);



// RET: 0 - Error
//      1 - OK
//
// NOTA: Esta rutina modifica el puntero del campo "data" de la imagen!!!
int IMG_Convert (image_t img, int dst_bits_per_pixel);


// RET: Last error string
IMG_API char *IMG_GetError (void);



///////////////////////////////////////////////////////////////////////
// IMAGE I/O
///////////////////////////////////////////////////////////////////////
typedef void * (* IO_Open)  (const char *filename, const char *mode);
typedef size_t (* IO_Read)  (void *buffer, size_t size, size_t count, void *handle);
typedef int    (* IO_Close) (void *handle);

typedef struct
{
    IO_Open  open;
    IO_Read  read;
    IO_Close close;
} TImageIO;
extern TImageIO image_io;

IMG_API void IMG_SetIO (IO_Open open, IO_Read read, IO_Close close);


///////////////////////////////////////////////////////////////////////
// Rutinas para obtener informacion sobre la imagen
///////////////////////////////////////////////////////////////////////
IMG_API int   IMG_GetWidth  (image_t img);  // Width in pixels
IMG_API int   IMG_GetHeight (image_t img);  // Height in pixels
IMG_API int   IMG_GetBPP    (image_t img);  // Bits per pixel
IMG_API void *IMG_GetData   (image_t img);  // Pointer to data
IMG_API int   IMG_GetSize   (image_t img);  // Image size in bytes


#ifdef __cplusplus
};
#endif

#endif // __IMAGE_H__
