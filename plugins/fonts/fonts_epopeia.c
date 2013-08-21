//
// GL Font handler [Demo Version]
//

#include <epopeia/epopeia_system.h>
#include <stdio.h>
#include "fonts.h"

//-----------------------------------------------------------------------------
static void* Font_New(int interspace, int space, char *filename, char *filename_nfo)
{
        return Fonts_New(interspace, space, filename, filename_nfo);
}
//-----------------------------------------------------------------------------
static void Font_Delete(TFont* self)
{
}
//-----------------------------------------------------------------------------
static void Font_DoFrame(TFont* self, TRenderContext* RC, TSoundContext* SC)
{
    float elapsed;
    
    if(!(self->ultimo_tick))
    {
        self->ultimo_tick = SC->Tick;
    }
    elapsed = (float)(SC->Tick - self->ultimo_tick) * 0.001f;

    self->x += self->delta_posx * elapsed;
    self->y += self->delta_posy * elapsed;

    self->scalex += self->delta_scalex * elapsed;
    self->scaley += self->delta_scaley * elapsed;


    
    // Tenemos un timeout establecido para que var¡e el alpha
    // Si nos hemos pasado, no se modifica el alpha m s

    if(self->alphachange_timeout > 0.00001f)
    {
        self->alphachange_timeout -= elapsed;
        if(self->alphachange_timeout > 0.00001f)
            self->alpha += self->delta_alpha * elapsed;
        else
            self->delta_alpha=0.0f;
    }
    self->ultimo_tick=SC->Tick;

    
    glColor4f(1.0f,1.0f,1.0f,self->alpha);
    Fonts_Write(self, self->texto, self->x, self->y, self->flags);
}
//-----------------------------------------------------------------------------
static void Font_Write(TFont* self, char *texto, float x, float y, int flags)
{
//    printf("Font_write executing, values %x, %s, %f, %f, %d\n", self, texto, x, y, flags);
    self->texto = texto;
    self->x = x;
    self->y = y;
    self->flags = flags;
    self->delta_posx = self->delta_posy = 0.0f;
}
//-----------------------------------------------------------------------------
static void Font_SetPos(TFont *self,float x,float y)
{
    self->delta_posx = self->delta_posy = 0.0f;
    self->x = x;
    self->y = y;
}
//-----------------------------------------------------------------------------
static void Font_SetPosChange(TFont *self,float delta_x,float delta_y)
{
    self->delta_posx = delta_x;
    self->delta_posy = delta_y;
}
//-----------------------------------------------------------------------------
static void Font_SetScale(TFont *self,float scalex,float scaley)
{
    self->delta_scalex = self->delta_scaley = 0.0f;
    self->scalex = scalex;
    self->scaley = scaley;
}
//-----------------------------------------------------------------------------
static void Font_SetScaleChange(TFont *self,float delta_scalex,float delta_scaley)
{
    self->delta_scalex = delta_scalex;
    self->delta_scaley = delta_scaley;
}

//-----------------------------------------------------------------------------
static void Font_SetAlpha(TFont *self, float alpha)
{
    self->alpha=alpha;
    self->delta_alpha=0.0f;
}
//-----------------------------------------------------------------------------
static void Font_SetAlphaChange(TFont *self, float delta_alpha,float tiempo)
{
    self->delta_alpha=delta_alpha;
    self->alphachange_timeout=tiempo;
}
//-----------------------------------------------------------------------------

static void Font_Start(void* self,int kk)
{
}
//-----------------------------------------------------------------------------
static void Font_Stop(TFont* self)
{
 self->delta_alpha=0.0f;
 self->delta_scalex = self->delta_scaley = 0.0f;
 self->ultimo_tick=0;
}
//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return "Fonts by Utopian&Epsilum?";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

//    printf("Registrando Font. "); //fflush(stdout);

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("Font");
//    printf("Cod. de tipo %x. ", Id); fflush(stdout);
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso concurrentes
    // El resto de las funciones, obligatorias y no obligatorias,
    // el primer parametro que reciben es ese puntero al objeto.

    pMethod = Method_New("New", (void*)Font_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)Fonts_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Font_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)Font_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)Font_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Write", (void*)Font_Write);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetPos", (void*)Font_SetPos);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetPosChange", (void*)Font_SetPosChange);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetScale", (void*)Font_SetScale);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetScaleChange", (void*)Font_SetScaleChange);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    
    pMethod = Method_New("SetAlphaChange", (void*)Font_SetAlphaChange);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAlpha", (void*)Font_SetAlpha);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
    
//    printf("Finalizado.\n");
}
