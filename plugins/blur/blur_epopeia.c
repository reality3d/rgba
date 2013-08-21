///////////////////////////////////////////////////////////////////////
// Blur
///////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <malloc.h>
#include <math.h>

#include <epopeia/epopeia_system.h>
#include "blur.h"
#include "interp.h"


void BLUR_DoFrame (TBlur *self, TRenderContext *RC, TSoundContext *SC)
{
    float k;

    if (!INTERP_IsEnd (self->it))
    {
        if (INTERP_Get (self->it, &k))
        {
            // Fin de la transicion!
            self->blur_level = k;
        }
    }


    BLUR_Do (self);
}


void BLUR_Start (TBlur *self, int z)
{
}

void BLUR_Stop (TBlur *self)
{
}

void BLUR_SetBlur (TBlur *self, float blur_level)
{
    self->blur_level = blur_level;
    self->it->transition = 0;  // Si habia transicion, la paramos
}

void BLUR_SetBlurFade (TBlur *self, float blur_level, float dt)
{
    INTERP_Start (self->it, self->blur_level, blur_level, (float) (dt * 1000.0));
}

void BLUR_SetFilter (TBlur *self, int mode)
{
    self->linear = (mode == 0);
}


EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return "blur";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId  Id;
    TMethod *pMethod;

    Id = Type_Register ("Blur");

    pMethod = Method_New ("New", (void*)BLUR_New);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Delete", (void*)BLUR_Delete);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("DoFrame", (void*)BLUR_DoFrame);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Start", (void*)BLUR_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Stop", (void*)BLUR_Stop);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetBlur", (void*)BLUR_SetBlur);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetFilter", (void*)BLUR_SetFilter);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetBlurFade", (void*)BLUR_SetBlurFade);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);
}
