///////////////////////////////////////////////////////////////////////
//
// CACHE
//
///////////////////////////////////////////////////////////////////////
//#include <windows.h>
//#include <GL/gl.h>
#ifdef LINUX
#define _strdup strdup
#endif
#include <string.h>
#include <stdio.h>
#include "cache.h"
#include "texture.h"


typedef struct
{
    TTexture *texture;           // Textura
    char     *id;                // Identificador (Nombre de la textura)
    int       num_references;    // Numero de veces que se "comparte"
} TTextureCache;
static TTextureCache cache[CACHE_MAX_TEXTURES];



void CACHE_Init (void)
{
    int i;

    // Init cache
    for (i=0; i<CACHE_MAX_TEXTURES; i++)
    {
        cache[i].id             = NULL;
        cache[i].texture        = NULL;
        cache[i].num_references = 0;
    }
}

void CACHE_Kill (void)
{
    int i;

    for (i=0; i<CACHE_MAX_TEXTURES; i++)
    {
        if (cache[i].texture)
        {
            TEX_Delete (cache[i].texture);

            free (cache[i].id);
            cache[i].texture        = NULL;
            cache[i].id             = NULL;
            cache[i].num_references = 0;
        }
    }
}



int CACHE_InsertTexture (char *id, TTexture *texture)
{
    int i;

    // Primero vemos si existe el identificador en la cache
    for (i=0; i<CACHE_MAX_TEXTURES; i++)
    {
        if (cache[i].texture && (strcmp (cache[i].id, id) == 0))
        {
            // Fail!! El identificador ya existe en la cache
            return 0;
        }
    }

    // Add texture to cache
    for (i=0; i<CACHE_MAX_TEXTURES; i++)
    {
        if (cache[i].texture == NULL) // Empty slot
        {
            cache[i].texture        = texture;
            cache[i].id             = _strdup (id);
            cache[i].num_references = 1;
            break;
        }
    }

    // Cache llena!
    if (i == CACHE_MAX_TEXTURES)
        return 0;

    return 1;
}


TTexture *CACHE_LoadTexture (char *id, int flags)
{
    int i;
    TTexture *texture;

    // Primero vemos si existe la textura en la cache
    for (i=0; i<CACHE_MAX_TEXTURES; i++)
    {
        if (cache[i].texture && (strcmp (cache[i].id, id) == 0))
        {
            cache[i].num_references ++;
            return cache[i].texture;
        }
    }

    // Si no existe, la cargamos y la subimos a la tarjeta
    texture = TEX_Load (id, flags);
    if (!texture)
    {
        return NULL;
    }
    // Add texture to cache
    for (i=0; i<CACHE_MAX_TEXTURES; i++)
    {
        if (cache[i].texture == NULL) // Empty slot
        {
            cache[i].texture        = texture;
            cache[i].id             = _strdup (id);
            cache[i].num_references = 1;
            break;
        }
    }

    // Cache llena!
    if (i == CACHE_MAX_TEXTURES)
    {
        TEX_Delete (texture);
        return 0;
    }

    return texture;
}


void CACHE_FreeTexture (TTexture *texture)
{
    int i;

    for (i=0; i<CACHE_MAX_TEXTURES; i++)
    {
        if (cache[i].texture == texture)
        {
            // Quitamos una referencia
            cache[i].num_references --;

            // Si no quedan referencias, borramos la textura de la cache
            if (cache[i].num_references == 0)
            {
                TEX_Delete (cache[i].texture);
                free (cache[i].id);
                cache[i].id       = NULL;
                cache[i].texture  = NULL;
            }

            break;
        }
    }
}
