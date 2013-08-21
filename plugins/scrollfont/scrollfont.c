// --------------------------------------------------------------------
// SCROLL . epsilum . March 2003
// --------------------------------------------------------------------
#ifdef WIN32
 #include <windows.h>
#endif
#ifdef LINUX
 #include <SDL/SDL.h>
#endif

#include <assert.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glt/glt.h>
#include <math.h>
#include <malloc.h>
#include <packer/packer.h>

#define fclose pak_close
#define fopen  pak_open
#define fwrite pak_write
#define fread  pak_read
#define ftell  pak_ftell
#define fseek  pak_fseek
#undef feof
#define feof   pak_feof
#define fgetc  pak_fgetc
#define fgets  pak_fgets
#define FILE   PFILE

#include "scrollfont.h"

/* Constantes para el scroll */
#define MAX_AVANCE    15    // Limite de avance
#define AVANCE_NORMAL (0.8f*60*elapsed)  // Avance para los cursores
#define AVANCE_RAPIDO (8.0f*60*elapsed)  // Avance para PG_UP y PG_DW
#define FRENO         (0.5f*60*elapsed)  // Decremento que se aplica cada paso


TScroll *SCROLLFont_New (char *text1,char *text2, char *text3,int interspace, int space, char *filename, char *filename_nfo)
{
    TScroll *self;
    FILE *p;
    int i,nlines;
    char tmplinea[256];

    self = (TScroll *) malloc (sizeof (TScroll));
    assert (self != NULL);
    self->font=Fonts_New(interspace, space, filename, filename_nfo);
    assert (self->font != NULL);
     
    // Primer texto 
        
    p=fopen(text1,"r");
    nlines=0; // contamos el número de líneas del fichero
    while(!feof(p))
    {
    	fgets(tmplinea,255,p);
    	nlines++;
    	if(tmplinea[0]==NULL) break;
    }
    // Ahora rebobinamos y nos las leemos todas desde el principio    
    fseek(p,0,SEEK_SET);
    self->texto[0]=(char **)malloc(nlines*sizeof(char*));
    self->lineas_texto[0]=nlines;
    for(i=0;i<nlines;i++)
    {
     self->texto[0][i]=(char *)malloc(256);
     fgets(self->texto[0][i],255,p);     
    }
    fclose(p);
    self->max_size[0]=0;
    for(i=0;i<nlines;i++)
     self->max_size[0]+=Fonts_GetHeight(self->font,self->texto[0][i]);
     
    // Segundo texto 
        
    p=fopen(text2,"r");
    nlines=0; // contamos el número de líneas del fichero
    while(!feof(p))
    {
    	fgets(tmplinea,255,p);
    	nlines++;
    	if(tmplinea[0]==NULL) break;
    }
    // Ahora rebobinamos y nos las leemos todas desde el principio    
    fseek(p,0,SEEK_SET);
    self->texto[1]=(char **)malloc(nlines*sizeof(char*));
    self->lineas_texto[1]=nlines;
    for(i=0;i<nlines;i++)
    {
     self->texto[1][i]=(char *)malloc(256);
     fgets(self->texto[1][i],255,p);     
    }
    fclose(p);
    self->max_size[1]=0;
    for(i=0;i<nlines;i++)
     self->max_size[1]+=Fonts_GetHeight(self->font,self->texto[1][i]);
     
    // Tercer texto 
        
    p=fopen(text3,"r");
    nlines=0; // contamos el número de líneas del fichero
    while(!feof(p))
    {
    	fgets(tmplinea,255,p);
    	nlines++;
    	if(tmplinea[0]==NULL) break;
    }
    // Ahora rebobinamos y nos las leemos todas desde el principio    
    fseek(p,0,SEEK_SET);
    self->texto[2]=(char **)malloc(nlines*sizeof(char*));
    self->lineas_texto[2]=nlines;
    for(i=0;i<nlines;i++)
    {
     self->texto[2][i]=(char *)malloc(256);
     fgets(self->texto[2][i],255,p);     
    }
    fclose(p);

    self->max_size[2]=0;
    for(i=0;i<nlines;i++)
     self->max_size[2]+=Fonts_GetHeight(self->font,self->texto[2][i]);
     
   // Otros parámetros

   self->flags=0;       
   self->avance=0.0f;
   self->y=0.0f;
   self->current=0;
   self->changing=0;
    return self;
}

void SCROLLFont_Do (TScroll *self,float elapsed)
{
    float y = self->y;
    float avance = self->avance;
    float  h = (float) self->max_size[self->current];
    int i,salto=0;

#ifdef LINUX
    unsigned char *keyboard;
#endif

  glColor4f(1.0f,1.0f,1.0f,self->font->alpha);
  glEnable (GL_TEXTURE_2D);
  glEnable (GL_BLEND);

  if (self->blend)
      glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  else
      glBlendFunc (GL_SRC_ALPHA, GL_ONE);

  for(i=0;i<self->lineas_texto[self->current];i++)
  {
   if(((250.0f+y-salto*self->font->scaley) < 280.0f) && ((250.0f+y-salto*self->font->scaley) > -30.0f))
   {
    Fonts_Write(self->font, self->texto[self->current][i], 4.0f, 250.0f+y-salto*self->font->scaley, self->flags);
   }
   salto += Fonts_GetHeight(self->font,self->texto[self->current][i]);
  }
    //
    // Movimiento
    //
    if (avance > 0.0f)
    {
        avance -= FRENO;
        if (avance < 0.0f)
            avance = 0.0f;
    }
    if (avance < 0.0f)
    {
        avance += FRENO;
        if (avance > 0.0f)
            avance = 0.0f;
    }

#ifdef WIN32
    if (GetKeyState (VK_UP) & 0x80)    avance -= AVANCE_NORMAL;
    if (GetKeyState (VK_DOWN) & 0x80)  avance += AVANCE_NORMAL;
    if (GetKeyState (VK_PRIOR) & 0x80) avance -= AVANCE_RAPIDO;
    if (GetKeyState (VK_NEXT) & 0x80)  avance += AVANCE_RAPIDO;
    if ((GetKeyState (VK_SPACE) & 0x80) && (self->changing!=1))
    { 
    	 self->changing=1;    	 
    	 // Ahora forzamos un cambio del alpha    	 
    	 self->font->delta_alpha=-self->font->alpha;
         self->font->alphachange_timeout=1.0f;
    }
#endif
#ifdef LINUX
    keyboard = SDL_GetKeyState(NULL);
    if (keyboard[SDLK_UP] )     avance -= AVANCE_NORMAL;
    if (keyboard[SDLK_DOWN])    avance += AVANCE_NORMAL;
    if (keyboard[SDLK_PAGEUP])  avance -= AVANCE_RAPIDO;
    if (keyboard[SDLK_PAGEDOWN]) avance += AVANCE_RAPIDO;
    if ((keyboard[SDLK_SPACE]) && (self->changing!=1))
    { 
    	 self->changing=1;    	 
    	 // Ahora forzamos un cambio del alpha    	 
    	 self->font->delta_alpha=-self->font->alpha;
         self->font->alphachange_timeout=1.0f;
    }    
#endif
    


    y += avance*self->font->scaley;

    //
    // Rebote
    //
    // Control sobre el avance maximo
    avance = (avance > (+MAX_AVANCE)) ? +MAX_AVANCE : avance;
    avance = (avance < (-MAX_AVANCE)) ? -MAX_AVANCE : avance;

    // Control sobre la salida por arriba
    if (y < 0)
    {
        y      = 0;
        avance = -avance;
    }

    // Control sobre la salida por abajo
    if (y > (h*self->font->scaley)-250.0f)
    {
        y      = (h*self->font->scaley)-250.0f;
        avance = -avance;
    }

    glDisable (GL_TEXTURE_2D);
    glDisable (GL_BLEND);
    self->y = y;
    self->avance = avance;
}


void SCROLLFont_Delete (TScroll *self)
{
  int i;

  for(i=0;i<self->lineas_texto[0];i++) free(self->texto[0][i]);
  for(i=0;i<self->lineas_texto[1];i++) free(self->texto[1][i]);
  for(i=0;i<self->lineas_texto[2];i++) free(self->texto[2][i]);
  free(self->texto[0]);
  free(self->texto[1]);
  free(self->texto[2]);
  Fonts_Delete(self->font);
  free (self);
}

void SCROLLFont_SetScale (TScroll *self, float scalex,float scaley)
{
    self->font->scalex = scalex;
    self->font->scaley = scaley;
}