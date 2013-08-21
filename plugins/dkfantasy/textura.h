#ifndef TEXTURA_H
#define TEXTURA_H

#include <stdlib.h>
#include <glt/glt.h>

#define NUM_MAX_TEXTURAS 256

class Textura
{
  
public:
    char nombre_textura[256];
    TTexture *textura;
    int numero_de_usos;
    Textura ()
  {
  };
  
    ~Textura ()
  {

  };
  
};


class CacheTexturas
{
  
public:
    Textura Array[NUM_MAX_TEXTURAS];
  
    int NumeroTexturas;
    int mipmapping;
    int multitexturing;
    int number_of_texture_units;
  
    int CargaTextura(char *filename);
    int CargaCubeMap(char *filename);
    int ExisteTextura(char *nombre, int *pos);
    void SeleccionaTextura(char * nombre);
    void SeleccionaTextura(int num_textura);
    void SeleccionaCubeMap(int num_textura);
    void EliminaTextura(int num_textura);
    int CreaTextura(char *nombre, int sizex, int sizey);
    
  CacheTexturas ()
  {
      mipmapping = 0;
      NumeroTexturas = 1;
      multitexturing = 0;
      number_of_texture_units = 1;
      Array[0].nombre_textura[0]='\0';
      Array[0].numero_de_usos=0;
      Array[0].textura=TEX_New(16,16,24,NULL,0);
  };
  
};


extern CacheTexturas ConjuntoTexturas;

// Las siguientes extensiones no están definidas en el fichero gl.h por defecto...

#ifndef GL_SGIS_generate_mipmap
#define GL_GENERATE_MIPMAP_SGIS           0x8191
#define GL_GENERATE_MIPMAP_HINT_SGIS      0x8192
#endif



#endif  
