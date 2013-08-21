#include <epopeia/epopeia_system.h>

#ifdef WIN32
 #include <windows.h>
#endif
#include <GL/gl.h>

#include "r2texture.h"

//-----------------------------------------------------------------------------
static void* R2Texture_New(int sizex, int sizey, char *name)
{
  R2Texture *self;
  
  self=(R2Texture *)malloc(sizeof(R2Texture));
  if(!self) return NULL;
  
  self->texture = TEX_New (sizex, sizey, 32, NULL,TEX_BUILD_MIPMAPS);  
  if(!self->texture) return NULL;  
  if(!CACHE_InsertTexture(name,self->texture)) return NULL;
  
  self->sizex = sizex;
  self->sizey = sizey;
  
  return self;
}
//-----------------------------------------------------------------------------
static void R2Texture_Delete(R2Texture* self)
{
	
    CACHE_FreeTexture(self->texture);
    free(self);    
}
//-----------------------------------------------------------------------------
static void R2Texture_DoFrame(R2Texture *self, TRenderContext* RC, TSoundContext* SC)
{

  TEX_Bind (self->texture);
  glEnable(GL_TEXTURE_2D);
  glCopyTexSubImage2D (GL_TEXTURE_2D, 0,
                         0, 0,
                         0,
                         0,
                         self->sizex,
                         self->sizey);
  /* 
   glCopyTexImage2D(GL_TEXTURE_2D,0,
   		    GL_RGBA,0,0,
   		    self->sizex,self->sizey,0); */
}
//-----------------------------------------------------------------------------
static void R2Texture_Start(void* self, int z)
{
}
//-----------------------------------------------------------------------------
static void R2Texture_Stop(void* self)
{
}



//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return "Render to texture by Utopian";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    //printf("Registrando Test. \n"); //fflush(stdout);

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("R2Texture");
//    printf("Cod. de tipo %x. ", Id); fflush(stdout);
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso concurrentes
    // El resto de las funciones, obligatorias y no obligatorias,
    // el primer parametro que reciben es ese puntero al objeto.

    pMethod = Method_New("New", (void*)R2Texture_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_StringId);    
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)R2Texture_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)R2Texture_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)R2Texture_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)R2Texture_Stop);
    Type_AddMethod(Id, pMethod);
}
