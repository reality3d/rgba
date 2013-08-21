///////////////////////////////////////////////////////////////////////
//
// IMAGE LIB v2.0
//
///////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include "image.h"

#ifdef LINUX
#define _stricmp strcasecmp
#endif

//
// INCLUIR CABECERAS NECESARIAS PARA LA LECTURA DE LOS FORMATOS GRAFICOS
//
#include "./img_png/img_png.h"
#include "./img_jpeg/img_jpeg.h"


//
// >> PROC__IMG_READ - Puntero a la rutina encargada de leer la imagen. Deve
//    reservar la memoria necesaria para los datos, y devolver una estructura
//    "image_t" valida. La destruccion de la imagen corre por parte del
//    usuario (IMG_Delete)
//
// >> PROC__IMG_GETERROR - Puntero a la rutina que, en caso de producirse
//    cualquier error, devuelve un string informando del error.
//
// >> A¤adir lo que sea necesario!
//
typedef image_t (*PROC__IMG_READ)     (char *);
typedef char *  (*PROC__IMG_GETERROR) (void);

typedef struct
{
    char                extension[8];   // File extension (INCLUYENDO EL PUNTO!)
    PROC__IMG_READ      img_read;       // Puntero a la rutina de lectura
    PROC__IMG_GETERROR  img_get_error;  // Puntero a la rutina de errir
} ImgFormatT;

// Image IO
TImageIO image_io =
{
    fopen,
    fread,
    fclose
};



//
// Rellenar con los formatos que se quieran leer
//
static ImgFormatT format[] =
{
    {".jpg",    JPEG_Read,     JPEG_GetError},
    {".png",    PNG_Read,      PNG_GetError}
};



//
// Variable que contendra el ultimo error producido
//
static char error_message[128];


char *IMG_GetError (void)
{
    return error_message;
}


image_t IMG_New (int width, int height, int bpp)
{
    image_t img;

    img = (image_t) malloc (sizeof (image_ptr_t));
    if (img)
    {
        img->data  = malloc (width * height * (bpp / 8));
        if (!img->data)
        {
            free (img);
            sprintf (error_message, "IMG_ERROR: Can't create image");
            return 0;
        }

        img->width          = width;
        img->height         = height;
        img->bits_per_pixel = bpp;
    }
    else
        sprintf (error_message, "IMG_ERROR: Can't create image");

    return img;
}


image_t IMG_Read (char *filename)
{
    ImgFormatT *img_mgr = NULL;
    char       *extension;
    image_t     img;
    int         i;

    // Buscamos el image manager para el formato que se quiere cargar
    for (i=0; i<sizeof(format)/sizeof(format[0]); i++)
    {
        extension = filename + strlen (filename) - strlen (format[i].extension);
        if (!_stricmp (extension, format[i].extension))
        {
            img_mgr = &format[i];
            break;
        }
    }

    if (!img_mgr)
    {
        sprintf (error_message, "IMG_ERROR: Can't find image manager for file %s", filename);
        return 0;
    }

    // Leemos la imagen y comprobamos errores
    img = img_mgr->img_read (filename);
    if (!img)
        strcpy (error_message, img_mgr->img_get_error ( ));

    return img;
}


int IMG_Delete (image_t img)
{
    assert (img != NULL);
    assert (img->data != NULL);

    free (img->data);
    free (img);

    return 1;
}


void IMG_SetIO (IO_Open open, IO_Read read, IO_Close close)
{
printf("IMG_SetIO CALLED\n");	
	
    image_io.open  = open;
    image_io.read  = read;
    image_io.close = close;
}



///////////////////////////////////////////////////////////////////////
//
// CONVERTERS
//
///////////////////////////////////////////////////////////////////////
int IMG_Convert24to32 (image_t img)
{
    int   pixels = img->width * img->height;
    unsigned char *dst;
    unsigned char *src = (unsigned char *) img->data;
    unsigned char *psrc = src;

    dst = (unsigned char *) malloc (img->width * img->height * 4);
    if (!dst)
    {
        sprintf (error_message, "IMG_ERROR: Can't alloc image buffer to convert");
        return 0;
    }

    img->bits_per_pixel = 32;
    img->data           = (void *) dst;

    while (pixels --)
    {
        *(dst + 0) = *(src + 2);
        *(dst + 1) = *(src + 1);
        *(dst + 2) = *(src + 0);
        *(dst + 3) = 0xFF;

        src += 3;
        dst += 4;
    }

    free (psrc);

    return 1;
}

// 16bpp GRAYSCALE + ALPHA => 32bpp BGRA
int IMG_Convert16to32 (image_t img)
{
    int   pixels = img->width * img->height;
    unsigned char *dst;
    unsigned char *src = (unsigned char *) img->data;
    unsigned char *psrc = src;

    dst = (unsigned char *) malloc (img->width * img->height * 4);
    if (!dst)
    {
        sprintf (error_message, "IMG_ERROR: Can't alloc image buffer to convert");
        return 0;
    }

    img->bits_per_pixel = 32;
    img->data           = (void *) dst;

    while (pixels --)
    {
        *(dst + 0) = *(src + 0);
        *(dst + 1) = *(src + 0);
        *(dst + 2) = *(src + 0);
        *(dst + 3) = *(src + 1);

        src += 2;
        dst += 4;
    }

    free (psrc);

    return 1;
}


int IMG_Convert (image_t img, int dst_bits_per_pixel)
{
    int ret = 0;

    // La imagen ya esta en el formato que queremos!
    if (img->bits_per_pixel == dst_bits_per_pixel)
        return 1;

    if ((img->bits_per_pixel == 24) && (dst_bits_per_pixel == 32))
        ret = IMG_Convert24to32 (img);
    if ((img->bits_per_pixel == 16) && (dst_bits_per_pixel == 32))
        ret = IMG_Convert24to32 (img);
    else
        sprintf (error_message, "IMG_ERROR: Can't find valid converter to %i -> %i", img->bits_per_pixel, dst_bits_per_pixel);

    return ret;
}




///////////////////////////////////////////////////////////////////////
//
// OBTENER DATOS DE LA IMAGEN
//
///////////////////////////////////////////////////////////////////////
int IMG_GetWidth (image_t img)
{
    return img->width;
}

int IMG_GetHeight (image_t img)
{
    return img->height;
}

int IMG_GetBPP (image_t img)
{
    return img->bits_per_pixel;
}

void *IMG_GetData (image_t img)
{
    return img->data;
}

int IMG_GetSize (image_t img)
{
    return img->width * img->height * img->bits_per_pixel / 8;
}
