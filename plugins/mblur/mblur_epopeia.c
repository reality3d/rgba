///////////////////////////////////////////////////////////////////////
//
// MOTION BLUR Epopeia interface
//
///////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <malloc.h>

#include <epopeia/epopeia_system.h>
#include "mblur.h"


void MBLUR_DoFrame (TMBlur *self, TRenderContext *RC, TSoundContext *SC)
{
    MBLUR_Do (self);
}

void MBLUR_Start (TMBlur *self, int z)
{
}

void MBLUR_Stop (TMBlur *self)
{
}

void MBLUR_SetAlpha (TMBlur *self, float alpha)
{
    self->alpha = alpha;
}

void MBLUR_SetBlendMode (TMBlur *self, int mode)
{
    switch (mode)
    {
    case 0:
        self->blend_src_factor = GL_SRC_ALPHA;
        self->blend_dst_factor = GL_ONE_MINUS_SRC_ALPHA;
        break;

    case 1:
        self->blend_src_factor = GL_SRC_ALPHA;
        self->blend_dst_factor = GL_ONE;
        break;

    default:
        break;
    }
}

EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return "MotionBlur by epsilum";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId  Id;
    TMethod *pMethod;

    Id = Type_Register ("MBlur");

    pMethod = Method_New ("New", (void*)MBLUR_Init);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Delete", (void*)MBLUR_Kill);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("DoFrame", (void*)MBLUR_DoFrame);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Start", (void*)MBLUR_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Stop", (void*)MBLUR_Stop);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetAlpha", (void*)MBLUR_SetAlpha);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetMode", (void*)MBLUR_SetBlendMode);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Clear", (void*)MBLUR_Clear);
    Type_AddMethod (Id, pMethod);
}
