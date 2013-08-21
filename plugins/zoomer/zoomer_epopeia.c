//
// ZOOMER - Epopeia interface
//

#include <epopeia/epopeia_system.h>
#include "zoomer.h"

static void ZOOMER_DoFrame (TZoomer *self, TRenderContext *RC, TSoundContext *SC)
{
    float t;

    t = (float) (SC->Tick * 60.0f) / 1000.0f;
    ZOOMER_Do (self, t);
}

static void ZOOMER_Start (void* self, int z)
{
}

static void ZOOMER_Stop (void* self)
{
}

static void ZOOMER_SetScaleOld (TZoomer *self, float scale_x, float scale_y)
{
    self->scale_x_old = scale_x;
    self->scale_y_old = scale_y;
}

static void ZOOMER_SetScaleNew (TZoomer *self, float scale_x, float scale_y)
{
    self->scale_x_new = scale_x;
    self->scale_y_new = scale_y;
}

static void ZOOMER_SetAlphaOld (TZoomer *self, float alpha)
{
    self->alpha_old = alpha;
}

static void ZOOMER_SetAlphaNew (TZoomer *self, float alpha)
{
    self->alpha_new = alpha;
}


EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return "ZOOMER";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId  Id;
    TMethod *pMethod;

    Id = Type_Register("Zoomer");

    pMethod = Method_New ("New", (void*)ZOOMER_Init);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Delete", (void*)ZOOMER_Kill);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("DoFrame", (void*)ZOOMER_DoFrame);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Start", (void*)ZOOMER_Start);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Stop", (void*)ZOOMER_Stop);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetScaleOld", (void*)ZOOMER_SetScaleOld);
    Method_AddParameterType (pMethod, Type_RealId);
    Method_AddParameterType (pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetScaleNew", (void*)ZOOMER_SetScaleNew);
    Method_AddParameterType (pMethod, Type_RealId);
    Method_AddParameterType (pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetAlphaOld", (void*)ZOOMER_SetAlphaOld);
    Method_AddParameterType (pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetAlphaNew", (void*)ZOOMER_SetAlphaNew);
    Method_AddParameterType (pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);
}
