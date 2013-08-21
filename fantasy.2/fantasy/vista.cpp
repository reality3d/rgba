#ifdef WINDOZE
 #include <windows.h>
#endif

#include <stdio.h>

#include "vista.h"
#include "3dworld.h"
#include "mates.h"
#include "material.h"
#include "textura.h"
#include "misc.h"
#include "vbo.h"

//#define MEGADEBUG

#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

int screen_width, screen_height;

CacheTexturas *ConjuntoTexturas;


#define PI 3.1415927f




void View::setworld (world_type *wrld, int polycount)
{  
   // Registra la base de datos del mundo actual para usarla con el
   // sistema de vista
    
    world = wrld;
}

void View::SetGlobalAlpha(float alpha)
{
 raster->SetGlobalAlpha(alpha);
 if(alpha > 0.98f) world->mats.SetForceAlwaysBlend(0);
   else world->mats.SetForceAlwaysBlend(1);
 params.globalAlpha = alpha;
}

void View::SetZBufferClear(int yesno)
{
 raster->SetZBufferClear(yesno);
}


void View::display (view_type curview)
{
 int i,j,spheremap;
 float dist;

#ifdef MEGADEBUG
   sprintf (FANTASY_DEBUG_STRING, "->Entrando en display. Tenemos %d objetos\n",world->number_of_objects);
   Debug(DEBUG_LEVEL_OVERDEBUG);
#endif        
   // Inicializacion de la pantalla
   raster->InitRender();
   raster->SetFov(curview.fov);

   if(curview.fog) 	// Tenemos niebla
   {    raster->SetFog(curview.fog_type,curview.fog_r,curview.fog_g,curview.fog_b,curview.fog_density,curview.near_range,curview.far_range,curview.fog_invert);
   }

   world->mats.ResetMatParameters(); // Los materiales cogen parámetros por defecto

   // Se pasan las coordenadas a la posici¢n actual de la vista    
   if(curview.using_cam) alignview (world, curview);

#ifdef MEGADEBUG 
   sprintf (FANTASY_DEBUG_STRING, "->Dibujo\n");
   Debug(DEBUG_LEVEL_OVERDEBUG);
#endif  

   // Dibujamos los objetos
   draw_objects ();

#ifdef MEGADEBUG
   sprintf (FANTASY_DEBUG_STRING, "->He dibujado\n");
   Debug(DEBUG_LEVEL_OVERDEBUG);
#endif
   
   // aquí lo que tendría que hacer es dibujar los flares, si aplican
   
   raster->FinRender();   
}



void View::draw_objects(void)
{
 int i,j;
 object_type *obj;

 // En primer lugar podríamos hacer algún tipo de descarte de objetos. De momento no se contempla.

 // Solo se calcula la iluminación dinámica si es necesario
 if(params.dyn_lighting)
 {
  raster->ProcessLights(world);   
 }
  // Primero los opacos

  for(i=0;i<world->number_of_objects;i++)
  {  	  	  	
   obj = &(world->obj[i]); // Cogemos el objeto
      
   if((!(obj->visible)) || (obj->parentid != 0)) continue; // En caso de que el objeto no sea visible, pasamos de el

   if(obj->normalize) // Hay que recalcular las normales
   {
    calcula_normales_poligono(obj); 
    calcula_normales_vertice(obj);
    obj->normalize=0;
   }
   if(obj->updatevertices) // hay que actualizar los buffers
   {
    UpdateBuffers(obj,UPDATE_VERTICES);
    obj->updatevertices=0;	
   }

   if(world->mats.isOpaque(obj->num_material)) 
   {    
   #ifdef MEGADEBUG
   	sprintf (FANTASY_DEBUG_STRING, "Drawing opaque object %s\n",obj->nombre_objeto);
   	Debug(DEBUG_LEVEL_OVERDEBUG);
   #endif
    raster->DrawObject(obj,params.globalIlum,world);
   }
  }

  // Luego los transparentes
  
  for(i=0;i<world->number_of_objects;i++)
  {
   obj = &(world->obj[i]); // Cogemos el objeto   
   if((!(obj->visible)) || (obj->parentid != 0)) continue; // En caso de que el objeto no sea visible, pasamos de el
   
   if((world->mats.isOpaque(obj->num_material))==0) 
   {
    #ifdef MEGADEBUG
   	sprintf (FANTASY_DEBUG_STRING, "Drawing transp object %s\n",obj->nombre_objeto);
   	Debug(DEBUG_LEVEL_OVERDEBUG);
    #endif
    raster->DrawObject(obj,params.globalIlum,world);
   }
  }  
}


void View::alignview (world_type * world, view_type view)
{
    Vector up;
    Vector zaxis;
    Vector yaxis;
    float angle;
    Quaternion q; 
    float rotmatrix[4][4];

    // Para conseguir el vector UP, rotamos el punto (0,1,0) alrededor del eje Z de la cámara. Ahora rotamos la orientación del target node

    //angle = ((view.inclinacion)*PI)/180.0f;

    if(world->camaras[world->current_camera].numero_keys_path_target_rotacion>1)
    {
	    angle = -view.inclinacion;
	
	    zaxis.x = view.wx - view.wxt;
	    zaxis.y = view.wy - view.wyt;
	    zaxis.z = - view.wz + view.wzt;	// Las Zs están invertidas a todo lo largo del engine.....
	 
	    Normaliza(&zaxis);
	
	/* We now Use the target rotation as up node
	
	    yaxis.x = 0.0f;
	    yaxis.y = 1.0f;
	    yaxis.z = 0.0f;*/
	    
	    Quat_to_RotMatrix(&(world->camaras[world->current_camera].current_rotation), rotmatrix);
	    yaxis.x = rotmatrix[0][1];
	    yaxis.y = rotmatrix[1][1];
	    yaxis.z = rotmatrix[2][1];    
	
	    // Calculamos el Quaternio asociado, de ahí sacamos la matriz de rotación...
	
	    Axis_to_Quat(zaxis.x,zaxis.y,zaxis.z,angle,&q);
	    Quat_to_RotMatrix(&q, rotmatrix);
	
	    // Y aplicamos la rotación al eje
	
	    Mult_Vector_Matrix(&yaxis,rotmatrix, &up);
	    Normaliza(&up);
	
	/*
	    up.x = cos(angle);
	    up.y = sin(angle);
	    up.z = 0.0f;
	*/
	//    printf("Zaxis: %f %f %f\n",zaxis.x,zaxis.y,zaxis.z);
	//    printf("Angulo %f; Up: %f %f %f\n",view.inclinacion,up.x,up.y,up.z);
    }
    else //No hay keys del target, no tenemos upnode con el que orientarnos realmente entonces
    {	
    	 yaxis.x = 0.0f;
	 yaxis.y = 1.0f;
	 yaxis.z = 0.0f;
    }
    raster->SetCameraPos(view.wx,view.wy,view.wz,view.wxt,view.wyt,view.wzt,up.x,up.y,up.z);    
    
}

void View::SetDynLighting(int dynlighting)
{
 params.dyn_lighting=dynlighting;
 raster->SetDynLighting(dynlighting);
}

void View::LoadVertexProgram(object_type *obj,char *filename)
{
 raster->LoadVertexProgram(&(obj->vtx_shader_id),filename);	
}

void View::SetSpecialFX(object_type *obj,int fxtype)
{
 raster->SetSpecialFX(obj,fxtype);	
}

void View::DisplaceTexcoords(object_type *obj)
{
 raster->DisplaceTexcoords(obj);
}

View::View()
{
 raster=new Raster();
 params.dyn_lighting=1;

 params.globalIlum=1.0f;
 params.globalAlpha=1.0f;
 ConjuntoTexturas=new CacheTexturas();
}

View::~View()
{
 delete raster;
 delete ConjuntoTexturas;
}

