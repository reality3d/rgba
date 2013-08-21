///////////////////////////////////////////////////////////////////////
//
// VIEWPORT
//
///////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <malloc.h>

#include <epopeia/epopeia_system.h>
#include "viewport.h"


void VIEWPORT_DoFrame (TViewport *self, TRenderContext *RC, TSoundContext *SC)
{
    float elapsed;

    // Update viewport area
    if ((self->dx != 0.0f) || (self->dy != 0.0f))
    {
        if (!self->last_tick)
            self->last_tick = SC->Tick;

        elapsed = (float)(SC->Tick - self->last_tick) * 0.001f;

        self->fx_offset += elapsed * self->dx;
        self->fy_offset += elapsed * self->dy;

        self->x_offset = (int) self->fx_offset;
        self->y_offset = (int) self->fy_offset;
    }

    glViewport (self->x_offset, self->y_offset, self->width, self->height);

    self->last_tick = SC->Tick;
}

//
// Establece un viewport: Todos los valores que se le pasan son
// porcentajes. El 100% sera el ancho/alto de la ventana.
//
void VIEWPORT_Set (TViewport *self,
                   int mode,
                   int x_offset,
                   int y_offset,
                   int width,
                   int height)
{
    int  wnd_width, wnd_height;

    wnd_width  = Epopeia_GetResX();
    wnd_height = Epopeia_GetResY();

    if (mode == 0)
    {
        // MODE 0: Set physical viewport
        self->width    = (int) width;
        self->height   = (int) height;
        self->x_offset = (int) x_offset;
        //self->y_offset = (int) (wnd_height - self->height - y_offset);
        self->y_offset = (int) y_offset;
    }
    else
    {
        // MODE 1: Set percentage viewport
        self->width    = (int) (width * wnd_width / 100);
        self->height   = (int) (height * wnd_height / 100);
        self->x_offset = (int) (x_offset * wnd_width  / 100);
        self->y_offset = (int) (y_offset * wnd_height / 100);
        //self->y_offset = (int) (wnd_height - self->height - y_offset * wnd_height / 100);
    }
}

TViewport *VIEWPORT_New (int mode,
                         int perc_x_offset,
                         int perc_y_offset,
                         int perc_width,
                         int perc_height)
{
    TViewport *self;

    self = (TViewport *) malloc (sizeof (TViewport));
    if (!self)
        return 0;

    VIEWPORT_Set (self,
                  mode,
                  perc_x_offset,
                  perc_y_offset,
                  perc_width,
                  perc_height);

    // Sin desplazamiento inicialmente
    self->dx        = 0.0f;
    self->dy        = 0.0f;
    self->last_tick = 0;

    return self;
}

void VIEWPORT_Kill (TViewport *self)
{
    free (self);
}

void VIEWPORT_Start (TViewport *self,int layer)
{
}

void VIEWPORT_Stop (TViewport *self)
{
}

void VIEWPORT_SetPosChange (TViewport *self, float dx, float dy)
{
    self->dx = dx;
    self->dy = dy;

    self->fx_offset = (float) self->x_offset;
    self->fy_offset = (float) self->y_offset;
}


EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return "OpenGL Viewport";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId  Id;
    TMethod *pMethod;

    Id = Type_Register ("Viewport");

    pMethod = Method_New ("New", (void*)VIEWPORT_New);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Delete", (void*)VIEWPORT_Kill);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("DoFrame", (void*)VIEWPORT_DoFrame);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Start", (void*)VIEWPORT_Start);
    Method_AddParameterType (pMethod, Type_IntegerId);    
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Stop", (void*)VIEWPORT_Stop);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Set", (void*)VIEWPORT_Set);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetPosChange", (void*)VIEWPORT_SetPosChange);
    Method_AddParameterType (pMethod, Type_RealId);
    Method_AddParameterType (pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);
}
