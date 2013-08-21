// --------------------------------------------------------------------
// SCROLL - Epopeia interface
// --------------------------------------------------------------------
#include <epopeia/epopeia_system.h>
#include "scrollfont.h"

//-----------------------------------------------------------------------------

static void SCROLLFont_SetBlend (void *self, int blend)
{
    TScroll *scr = (TScroll *) self;

    scr->blend = blend;
}

//-----------------------------------------------------------------------------

static void SCROLLFont_SetAlpha (TScroll *self, float alpha)
{
   
    self->font->alpha=alpha;
    self->font->delta_alpha=0.0f;
}

//-----------------------------------------------------------------------------
static void SCROLLFont_SetAlphaChange(TScroll *self, float delta_alpha,float tiempo)
{
    self->font->delta_alpha=delta_alpha;
    self->font->alphachange_timeout=tiempo;
}
//-----------------------------------------------------------------------------
static void SCROLLFont_DoFrame (TScroll *self, TRenderContext *RC, TSoundContext *SC)
{
    float elapsed;
    
    if(!(self->font->ultimo_tick))
    {
        self->font->ultimo_tick = SC->Tick;
    }
    elapsed = (float)(SC->Tick - self->font->ultimo_tick) * 0.001f;
	
    // Tenemos un timeout establecido para que var¡e el alpha
    // Si nos hemos pasado, no se modifica el alpha m s

    if(self->font->alphachange_timeout > 0.00001f)
    {
        self->font->alphachange_timeout -= elapsed;
        if(self->font->alphachange_timeout > 0.00001f)
            self->font->alpha += self->font->delta_alpha * elapsed;
        else
        {
         if((self->font->delta_alpha < 0.0f) && (self->changing==1)) // Estabamos en un fadeout de un cambio
         {
          self->current++;  	
          if(self->current > 2) self->current=0;
          self->y=0.0f;
          self->avance=0.0f;
          self->font->alpha=0.0f;
          self->font->delta_alpha=1.0f;
          self->font->alphachange_timeout=1.0f;    
          self->changing=2;        	
         }
         else if ((self->font->delta_alpha > 0.0f) && (self->changing==2)) // Estabamos en un fadeout de vuelta
         {
          self->changing=0;	
          self->font->alphachange_timeout=0.0f;    
         }
         else
           self->font->delta_alpha=0.0f;
        }
            
    }
       
    
    self->font->ultimo_tick=SC->Tick;
	
	
	
    SCROLLFont_Do (self,elapsed);
}
//-----------------------------------------------------------------------------
static void SCROLLFont_Start (void *self, int z)
{
}
//-----------------------------------------------------------------------------
static void SCROLLFont_Stop (void* self)
{
}
//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return "scrollfont by epsilum & utopian";
}


//-----------------------------------------------------------------------------


EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    Id = Type_Register("ScrollFont");

    pMethod = Method_New("New", (void*)SCROLLFont_New);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)SCROLLFont_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)SCROLLFont_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)SCROLLFont_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)SCROLLFont_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetBlend", (void*)SCROLLFont_SetBlend);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetScale", (void*)SCROLLFont_SetScale);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetAlpha", (void*)SCROLLFont_SetAlpha);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetAlphaChange", (void*)SCROLLFont_SetAlphaChange);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
}
