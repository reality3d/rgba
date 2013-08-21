#include <epopeia/epopeia_system.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <GL/gl.h>

#include "3dworld.h"
#include "fantasy_api.h"

extern "C"
{
	
struct test
{
 FantasyAPI *API;	
};

//-----------------------------------------------------------------------------
static void* APIFantasy2_New(void)
{
  struct test *self;
  
  self=(struct test *)malloc(sizeof(struct test));
  if(!self) return NULL;
  self->API=NULL;
  
  return self;
}
//-----------------------------------------------------------------------------
static void APIFantasy2_Delete(void* self)
{
    free(self);
}
//-----------------------------------------------------------------------------
static void APIFantasy2_DoFrame(struct test *self, TRenderContext* RC, TSoundContext* SC)
{
    if(self->API)
     printf("El mundo tiene %d objetos\n",self->API->world->number_of_objects);
    else
     printf("Self es null\n");
     
    if(self->API)
     printf("Cámara activa: %d\n",self->API->GetActiveCamera());
}
//-----------------------------------------------------------------------------
static void APIFantasy2_Start(void* self, int z)
{
}
//-----------------------------------------------------------------------------
static void APIFantasy2_Stop(void* self)
{
}

static void  APIFantasy2_GetPointer(struct test* self,int puntero)
{
    self->API = (FantasyAPI *)puntero;
}


//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return "API test for Fantasy 2.0";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    //printf("Registrando Test. \n"); //fflush(stdout);

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("APIFantasy2");
//    printf("Cod. de tipo %x. ", Id); fflush(stdout);
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso concurrentes
    // El resto de las funciones, obligatorias y no obligatorias,
    // el primer parametro que reciben es ese puntero al objeto.

    pMethod = Method_New("New", (void*)APIFantasy2_New);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)APIFantasy2_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)APIFantasy2_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)APIFantasy2_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)APIFantasy2_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("GetPointer", (void*)APIFantasy2_GetPointer);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);
}
}
