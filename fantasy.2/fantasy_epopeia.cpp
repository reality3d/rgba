#ifdef WIN32
 #include <windows.h>
#endif

#include <epopeia/epopeia_system.h>
#include "fantasy/fantasy.h"
#include <stdio.h>

//
// Interfaz con Epopeia
//

extern "C" {

typedef struct {
    int handle;
    int active_camera;
    float velocidad;
    int first_tick;
    int last_tick;
    int dyn_lights;
    int fogenable;
    int fog_type;
    float fog_r;
    float fog_g;
    float fog_b;
    float fog_density;
    int fog_invert;
    float globalIlum;
    int fadeticks;		// Ticks que va a durar el fade
    float deltaIlum;		// Variación de la ilum. global (fade)
    float globalAlpha;
    int alphafadeticks;		// Ticks que va a durar el fade del alpha
    float deltaAlpha;		// Variación del alpha global (fade2)
    int ZBufferClear;		// Debería esta instancia limpiar el zbuffer?
    float last_frame;		// último frame pintado
    int debuglevel;		// nivel de debug a aplicar en la escena
} TEscena3D;


static int engine_use_count = 0;
static Fantasy* engine = NULL;

//-----------------------------------------------------------------------------
static void* FANTASY_New(char* FicheroEscena,char *FicheroMateriales)
{
    TEscena3D *self;

    if(engine_use_count == 0)
    {
        engine = new Fantasy(256);
 	if(engine->render_path==0)
 	{
 	 printf("Error: unsupported gfx card\n");
 	 return NULL;	
 	}
 	/*else
 	{
 	 printf("Render path: %d\n",engine->render_path);	
 	}*/
    }
    engine_use_count++;

    self = (TEscena3D*)malloc(sizeof(TEscena3D));
    if(self == NULL)
        return NULL;

    self->handle = engine->Load(FicheroEscena,FicheroMateriales);
    if(self->handle == -1) return NULL;

    self->active_camera = 0;
    self->velocidad = 30.0;
    self->first_tick=-1;
    self->last_tick=-1;
    self->dyn_lights=1;
    self->fogenable=0;
    self->globalIlum = 1.0f;
    self->fadeticks = 0;
    self->deltaIlum = 0.0f;
    self->globalAlpha = 1.0f;
    self->alphafadeticks = 0;
    self->deltaAlpha = 0.0f;
    self->ZBufferClear = 1;
    self->last_frame = 0.0f;
    self->debuglevel = 1;  // DEBUG_LEVEL_WARNING;
    //self->debuglevel = 3;  // DEBUG_LEVEL_OVERDEBUG;

    return self;
}
//-----------------------------------------------------------------------------
static void FANTASY_Delete(TEscena3D* self)
{
    // No hay forma de descargar la escena!!! De momento descargamos las texturas
    
    engine->CleanupTextures(self->handle);
    
    engine_use_count--;
    if(engine_use_count == 0)
        delete engine;
    free(self);
}

//-----------------------------------------------------------------------------
static void FANTASY_DoFrame(TEscena3D* self, TRenderContext* RC, TSoundContext* SC)
{
  int elapsed;


    if(self->first_tick==-1)
    {
        self->first_tick=SC->Tick;
    	self->last_tick=SC->Tick;    
    }
    
    if(self->fadeticks)
    {
     if(self->last_tick == -1) elapsed =0;
       else elapsed = SC->Tick - self->last_tick;
     self->globalIlum += elapsed * self->deltaIlum;
     self->fadeticks -=elapsed;
     if(self->fadeticks < 0) self->fadeticks = 0;
    }    

    if(self->alphafadeticks)
    {
     if(self->last_tick == -1) elapsed =0;
       else elapsed = SC->Tick - self->last_tick;
     self->globalAlpha += elapsed * self->deltaAlpha;
     self->alphafadeticks -=elapsed;
     if(self->alphafadeticks < 0) self->alphafadeticks = 0;
    }    


    engine->SetScene(self->handle);
    engine->SetDebugLevel(self->debuglevel); 	
    engine->SetCamera(self->active_camera);
    engine->SetDynLighting(self->dyn_lights);
    engine->SetGlobalIlum(self->globalIlum);           
    engine->SetGlobalAlpha(self->globalAlpha);           
    engine->SetZBufferClear(self->ZBufferClear);
    engine->SetFog(self->fogenable,self->fog_type,self->fog_r,self->fog_g,self->fog_b,self->fog_density,self->fog_invert);
    

    self->last_frame += (((SC->Tick-self->last_tick)*self->velocidad)/1000.0);
    engine->DrawFrame(self->last_frame, (SC->Tick-self->first_tick)/1000.0);
    self->last_tick = SC->Tick;
}
//-----------------------------------------------------------------------------
static void FANTASY_Start(TEscena3D *self,int kk)
{
    self->first_tick=-1;
    self->last_tick=-1;
}

//-----------------------------------------------------------------------------
static void FANTASY_Stop(TEscena3D* self)
{
 
}

static void FANTASY_SetActiveCamera(TEscena3D *self, int n)
{
    self->active_camera = n;
}

static void FANTASY_SetSpeed(TEscena3D *self, float speed)
{
    self->velocidad = speed;
}

static void FANTASY_SetDynLights(TEscena3D *self, int yesno)
{
    self->dyn_lights = yesno;
}

static void FANTASY_SetZBufferClear(TEscena3D *self, int yesno)
{
    self->ZBufferClear = yesno;
}

static void FANTASY_SetGlobalIlum(TEscena3D *self, float ilum)
{
    self->globalIlum = ilum;
    self->fadeticks = 0;
}

static void FANTASY_SetFade(TEscena3D *self, float ilumOrigen,float ilumDestino, float time)
{
 self->globalIlum = ilumOrigen;
 self->fadeticks = (int)(time * 1000.0f); 
 self->deltaIlum = (ilumDestino - ilumOrigen) / self->fadeticks;
}

static void FANTASY_SetGlobalAlpha(TEscena3D *self, float alpha)
{
    self->globalAlpha = alpha;
    self->alphafadeticks = 0;
}

static void FANTASY_SetAlphaFade(TEscena3D *self, float alphaOrigen,float alphaDestino, float time)
{
 self->globalAlpha = alphaOrigen;
 self->alphafadeticks = (int)(time * 1000.0f); 
 self->deltaAlpha = (alphaDestino - alphaOrigen) / (float)(self->alphafadeticks);
}

static void FANTASY_SetFrame(TEscena3D *self, int framenum)
{
 self->last_frame = (float)(framenum);
}


static void FANTASY_SetFog(TEscena3D *self, int yesno,int fog_type,float fog_r,float fog_g,float fog_b,float density,int fog_invert)
{
 if(yesno)
 {
  self->fogenable = 1;
  self->fog_type = fog_type;
  self->fog_r = fog_r;
  self->fog_g = fog_g;
  self->fog_b = fog_b;
  self->fog_density = density;
  self->fog_invert = fog_invert;
 }
 else
  self->fogenable = 0;
}

static void FANTASY_SetUChange(TEscena3D *self, char *object_name, float speed)
{
 engine->SetScene(self->handle);	
 engine->SetUChange(object_name,speed);    
}

static void FANTASY_SetVChange(TEscena3D *self, char *object_name, float speed)
{
 engine->SetScene(self->handle);	
 engine->SetVChange(object_name,speed);   
}

static void FANTASY_SetVertexProgram(TEscena3D *self, char *object_name, char *filename)
{
 engine->SetScene(self->handle);
 engine->LoadVertexProgram(object_name,filename);   
}


static int FANTASY_GetWorldPointer(TEscena3D *self)
{
 void *pointer;
 
 engine->SetScene(self->handle);
 pointer = engine->GetWorldPointer();
 return (int)pointer;
}

static int FANTASY_GetAPI(TEscena3D *self)
{
 void * pointer;
 engine->SetScene(self->handle);
 pointer = engine->GetAPI();
 return (int)pointer;	
}

// Tipos de render para cada objeto (por defecto es RENDER_NORMAL)
// RENDER_NORMAL 		0
// RENDER_WIRE   		1
// RENDER_FLAT   		2
// RENDER_NORMALWIRE		3
// RENDER_CARTOON		4

static void FANTASY_SetRenderMode(TEscena3D *self, char *obj_name,int mode)
{
 engine->SetScene(self->handle);	
 engine->SetRenderMode(obj_name,mode);   
}

static float FANTASY_GetPos_X(TEscena3D *self, char *obj_name, float frame)
{
 engine->SetScene(self->handle);
 return engine->GetPos(obj_name,frame,0);	
}

static float FANTASY_GetPos_Y(TEscena3D *self, char *obj_name, float frame)
{
 engine->SetScene(self->handle);
 return engine->GetPos(obj_name,frame,1);	
}

static float FANTASY_GetPos_Z(TEscena3D *self, char *obj_name, float frame)
{
 engine->SetScene(self->handle);
 return engine->GetPos(obj_name,frame,2);	
}

static float FANTASY_GetScale_X(TEscena3D *self, char *obj_name, float frame)
{
 engine->SetScene(self->handle);
 return engine->GetScale(obj_name,frame,0);	
}

static float FANTASY_GetScale_Y(TEscena3D *self, char *obj_name, float frame)
{
 engine->SetScene(self->handle);
 return engine->GetScale(obj_name,frame,1);	
}

static float FANTASY_GetScale_Z(TEscena3D *self, char *obj_name, float frame)
{
 engine->SetScene(self->handle);
 return engine->GetScale(obj_name,frame,2);	
}

static float FANTASY_GetLastFrame(TEscena3D *self)
{
 return self->last_frame;
}


//r3D
static void FANTASY_SetPos_X(TEscena3D *self, char *obj_name, float x, int mode, int type)
{
 float xyz[3];
 xyz[0]=x;
 xyz[1]=0.0;
 xyz[2]=0.0;	
 engine->SetScene(self->handle);
 engine->SetPos(obj_name,0,xyz,mode,self->last_frame,type);	
}

static void FANTASY_SetPos_Y(TEscena3D *self, char *obj_name, float y, int mode, int type)
{
 float xyz[3];
 xyz[0]=0.0;
 xyz[1]=y;
 xyz[2]=0.0;		
 engine->SetScene(self->handle);
 engine->SetPos(obj_name,1,xyz,mode,self->last_frame,type);	
}

static void FANTASY_SetPos_Z(TEscena3D *self, char *obj_name, float z, int mode, int type)
{
 float xyz[3];
 xyz[0]=0.0;
 xyz[1]=0.0;
 xyz[2]=z;		
 engine->SetScene(self->handle);
 engine->SetPos(obj_name,2,xyz,mode,self->last_frame,type);	
}

static void FANTASY_SetScale_X(TEscena3D *self, char *obj_name, float scalex, int mode)
{
 float xyz[3];
 xyz[0]=scalex;
 xyz[1]=0.0;
 xyz[2]=0.0;		
 engine->SetScene(self->handle);
 engine->SetScale(obj_name,0,xyz,mode,self->last_frame);	
}

static void FANTASY_SetScale_Y(TEscena3D *self, char *obj_name, float scaley, int mode)
{
 float xyz[3];
 xyz[0]=0.0;
 xyz[1]=scaley;
 xyz[2]=0.0;	
 engine->SetScene(self->handle);
 engine->SetScale(obj_name,1,xyz,mode,self->last_frame);	
}

static void FANTASY_SetScale_Z(TEscena3D *self, char *obj_name, float scalez, int mode)
{
 float xyz[3];
 xyz[0]=0.0;
 xyz[1]=0.0;
 xyz[2]=scalez;	
 engine->SetScene(self->handle);
 engine->SetScale(obj_name,2,xyz,mode,self->last_frame);	
}

static void FANTASY_SetAnimateFlag(TEscena3D *self, char *obj_name, int flag, int type)
{
 engine->SetScene(self->handle);
 engine->SetAnimateFlag(obj_name,flag,type);
}

static void FANTASY_SetMaterial(TEscena3D *self, char *obj_name, char *material_name)
{
 engine->SetScene(self->handle); 
 engine->SetMaterial(obj_name,material_name);
}

static void FANTASY_SetFov(TEscena3D *self, char *camera_name, float value)
{
 engine->SetScene(self->handle); 
 engine->SetFov(camera_name,value);
}

static float FANTASY_GetFov(TEscena3D *self, char *camera_name)
{
 engine->SetScene(self->handle); 
 return engine->GetFov(camera_name);
}

static void FANTASY_SetRoll(TEscena3D *self, char *camera_name, float value)
{
 engine->SetScene(self->handle); 
 engine->SetRoll(camera_name,value);
}

static float FANTASY_GetRoll(TEscena3D *self, char *camera_name)
{
 engine->SetScene(self->handle); 
 return engine->GetRoll(camera_name);
}

static void FANTASY_SetLightMult(TEscena3D *self, char *light_name, float value)
{
 engine->SetScene(self->handle); 
 engine->SetLightMult(light_name,value);
}

static void FANTASY_SetLightColor(TEscena3D *self, char *light_name, float r, float g, float b)
{
 engine->SetScene(self->handle); 
 engine->SetLightColor(light_name,r,g,b);
}

static void FANTASY_SetRotation(TEscena3D *self, char *obj_name, float x, float y, float z, float angle, int mode)
{
 engine->SetScene(self->handle); 
 engine->SetRotation(obj_name,x,y,z,angle,mode,self->last_frame);
}

static void FANTASY_SetObjectAlpha(TEscena3D *self, char *obj_name, float alpha)
{
 engine->SetScene(self->handle);
 engine->SetObjectAlpha(obj_name, alpha);	
}

static void FANTASY_SetSpecialFX(TEscena3D *self, char *obj_name, int fx)
{
 engine->SetScene(self->handle);
 engine->SetSpecialFX(obj_name, fx);	
}

static void FANTASY_SetFXParamf(TEscena3D *self, char *obj_name, int index, float value)
{
 engine->SetScene(self->handle);
 engine->SetFXParamf(obj_name, index, value);		
}

static void FANTASY_SetFXParamu(TEscena3D *self, char *obj_name, int index, unsigned int value)
{
 engine->SetScene(self->handle);
 engine->SetFXParamu(obj_name, index, value);		
}

static void FANTASY_SetShaderParamF(TEscena3D *self, char *mat_name, char *param_name, float value)
{
 engine->SetScene(self->handle);
 engine->SetShaderParamf(mat_name, param_name, value);		
}

static void FANTASY_SetDebugLevel(TEscena3D *self, int debuglevel)
{
 self->debuglevel = debuglevel;	 
}
//-----------------------------------------------------------------------------

EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return "Fantasy 3D Engine";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("Escena3D");
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso concurrentes
    // El resto de las funciones, obligatorias y no obligatorias,
    // el primer parametro que reciben es ese puntero al objeto.

    pMethod = Method_New("New", (void*)FANTASY_New);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)FANTASY_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)FANTASY_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)FANTASY_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)FANTASY_Stop);
    Type_AddMethod(Id, pMethod);

    // Optional Methods
    pMethod = Method_New("SetActiveCamera", (void*)FANTASY_SetActiveCamera);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetSpeed", (void*)FANTASY_SetSpeed);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetDynLights", (void*)FANTASY_SetDynLights);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetZBufferClear", (void*)FANTASY_SetZBufferClear);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetFrame", (void*)FANTASY_SetFrame);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetGlobalIlum", (void*)FANTASY_SetGlobalIlum);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetFade", (void*)FANTASY_SetFade);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetGlobalAlpha", (void*)FANTASY_SetGlobalAlpha);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetAlphaFade", (void*)FANTASY_SetAlphaFade);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);


    pMethod = Method_New("SetFog", (void*)FANTASY_SetFog);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetUChange", (void*)FANTASY_SetUChange);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetVChange", (void*)FANTASY_SetVChange);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("SetVertexProgram", (void*)FANTASY_SetVertexProgram);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("SetRenderMode", (void*)FANTASY_SetRenderMode);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_IntegerId);    
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("GetWorldPointer", (void*)FANTASY_GetWorldPointer);
    Method_SetReturnType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("GetAPI", (void*)FANTASY_GetAPI);
    Method_SetReturnType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("GetPos_X", (void*)FANTASY_GetPos_X);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_SetReturnType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("GetPos_Y", (void*)FANTASY_GetPos_Y);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_SetReturnType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);    
    
    pMethod = Method_New("GetPos_Z", (void*)FANTASY_GetPos_Z);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_SetReturnType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("GetScale_X", (void*)FANTASY_GetScale_X);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_SetReturnType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("GetScale_Y", (void*)FANTASY_GetScale_Y);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_SetReturnType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);    
    
    pMethod = Method_New("GetScale_Z", (void*)FANTASY_GetScale_Z);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_SetReturnType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("GetLastFrame", (void*)FANTASY_GetLastFrame);
    Method_SetReturnType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);    
    
    
    //r3D  
    pMethod = Method_New("SetPos_X", (void*)FANTASY_SetPos_X);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_IntegerId);  
    Method_AddParameterType(pMethod, Type_IntegerId);  
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("SetPos_Y", (void*)FANTASY_SetPos_Y);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_IntegerId);       
    Method_AddParameterType(pMethod, Type_IntegerId);      
    Type_AddMethod(Id, pMethod);    
    
    pMethod = Method_New("SetPos_Z", (void*)FANTASY_SetPos_Z);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_IntegerId);  
    Method_AddParameterType(pMethod, Type_IntegerId);             
    Type_AddMethod(Id, pMethod);    
      
    pMethod = Method_New("SetScale_X", (void*)FANTASY_SetScale_X);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_IntegerId);       
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("SetScale_Y", (void*)FANTASY_SetScale_Y);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_IntegerId);         
    Type_AddMethod(Id, pMethod);    
    
    pMethod = Method_New("SetScale_Z", (void*)FANTASY_SetScale_Z);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_IntegerId);     
    Type_AddMethod(Id, pMethod);    
    
    pMethod = Method_New("SetAnimateFlag", (void*)FANTASY_SetAnimateFlag);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_IntegerId);    
    Method_AddParameterType(pMethod, Type_IntegerId);    
    Type_AddMethod(Id, pMethod);    
    
    pMethod = Method_New("SetMaterial", (void*)FANTASY_SetMaterial);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_StringId);    
    Type_AddMethod(Id, pMethod);
    
    pMethod = Method_New("SetFov", (void*)FANTASY_SetFov);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);      
    
    pMethod = Method_New("GetFov", (void*)FANTASY_GetFov);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_SetReturnType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);          
    
    pMethod = Method_New("SetRoll", (void*)FANTASY_SetRoll);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);      
    
    pMethod = Method_New("GetRoll", (void*)FANTASY_GetRoll);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_SetReturnType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetLightMult", (void*)FANTASY_SetLightMult);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);      
    
    pMethod = Method_New("SetLightColor", (void*)FANTASY_SetLightColor);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);    
    
    pMethod = Method_New("SetRotation", (void*)FANTASY_SetRotation);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_RealId);    
    Method_AddParameterType(pMethod, Type_IntegerId);    
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("SetObjectAlpha", (void*)FANTASY_SetObjectAlpha);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);    

    pMethod = Method_New("SetSpecialFX", (void*)FANTASY_SetSpecialFX);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_IntegerId);    
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetFXParamf", (void*)FANTASY_SetFXParamf);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_IntegerId);    
    Method_AddParameterType(pMethod, Type_RealId);    
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetFXParamu", (void*)FANTASY_SetFXParamu);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_IntegerId);    
    Method_AddParameterType(pMethod, Type_IntegerId);    
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetShaderParamF", (void*)FANTASY_SetShaderParamF);
    Method_AddParameterType(pMethod, Type_StringId);//Material name
    Method_AddParameterType(pMethod, Type_StringId);//Param name(used in the shader)    
    Method_AddParameterType(pMethod, Type_RealId);//Value    
    Type_AddMethod(Id, pMethod);    
    
    pMethod = Method_New("SetDebugLevel", (void*)FANTASY_SetDebugLevel);
    Method_AddParameterType(pMethod, Type_IntegerId);    
    Type_AddMethod(Id, pMethod);    
    
    
}

};

//-----------------------------------------------------------------------------
