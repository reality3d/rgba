//#define MEGADEBUG

#ifdef WINDOZE
 #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <string.h>

#ifdef WINDOZE
 #include <GL/glext.h>
#else
 #include <GL/glext.h>
 #include <ctype.h>
#endif
    
#include "textura.h"
#include "misc.h"

#define IMAGELIB_TEXTURES

#ifdef PNG_TEXTURES
 #include <img_png.h>
#endif

#ifdef IMAGELIB_TEXTURES
 #include <glt/glt.h>
#endif

int CacheTexturas::CargaTextura (char *filename)
{
#ifndef PCX_TEXTURES
 unsigned *buffer;
 int tamx,tamy;
#endif
#ifdef PNG_TEXTURES
 image_t textura;
#endif
#ifdef IMAGELIB_TEXTURES
 TTexture *textura;
#endif

 int i;
 int pos;
 int format;

 // Comprobamos si existe en la cach‚ una textura con ese nombre
 if(!ExisteTextura(filename,&pos))
 {
  NumeroTexturas++;

//        sprintf (FANTASY_DEBUG_STRING, "Cargando textura %s\n",filename);
//        Debug();
  Array[NumeroTexturas-1].textura = CACHE_LoadTexture (filename,TEX_BUILD_MIPMAPS);  
//        sprintf (FANTASY_DEBUG_STRING, "Cargado correctamente");
//        Debug();
  TEX_SetFilter ( GL_LINEAR_MIPMAP_NEAREST,GL_LINEAR);
  TEX_SetWrap (GL_REPEAT, GL_REPEAT);

  if (!Array[NumeroTexturas-1].textura)
  {
        sprintf (FANTASY_DEBUG_STRING, "Error al cargar la textura %s, le pongo la textura por defecto\n",filename);
        Debug(DEBUG_LEVEL_WARNING);
        strcpy(Array[NumeroTexturas-1].nombre_textura,"badtexture");
        Array[NumeroTexturas-1].numero_de_usos=0;
        return 0; // 0 es la textura por defecto
  }
  
/*  sprintf(FANTASY_DEBUG_STRING,"Name: %s, OpenGL id: %u, Size: %d %d, BPP: %d, flags: %d\n",filename,
  	  Array[NumeroTexturas-1].textura->id,
  	  Array[NumeroTexturas-1].textura->width,
  	  Array[NumeroTexturas-1].textura->height,
  	  Array[NumeroTexturas-1].textura->bits_per_pixel,
  	  Array[NumeroTexturas-1].textura->flags);
  Debug();
*/

  strcpy(Array[NumeroTexturas-1].nombre_textura,filename);

  Array[NumeroTexturas-1].numero_de_usos=1;
  return(NumeroTexturas-1);  
 }
 else
 {
     Array[pos].numero_de_usos++;
     return(pos); // en caso de existir ya se devuelve su posici¢n
 }
}


// Esta función es idéntica a la anterior, pero para cargar cubemaps


int CacheTexturas::CargaCubeMap (char *filename)
{
 int pos;
 
 // Comprobamos si existe en la cach‚ una textura con ese nombre

 // Comprobamos si existe en la cach‚ una textura con ese nombre

 if(!ExisteTextura(filename,&pos))
 {
  NumeroTexturas++;

  Array[NumeroTexturas-1].textura = CACHE_LoadTexture (filename,TEX_BUILD_MIPMAPS|TEX_CUBEMAP);
  TEX_SetCubeMapFilter (GL_LINEAR,GL_LINEAR_MIPMAP_NEAREST);
  TEX_SetCubeMapWrap (GL_REPEAT, GL_REPEAT,GL_REPEAT);

  if (!Array[NumeroTexturas-1].textura)
  {
        sprintf (FANTASY_DEBUG_STRING, "Error al cargar la textura %s, le pongo la textura por defecto\n",filename);
        Debug(DEBUG_LEVEL_WARNING);
        return 0; // 0 es la textura por defecto
  }
 

  strcpy(Array[NumeroTexturas-1].nombre_textura,filename);

  Array[NumeroTexturas-1].numero_de_usos=1;
  return(NumeroTexturas-1);  
 }
 else
 {
     Array[pos].numero_de_usos++;
     return(pos); // en caso de existir ya se devuelve su posici¢n
 }
}

// Esta funci¢n comprueba si dentro de la cache de texturas existe la
// textura con el nombre pasado

int CacheTexturas::ExisteTextura (char *nombre,int *pos)
{
 int i;
 for(i=0;i<NumeroTexturas;i++) {
  if(!(strcmp(Array[i].nombre_textura,nombre))) {
   *pos=i;
   return(1);
  }
 }
 return(0);
}

// Esta ser  la funci¢n que se llame para seleccionar la textura...

void CacheTexturas::SeleccionaTextura(char * nombre)
{
 int pos;
 
 if(ExisteTextura(nombre,&pos))
 {
  TEX_Bind (Array[pos].textura);
  TEX_SetFilter (GL_LINEAR_MIPMAP_NEAREST,GL_LINEAR);
  TEX_SetWrap (GL_REPEAT, GL_REPEAT);
 }
}

void CacheTexturas::SeleccionaTextura(int num_textura)
{	
#ifdef MEGADEBUG
   sprintf (FANTASY_DEBUG_STRING, "->Me piden seleccionar la textura %d\n",num_textura);
   Debug(DEBUG_LEVEL_OVERDEBUG);
#endif  
		
    if(num_textura < NumeroTexturas) 
    {
#ifdef MEGADEBUG
   sprintf (FANTASY_DEBUG_STRING, "->El puntero es %x\n",Array[num_textura].textura);
   Debug(DEBUG_LEVEL_OVERDEBUG);
#endif  
    	
    	
      TEX_Bind (Array[num_textura].textura);        
      TEX_SetFilter ( GL_LINEAR_MIPMAP_NEAREST,GL_LINEAR);
      TEX_SetWrap (GL_REPEAT, GL_REPEAT);
     }
}

void CacheTexturas::SeleccionaCubeMap(int num_textura)
{
    if(num_textura < NumeroTexturas)
    {
   	
      TEX_Bind (Array[num_textura].textura);        
      TEX_SetCubeMapFilter (GL_LINEAR,GL_LINEAR_MIPMAP_NEAREST);
      TEX_SetCubeMapWrap (GL_REPEAT, GL_REPEAT,GL_REPEAT);
     }	
}

void CacheTexturas::EliminaTextura(int num_textura)
{
    if((num_textura < NumeroTexturas) && (num_textura > 0) && (Array[num_textura].textura != NULL))
    {
        Array[num_textura].numero_de_usos--;
        if(Array[num_textura].numero_de_usos < 1)
            CACHE_FreeTexture(Array[num_textura].textura);
    }
}

int CacheTexturas::CreaTextura(char *nombre, int sizex, int sizey)
{
 int pos;
 int format;

 // Comprobamos si existe en la cach‚ una textura con ese nombre

 if(!ExisteTextura(nombre,&pos))
 {
  NumeroTexturas++;
  strcpy(Array[NumeroTexturas-1].nombre_textura,nombre);
  Array[NumeroTexturas-1].textura = TEX_New (sizex, sizey, 32, NULL,TEX_BUILD_MIPMAPS);  
  if(!Array[NumeroTexturas-1].textura) return -1;
  Array[NumeroTexturas-1].numero_de_usos=1;
  return(NumeroTexturas-1);  
 }
 else
 {
     Array[pos].numero_de_usos++;
     return(pos); // en caso de existir ya se devuelve su posici¢n
 }
}

CacheTexturas::CacheTexturas ()
{
 char *data;

      NumeroTexturas = 1;
      Array[0].nombre_textura[0]='X';
      Array[0].nombre_textura[1]='\0';
      Array[0].numero_de_usos=0;
      data=(char *)malloc(16*16*4);
      memset(data,255,16*16*4);
      Array[0].textura=TEX_New(16,16,32,data,0);      
      free(data);
}
