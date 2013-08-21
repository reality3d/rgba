///////////////////////////////////////////////////////////////////////
//
// JPEG Loader
//
///////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <setjmp.h>
#ifdef LINUX
#include <jpeglib.h>
#include <jerror.h>
#else
#include "jpeglib.h"
#include "jerror.h"
#endif
#include "jpeg_io.h"
#include "img_jpeg.h"
#include "../image.h"
#define DEBUG_VAR 0
#define DEBUG_MODULE "glt_cache"
#include <epopeia/debug.h>


static jmp_buf setjmp_buffer;

///////////////////////////////////////////////////////////////////////
// ERROR HANDLER
///////////////////////////////////////////////////////////////////////
static char _error_message[128];

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
    char tmp_message[128];
    DEBUG_msg("my_error_exit");
    (*cinfo->err->format_message) (cinfo, _error_message);

    sprintf (tmp_message, "JPEG_ERROR: %s", _error_message);
    DEBUG_msg(tmp_message);
    strcpy  (_error_message, tmp_message);
    longjmp (setjmp_buffer, 1);
}

char *JPEG_GetError (void)
{
    return _error_message;
}


///////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////
image_t JPEG_Read (char *filename)
{
    void    *infile;
    JSAMPLE *buffer;
    struct   jpeg_decompress_struct cinfo;
    struct   jpeg_error_mgr         jerr;
    image_t  image;
    unsigned char *pdata;
    int      pitch;
    DEBUG_msg("JPEG_Read");
    // Set our error routines
    cinfo.err = jpeg_std_error (&jerr);
    jerr.error_exit = my_error_exit;
    DEBUG_msg("Calling create decompress");
    jpeg_create_decompress (&cinfo);
    DEBUG_msg("Calling open");
    infile = image_io.open (filename, "rb");
    if (!infile)
    {
        sprintf (_error_message, "JPEG_ERROR: Can't open file %s", filename);
        return 0;
    }

    DEBUG_msg("IO init");
    // Use own file I/O handler
    JPEGIO_Init (&cinfo, infile);

    if (setjmp (setjmp_buffer))
    {
        // Si llegamos aqui, es porque ha habido un error en cualquier parte
        // de la descompresion del JPEG
        jpeg_destroy_decompress (&cinfo);
        image_io.close (infile);
        return 0;
    }

    DEBUG_msg("Read header");
    jpeg_read_header (&cinfo, TRUE);

    // Configure output image
    //cinfo.out_color_space = JCS_RGB; // RGB Image output
    cinfo.scale_num       = 1;       // No scale
    cinfo.scale_denom     = 1;

    jpeg_start_decompress (&cinfo);
    buffer = (JSAMPLE *) calloc (cinfo.output_width * cinfo.output_components, sizeof (JSAMPLE));
    if (!buffer)
    {
        sprintf (_error_message, "JPEG_ERROR: Can't alloc buffer");
        return 0;
    }

    // Alloc image
    image = IMG_New (cinfo.output_width, cinfo.output_height, cinfo.output_components * 8);
    if (!image)
    {
        sprintf (_error_message, "JPEG_ERROR: Can't create image");
        return 0;
    }

    pdata = (unsigned char *) IMG_GetData (image);
    pitch = IMG_GetWidth (image) * IMG_GetBPP (image) / 8;
    while ((int)cinfo.output_scanline < IMG_GetHeight (image))
    {
        jpeg_read_scanlines (&cinfo, &buffer, 1);
        memcpy (pdata + (cinfo.output_scanline - 1) * pitch, buffer, pitch);
    }

    free (buffer);
    jpeg_finish_decompress (&cinfo);
    jpeg_destroy_decompress (&cinfo);
    image_io.close (infile);

    return image;
}
