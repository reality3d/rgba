//
// fondo plugin for Epopeia 1.4.0
// This is a very simple background cleaning plugin for Epopeia demosystem.
//
#include <epopeia/epopeia_system.h>
#include <malloc.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <GL/gl.h>

#include "fondo.h"

//-----------------------------------------------------------------------------
static void* Fondo_New(void)
{
   Fondo *self;
   
   self=(Fondo *)malloc(sizeof(Fondo));
   if(!self) return NULL;
   self->R = self->G = self->B = self->A = 1.0f ;
   return (void *)self;
}

//-----------------------------------------------------------------------------
static void Fondo_Delete(void* self)
{
 if(self) free(self);
}

//-----------------------------------------------------------------------------
static void Fondo_DoFrame(Fondo* self, TRenderContext* RC, TSoundContext* SC)
{
    // Clear the color buffer
    glClearColor(self->R,self->G,self->B,self->A);
    glClear (GL_COLOR_BUFFER_BIT);
}

//-----------------------------------------------------------------------------
static void Fondo_Start(void* self, int z)
{
    // Nothing to do in a Start command
}

//-----------------------------------------------------------------------------
static void Fondo_Stop(void* self)
{
    // Nothing to do in a Stop command
}

//-----------------------------------------------------------------------------
static void Fondo_SetColor(Fondo* self, float r, float g, float b, float a)
{
    // Set color used for background filling
    self->R = r;
    self->G = g;
    self->B = b;
    self->A = a;
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
EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return "Fondo: Simple background color filler by Enlar";
}

//-----------------------------------------------------------------------------
// This is the main entry-point in a Epopeia plugin
EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    // Register the effect type/class, given name is used as type name
    // in the script
    Id = Type_Register("Fondo");
   
    // Add mandatory methods:
    // They are: New, Delete, Start, Stop, DoFrame
    // New returns a newly created object pointer. Each call should give a
    // different pointer, so that each effect/type can have multiple
    // concurrent instances running.
    // The rest of methods, mandatory and user-defined, receive that pointer as
    // the first parameter. Note that the first parameter must not be registered
    // using Method_AddParameterType, it is automatically handled by the system.

    pMethod = Method_New("New", (void*)Fondo_New);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)Fondo_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Fondo_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)Fondo_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)Fondo_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetColor", (void*)Fondo_SetColor);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
}

