//
// Simple OpenGL FX
//
#ifdef WIN32
#include <windows.h>
#endif
#include <epopeia/epopeia_system.h>
// Epopeia mesh END
//#define DEBUG_VAR 0
//#define DEBUG_MODULE "epopeia_sprite"

//#include <epopeia/debug.h>
#include <math.h>
#include <malloc.h>

#include "sprite.h"
#include "interp.h"



//-----------------------------------------------------------------------------
static void Sprite_DoFrame(TSprite* self, TRenderContext* RC, TSoundContext* SC)
{
    float elapsed;
    // DEBUG_msg("Sprite doframe in progress");
    //    Scroll_Dibuja(self, SC->Tick);

    if(!(self->ultimo_tick))
    {
        self->ultimo_tick = SC->Tick;
    }
    elapsed = (float)(SC->Tick - self->ultimo_tick) * 0.001f;

    // Select sprite (multisprite)
    if (self->loop_mode == 1) // Loop
        self->actual_sprite = ((Epopeia_GetTimeMs() - self->base_time) / self->change_speed) % self->num_sprites;
    else if (self->loop_mode == 2) // Ping-Pong
    {
        // 0, 1, 2, 3, 2, 1...
        if (self->num_sprites > 2)
        {
            self->actual_sprite = ((Epopeia_GetTimeMs() - self->base_time) / self->change_speed) % (2 * self->num_sprites - 2);
            if (self->actual_sprite >= self->num_sprites)
            {
                self->actual_sprite = 2 * self->num_sprites - 2 - self->actual_sprite;
            }
        }
        else
            self->actual_sprite = ((Epopeia_GetTimeMs() - self->base_time) / self->change_speed) % self->num_sprites;
    }
    else if (self->loop_mode == 3) // Random
    {
        if ((Epopeia_GetTimeMs() - self->base_time) > self->change_speed)
        {
            self->base_time = Epopeia_GetTimeMs();
            self->actual_sprite = rand ( ) % self->num_sprites;
        }
    }

    if(self->pos_method ==1) // Movimiento aleatorio
    {
     self->posx = self->rand_posx + ((rand()%200)-100)*0.01f * self->rand_maxx;
     self->posy = self->rand_posy + ((rand()%200)-100)*0.01f * self->rand_maxy;
    }
    else
    {
        self->posx += self->delta_posx * elapsed;
        self->posy += self->delta_posy * elapsed;
    }

    if (self->scale_fade)
    {
        if (!INTERP_IsEnd (self->sclx_it)) INTERP_Get (self->sclx_it, &self->scalex);
        if (!INTERP_IsEnd (self->scly_it)) INTERP_Get (self->scly_it, &self->scaley);
    }
    else
    {
        self->scalex += self->delta_scalex * elapsed;
        self->scaley += self->delta_scaley * elapsed;
    }

    // Tenemos un timeout establecido para que var¡e el alpha
    // Si nos hemos pasado, no se modifica el alpha m s
    if(self->alpha_method ==1)    // Variaci¢n lineal del alfa
    {
        if(self->alphachange_timeout > 0.00001f)
        {
            self->alphachange_timeout -= elapsed;
            if(self->alphachange_timeout > 0.00001f)
                self->alpha += self->delta_alpha * elapsed;
            else
                self->delta_alpha=0.0f;                              
        }
        else
            self->alpha_method=0;
    }
    else if(self->alpha_method==2) // Variaci¢n sinusoidal del alfa
    {
        self->alpha_sinustime += elapsed;
        self->alpha = self->alpha_amplitud * fabs(sin((self->alpha_sinustime*2.0f*3.1415927f)/self->alpha_fase));
    }
    else if(self->alpha_method==3)
    {
     self->alpha = self->randalpha_alpha + ((rand()%200)-100)*0.01f * self->randalpha_maxdesp;
    }
    else if(self->alpha_method==4) // Variación pulso on-off(empieza en on)
    {
    	if(self->alpha_pulse>=self->alpha_fase)
    	{
    		if(self->paridad==0)
    		{
    			self->alpha = self->alpha_amplitud;
    			self->paridad=1;//Ciclo impar
    		}
    		else
    		{
    			self->alpha = 0;
    			self->paridad=0;//Ciclo par
    		}
    		self->alpha_pulse=0.0f;//vuelve a comenzar el pulso
    	}
    	else	self->alpha_pulse+= elapsed;
    }
    			
    		
    
    self->ultimo_tick=SC->Tick;
   
    Sprite_Paint(self,
                 self->posx - (self->sprite[self->actual_sprite].sizex_2 * self->scalex),self->posy - (self->sprite[self->actual_sprite].sizey_2 * self->scaley),
                 self->posx + (self->sprite[self->actual_sprite].sizex_2 * self->scalex),self->posy - (self->sprite[self->actual_sprite].sizey_2 * self->scaley),
                 self->posx + (self->sprite[self->actual_sprite].sizex_2 * self->scalex),self->posy + (self->sprite[self->actual_sprite].sizey_2 * self->scaley),
                 self->posx - (self->sprite[self->actual_sprite].sizex_2 * self->scalex),self->posy + (self->sprite[self->actual_sprite].sizey_2 * self->scaley),
                 self->alpha,self->doblend,
                 (SC->Tick*60.0)/1000.0);


/*
                 self->posx,self->posy,
                 self->posx + (self->scalex * self->sizex),self->posy,
                 self->posx + (self->scalex * self->sizex),self->posy + (self->scaley * self->sizey),
                 self->posx,self->posy + (self->scaley * self->sizey),self->alpha,0);
  */


}
//-----------------------------------------------------------------------------
static void Sprite_Start(TSprite* self,int kk)
{
}
//-----------------------------------------------------------------------------
static void Sprite_Stop(TSprite* self)
{
}
//-----------------------------------------------------------------------------
static void Sprite_SetPos(TSprite *self,float x, float y)
{
    self->posx = x;
    self->posy = y;
    self->delta_posx=self->delta_posy = 0.0f;
    self->pos_method=0;
}
//-----------------------------------------------------------------------------
static void Sprite_SetPosChange(TSprite *self,float delta_x, float delta_y)
{
    self->delta_posx = delta_x;
    self->delta_posy = delta_y;
    self->pos_method=0;
}
//-----------------------------------------------------------------------------

static void Sprite_SetPosRandom(TSprite *self,float centerx, float centery,float maxdespx,float maxdespy)
{
    self->rand_posx=centerx;
    self->rand_posy=centery;
    self->rand_maxx=maxdespx;
    self->rand_maxy=maxdespy;
    self->pos_method=1;
}
//-----------------------------------------------------------------------------
static void Sprite_SetScale(TSprite *self,float x, float y)
{
    self->scalex = x;
    self->scaley = y;
    self->delta_scalex=self->delta_scaley = 0.0f;
}
//-----------------------------------------------------------------------------
static void Sprite_SetScaleChange(TSprite *self,float delta_x, float delta_y)
{
    self->delta_scalex = delta_x;
    self->delta_scaley = delta_y;

    self->scale_fade = 0;
}

static void Sprite_SetScaleFade (TSprite *self,
                                 float src_scale_x,
                                 float src_scale_y,
                                 float dst_scale_x,
                                 float dst_scale_y,
                                 float dt)
{
    INTERP_Start (self->sclx_it, src_scale_x, dst_scale_x, (float) (dt * 1000.0));
    INTERP_Start (self->scly_it, src_scale_y, dst_scale_y, (float) (dt * 1000.0));

    self->scale_fade = 1;
}

//-----------------------------------------------------------------------------

static void Sprite_SetAlpha(TSprite *self, float alpha)
{
	
    self->alpha=alpha;
    self->delta_alpha=0.0f;
    self->alpha_method=0;
}
//-----------------------------------------------------------------------------
static void Sprite_SetAlphaChange(TSprite *self, float delta_alpha,float tiempo)
{
    self->delta_alpha=delta_alpha;
    self->alphachange_timeout=tiempo;
    self->alpha_method=1;
}
//-----------------------------------------------------------------------------
static void Sprite_SetAlphaPulse(TSprite *self, float amplitud, float fase)
{
    self->alpha_amplitud=amplitud;
    self->alpha_fase=fase;
    self->alpha_method=4;
}
//-----------------------------------------------------------------------------
static void Sprite_SetAlphaSin(TSprite *self, float amplitud, float fase)
{
    self->delta_alpha=0.0f;
    self->alphachange_timeout=0.0f;
    self->alpha_amplitud=amplitud;
    self->alpha_fase=fase;
    self->alpha_sinustime=0.0f;
    self->alpha_method=2;
}

//-----------------------------------------------------------------------------
static void Sprite_SetAlphaRandom(TSprite *self, float alpha, float maxdesp)
{
    self->randalpha_alpha=alpha;
    self->randalpha_maxdesp=maxdesp;
    self->alpha_method=3;
}
//-----------------------------------------------------------------------------

static void Sprite_SetBlend(TSprite *self,int blend)
{
    self->doblend=blend;
}



///////////////////////////////////////////////////////////////////////
// BEGIN - SINE DEFORM CODE
///////////////////////////////////////////////////////////////////////
static void Sprite_SetDeform (TSprite *self, int st)
{
    self->deform = st;
}

static void Sprite_SetGridSize (TSprite *self, int x, int y)
{
    self->grid_w = x;
    self->grid_h = y;

    // Alloc grid
    if (self->grid)
        free (self->grid);

    self->grid = (grid_t *) calloc ((self->grid_w+1) * (self->grid_h+1), sizeof (grid_t));
}

static void Sprite_SetDeformX (TSprite *self, float waves, float amplitude, float freq)
{
    self->grid_ax = amplitude;
    self->grid_fx = freq;
    self->grid_wx = waves;
}

static void Sprite_SetDeformY (TSprite *self, float waves, float amplitude, float freq)
{
    self->grid_ay = amplitude;
    self->grid_fy = freq;
    self->grid_wy = waves;
}

static void Sprite_SetDeformU (TSprite *self, float waves, float amplitude, float freq)
{
    self->grid_au = amplitude;
    self->grid_fu = freq;
    self->grid_wu = waves;
}

static void Sprite_SetDeformV (TSprite *self, float waves, float amplitude, float freq)
{
    self->grid_av = amplitude;
    self->grid_fv = freq;
    self->grid_wv = waves;
}
///////////////////////////////////////////////////////////////////////
// END - SINE DEFORM CODE
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
// MULTISPRITE
///////////////////////////////////////////////////////////////////////
static void Sprite_SetSprite (TSprite *self, int sprite)
{
    if ((sprite < self->num_sprites) && (sprite >= 0))
        self->actual_sprite = sprite;
}

static void Sprite_SetChangeSpeed (TSprite *self, float secs)
{
    self->change_speed = (int) (secs * 1000);
    self->base_time    = Epopeia_GetTimeMs ( );

    // En los "loop modes" distintos de RANDOM (3), se divide
    // entre "change_speed", asi que no puede ser cero!!
    if ((self->change_speed == 0) && (self->loop_mode != 3))
        self->change_speed = 1;

}

static void Sprite_SetLoopMode (TSprite *self, int mode)
{
    // mode
    // 0 - Static sprite
    // 1 - Loop!
    // 2 - PingPong
    // 3 - Random
    self->loop_mode = mode;

    // Si queremos que sea aleatorio de verdad
    if (mode == 3)
        srand ((unsigned) time(NULL));

}




//-----------------------------------------------------------------------------

EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return "OpenGL Sprite by Utopian";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

//    printf("Registrando Scroll. "); //fflush(stdout);

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("Sprite");
//    printf("Cod. de tipo %x. ", Id); //fflush(stdout);
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso concurrentes
    // El resto de las funciones, obligatorias y no obligatorias,
    // el primer parametro que reciben es ese puntero al objeto.

    pMethod = Method_New("New", (void*)Sprite_New);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)Sprite_Kill);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Sprite_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)Sprite_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)Sprite_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetPos",(void*)Sprite_SetPos);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetPosChange",(void*)Sprite_SetPosChange);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetPosRandom",(void*)Sprite_SetPosRandom);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetScale",(void*)Sprite_SetScale);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetScaleChange",(void*)Sprite_SetScaleChange);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetScaleFade",(void*)Sprite_SetScaleFade);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetAlpha",(void*)Sprite_SetAlpha);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAlphaChange",(void*)Sprite_SetAlphaChange);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAlphaRandom",(void*)Sprite_SetAlphaRandom);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAlphaSin",(void*)Sprite_SetAlphaSin);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);
    
    
    //r3D::SetAlphaPulse
    pMethod = Method_New("SetAlphaPulse",(void*)Sprite_SetAlphaPulse);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetBlend", (void*)Sprite_SetBlend);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);


    pMethod = Method_New("SetDeform", (void*)Sprite_SetDeform);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetGridSize", (void*)Sprite_SetGridSize);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetDeformX", (void*)Sprite_SetDeformX);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetDeformY", (void*)Sprite_SetDeformY);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetDeformU", (void*)Sprite_SetDeformU);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetDeformV", (void*)Sprite_SetDeformV);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetSprite", (void*)Sprite_SetSprite);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetChangeSpeed", (void*)Sprite_SetChangeSpeed);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetLoopMode", (void*)Sprite_SetLoopMode);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

//    SetPos(float x, float y, float z)
    
//    pMethod = Method_New("SetPosChange", (void*)Scroll_SetPosChange);
//    Method_AddParameterType(pMethod, Type_RealId);
//    Type_AddMethod(Id, pMethod);

//    printf("Finalizado.\n");
}

//-----------------------------------------------------------------------------