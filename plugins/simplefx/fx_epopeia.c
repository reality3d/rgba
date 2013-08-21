//
// Simple OpenGL FX
// Original by Epsilum
// Pluginized by Enlar
//

#include <epopeia/epopeia_system.h>
#include "fx.h"


//
// Epopeia plugin API 1.4.0 implementation
//
//-----------------------------------------------------------------------------
static void* FX_New(int width, int height)
{
    TFx *self;

    self = fx_init(width, height);

    return self;
}

//-----------------------------------------------------------------------------
static void FX_Delete(void* self)
{
    fx_kill(self);
    free(self);
}

//-----------------------------------------------------------------------------
static void FX_DoFrame(void* self, TRenderContext* RC, TSoundContext* SC)
{
    fx_do(self, (SC->Tick*60.0)/1000.0);
}

//-----------------------------------------------------------------------------
static void FX_Start(void* self)
{
    // Do nothing
}

//-----------------------------------------------------------------------------
static void FX_Stop(void* self)
{
    // Do nothing
}

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
EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return "Simple OpenGL FX by Epsilum";
}

//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    pMethod = Method_New("New", (void*)FX_New);

    // Register the effect type/class, given name is used as type name
    // in the script
    Id = Type_Register("SimpleOpenGLFX");
   
    // Add mandatory methods:
    // They are: New, Delete, Start, Stop, DoFrame
    // New returns a newly created object pointer. Each call should give a
    // different pointer, so that each effect/type can have multiple
    // concurrent instances running.
    // The rest of methods, mandatory and user-defined, receive that pointer as
    // the first parameter. Note that the first parameter must not be registered
    // using Method_AddParameterType, it is automatically handled by the system.

    // Constructor has parameters :)
    pMethod = Method_New("New", (void*)FX_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
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

}
//-----------------------------------------------------------------------------
