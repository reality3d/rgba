#include <epopeia/epopeia_system.h>

#include "blurfp.h"

//-----------------------------------------------------------------------------
static void* BlurFP_New( int type, int r2tsize)
{
    return( BlurFP_Init(type, r2tsize, Epopeia_GetResX(), Epopeia_GetResY()) );
}
//-----------------------------------------------------------------------------
static void BlurFP_Delete( BlurFP * self)
{
    BlurFP_End( self );
}
//-----------------------------------------------------------------------------
static void BlurFP_DoFrame( BlurFP *self, TRenderContext* RC, TSoundContext* SC)
{
    float   t;

    t = (SC->Tick*60.0f*1.0f/1000.0f);

    BlurFP_BlurDoFrame( self );
}
//-----------------------------------------------------------------------------
static void BlurFP_Start( BlurFP * self, int z)
{
}
//-----------------------------------------------------------------------------
static void BlurFP_Stop( BlurFP * self)
{
}
//-----------------------------------------------------------------------------
static void BlurFP_SetIterations( BlurFP * self, int number)
{
	self->iterations = number;	
}
//-----------------------------------------------------------------------------
static void BlurFP_SetCoeffs( BlurFP * self, float coeff0, float coeff1,float coeff2,float coeff3,float coeff4,float coeff5,float coeff6,float coeff7,float coeff8)
{
	self->coeff[0] = coeff0;
	self->coeff[1] = coeff1;
	self->coeff[2] = coeff2;
	self->coeff[3] = coeff3;			
	self->coeff[4] = coeff4;
	self->coeff[5] = coeff5;
	self->coeff[6] = coeff6;			
	self->coeff[7] = coeff7;			
	self->coeff[8] = coeff8;
}
//-----------------------------------------------------------------------------


EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return( "Blur (fp version) by utopian" );
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;


    Id = Type_Register("BLUR_FP");
   
    pMethod = Method_New("New", (void*)BlurFP_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)BlurFP_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)BlurFP_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)BlurFP_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)BlurFP_Stop);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetIterations", (void*)BlurFP_SetIterations);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetCoeffs", (void*)BlurFP_SetCoeffs);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);
    
    // --- params ---


}
