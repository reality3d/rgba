#ifndef __FONTS_H__
#define __FONTS_H__

#ifdef WIN32
 #include <windows.h>
#endif
#include <GL/gl.h>

struct fonts_info
{
    unsigned short x0, y0, x1, y1;
};

#define FLAG_SOLID 0
#define FLAG_BLEND 1
#define FLAG_BLEND_MORE 2

typedef struct TFont_t
{
  GLuint _fonts_texture_id;      // Textura con las fuentes
  GLuint _list_base;             // Base del display list para los caracteres
  struct fonts_info *fonts_info; // Estructura con informacion sobre cada fuente
  int    _interspace;
  float x, y;
  char *texto;
  int flags;

  // A¤adido a peticion de Reality3D
  float alpha,delta_alpha;
  float delta_posx,delta_posy;
  float alphachange_timeout;
  long ultimo_tick;

  float scalex,scaley;             // Variacion lineal del escalado
  float delta_scalex,delta_scaley;

  
} TFont;

TFont* Fonts_New     (int interspace, int space, char *filename, char *filename_nfo);
void   Fonts_Delete  (TFont* self);
int    Fonts_Write   (TFont* self, char *text, float x, float y, int flags);
int    Fonts_GetWidth(TFont* self, char *text);

#endif // __FONTS_H__
