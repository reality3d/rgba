///////////////////////////////////////////////////////////////////////
// SPLINE EVALUATOR Epopeia interface
///////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <malloc.h>
#include <math.h>

#include <epopeia/epopeia_system.h>
#include "spline.h"


void SPLINE_DoFrame (TSpline *self, TRenderContext *RC, TSoundContext *SC)
{
}

void SPLINE_Start (TSpline *self, int z)
{
}

void SPLINE_Stop (TSpline *self)
{
}

void SPLINE_SetLoopMode (TSpline *self, int mode)
{
    self->loop_mode = mode;
}



EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return "spline evaluator by epsilum";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId  Id;
    TMethod *pMethod;

    Id = Type_Register ("Spline");

    pMethod = Method_New ("New", (void*)SPLINE_New);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Delete", (void*)SPLINE_Delete);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("DoFrame", (void*)SPLINE_DoFrame);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Start", (void*)SPLINE_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Stop", (void*)SPLINE_Stop);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Eval", (void*)SPLINE_Eval);
    Method_AddParameterType (pMethod, Type_RealId);
    Method_SetReturnType (pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetFactor", (void*)SPLINE_SetFactor);
    Method_AddParameterType (pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetLoopMode", (void*)SPLINE_SetLoopMode);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);
}
