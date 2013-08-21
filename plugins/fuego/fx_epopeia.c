#include <epopeia/epopeia_system.h>


#include "fuego.h"

static void * FX_New( int sem, int an, char *texid )
{
    FUEGO   *self;

    self = FUEGO_Init( sem, an, texid );

    return( self );
}


static void FX_Delete( void* self )
{
    FUEGO_End( self );
}

static void FX_DoFrame( void* self, TRenderContext* RC, TSoundContext* SC )
{
    FUEGO_Draw( self, (SC->Tick*60.0)/1000.0 );
}

static void FX_Start( void* self , int kk)
{
    FUEGO_Start( self );
}

static void FX_Stop( void* self )
{
    FUEGO_Stop( self );
}

static void FX_SetParam1( void *self,
                          int i1, int a1, int r1, int g1, int b1,
                          int i2, int a2, int r2, int g2, int b2,
                          int i3, int a3, int r3, int g3, int b3,
                          int i4, int a4, int r4, int g4, int b4 )

{
    FUEGO_SetColor( self, i1, a1, r1, g1, b1,
                          i2, a2, r2, g2, b2,
                          i3, a3, r3, g3, b3,
                          i4, a4, r4, g4, b4 );

}


static void FX_SetParam2( void *self, float v1, float v2 )
{
    FUEGO_SetSpeed( self, v1, v2 );
}

EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return( "Fuego by iq" );
}

//-----------------------------------------------------------------------------

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("Fuego");
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso renderizandose a la vez.
    // El resto de las funciones (las que no son New), obligatorias y no
    // obligatorias, el primer parametro que reciben es el
    // puntero al objeto devuelto por New.

    pMethod = Method_New("New", (void*)FX_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)FX_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)FX_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)FX_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)FX_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetColor", (void*)FX_SetParam1);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetSpeed", (void*)FX_SetParam2);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

}
