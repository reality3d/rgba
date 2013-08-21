///////////////////////////////////////////////////////////////////////
//
// PNG Image Loader
//
///////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <png.h>
#include "image.h"
#include <assert.h>
#define PNG_HEADER_SIZE 8


static char _error_msg[256];

static void PNG_Error (png_structp png_ptr, png_const_charp msg)
{
    // Avoid warnings
    png_ptr = png_ptr;

    fprintf (stderr, "PNG_ERROR: %s\n", msg);
    exit (1);
}

char *PNG_GetError (void)
{
    return _error_msg;
}


static void read_data (png_structp png_ptr, png_bytep data, png_size_t length)
{
    void *handle;

    handle = png_ptr->io_ptr;
    image_io.read (data, 1, length, handle);
}


// 1 - OK
// 0 - Error
image_t PNG_Read (char *file_name)
{
    void        *fp;
    unsigned char header[PNG_HEADER_SIZE];
    int         color_type, bit_depth;
    double      dGamma;
    png_uint_32 width, height;
    png_uint_32 channels;
    png_uint_32 row_bytes;
    png_byte    *data;
    png_byte    **row_pointers = NULL;

    image_t     png_image;
    int         img_bpp, i;


    png_structp png_ptr  = NULL;
    png_infop   info_ptr = NULL;

    // Open file (read - binary)
    fp = image_io.open (file_name, "rb");
    if (!fp)
    {
        sprintf (_error_msg, "PNG Error: Can't open file <%s>", file_name);
        return 0;
    }

    // Check header (PNG Signature)
    image_io.read (header, 1, PNG_HEADER_SIZE, fp);
    if (!png_check_sig (header, PNG_HEADER_SIZE))
    {
        sprintf (_error_msg, "PNG Error: Isn't a png file");
        image_io.close (fp);
        return 0;
    }

    // Create the two PNG(-info) structures
    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, (png_error_ptr)PNG_Error, (png_error_ptr)NULL);
    if (!png_ptr)
    {
        sprintf (_error_msg, "PNG Error: Can't create read struct");
        image_io.close (fp);
        return 0;
    }

    info_ptr = png_create_info_struct (png_ptr);
    if (!info_ptr)
    {
        sprintf (_error_msg, "PNG Error: Can't create info struct");
        png_destroy_read_struct (&png_ptr, NULL, NULL);
        image_io.close (fp);
        return 0;
    }

    // Init the png structure
    //png_init_io (png_ptr, fp);
    png_set_read_fn (png_ptr, fp, read_data);
    png_set_sig_bytes (png_ptr, PNG_HEADER_SIZE);

    // Read all PNG info up to image data
    png_read_info (png_ptr, info_ptr);

    // Get width, height, bit-depth and color-type
    png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth,
                  &color_type, NULL, NULL, NULL);

    // Obtain valid color type - bpp
    switch (color_type)
    {
    case PNG_COLOR_TYPE_GRAY:
        img_bpp = 8;
        break;

    case PNG_COLOR_TYPE_GRAY_ALPHA:
        img_bpp = 16;
        break;

    case PNG_COLOR_TYPE_RGB:
    case PNG_COLOR_TYPE_PALETTE: // Si hay paleta, convertimos a 24bpp
        img_bpp = 24;
        break;

    case PNG_COLOR_TYPE_RGBA:
        img_bpp = 32;
        break;

    default:
        sprintf (_error_msg, "PNG Error: Unknown color type 0x%X", color_type);
        return 0;
        break;
    }

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand (png_ptr);
    if ((color_type == PNG_COLOR_TYPE_GRAY) && (bit_depth < 8))
        png_set_expand (png_ptr);
    if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand (png_ptr);
    //if (color_type == PNG_COLOR_TYPE_RGBA)
    //    png_set_bgr (png_ptr);


    // After the transformations have been registered update info_ptr data
    png_read_update_info (png_ptr, info_ptr);

    // Get again width, height and the new bit-depth and color-type
    png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth,
                  &color_type, NULL, NULL, NULL);

    // Make the new image
    png_image = IMG_New (width, height, img_bpp);

    // Allocate memory for an array of row-pointers
    row_pointers = (png_bytepp) malloc (height * sizeof (png_bytep));
    if (!row_pointers)
    {
        sprintf (_error_msg, "PNG Error: Out of memory");
        return 0;
    }

    // row_bytes is the width x number of channels
    row_bytes = png_get_rowbytes (png_ptr, info_ptr);
    //channels  = png_get_channels (png_ptr, info_ptr);

    // Set the individual row-pointers to point at the correct offsets
    data = (png_byte *) IMG_GetData (png_image);
    for (i=0; i<height; i++)
        row_pointers[i] = (png_bytep) (data + i * row_bytes);

    // Now we can go ahead and just read the whole image
    png_read_image (png_ptr, row_pointers);

    // Read the additional chunks in the PNG file (not really needed)
    png_read_end (png_ptr, NULL);

    free (row_pointers);

    png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
    image_io.close (fp);

    return png_image;
}
