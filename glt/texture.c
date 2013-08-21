#ifdef WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "imagelib/image.h"
#include "texture.h"

///////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////

static int glExtSupported (const char *extension)
{
    char *string;
    const GLubyte *gl_extensions;

    gl_extensions = glGetString (GL_EXTENSIONS);

    // Metodo rapido para comprobar si esta soportada la extension,
    // pero no es 100% seguro!! ya se modificara en un futuro :)
    string = strstr ((const char *)gl_extensions, extension);

    return (string != NULL);
}



// Devuelve un formato de textura de opengl dado los bpp o 0 si hay error.
// Si "swap_rb" esta a 1, en los modos de 24 y 32 bpp se intercambian
// las componenres red y blue.
static GLenum GetBaseFormat (int bits_per_pixel, int flags)
{
    if(flags & TEX_DEPTH_COMPONENT) return GL_DEPTH_COMPONENT;	// Quick exit for depth textures
	
    switch (bits_per_pixel)
    {
    case 8:
        return GL_LUMINANCE;
        break;

    case 16:
        return GL_LUMINANCE_ALPHA;
        break;

    case 24:
        return (flags & TEX_SWAP_RB) ? GL_BGR_EXT : GL_RGB;
        break;

    case 32:
        return (flags & TEX_SWAP_RB) ? GL_BGRA_EXT : GL_RGBA;
        break;
    }

    // Can't find valid format
    return 0;
}



static GLenum GetInternalFormat (int bits_per_pixel)
{
    switch (bits_per_pixel)
    {
    case 8:  return GL_LUMINANCE8;
    case 16: return GL_LUMINANCE8_ALPHA8;
    case 24: return GL_RGB8;
    case 32: return GL_RGBA8;
    }

    return 0;
}

// Select texture target
static GLenum GetTarget (int flags)
{
    if (flags & TEX_CUBEMAP_POSX)
        return GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
    else if (flags & TEX_CUBEMAP_NEGX)
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
    else if (flags & TEX_CUBEMAP_POSY)
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
    else if (flags & TEX_CUBEMAP_NEGY)
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
    else if (flags & TEX_CUBEMAP_POSZ)
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
    else if (flags & TEX_CUBEMAP_NEGZ)
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
    else
        return GL_TEXTURE_2D;
}


///////////////////////////////////////////////////////////////////////
// PUBLIC
///////////////////////////////////////////////////////////////////////

int TEX_UpdateDataInitial (TTexture *texture, void *data, int flags)
{
    GLenum target = GL_TEXTURE_2D;  // Default texture target

//    assert (data != NULL);

    // Find GL valid texture format
    texture->format = GetBaseFormat (texture->bits_per_pixel, flags);
           
    if (texture->format == 0)
    {
        fprintf(stderr, "No valid format at TEX_UpdateData1\n");
        return 0; // No valid format!
    }
    if (flags & TEX_CUBEMAP)
        target = GetTarget (flags);

    // Bind texture
    glBindTexture (target, texture->id);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);


    // If this is a shadow texture, again we follow a specific path...
    if((flags & TEX_DEPTH_COMPONENT) && glExtSupported("GL_ARB_depth_texture"))
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
      		   texture->width,
      		   texture->height,
      		   0,
      		   GL_DEPTH_COMPONENT,
      		   GL_UNSIGNED_BYTE,
      		   NULL);	      		   
    }
    // Build mipmaps?
    else if (flags & TEX_BUILD_MIPMAPS)
    {
        if (glExtSupported ("GL_SGIS_generate_mipmap"))
        {
            glTexParameteri (target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
                    glTexImage2D (target,
                      0,
                      GetInternalFormat (texture->bits_per_pixel), //texture->bits_per_pixel/8,
                      texture->width,
                      texture->height,
                      (flags & TEX_SET_BORDER) ? 1 : 0,
                      texture->format,
                      GL_UNSIGNED_BYTE,
                      data);
        }
        else
            gluBuild2DMipmaps (target,
                               GetInternalFormat (texture->bits_per_pixel),
                               texture->width,
                               texture->height,
                               texture->format,
                               GL_UNSIGNED_BYTE,
                               data);
    }
    else
    {
        // Upload normal data (no mipmaps)
        glTexParameteri (target, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);
        glTexImage2D (target,
                      0,
                      GetInternalFormat (texture->bits_per_pixel), //texture->bits_per_pixel/8,
                      texture->width,
                      texture->height,
                      (flags & TEX_SET_BORDER) ? 1 : 0,
                      texture->format,
                      GL_UNSIGNED_BYTE,
                      data);
    }

    // Update texture flags
    texture->flags = flags;

    return 1;
}

int TEX_UpdateData (TTexture *texture, void *data, int flags)
{
    GLenum target = GL_TEXTURE_2D;  // Default texture target

    assert (data != NULL);

    // Find GL valid texture format
    texture->format = GetBaseFormat (texture->bits_per_pixel, flags);
    if (texture->format == 0)
        return 0; // No valid format!

    if (flags & TEX_CUBEMAP)
        target = GetTarget (flags);

    // Bind texture
    glBindTexture (target, texture->id);
    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

    // Build mipmaps?
    if (flags & TEX_BUILD_MIPMAPS)
    {
        if (glExtSupported ("GL_SGIS_generate_mipmap"))
        {
            glTexParameteri (target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
            
            glTexSubImage2D(target,
            		    0,
            		    0,0,
                      texture->width,
                      texture->height,
                      texture->format,
                      GL_UNSIGNED_BYTE,
                      data);            		    
                    
        }
        else
            gluBuild2DMipmaps (target,
                               GetInternalFormat (texture->bits_per_pixel),
                               texture->width,
                               texture->height,
                               texture->format,
                               GL_UNSIGNED_BYTE,
                               data);
    }
    else
    {
        // Upload normal data (no mipmaps)
            glTexSubImage2D(target,
            		    0,
            		    0,0,
                      texture->width,
                      texture->height,
                      texture->format,
                      GL_UNSIGNED_BYTE,
                      data);            		    
            }

    // Update texture flags
    texture->flags = flags;

    return 1;
}



TTexture *TEX_New (int width, int height, int bits_per_pixel, void *data,
                   int flags)
{
    int       data_initialized = 0;
    TTexture *texture;

    // Comprobamos que no sea un CubeMap
    if (flags & TEX_CUBEMAP)
        return 0;

    texture = (TTexture *) malloc (sizeof (TTexture));
    if (!texture)
        return 0;

    glGenTextures (1, &texture->id);
    texture->width          = width;
    texture->height         = height;
    texture->bits_per_pixel = bits_per_pixel;

    // NULL Data (init render to texture)
    /*if (!data)
    {
        data = malloc (width * height * (bits_per_pixel / 8));
        if (!data)
        {
            glDeleteTextures (1, &texture->id);
            free (texture);
            return 0;
        }

        memset (data, 0, width * height * (bits_per_pixel / 8));

        data_initialized = 1;
    }*/

    // Upload
    if (!TEX_UpdateDataInitial (texture, data, flags))
    {
        glDeleteTextures (1, &texture->id);
        free (texture);
        return 0;
    }

    if (data_initialized)

        free (data);

    return texture;
}


TTexture *TEX_LoadCubeMap (char *filename, int flags)
{
    TTexture    *texture;
    static char *suffix[6] = {
        "_posx",
        "_negx",
        "_posy",
        "_negy",
        "_posz",
        "_negz"
    };
    /*static GLuint cube_faces[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
    };*/
    static int cube_faces[6] = {
	TEX_CUBEMAP_POSX,
	TEX_CUBEMAP_NEGX,
	TEX_CUBEMAP_POSY,
	TEX_CUBEMAP_NEGY,
	TEX_CUBEMAP_POSZ,
	TEX_CUBEMAP_NEGZ
     };

    char        *marker;
    char         cubename[256];
    int          i;
    image_t      image;

    // Alloc texture
    texture = (TTexture *) malloc (sizeof (TTexture));
    if (!texture)
        return 0;

    // Obtenemos la extension del fichero base junto con el punto
    marker = &(filename[strlen(filename)-1]);
    while ((*marker != '.') && (marker > filename))
        marker --;

    // Check CubeMap extension
    if (!glExtSupported ("GL_ARB_texture_cube_map"))
    {
        // Si no se soporta la extension, tratamos de cargar la
        // primera textura del cubemap (algo es algo, no?) ;)
        strncpy (cubename, filename, marker - filename);
        cubename[marker - filename] = '\0';
        strcat (cubename, suffix[0]);
        strcat (cubename, marker);

        return TEX_Load (cubename, flags & (~TEX_CUBEMAP));
    }

    // Creamos la textura
    glGenTextures (1, &texture->id);
    glBindTexture (GL_TEXTURE_CUBE_MAP_ARB, texture->id);

    // Cargamos las 6 texturas
    for (i=0; i<6; i++)
    {
        strncpy (cubename, filename, marker - filename);
        cubename[marker - filename] = '\0';
        strcat (cubename, suffix[i]);
        strcat (cubename, marker);

        //printf ("Loading cubemap file: %s\n", cubename);
        image = IMG_Read (cubename);
        if (!image)
        {
            TEX_Delete (texture);
            return 0;
        }

        texture->width          = IMG_GetWidth (image);
        texture->height         = IMG_GetHeight (image);
        texture->bits_per_pixel = IMG_GetBPP (image);

        // Upload cubemap texture
        if (!TEX_UpdateDataInitial (texture, image->data, flags | cube_faces[i]))
        {
            TEX_Delete (texture);
            return 0;
        }

        IMG_Delete (image);
    }

    return texture;
}


TTexture *TEX_Load (char *filename, int flags)
{
    TTexture *texture;
    image_t  image;

    if (flags & TEX_CUBEMAP)
        return TEX_LoadCubeMap (filename, flags);

    image = IMG_Read (filename);
    if (!image)
        return NULL;

    texture = TEX_New (image->width,
                       image->height,
                       image->bits_per_pixel,
                       image->data,
                       flags);
    if (!texture)
        return NULL;

    IMG_Delete (image);

    return texture;
}

void TEX_Delete (TTexture *texture)
{
    glDeleteTextures (1, &texture->id);
    free (texture);
}

void TEX_Bind (TTexture *texture)
{
    if (texture->flags & TEX_CUBEMAP)
        glBindTexture (GL_TEXTURE_CUBE_MAP_ARB, texture->id);
    else
        glBindTexture (GL_TEXTURE_2D, texture->id);
}



///////////////////////////////////////////////////////////////////////
//
// GL TEXTURE PARAMETERS
//
///////////////////////////////////////////////////////////////////////

void TEX_SetFilter (GLenum min_filter, GLenum mag_filter)
{
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
}

void TEX_SetWrap (GLenum wrap_s, GLenum wrap_t)
{
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
}

void TEX_SetEnvironment (GLint envi)
{
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, envi);
}

void TEX_SetCubeMapFilter (GLenum min_filter, GLenum mag_filter)
{
    glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, mag_filter);
}

void TEX_SetCubeMapWrap (GLenum wrap_s, GLenum wrap_t, GLenum wrap_r)
{
    glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, wrap_r);
}
