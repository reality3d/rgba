#include <epopeia/epopeia_system.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <GL/gl.h>

#include "3dworld.h"

extern "C"
{
	
struct test
{
 world_type *world;;	
};

//-----------------------------------------------------------------------------
static void* DKFantasy_New(void)
{
  struct test *self;
  
  self=(struct test *)malloc(sizeof(struct test));
  if(!self) return NULL;
  self->world=NULL;
  
  return self;
}
//-----------------------------------------------------------------------------
static void DKFantasy_Delete(void* self)
{
    free(self);
}
//-----------------------------------------------------------------------------
static void DKFantasy_DoFrame(struct test *self, TRenderContext* RC, TSoundContext* SC)
{
    if(self->world)
     printf("El mundo tiene %d objetos\n",self->world->number_of_objects);
    else
     printf("Self es null\n");
}
//-----------------------------------------------------------------------------
static void DKFantasy_Start(void* self, int z)
{
}
//-----------------------------------------------------------------------------
static void DKFantasy_Stop(void* self)
{
}

static void  DKFantasy_GetPointer(struct test* self,int puntero)
{
    self->world = (world_type *)puntero;
}


//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return "Developers Kit for Fantasy";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    //printf("Registrando Test. \n"); //fflush(stdout);

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("DKFantasy");
//    printf("Cod. de tipo %x. ", Id); fflush(stdout);
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso concurrentes
    // El resto de las funciones, obligatorias y no obligatorias,
    // el primer parametro que reciben es ese puntero al objeto.

    pMethod = Method_New("New", (void*)DKFantasy_New);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)DKFantasy_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)DKFantasy_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)DKFantasy_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)DKFantasy_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("GetPointer", (void*)DKFantasy_GetPointer);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);
}
}
