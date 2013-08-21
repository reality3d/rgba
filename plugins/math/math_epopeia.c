#include <epopeia/epopeia_system.h>
#include <malloc.h>
#include "turb.h"


typedef struct
{
    float  value;
}Math;


//-----------------------------------------------------------------------------
static void* math_New(void)
{
	Math *self;
	self = (Math *)malloc( sizeof(Math) );
	self->value=0.0;
	return (void *)self;
}

//-----------------------------------------------------------------------------
static void math_Delete(void* self)
{
 if(self) free(self);
}

//-----------------------------------------------------------------------------
static void math_DoFrame(void* self, TRenderContext* RC, TSoundContext* SC)
{
}

//-----------------------------------------------------------------------------
static void math_Stop(void* self)
{
 
}

//-----------------------------------------------------------------------------
static void math_Start(void* self,int z)
{
 
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API int Epopeia_PluginAPIVersion(void)
{
    return 140;
}
//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API EPluginType Epopeia_PluginType(void)
{
    return ptEffect;
}

//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return "Math By iq and r3D";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    
	TFunctionId Id;
	TTypeId Id_plug;
    TMethod* pMethod;


    Id_plug = Type_Register("math");
    
    pMethod = Method_New("New", (void*)math_New);
    Type_AddMethod(Id_plug, pMethod);

    pMethod = Method_New("Delete", (void*)math_Delete);
    Type_AddMethod(Id_plug, pMethod);


    pMethod = Method_New("DoFrame", (void*)math_DoFrame);
    Type_AddMethod(Id_plug, pMethod);

    pMethod = Method_New("Start", (void*)math_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id_plug, pMethod);

    pMethod = Method_New("Stop", (void*)math_Stop);
    Type_AddMethod(Id_plug, pMethod);

 	Id = Function_New("noise2", &noise2d, Type_RealId);
    Function_AddParameterType(Id, Type_RealId);
    Function_AddParameterType(Id, Type_RealId);
    FunctionTable_Register(Id);
    
    
    Id = Function_New("noise", &noise1d, Type_RealId);
    Function_AddParameterType(Id, Type_RealId);
    FunctionTable_Register(Id);
    
    TURB_Init(11315);//Table initialization

    
    
}
