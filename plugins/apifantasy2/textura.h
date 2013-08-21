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
private:
    Textura Array[NUM_MAX_TEXTURAS];
public:    
  
    int NumeroTexturas;
  
    int CargaTextura(char *filename);
    int CargaCubeMap(char *filename);
    int ExisteTextura(char *nombre, int *pos);
    void SeleccionaTextura(char * nombre);
    void SeleccionaTextura(int num_textura);
    void SeleccionaCubeMap(int num_textura);
    void EliminaTextura(int num_textura);
    int CreaTextura(char *nombre, int sizex, int sizey);
    CacheTexturas();    
  
};
#endif  
