#include <epopeia/epopeia_system.h>

#include "dof_fp.h"

//-----------------------------------------------------------------------------
static void* DOF_FP_New( int r2tsizex, int r2tsizey )
{
    return( DOF_FP_Init(Epopeia_GetResX(), Epopeia_GetResY(), r2tsizex, r2tsizey) );
}
//-----------------------------------------------------------------------------
static void DOF_FP_Delete( DOF_FP * self)
{
    DOF_FP_End( self );
}
//-----------------------------------------------------------------------------
static void Dof_FP_DoFrame( DOF_FP *self, TRenderContext* RC, TSoundContext* SC)
{
    float   t;

    t = (SC->Tick*60.0f*1.0f/1000.0f);

    DOF_FP_DoFrame( self );
}
//-----------------------------------------------------------------------------
static void DOF_FP_Start( DOF_FP * self, int z)
{
}
//-----------------------------------------------------------------------------
static void DOF_FP_Stop( DOF_FP * self)
{
}
//-----------------------------------------------------------------------------
static void DOF_FP_SetFocus( DOF_FP * self, float focus)
{
	self->focus = focus;
}
//-----------------------------------------------------------------------------
static void DOF_FP_SetRange( DOF_FP * self, float range)
{
	self->range = range;
}
//-----------------------------------------------------------------------------
static void DOF_FP_SetBlurIterations( DOF_FP * self, int iterations)
{
	self->blur_iterations = iterations;
}
//-----------------------------------------------------------------------------
static void DOF_FP_SetAlgorithm( DOF_FP * self, int algorithm)
{
	self->algorithm = algorithm;
}
//-----------------------------------------------------------------------------

extern "C"
{

EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return( "Depth of field (fp version) by utopian" );
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;


    Id = Type_Register("DOF_FP");
   
    pMethod = Method_New("New", (void*)DOF_FP_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)DOF_FP_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Dof_FP_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)DOF_FP_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)DOF_FP_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetFocus", (void*)DOF_FP_SetFocus);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetRange", (void*)DOF_FP_SetRange);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetBlurIterations", (void*)DOF_FP_SetBlurIterations);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAlgorithm", (void*)DOF_FP_SetAlgorithm);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetSharedDepthTexture", (void*)DOF_FP_SetSharedDepthTexture);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);    
    
    // --- params ---


}

}
