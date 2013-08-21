#include <epopeia/epopeia_system.h>

#include "glow.h"

//-----------------------------------------------------------------------------
static void* Glow_New( void )
{
    return( GLOW_Init(Epopeia_GetResX(), Epopeia_GetResY()) );
}
//-----------------------------------------------------------------------------
static void Glow_Delete( GLOW * self)
{
    GLOW_End( self );
}
//-----------------------------------------------------------------------------
static void Glow_DoFrame( GLOW *self, TRenderContext* RC, TSoundContext* SC)
{
    float   t;

    t = (SC->Tick*60.0f*1.0f/1000.0f);

    GLOW_DoFrame( self );
}
//-----------------------------------------------------------------------------
static void Glow_Start( GLOW* self, int z)
{
}
//-----------------------------------------------------------------------------
static void Glow_Stop( GLOW* self)
{
}
//-----------------------------------------------------------------------------
static void Glow_SetPesos( GLOW *me, float p0, float p1, float p2, float p3,
                                     float p4, float p5, float p6, float p7 )
{
    GLOW_SetPesos( me, p0, p1, p2, p3, p4, p5, p6, p7 );
}

//-----------------------------------------------------------------------------
extern "C"
{

EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return( "Glow by iq" );
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;


    Id = Type_Register("Glow");
   
    pMethod = Method_New("New", (void*)Glow_New);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)Glow_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Glow_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)Glow_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)Glow_Stop);
    Type_AddMethod(Id, pMethod);

    // --- params ---

    pMethod = Method_New("SetPesos", (void*)Glow_SetPesos);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

}

}
