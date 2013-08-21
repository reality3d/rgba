//
// Flash
//

#include <epopeia/epopeia_system.h>
// Epopeia mesh END
#define DEBUG_VAR 0
#define DEBUG_MODULE "epopeia_flash"
#include <epopeia/debug.h>

#include "flash.h"

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <GL/gl.h>


//-----------------------------------------------------------------------------
static void Flash_DoFrame(TFlash* self, TRenderContext* RC, TSoundContext* SC)
{
    float elapsed;
    
//    DEBUG_msg("Flash doframe in progress");
    if(!(self->ultimo_tick))
    {
        self->ultimo_tick = SC->Tick;
    }
    elapsed = (float)(SC->Tick - self->ultimo_tick) * 0.001f;

    if(self->timeout > 0.00001f)
    {
        self->timeout -= elapsed;
        if(self->timeout > 0.00001f)
            self->alpha -= elapsed*self->delta;
        else
            self->alpha=0.0f;
    }

    self->ultimo_tick=SC->Tick;

    // Hacer el flash solo si no se ha acabado!
    if (self->alpha != 0.0f)
    {
        glPushAttrib (GL_ALL_ATTRIB_BITS);

        glColor4f(self->r,self->g,self->b,self->alpha);

        glEnable (GL_BLEND);
        if (self->blend_mode == 0) // BLEND_NORMAL
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        else // BLEND_ADD
            glBlendFunc (GL_SRC_ALPHA, GL_ONE);
        glDisable (GL_DEPTH_TEST);
        glDisable (GL_TEXTURE_2D);

        glMatrixMode (GL_PROJECTION);
        glPushMatrix ( );
        glLoadIdentity ( );
        glOrtho (0, 1, 0, 1, -100, 100);
        glMatrixMode (GL_MODELVIEW);
        glPushMatrix ( );
        glLoadIdentity ( );

        glBegin(GL_QUADS);
         glVertex3f(0,0,1.0f);
         glVertex3f(0,1,1.0f);
         glVertex3f(1,1,1.0f);
         glVertex3f(1,0,1.0f);
        glEnd();

        glMatrixMode (GL_PROJECTION);
        glPopMatrix ( );
        glMatrixMode (GL_MODELVIEW);
        glPopMatrix ( );
        glEnable (GL_DEPTH_TEST);
        glDisable (GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        glPopAttrib ( );
    }
}

//-----------------------------------------------------------------------------
TFlash* Flash_New(void)
{
    TFlash* self;

    
    self = (TFlash *)malloc(sizeof(TFlash));
    self->ultimo_tick=0;
    self->r = self->g = self->b = 0.0f;
    self->timeout = 0.0f;
    self->alpha = 1.0f;
    self->blend_mode = 0;

    return self;
}

//-----------------------------------------------------------------------------
static void Flash_Kill  (TFlash* self)
{
    free(self);
}
//-----------------------------------------------------------------------------
static void Flash_MakeFlash (TFlash *self,float r, float g, float b,float alpha,float duracion)
{
    self->r = r;
    self->g = g;
    self->b = b;
    self->timeout = duracion;
    self->delta = alpha / duracion;
    self->alpha = alpha;
}

//-----------------------------------------------------------------------------
static void Flash_Start(TFlash* self,int kk)
{
}
//-----------------------------------------------------------------------------
static void Flash_Stop(TFlash* self)
{

}

static void Flash_SetBlend (TFlash *self, int blend_mode)
{
    // blend_mode 0 => Blend normal
    // blend_mode 1 => Blend ADD

    self->blend_mode = blend_mode;
}
//-----------------------------------------------------------------------------

EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return "OpenGL cutre Flash by Utopian";
}
EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

//    printf("Registrando Scroll. "); //fflush(stdout);

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("Flash");
//    printf("Cod. de tipo %x. ", Id); //fflush(stdout);
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso concurrentes
    // El resto de las funciones, obligatorias y no obligatorias,
    // el primer parametro que reciben es ese puntero al objeto.

    pMethod = Method_New("New", (void*)Flash_New);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)Flash_Kill);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Flash_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)Flash_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)Flash_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("MakeFlash",(void*)Flash_MakeFlash);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetBlend",(void*)Flash_SetBlend);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);
}

//-----------------------------------------------------------------------------
