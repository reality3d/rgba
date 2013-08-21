// --------------------------------------------------------------------
// DOTS - Epopeia interface
// --------------------------------------------------------------------
#include <epopeia/epopeia_system.h>
#include "dots.h"

static void DOTS_SetPosition (void *self, float x, float y)
{
    TDots *d = (TDots *) self;
    d->xpos = x;
    d->ypos = y;
}

static void DOTS_SetScale (void *self, float sx, float sy)
{
    TDots *d = (TDots *) self;
    d->xscale = sx;
    d->yscale = sy;
}

static void DOTS_SetMode (void *self, int mode)
{
    TDots *d = (TDots *) self;
    d->mode = mode;
}

static void DOTS_SetBlend (void *self, int mode)
{
    TDots *d = (TDots *) self;
    d->blend = mode;
}

static void DOTS_SetAlpha (TDots *dots, float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    dots->alpha = (int) (255.0f * alpha);

    dots->it_alpha->transition = 0;
}

static void DOTS_SetAlphaChange (TDots *dots, float a, float t)
{
    if (a < 0.0f) a = 0.0f;
    if (a > 1.0f) a = 1.0f;
    INTERP_Start (dots->it_alpha, (float)dots->alpha/255.0f, a, t * 1000.0f);
}


//-----------------------------------------------------------------------------
static void DOTS_DoFrame (TDots *self, TRenderContext *RC, TSoundContext *SC)
{
    if (!INTERP_IsEnd (self->it_alpha))
    {
        float alpha;
        INTERP_Get (self->it_alpha, &alpha);
        self->alpha = (int) (255.0f * alpha);
    }

    DOTS_Do (self, (float) (SC->Tick*60.0)/1000.0);
}
//-----------------------------------------------------------------------------
static void DOTS_Start (void *self, int z)
{
}
//-----------------------------------------------------------------------------
static void DOTS_Stop (void* self)
{
}
//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return "dots by epsilum";
}


//-----------------------------------------------------------------------------


EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    Id = Type_Register("Dots");

    pMethod = Method_New("New", (void*)DOTS_New);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)DOTS_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)DOTS_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)DOTS_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)DOTS_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetPosition", (void*)DOTS_SetPosition);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetScale", (void*)DOTS_SetScale);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetWindow", (void*)DOTS_SetWindow);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetFilter", (void*)DOTS_SetFilter);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetMode", (void*)DOTS_SetMode);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetBlend", (void*)DOTS_SetBlend);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAlpha", (void*)DOTS_SetAlpha);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAlphaChange", (void*)DOTS_SetAlphaChange);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
}
