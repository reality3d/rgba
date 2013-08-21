///////////////////////////////////////////////////////////////////////
//
// RGBA PACKER
//
// >> Si el packer esta iniciado, primero prueba a leer del packer, y
//    si no lo encuentra, leera directamente de fichero.
//
//
///////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "unrarlib.h"
#include "packer.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "packer"
#include "../debug.h"

#define PACKER_PASSWORD "www.rgba.org"

static char *packer_filename = NULL;
static int   packer_is_init  = 0;
static ArchiveList_struct* packer_list_current = NULL;


///////////////////////////////////////////////////////////////////////
//
// PACKER LOCAL FUNCTIONS
//
///////////////////////////////////////////////////////////////////////

static PFILE *pak_open_from_file (const char *file, const char *mode)
{
    PFILE *pf = NULL;
    FILE  *fp;
    // Try read from file
//    fp = fopen (file, mode);
// Shapuza: si abrimos en ASCII, luego salen menos caracteres de la cuenta, así que abrimos siempre en binario
    fp = fopen (file, "rb");
    if (!fp)
        return 0;

    // Alloc PFILE structure
    pf = (PFILE *) malloc (sizeof (PFILE));
    if (!pf)
        return 0;

    memset (pf, 0, sizeof (PFILE));

    pf->eof = 0;
    // Copy file name
    pf->file_name = strdup (file);

    fseek (fp, 0, SEEK_END);
    pf->size     = ftell (fp);
    pf->position = 0;
    fseek (fp, 0, SEEK_SET);
    pf->data = malloc (pf->size);
    if (!pf->data)
        return 0;

    fread (pf->data, pf->size, 1, fp);
    fclose (fp);

    pf->mode |= PAK_FREE_DATA;

    return pf;
}


///////////////////////////////////////////////////////////////////////
//
// PACKER COMMON C I/O FUNCTIONS
//
///////////////////////////////////////////////////////////////////////

PACKER_API PFILE *pak_open (const char *file, const char *mode)
{
    PFILE *pf = NULL;
    const char *temp;
    char modo = 0; // 0 es ascii, 1 es binario

#ifdef LINUX
    char *tmpfile = strdup(file);
//    printf("Original: %s\t", file);
    file = tmpfile;
    while((tmpfile = strchr(tmpfile, '\\')))
        *tmpfile = '/';
//    printf("Modificado: %s\n", file);
#endif
    DEBUG_msg_str("pak_open init, mode", "kk");
    DEBUG_msg_str("filename", file);
    DEBUG_msg_hex("&mode", (int)mode);
    DEBUG_msg_hex("*mode", *mode);

    if (packer_is_init)
    {
        // Alloc PackerFile structure
        pf = (PFILE *) malloc (sizeof (PFILE));
	if (!pf)
	{
#ifdef LINUX
	    free(tmpfile);
#endif
	    return 0;
	}

	memset (pf, 0, sizeof (PFILE));

        // Read from packer
        if (urarlib_get (&pf->data, &pf->size, (char *)file, packer_filename, PACKER_PASSWORD))
        {
            pf->file_name = strdup (file);
            pf->position  = 0;
            pf->mode      |= PAK_FREE_DATA;
            pf->eof       = 0;
            DEBUG_msg("Abriendo desde PAK");
        }
        else
        {
            free (pf);
            pf = NULL;
            pf = pak_open_from_file (file, mode);
            DEBUG_msg("Abriendo desde fichero.");
        }
    }
    else
    {
        DEBUG_msg("Abriendo desde fichero.");
	pf = pak_open_from_file (file, mode);
    }
// Bug hunting number 1: si pf es NULL, porque no encuentra el fichero, aquí petamos!
// Solo añado el if(pf)
    if(pf)
    {
        DEBUG_msg("Abierto correctamente");

	pf->eof = 0;
	// Control de apertura del fichero como binario o ASCII
	temp = mode;
        DEBUG_msg("Ahora casca");
	while(*temp != '\0')
	{
	   DEBUG_msg("aqui no llega");

           if(*temp == 'b') modo=1;
           temp++;
	}
        DEBUG_msg("aqui no llega2");
	if(modo) pf->mode |=PAK_OPEN_BINARY;
	else pf->mode |=PAK_OPEN_ASCII;
        DEBUG_msg("Abierto correctamente - end");
    }

#ifdef LINUX
	    free(tmpfile);
#endif

    DEBUG_msg("pak_open end");

    return pf;
}

PACKER_API int pak_read (void *buffer, int size, int count, PFILE *pf)
{
    unsigned char *p;
    unsigned copy_count = 0;

    DEBUG_msg("pak_read init");


    if(pf->position >= pf->size)
    {
        pf->eof = 1;
        return 0;
    }

    p = (unsigned char *) pf->data;
    if(size*count <= (pf->size - pf->position))
        copy_count = size*count;
    else
    {
        count = (pf->size - pf->position)/size;
        copy_count = count*size;
    }
    memcpy (buffer, p + pf->position, copy_count);
    pf->position += size * count;

    DEBUG_msg("pak_read end");

    return count;
}

PACKER_API int pak_write (const void *buffer, int size, int count, PFILE *pf)
{
    DEBUG_msg("pak_write init");
    // Avoid warnings
    buffer = buffer;
    size   = size;
    count  = count;
    pf     = pf;

    DEBUG_msg("pak_write end");

    return count;
}

PACKER_API int pak_close (PFILE *pf)
{
    DEBUG_msg("pak_close init");
    if (pf->mode & PAK_FREE_DATA)
        free (pf->data);

    free (pf->file_name);
    free (pf);

    DEBUG_msg("pak_close end");

    return 0;
}

PACKER_API int pak_ftell (PFILE *pf)
{
    DEBUG_msg("pak_ftell init/end");
    return pf->position;
}

PACKER_API int pak_fseek (PFILE *pf, int offset, int origin)
{
    int err = 0;

    DEBUG_msg("pak_fseek init");

    switch (origin)
    {
    case SEEK_SET:
        pf->position = offset;
        break;

    case SEEK_END:
        pf->position = pf->size + offset;
        break;

    case SEEK_CUR:
        pf->position += offset;
        break;

    default:
        err = 1;
        break;
    }

    DEBUG_msg("pak_fseek end");
    return err;
}

PACKER_API int pak_feof(PFILE* pf)
{
    //    printf("feof: %d\n", pf->eof);
    DEBUG_msg("pak_feof init/end");

    return pf->eof;
}

PACKER_API int pak_fgetc(PFILE* pf)
{
    int c=0;

    if(pf->position >= pf->size)
    {
        pf->eof = 1;
//        printf("fgetc: EOF puesto a 1; devolviendo 0\n");
        return EOF;
    }

    if(!pak_read(&c, 1, 1, pf))
    {
///        printf("fgetc: fread devuelve 0, devolviendo 0\n");
        return EOF;
    }

    // Bug hunting number 2: si hemos abierto el fichero en ASCII, s¡ que
    // hay que leer, pero si el fichero es binario, no hace falta. Hay que
    // a¤adir cosillas a la estructura pf->mode
    
    if((c == 13) && (pf->mode & PAK_OPEN_ASCII)) //CR
        c = pak_fgetc(pf);

///    printf("fgetc: devolviendo %x '%c'\n", c, c);
    return c;
}


// Añadido por Utopian

PACKER_API int pak_fgets(char *cadena,int maxchars,PFILE *pf)
{
 int i;
 int c;

//printf("%d -> ",pf->position);

 for(i=0;i<maxchars;i++)
 {
  c = pak_fgetc(pf); 
  if( (pak_feof(pf)) || (c == '\0') || (c == EOF) )
  {
//   printf("EOF\n");
   cadena[i] = '\0';
   return i;
  }
  else if(c=='\n')
  {
   cadena[i]=c;
   cadena[i+1]='\0';
   return i+1;
  }
  else  cadena[i]=c; 
 }

 return i;
}


//
//
//
PACKER_API int pak_init (const char *file)
{
    packer_filename = strdup (file);
    packer_is_init = 1;
    return 1;
}


//
// Free all
//
PACKER_API int pak_kill (void)
{
    // Nothing to do
    if (packer_filename)
        free (packer_filename);

    return 1;
}

PACKER_API int pak_list_reset(void)
{
    int count;

    count = urarlib_list(packer_filename, &packer_list_current);
    if(count == 0)
	return 0;
    return 1;
}

PACKER_API char*pak_list_current_filename(void)
{
    assert(packer_list_current!=NULL);
    return packer_list_current->item.Name;
}

PACKER_API int pak_list_current_size(void)
{
    assert(packer_list_current!=NULL);
    return packer_list_current->item.UnpSize;
}

PACKER_API int pak_list_next(void)
{
    assert(packer_list_current != NULL);
    packer_list_current = packer_list_current->next;
    if(packer_list_current == NULL)
        return 0;
    else
        return 1;
}


