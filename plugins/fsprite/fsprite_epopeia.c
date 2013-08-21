#ifdef WIN32
 #include <windows.h>
#endif
#include <epopeia/epopeia_system.h>

#include <math.h>
#include <malloc.h>

#include "fsprite.h"



//-----------------------------------------------------------------------------
static void FantasySprite_Start(TSprite* self,int kk)
{
}
//-----------------------------------------------------------------------------
static void FantasySprite_Stop(TSprite* self)
{
}


static void FantasySprite_SetChangeSpeed (TSprite *self, float secs)
{
    self->change_speed = (int) (secs * 1000);
    self->base_time    = 0;
    self->ultimo_tick    = 0;

    // En los "loop modes" distintos de RANDOM (3), se divide
    // entre "change_speed", asi que no puede ser cero!!
    if ((self->change_speed == 0) && (self->loop_mode != 3))
        self->change_speed = 1;

}

static void FantasySprite_SetLoopMode (TSprite *self, int mode)
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

static void FantasySprite_SetAlpha (TSprite *self, float alpha)
{
	int i;
	int source_alpha;
	unsigned int datatemp;//un int de 4 bytes,sin signo
	int target_alpha;

	int imagesize = self->ima[self->actual_sprite].imagen->width * self->ima[self->actual_sprite].imagen->height;
	int bpp = (self->ima[self->actual_sprite].imagen->bits_per_pixel/8);	//Bytes per pixel
	
	
	if(alpha>1.0) alpha=1.0;
	if(alpha<0.0) alpha=0.0;
	
	self->anterior_alpha = self->alpha;//Actualizo el alpha viejo
	self->alpha = alpha;//Actualizo el alpha nuevo
		
	if(/*self->alpha!=0.0 &&*/ (self->alpha - self->anterior_alpha)!=0.0)
        {
            unsigned int *src;
            if(self->pixel_buffer == NULL)
                self->pixel_buffer = (unsigned *)malloc(4*self->max_image_size);

                //memcpy(data, self->ima[self->actual_sprite].imagen->data,imagesize*bpp); //Copio en data la imagen a tratar
            src = self->ima[self->actual_sprite].imagen->data;
	
            for(i=0;i<imagesize; i++)
            {
                datatemp = src[i] & 0x00ffffff;
//                datatemp = datatemp<<8;//Quito el alpha(1 byte)
//                datatemp = datatemp>>8;//Ya sin el alpha(1 byte)
                //printf(" \n data_temp %x \n",datatemp);
                source_alpha= src[i]>>24;	//Obtengo el alpha de la imagen desplazando 3 bytes a la detecha
                //printf(" \n source_alpha %x \n",source_alpha);
                //printf(" \n data[i] %x \n",data[i]);

                target_alpha = alpha*source_alpha; //Alpha final. Se redondea automaticamente a entero
                //printf(" \n target_alpha %x \n",target_alpha);
                target_alpha = target_alpha<<24;//Desplazo 3 bytes a la izquierda para poder sumarlo
                //printf(" \n target_alpha(final) %x \n",target_alpha);
//                printf("%08x + %08X = %08x\n", datatemp, target_alpha, datatemp+target_alpha);
                self->pixel_buffer[i] = datatemp + target_alpha;//Y lo sumo al primer slot, asi no borro la imagen original
            }

            TEX_UpdateData (self->sprite[0].texture, self->pixel_buffer, TEX_BUILD_MIPMAPS);//Actualizo textura
        }
}

///////////////////////////////////////////////////////////////////////
// MULTISPRITE
///////////////////////////////////////////////////////////////////////
static void FantasySprite_SetSprite (TSprite *self, int sprite)
{
    if ((sprite < self->num_sprites) && (sprite >= 0))
    {
        self->actual_sprite = sprite;
        TEX_UpdateData (self->sprite[0].texture, self->ima[self->actual_sprite].imagen->data, TEX_BUILD_MIPMAPS);
		FantasySprite_SetAlpha (self, self->alpha);
	}
}

//-----------------------------------------------------------------------------
static void FantasySprite_DoFrame(TSprite* self, TRenderContext* RC, TSoundContext* SC)
{

    float elapsed;

    if(!(self->ultimo_tick))
    {
        self->ultimo_tick = SC->Tick;
        self->base_time = SC->Tick;
    }
    elapsed = (float)(SC->Tick - self->ultimo_tick) * 0.001f;

    // Select sprite (multisprite)
    if (self->loop_mode == 1) // Loop
        self->actual_sprite = ((SC->Tick - self->base_time) / self->change_speed) % self->num_sprites;
        
    else if (self->loop_mode == 2) // Ping-Pong
    {
        // 0, 1, 2, 3, 2, 1...
        if (self->num_sprites > 2)
        {
            self->actual_sprite = ((SC->Tick - self->base_time) / self->change_speed) % (2 * self->num_sprites - 2);
            if (self->actual_sprite >= self->num_sprites)
            {
                self->actual_sprite = 2 * self->num_sprites - 2 - self->actual_sprite;
            }
        }
        else
            self->actual_sprite = ((SC->Tick - self->base_time) / self->change_speed) % self->num_sprites;
    }
    else if (self->loop_mode == 3) // Random
    {
        if ((SC->Tick - self->base_time) > self->change_speed)
        {
            self->base_time = SC->Tick;
            self->actual_sprite = rand ( ) % self->num_sprites;
        }
    }
    
    self->ultimo_tick=SC->Tick;
    
    // Aqui tengo que poner el sprite que sea
    
    //Cambio solo la textura si es distinta
    if(self->spriteanterior!=self->actual_sprite)
    {
    	self->spriteanterior=self->actual_sprite;
	TEX_UpdateData (self->sprite[0].texture, self->ima[self->actual_sprite].imagen->data, TEX_BUILD_MIPMAPS);
	FantasySprite_SetAlpha (self, self->alpha);
	}	 

}

//-----------------------------------------------------------------------------

EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return "Fantasy MultiSprite by r3D";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

//    printf("Registrando Scroll. "); //fflush(stdout);

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("FantasySprite");
//    printf("Cod. de tipo %x. ", Id); //fflush(stdout);
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso concurrentes
    // El resto de las funciones, obligatorias y no obligatorias,
    // el primer parametro que reciben es ese puntero al objeto.

    pMethod = Method_New("New", (void*)FantasySprite_New);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_StringId);//Descriptor
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)FantasySprite_Kill);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)FantasySprite_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)FantasySprite_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)FantasySprite_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetSprite", (void*)FantasySprite_SetSprite);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetChangeSpeed", (void*)FantasySprite_SetChangeSpeed);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetLoopMode", (void*)FantasySprite_SetLoopMode);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAlpha", (void*)FantasySprite_SetAlpha);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

}

//-----------------------------------------------------------------------------
