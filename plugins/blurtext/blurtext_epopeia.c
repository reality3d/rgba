///////////////////////////////////////////////////////////////////////
// BLURRING TEXTURE :: epsilum
///////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <malloc.h>
#include <math.h>

#include <epopeia/epopeia_system.h>
#include "blurtext.h"


void BT_DoFrame (TBlurText *self, TRenderContext *RC, TSoundContext *SC)
{
    float k;

    if (!INTERP_IsEnd (self->it))
    {
        if (INTERP_Get (self->it, &k))
        {
            // Fin de la transicion!
            self->level = k;
        }
    }

    BT_Do (self);
}


void BT_Start (TBlurText *self, int z)
{
}

void BT_Stop (TBlurText *self)
{
}

void BT_SetMode (TBlurText *self, int mode)
{
    self->mode = mode;
}

void BT_SetBlur (TBlurText *self, float level)
{
    self->level = level;
    self->it->transition = 0;  // Si habia transicion, la paramos
}

void BT_SetBlurFade (TBlurText *self, float level, float dt)
{
    //self->level = level;
    INTERP_Start (self->it, self->level, level, (float) (dt * 1000.0));
}




EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return "blurring texture by epsilum";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId  Id;
    TMethod *pMethod;

    Id = Type_Register ("BlurText");

    pMethod = Method_New ("New", (void*)BT_New);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Delete", (void*)BT_Delete);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("DoFrame", (void*)BT_DoFrame);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Start", (void*)BT_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Stop", (void*)BT_Stop);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetMode", (void*)BT_SetMode);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetLevel", (void*)BT_SetBlur);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetLevelFade", (void*)BT_SetBlurFade);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetBlur", (void*)BT_SetBlur);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetBlurFade", (void*)BT_SetBlurFade);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);
}
