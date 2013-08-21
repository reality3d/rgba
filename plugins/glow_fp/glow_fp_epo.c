#include <epopeia/epopeia_system.h>

#include "glow_fp.h"

//-----------------------------------------------------------------------------
static void* GLOW_FP_New( int r2tsizex, int r2tsizey )
{
    return( GLOW_FP_Init(Epopeia_GetResX(), Epopeia_GetResY(), r2tsizex, r2tsizey) );
}
//-----------------------------------------------------------------------------
static void GLOW_FP_Delete( GLOW_FP * self)
{
    GLOW_FP_End( self );
}
//-----------------------------------------------------------------------------
static void Glow_FP_DoFrame( GLOW_FP *self, TRenderContext* RC, TSoundContext* SC)
{
    float   t;

    t = (SC->Tick*60.0f*1.0f/1000.0f);

    GLOW_FP_DoFrame( self );
}
//-----------------------------------------------------------------------------
static void GLOW_FP_Start( GLOW_FP * self, int z)
{
}
//-----------------------------------------------------------------------------
static void GLOW_FP_Stop( GLOW_FP * self)
{
}
//-----------------------------------------------------------------------------
static void GLOW_FP_SetAlgorithm( GLOW_FP * self, int algorithm)
{
    self->algorithm = algorithm;
}
//-----------------------------------------------------------------------------
static void GLOW_FP_SetCutSubstract( GLOW_FP * self, float r, float g, float b)
{
    self->cut_resta[0] = r;
    self->cut_resta[1] = g;
    self->cut_resta[2] = b;
    self->cut_resta[3] = 0.0f;
}
//-----------------------------------------------------------------------------
static void GLOW_FP_SetOverbright( GLOW_FP * self, float r, float g, float b)
{
    self->producto[0] = r;
    self->producto[1] = g;
    self->producto[2] = b;
    self->producto[3] = 1.0f;
}
//-----------------------------------------------------------------------------
static void GLOW_FP_SetBlurIterations( GLOW_FP * self, int iterations)
{
    self->blur_iterations = iterations;
}
//-----------------------------------------------------------------------------
static void GLOW_FP_SetCutAlpha( GLOW_FP * self, float alpha)
{
    self->cut_alpha = alpha;
}
//-----------------------------------------------------------------------------
static void GLOW_FP_SetAddorMultiply( GLOW_FP * self, int yesno)
{
    self->add_or_multiply = yesno;
}
//-----------------------------------------------------------------------------
static void GLOW_FP_SetPostOverbright( GLOW_FP * self, int yesno)
{
    self->overbright_pre_or_post = yesno;
}
//-----------------------------------------------------------------------------


EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return( "Glow (fp version) by utopian" );
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;


    Id = Type_Register("GLOW_FP");
   
    pMethod = Method_New("New", (void*)GLOW_FP_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)GLOW_FP_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Glow_FP_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)GLOW_FP_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)GLOW_FP_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAlgorithm", (void*)GLOW_FP_SetAlgorithm);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetCutSubstract", (void*)GLOW_FP_SetCutSubstract);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetOverbright", (void*)GLOW_FP_SetOverbright);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetPostOverbright", (void*)GLOW_FP_SetPostOverbright);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);    
    
    pMethod = Method_New("SetBlurIterations", (void*)GLOW_FP_SetBlurIterations);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetCutAlpha", (void*)GLOW_FP_SetCutAlpha);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAddorMultiply", (void*)GLOW_FP_SetAddorMultiply);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetSharedGLTTexture", (void*)GLOW_FP_SetSharedGLTTexture);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);
    
    // --- params ---

}
