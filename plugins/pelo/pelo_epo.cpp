#include <epopeia/epopeia_system.h>

#include "pelo.h"
	
//-----------------------------------------------------------------------------
static void* Fan_New( int num, float lon, char *name, float tscale )
{
    return( PELO_Init(num,lon,name,tscale) );
}
//-----------------------------------------------------------------------------
static void Fan_Delete( PELO* self )
{
//    PELO_End( self );
}
//-----------------------------------------------------------------------------
static void Fan_DoFrame( PELO *self, TRenderContext* RC, TSoundContext* SC )
{
    PELO_DoFrame( self, SC->Tick*(60.0f/1000.0f) );
}
//-----------------------------------------------------------------------------
static void Fan_Start( void* self, int z )
{
}
//-----------------------------------------------------------------------------
static void Fan_Stop( void* self )
{
}
//-----------------------------------------------------------------------------
static void Fan_GetPointer( PELO *self, int puntero, char *name )
{
    PELO_SetPointer( self, puntero, name );
}

static void Fan_SetMovim( PELO *self, float v, float d )
{
    PELO_SetMovim( self, v, d );
}

static void Fan_SetLigthing( PELO *self, long val )
{
    PELO_SetLigthing( self, val );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
extern "C"
{

EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return( "Pelo plugin for Fantasy [by iq]" );
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId     Id;
    TMethod*    pMethod;

    Id = Type_Register("Pelo");

    pMethod = Method_New("New", (void*)Fan_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)Fan_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Fan_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)Fan_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)Fan_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("GetPointer", (void*)Fan_GetPointer);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetMovim", (void*)Fan_SetMovim);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetLighting", (void*)Fan_SetLigthing);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);
}
}
