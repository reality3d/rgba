
#include <epopeia/epopeia_system.h>
#include "portada.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif

//-----------------------------------------------------------------------------


static void* Portada_New( void )
{
    return( PORTADA_New() );
}
//-----------------------------------------------------------------------------
static void Portada_Delete( void * self)
{
    PORTADA_Delete( self );
}

//-----------------------------------------------------------------------------


static void Portada_DoFrame( void *self, TRenderContext* RC, TSoundContext* SC)
{
    float   t;

    //t = SC->Tick*(60.0f/1000.0f);
    t = Epopeia_GetTimeMs()*.001f;

    PORTADA_DoFrame( self, t );
}

//-----------------------------------------------------------------------------
static void Portada_Start( void* self, int z)
{
}
//-----------------------------------------------------------------------------
static void Portada_Stop( void* self)
{
}

//-----------------------------------------------------------------------------

static void Portada_SetMode( void * self, int mode )
{
    PORTADA_SetMode( self, mode );
}

static void Portada_SetFreq( void * self, float freq )
{
    PORTADA_SetFreq( self, freq );
}

//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return( "Portada Greena, la chica mas sexy de la scena, by iq" );
}

//-----------------------------------------------------------------------------


EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;


    Id = Type_Register( "Portada" );
   
    pMethod = Method_New("New", (void*)Portada_New);
    //Method_AddParameterType(pMethod, Type_IntegerId);
    //Method_AddParameterType(pMethod, Type_IntegerId);
    //Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)Portada_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Portada_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)Portada_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)Portada_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetMode", (void*)Portada_SetMode );
    Method_AddParameterType(pMethod, Type_IntegerId);
	Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetFreq", (void*)Portada_SetFreq );
    Method_AddParameterType(pMethod, Type_RealId);
	Type_AddMethod(Id, pMethod);

}

#ifdef __cplusplus
}
#endif
