#ifdef WIN32
 #include <windows.h>
#endif
#include <epopeia/epopeia_system.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <glt/glt.h>
#include <math.h>
#include <malloc.h>

#include "kscroll.h"


//-----------------------------------------------------------------------------
static void KScroll_DoFrame(TKScroll* self, TRenderContext* RC, TSoundContext* SC)
{
  int   keystate;

#ifdef WIN32  
  keystate = GetKeyState (VK_UP) & 0x80;
#else

#endif
  if(keystate)
    if(self->posy > 0.0f) self->posy -= 0.001f*self->scaley;
#ifdef WIN32
  keystate = GetKeyState (VK_DOWN) & 0x80;
#else
#endif
  if(keystate)
    if(self->posy < self->scaley) self->posy += 0.001f*self->scaley;

    
  glEnable (GL_TEXTURE_2D); 
  TEX_Bind(self->texture);
  
  glColor4f(1.0f,1.0f,1.0f,1.0f);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_DST_ALPHA);

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ( );
  glLoadIdentity ( );
  glOrtho (-0.5, 0.5, -0.5, 0.5, -100, 100);
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ( );
  glLoadIdentity ( );

  glTranslatef(self->posx-0.5*self->scalex,self->posy-0.5*self->scaley,0);
  glScalef(self->scalex,self->scaley,0);


    glBegin(GL_QUADS);
     glTexCoord2f(0.0f,0.0f);
     glVertex3f(-0.5f,0.5f,1.0f);
     glTexCoord2f(0.0f,1.0f);
     glVertex3f(-0.5f,-0.5f,1.0f);
     glTexCoord2f(1.0f,1.0f);
     glVertex3f(0.5f,-0.5f,1.0f);
     glTexCoord2f(1.0f,0.0f);
     glVertex3f(0.5f,0.5f,1.0f);
    glEnd();

  glMatrixMode (GL_PROJECTION);
  glPopMatrix ( );
  glMatrixMode (GL_MODELVIEW);
  glPopMatrix ( );

  glDisable (GL_TEXTURE_2D);
  glDisable(GL_BLEND);    
  
}
//-----------------------------------------------------------------------------
static void KScroll_Start(TKScroll* self,int kk)
{
}
//-----------------------------------------------------------------------------
static void KScroll_Stop(TKScroll* self)
{
}

static void KScroll_SetScale (TKScroll *self, float scalex,float scaley)
{
    self->scalex = scalex;
    self->scaley = scaley;
}




//-----------------------------------------------------------------------------

EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return "Keyboard Scroller by utopian";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("KScroll");
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso concurrentes
    // El resto de las funciones, obligatorias y no obligatorias,
    // el primer parametro que reciben es ese puntero al objeto.

    pMethod = Method_New("New", (void*)KScroll_New);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)KScroll_Kill);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)KScroll_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)KScroll_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)KScroll_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetScale", (void*)KScroll_SetScale);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);


}

//-----------------------------------------------------------------------------