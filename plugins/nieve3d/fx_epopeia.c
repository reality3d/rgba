#include <epopeia/epopeia_system.h>

#include "nieve3d.h"

static void * FX_New( int num, float width,
                      float x1, float y1, float z1,
                      float x2, float y2, float z2 )
{
    NIEVE3D   *self;

    self = NIEVE3D_Init( num, width, x1, y1, z1, x2, y2, z2 );

    return( self );
}


static void FX_Delete( void* self )
{
    NIEVE3D_End( self );
}

static void FX_DoFrame( void* self, TRenderContext* RC, TSoundContext* SC )
{
    NIEVE3D_Draw( self, (SC->Tick*60.0)/1000.0 );
}

static void FX_Start( void* self,int kk )
{
    NIEVE3D_Start( self );
}

static void FX_Stop( void* self )
{
}

static void FX_SetParam1( void *self, float x, float y, float z )
{
    NIEVE3D_SetWind( self, x, y, z );
}


EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return( "Nieve3D by iq" );
}

//-----------------------------------------------------------------------------

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    Id = Type_Register("Nieve3d");

    pMethod = Method_New("New", (void*)FX_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
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

    pMethod = Method_New("SetWind", (void*)FX_SetParam1);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
}
