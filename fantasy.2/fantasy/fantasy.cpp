#include <math.h>
#include <string.h>
#include <stdio.h>

#include "fantasy.h"
#include "carga.h"
#include "mates.h"
#include "misc.h"
#include "quatern.h"
#include "material.h"
#include "interp.h"
#include "fantasy_api.h"
#include "extgl.h"
#include "vbo.h"

// #define ENABLE_HARDWARE_SKINNING

Fantasy::Fantasy(int maxscenes)
{
    int i;
    
    worlds =  new world_type[maxscenes];
    polycount = new int[maxscenes];
    previous_time = new float[maxscenes];
    for(i=0;i<maxscenes;i++) previous_time[i]=0.0f;
    
    max_scenes=maxscenes;
    num_scenes=0;
    current_camera=0;
    current_scene=0;
    
    render_path = EXTGL_render_path;
    curview.fog = 0;
}

Fantasy::~Fantasy()
{
    delete worlds;
    delete polycount;
    delete previous_time;
}

int Fantasy::Load(char *anim_filename,char *mat_filename)
{
    int i;

    if(num_scenes >= max_scenes) return -1; // Overflow de escenas

 sprintf (FANTASY_DEBUG_STRING, "Cargando materiales %s\n",mat_filename);
 Debug(DEBUG_LEVEL_INFO);

    worlds[num_scenes].number_of_materials = worlds[num_scenes].mats.MatReader(mat_filename);
    if(worlds[num_scenes].number_of_materials == -1) return -1;

 sprintf (FANTASY_DEBUG_STRING, "Cargando %s\n",anim_filename);
 Debug(DEBUG_LEVEL_INFO);

    polycount[num_scenes] = Load3DFile(&(worlds[num_scenes]),anim_filename);
    if(polycount[num_scenes]== -1) return -1;
    worlds[num_scenes].current_camera=0;

 sprintf (FANTASY_DEBUG_STRING, "Cargado %s\n",anim_filename);

    //DumpWorld(&(worlds[num_scenes]));
            
    for(i=0;i<worlds[num_scenes].number_of_objects;i++)
    {
     AdjustVertices(&(worlds[num_scenes].obj[i]));	           
    }

    for(i=0;i<worlds[num_scenes].number_of_objects;i++)
    {  	
     calcula_normales_poligono(&(worlds[num_scenes].obj[i])); 
     calcula_normales_vertice(&(worlds[num_scenes].obj[i]));
    }

    for(i=0;i<worlds[num_scenes].number_of_objects;i++)
    {
      if(worlds[num_scenes].obj[i].BoneIds)
      {
      	// Since this object has bones, we load the appropriate vertex program
#ifdef ENABLE_HARDWARE_SKINNING      	
      	winview.LoadVertexProgram(&(worlds[num_scenes].obj[i]),"shaders/skin_shader.txt");	
#endif
      	if(worlds[num_scenes].obj[i].vtx_shader_id==0xffffffff)
      	{
      	 // As we will do soft skinning, set hint_dynamic to 1
      	 worlds[num_scenes].obj[i].hint_dynamic = 1;
      		
 	 sprintf (FANTASY_DEBUG_STRING, "Oops, this card won't do hardware skinning\n");
 	 Debug(DEBUG_LEVEL_INFO);      		
      	}
      }
    }
    if(EXTGL_vertex_buffer_objects)
    {
 	sprintf (FANTASY_DEBUG_STRING, "Uploading vertex buffers\n");
 	Debug(DEBUG_LEVEL_INFO);

	    for(i=0;i<worlds[num_scenes].number_of_objects;i++)
    	    {
     		UploadBuffers(&(worlds[num_scenes].obj[i]));	           
    	    }
   	sprintf (FANTASY_DEBUG_STRING, "Vertex buffers uploaded\n");
  	Debug(DEBUG_LEVEL_INFO);
    }
    prepara_pos_animacion(&(worlds[num_scenes]));   
    preprocesa_rotaciones(&(worlds[num_scenes]));       
    preprocesa_roll_and_fov(&(worlds[num_scenes]));
    
    
    

    num_scenes++;
    
   	sprintf (FANTASY_DEBUG_STRING, "Succesfully loaded scene %s\n",anim_filename);
 	Debug(DEBUG_LEVEL_INFO);

    return (num_scenes-1);
}

void Fantasy::SetScene(int scene)
{
    if(scene >= num_scenes) return;
    current_scene=scene;
    winview.setworld (&(worlds[current_scene]), polycount[current_scene]);
}

void Fantasy::SetCamera(int camera)
{
    if(camera>=worlds[current_scene].number_of_camaras) 
    {
   	sprintf (FANTASY_DEBUG_STRING, "Warning, trying to set an invalid camera \n");
 	Debug(DEBUG_LEVEL_WARNING);    	
    	current_camera=0;	
    }
    else
    {
    	current_camera=camera;
    }
    worlds[current_scene].current_camera=camera;
}

// Control de uso de luces dinámicas o normales

void Fantasy::SetDynLighting(int dynlighting)
{
 winview.SetDynLighting(dynlighting);
}

// Control de la niebla

void Fantasy::SetFog(int yesno,int fog_type,float fog_r,float fog_g,float fog_b,float density,int fog_invert)
{
 if(yesno) // Habilitamos la niebla 
 {
  curview.fog=1;
  curview.fog_type = fog_type;
  curview.fog_r = fog_r;
  curview.fog_g = fog_g;
  curview.fog_b = fog_b;
  curview.fog_density = density;  
  curview.fog_invert = fog_invert;
 }
 else
  curview.fog = 0;
}

// Control de la luz global (útil para los crossfades)

void Fantasy::SetGlobalIlum(float ilum)
{
 winview.params.globalIlum=ilum;
}

// Control del alpha global (también útil para los crossfades)

void Fantasy::SetGlobalAlpha(float alpha)
{
 winview.SetGlobalAlpha(alpha);
}

void Fantasy::SetZBufferClear(int yesno)
{
 winview.SetZBufferClear(yesno);
}


void Fantasy::CleanupTextures(int scene)
{
    int i,j;
    
    if(scene >= num_scenes) return;
    worlds[scene].mats.CleanupTextures();
    worlds[scene].mats.CleanupShaderObjects();
    
    // Esto debería tener su propia función, pero de momento va aquí
    for(i=0;i<worlds[scene].number_of_objects;i++)
    {
     	DestroyBuffers(&(worlds[scene].obj[i]));	           
     }            
}

void Fantasy::SetUChange(char *object_name, float speed)
{
 int i;
 
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  if(strmatch(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  {
  	worlds[current_scene].obj[i].USpeed = speed;
  }
 }	
}

void Fantasy::SetVChange(char *object_name, float speed)
{
 int i;
 
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  if(strmatch(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  {
  	worlds[current_scene].obj[i].VSpeed = speed;
  }
 }	
}


void Fantasy::DrawFrame(float frame,float tiempo)
{
    int i,j;
    float offset;
    world_type *world=&(worlds[current_scene]);
    // Por petición del público, el fantasy va a hacer loop a partir de ahora
    //    if((frame < world->frame_inicial) || (frame > world->frame_final)) return;
 
    if(frame > world->frame_final) 
    {
     while (frame > world->frame_final) frame -= world->num_frames-1;     
    }
    else if (frame < world->frame_inicial) 
    {
     while (frame < world->frame_inicial) frame += world->num_frames-1;
    }
    
    if( winview.params.globalIlum < 0.05f) return;
    if( winview.params.globalAlpha < 0.05f) return;
    
 /* se recorren los objetos del mundo, que cambian de posici¢n en la
    animaci¢n, y se realiza la actualizaci¢n en funci¢n del frame actual */

// sprintf (FANTASY_DEBUG_STRING, "En actualiza_animacion tenemos %d objetos\n",world->number_of_objects);
 // Debug(DEBUG_LEVEL_OVERDEBUG);

 sprintf (FANTASY_DEBUG_STRING, "Drawing frame %f of scene %d\n",frame,current_scene);
 Debug(DEBUG_LEVEL_INFO);

 for(i=0;i<world->number_of_objects;i++)
 {
   if(world->obj[i].animate)  // Si queremos que el objeto tenga animacion
   {

     if(world->obj[i].numero_keys_path>1) // si tiene alg£n movimiento 
     {    	
       InterpolaPosicion(world->obj[i].numero_keys_path, 
      			world->obj[i].tangentes_entrada,
      			world->obj[i].tangentes_salida,
      			world->obj[i].posiciones_keys,
      			frame,
      			&(world->obj[i].x),&(world->obj[i].y),&(world->obj[i].z));      		      
     }     
     if(world->obj[i].numero_keys_path_rotacion>1)
     {
       InterpolaRotacion(world->obj[i].numero_keys_path_rotacion, 
      			world->obj[i].tangentes_entrada_rotacion,
      			world->obj[i].tangentes_salida_rotacion,
      			world->obj[i].path_rotacion,
      			frame,
      			&(world->obj[i].current_rotation));
     }         
     if(world->obj[i].numero_keys_path_scale > 1)
     {
       InterpolaScale(world->obj[i].numero_keys_path_scale, 
      			world->obj[i].tangentes_entrada_scale,
      			world->obj[i].tangentes_salida_scale,
      			world->obj[i].scale_keys,
      			frame,
      			&(world->obj[i].xscale),&(world->obj[i].yscale),&(world->obj[i].zscale));            
      			
      	// This is a hack!!!!
      	world->obj[i].xscale /= world->obj[i].scale_keys[0].scalex;
      	world->obj[i].yscale /= world->obj[i].scale_keys[0].scaley;
      	world->obj[i].zscale /= world->obj[i].scale_keys[0].scalez;
      	
      	// End of hack
     }
    } 
     // Añadido: si el objeto tiene animación de texturas, las animamos ahorita mismo
     
    
     if(fabs(world->obj[i].USpeed) > 0.00001f)
     {
      offset = (tiempo - previous_time[current_scene]) * world->obj[i].USpeed;
      world->obj[i].UTranslate=offset;
     }
     
     if(fabs(world->obj[i].VSpeed) > 0.00001f)
     {
      offset = (tiempo - previous_time[current_scene]) * world->obj[i].VSpeed;
      world->obj[i].VTranslate=offset;
     }
     
     if((fabs(world->obj[i].VSpeed) > 0.00001f) ||(fabs(world->obj[i].USpeed) > 0.00001f))
       winview.DisplaceTexcoords(&(world->obj[i]));     
 }

 /* idem con las fuentes de luz */

 for(i=0;i<world->number_of_fuentes;i++) 
 {
   if(world->fuentes[i].animate)  // Si queremos que la luz tenga animacion
   {
   	if(world->fuentes[i].numero_keys_path>1) 
   	{ /* la fuente tiene alg£n movimiento */      
      	InterpolaPosicion(world->fuentes[i].numero_keys_path, 
      				world->fuentes[i].tangentes_entrada,
      				world->fuentes[i].tangentes_salida,
      				world->fuentes[i].posiciones_keys,
      				frame,
      				&(world->fuentes[i].wx),&(world->fuentes[i].wy),&(world->fuentes[i].wz));      
   	}
    	if(world->fuentes[i].numero_keys_direction>1) /* la direccion de la fuente tiene alg£n movimiento */
    	{
       InterpolaPosicion(world->fuentes[i].numero_keys_direction,
	              	  	world->fuentes[i].tangentes_direction_entrada,
		        	world->fuentes[i].tangentes_direction_salida,
		        	world->fuentes[i].direction_keys,
		        	frame,
		        	&(world->fuentes[i].tx),&(world->fuentes[i].ty),&(world->fuentes[i].tz));      
    	}     	
   	
   }
   
 }


 /* lo mismo para las c maras */

 for(i=0;i<world->number_of_camaras;i++)
 {
   if(world->camaras[i].animate)  // Si queremos que la camara tenga animacion
   {
   	if(world->camaras[i].numero_keys_path>1)
   	{ /* tiene alg£n movimiento */
    	InterpolaPosicion(world->camaras[i].numero_keys_path, 
      				world->camaras[i].tangentes_entrada,
      				world->camaras[i].tangentes_salida,
      				world->camaras[i].posiciones_keys,
      				frame,
      				&(world->camaras[i].wx),&(world->camaras[i].wy),&(world->camaras[i].wz));          
   	}
   }
   if(world->camaras[i].animate_target)  // Si queremos que el target de camara tenga animacion
   {
   	if(world->camaras[i].numero_keys_path_target>1)
   	{ /* tiene alg£n movimiento */
      	InterpolaPosicion(world->camaras[i].numero_keys_path_target, 
      				world->camaras[i].tangentes_entrada_target,
      				world->camaras[i].tangentes_salida_target,
      				world->camaras[i].posiciones_keys_target,
      				frame,
      				&(world->camaras[i].wxt),&(world->camaras[i].wyt),&(world->camaras[i].wzt));             
    	}
    	if(world->camaras[i].numero_keys_path_target_rotacion>1)
     	{
        InterpolaRotacion(world->camaras[i].numero_keys_path_target_rotacion, 
      			world->camaras[i].tangentes_entrada_target_rotacion,
      			world->camaras[i].tangentes_salida_target_rotacion,
      			world->camaras[i].path_target_rotacion,
      			frame,
      			&(world->camaras[i].current_rotation));
     }   
   }
   if(world->camaras[i].numero_keys_rollpath>1) // El roll varia
   {
      InterpolaRoll(world->camaras[i].numero_keys_rollpath,
		   world->camaras[i].tangentes_entrada_roll,
		   world->camaras[i].tangentes_salida_roll,
		   world->camaras[i].posiciones_rollkeys,
		   frame,
		   &(world->camaras[i].inclinacion));
   } 

   if(world->camaras[i].numero_keys_fovpath>1) // El fov varia
   {
      InterpolaFov(world->camaras[i].numero_keys_fovpath,
		   world->camaras[i].tangentes_entrada_fov,
		   world->camaras[i].tangentes_salida_fov,
		   world->camaras[i].posiciones_fovkeys,
		   frame,
		   &(world->camaras[i].fov));		  		   
   }  
 }

 /* Idem para los bones */
 
 
 for(i=0;i<world->number_of_bones;i++)
 {
   if(world->bones[i].numero_keys_path>1)
   {
      InterpolaPosicion(world->bones[i].numero_keys_path, 
      			world->bones[i].tangentes_entrada,
      			world->bones[i].tangentes_salida,
      			world->bones[i].posiciones_keys,
      			frame,
      			&(world->bones[i].x),&(world->bones[i].y),&(world->bones[i].z));  
   }
   if(world->bones[i].numero_keys_path_rotacion>1)
   {
       InterpolaRotacion(world->bones[i].numero_keys_path_rotacion, 
      			world->bones[i].tangentes_entrada_rotacion,
      			world->bones[i].tangentes_salida_rotacion,
      			world->bones[i].path_rotacion,
      			frame,
      			&(world->bones[i].current_rotation));
   }         
   if(world->bones[i].numero_keys_path_scale > 1)
   {
       InterpolaScale(world->bones[i].numero_keys_path_scale, 
      			world->bones[i].tangentes_entrada_scale,
      			world->bones[i].tangentes_salida_scale,
      			world->bones[i].scale_keys,
      			frame,
      			&(world->bones[i].xscale),&(world->bones[i].yscale),&(world->bones[i].zscale));            
      	
      	world->bones[i].xscale /= world->bones[i].scale_keys[0].scalex;
      	world->bones[i].yscale /= world->bones[i].scale_keys[0].scaley;
      	world->bones[i].zscale /= world->bones[i].scale_keys[0].scalez;

     }


 }
 
  // One last step is to update the bone matrices according to the hierarchy and so...
  
  UpdateBoneMatrices(world);

  if(current_camera != -1)
  {
   curview.wx=world->camaras[current_camera].wx;
   curview.wy=world->camaras[current_camera].wy;
   curview.wz=world->camaras[current_camera].wz;
   curview.wxt=world->camaras[current_camera].wxt;
   curview.wyt=world->camaras[current_camera].wyt;
   curview.wzt=world->camaras[current_camera].wzt;
   curview.inclinacion=world->camaras[current_camera].inclinacion;
   curview.fov=world->camaras[current_camera].fov;
   curview.near_range = world->camaras[current_camera].near_range;
   curview.far_range = world->camaras[current_camera].far_range;
   curview.using_cam=1;
  }
  else  curview.using_cam=0;
 
  /* se vuelca el frame actual */
  winview.display (curview);

  /* Actualizamos el tiempo */
  
  previous_time[current_scene] = tiempo;
}





void Fantasy::prepara_pos_animacion(world_type *world)
{
int i,j,k;
float angx,angy;
float angx2,angy2;
float x1,y1,z1;
float x2,y2,z2;
float x1c,y1c,z1c;
float x2c,y2c,z2c;

int d1,d2;

/* para cada objeto del mundo, se comprueba si tiene movimiento, y en
   caso afirmativo, se rellena un array con la informaci¢n necesaria
   para ubicar el objeto en cada frame en funci¢n de los datos
   proporcionados */

 for(i=0;i<world->number_of_objects;i++)
 {
    if(world->obj[i].numero_keys_path>1)    /* el objeto tiene alg£n movimiento */
    {      
     interpola_spline (world->obj[i].numero_keys_path,
	               &(world->obj[i].tangentes_entrada),
		       &(world->obj[i].tangentes_salida),
		       world->obj[i].posiciones_keys);		           
    }

    // Además de interpolar el path de movimiento, interpolamos el de escalado, si aplica

    if(world->obj[i].numero_keys_path_scale>1)
    {
    /* el objeto tiene alg£n movimiento */
     interpola_spline_scale (world->obj[i].numero_keys_path_scale,
	               &(world->obj[i].tangentes_entrada_scale),
		       &(world->obj[i].tangentes_salida_scale),
		       world->obj[i].scale_keys);		           
    }
 }

 /* igual para las c maras */

 for(i=0;i<world->number_of_camaras;i++)
 {
     if(world->camaras[i].numero_keys_path>1)
     {
      interpola_spline (world->camaras[i].numero_keys_path,
	               &(world->camaras[i].tangentes_entrada),
		       &(world->camaras[i].tangentes_salida),
		       world->camaras[i].posiciones_keys);
     }
     if(world->camaras[i].numero_keys_path_target>1)          /* el objetivo tiene alg£n movimiento */
     {
      interpola_spline (world->camaras[i].numero_keys_path_target,
	               &(world->camaras[i].tangentes_entrada_target),
		       &(world->camaras[i].tangentes_salida_target),
		       world->camaras[i].posiciones_keys_target);

     }
 }
 /* idem para las fuentes de luz */

 for(i=0;i<world->number_of_fuentes;i++)
 {
    if(world->fuentes[i].numero_keys_path>1) /* la fuente tiene alg£n movimiento */
    {
       interpola_spline(world->fuentes[i].numero_keys_path,
	                &(world->fuentes[i].tangentes_entrada),
		        &(world->fuentes[i].tangentes_salida),
		        world->fuentes[i].posiciones_keys);
    }
    if(world->fuentes[i].numero_keys_direction>1) /* la direccion de la fuente tiene alg£n movimiento */
    {
       interpola_spline(world->fuentes[i].numero_keys_direction,
	                &(world->fuentes[i].tangentes_direction_entrada),
		        &(world->fuentes[i].tangentes_direction_salida),
		        world->fuentes[i].direction_keys);
    }    
    
 }
 /* idem para los bones */
 
 for(i=0;i<world->number_of_bones;i++)
 {
    if(world->bones[i].numero_keys_path>1) /* la fuente tiene alg£n movimiento */
    {
       interpola_spline(world->bones[i].numero_keys_path,
	                &(world->bones[i].tangentes_entrada),
		        &(world->bones[i].tangentes_salida),
		        world->bones[i].posiciones_keys);
    } 	
    if(world->bones[i].numero_keys_path_scale>1) /* el bone tiene alg£n movimiento */
    {    
     interpola_spline_scale (world->bones[i].numero_keys_path_scale,
	               &(world->bones[i].tangentes_entrada_scale),
		       &(world->bones[i].tangentes_salida_scale),
		       world->bones[i].scale_keys);		           
    }
 }
}


void Fantasy::preprocesa_rotaciones(world_type *world)
{
    int i,j;
    Quaternion temp;
    
    for(i=0;i<world->number_of_objects;i++)
    {
        if(world->obj[i].numero_keys_path_rotacion>1)
        {
            /* el objeto tiene alguna rotaci¢n */
            /* En primer lugar hay que convertir los quaternios relativos */
            /* en absolutos, para luego poder interpolarlos */

            for(j=1;j<world->obj[i].numero_keys_path_rotacion;j++)
            {
// Now you have to convert your rotation track from relative to absolute
// representation. Just multiply the quaternions, beginning with the second,
// like this: Qn' = Q(n-1) x Qn,
// where Q(n-1) is already absolute and 'x' is the quaternion multiplication

                Prod_Quat(&(world->obj[i].path_rotacion[j-1].quat),
                          &(world->obj[i].path_rotacion[j].quat),
                          &temp);
                world->obj[i].path_rotacion[j].quat.x = temp.x;
                world->obj[i].path_rotacion[j].quat.y = temp.y;
                world->obj[i].path_rotacion[j].quat.z = temp.z;
                world->obj[i].path_rotacion[j].quat.w = temp.w;
            }
            
            // Ahora llega el momento de interpolar
       	   interpola_rotaciones (world->obj[i].numero_keys_path_rotacion,
	               &(world->obj[i].tangentes_entrada_rotacion),
		       &(world->obj[i].tangentes_salida_rotacion),
		       world->obj[i].path_rotacion);
        }
    }
    
    /* Repetimos la operación, ahora para los bones */

    for(i=0;i<world->number_of_bones;i++)
    {
        if(world->bones[i].numero_keys_path_rotacion>1)
        {
            /* el bone tiene alguna rotaci¢n (es de esperar :)*/
            /* En primer lugar hay que convertir los quaternios relativos */
            /* en absolutos, para luego poder interpolarlos */

            for(j=1;j<world->bones[i].numero_keys_path_rotacion;j++)
            {
// Now you have to convert your rotation track from relative to absolute
// representation. Just multiply the quaternions, beginning with the second,
// like this: Qn' = Q(n-1) x Qn,
// where Q(n-1) is already absolute and 'x' is the quaternion multiplication

                Prod_Quat(&(world->bones[i].path_rotacion[j-1].quat),
                          &(world->bones[i].path_rotacion[j].quat),
                          &temp);
                world->bones[i].path_rotacion[j].quat.x = temp.x;
                world->bones[i].path_rotacion[j].quat.y = temp.y;
                world->bones[i].path_rotacion[j].quat.z = temp.z;
                world->bones[i].path_rotacion[j].quat.w = temp.w;
            }
            
            // Ahora llega el momento de interpolar
       	   interpola_rotaciones (world->bones[i].numero_keys_path_rotacion,
	               &(world->bones[i].tangentes_entrada_rotacion),
		       &(world->bones[i].tangentes_salida_rotacion),
		       world->bones[i].path_rotacion);
        }
    }

    /* Repetimos la operación, ahora para el target de la camara */

    for(i=0;i<world->number_of_camaras;i++)
    {
        if(world->camaras[i].numero_keys_path_target_rotacion>1)
        {
            /* el target tiene alguna rotaci¢n (es de esperar :)*/
            /* En primer lugar hay que convertir los quaternios relativos */
            /* en absolutos, para luego poder interpolarlos */

            for(j=1;j<world->camaras[i].numero_keys_path_target_rotacion;j++)
            {
// Now you have to convert your rotation track from relative to absolute
// representation. Just multiply the quaternions, beginning with the second,
// like this: Qn' = Q(n-1) x Qn,
// where Q(n-1) is already absolute and 'x' is the quaternion multiplication

                Prod_Quat(&(world->camaras[i].path_target_rotacion[j-1].quat),
                          &(world->camaras[i].path_target_rotacion[j].quat),
                          &temp);
                world->camaras[i].path_target_rotacion[j].quat.x = temp.x;
                world->camaras[i].path_target_rotacion[j].quat.y = temp.y;
                world->camaras[i].path_target_rotacion[j].quat.z = temp.z;
                world->camaras[i].path_target_rotacion[j].quat.w = temp.w;
            }
            
            // Ahora llega el momento de interpolar
       	   interpola_rotaciones (world->camaras[i].numero_keys_path_target_rotacion,
	               &(world->camaras[i].tangentes_entrada_target_rotacion),
		       &(world->camaras[i].tangentes_salida_target_rotacion),
		       world->camaras[i].path_target_rotacion);
        }
    }

}


void Fantasy::preprocesa_roll_and_fov(world_type *world)
{
 int i,j,k;
 int inicio,fin;
 float delta_t,t;
 int numero_keys;

 for(i=0;i<world->number_of_camaras;i++)
 {
     
     if(world->camaras[i].numero_keys_rollpath > 1)
     {
       /*world->camaras[i].posiciones_rollpath=(info_roll_path *) malloc(sizeof(info_roll_path)*world->num_frames);
       interpola_spline_roll(world->camaras[i].posiciones_rollkeys, 
			     world->camaras[i].posiciones_rollpath,numero_keys,world->num_frames);*/
       interpola_spline_roll (world->camaras[i].numero_keys_rollpath,
	               &(world->camaras[i].tangentes_entrada_roll),
		       &(world->camaras[i].tangentes_salida_roll),
		       world->camaras[i].posiciones_rollkeys);
    }
    if(world->camaras[i].numero_keys_fovpath > 1)
    {
      /* world->camaras[i].posiciones_fovpath=(info_fov_path *) malloc(sizeof(info_fov_path)*world->num_frames);
       interpola_spline_fov(world->camaras[i].posiciones_fovkeys, 
			    world->camaras[i].posiciones_fovpath,numero_keys,world->num_frames);*/
       interpola_spline_fov (world->camaras[i].numero_keys_fovpath,
	               &(world->camaras[i].tangentes_entrada_fov),
		       &(world->camaras[i].tangentes_salida_fov),
		       world->camaras[i].posiciones_fovkeys);
    }
 }
}

world_type *Fantasy::GetWorldPointer(void)
{
 return &(worlds[current_scene]);	
}

void Fantasy::LoadVertexProgram(char *objname, char * filename)
{
 int i;
 
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  if(strmatch(objname,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  {
  	if(worlds[current_scene].obj[i].bonetree) // warning, it has a skin!!
  	{
  	 sprintf (FANTASY_DEBUG_STRING, "WARNING: object %s is a skinned mesh, so I can't load any vertex program for it!\n",objname);
 	 Debug(DEBUG_LEVEL_WARNING); 		
  	}
        winview.LoadVertexProgram(&(worlds[current_scene].obj[i]),filename);
  }
 }		
}

void Fantasy::SetRenderMode(char *object_name, int render_mode)
{
 int i;
 
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  if(strmatch(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  {
  	worlds[current_scene].obj[i].render_mode = render_mode;
  }
 }	
}

void *Fantasy::GetAPI(void)
{
 FantasyAPI *objeto;
 
 objeto = new FantasyAPI(&(worlds[current_scene]));
 return (void*)objeto;
}









void DebugMatrices(Bone *firstbone)
{
  sprintf (FANTASY_DEBUG_STRING, "Bone %s. Position %f %f %f, Rotation %f %f %f %f\n",firstbone->nombre,firstbone->x,firstbone->y,firstbone->z,
  firstbone->current_rotation.x,firstbone->current_rotation.y,firstbone->current_rotation.z,firstbone->current_rotation.w);
  Debug(DEBUG_LEVEL_OVERDEBUG);
  sprintf (FANTASY_DEBUG_STRING, "m_transform:\n");
  Debug(DEBUG_LEVEL_OVERDEBUG);
  sprintf (FANTASY_DEBUG_STRING, "%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
  firstbone->m_transform[0][0],firstbone->m_transform[0][1],firstbone->m_transform[0][2],firstbone->m_transform[0][3],
  firstbone->m_transform[1][0],firstbone->m_transform[1][1],firstbone->m_transform[1][2],firstbone->m_transform[1][3],
  firstbone->m_transform[2][0],firstbone->m_transform[2][1],firstbone->m_transform[2][2],firstbone->m_transform[2][3],
  firstbone->m_transform[3][0],firstbone->m_transform[3][1],firstbone->m_transform[3][2],firstbone->m_transform[3][3]
  );
  Debug(DEBUG_LEVEL_OVERDEBUG);
  sprintf (FANTASY_DEBUG_STRING, "final_matrix:\n");
  Debug(DEBUG_LEVEL_OVERDEBUG);
  sprintf (FANTASY_DEBUG_STRING, "%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
  firstbone->final_matrix[0][0],firstbone->final_matrix[0][1],firstbone->final_matrix[0][2],firstbone->final_matrix[0][3],
  firstbone->final_matrix[1][0],firstbone->final_matrix[1][1],firstbone->final_matrix[1][2],firstbone->final_matrix[1][3],
  firstbone->final_matrix[2][0],firstbone->final_matrix[2][1],firstbone->final_matrix[2][2],firstbone->final_matrix[2][3],
  firstbone->final_matrix[3][0],firstbone->final_matrix[3][1],firstbone->final_matrix[3][2],firstbone->final_matrix[3][3]
  );
  Debug(DEBUG_LEVEL_OVERDEBUG);
	
}




void Fantasy::UpdateChildBoneMatrices(Bone *bone, float parent_matrix[4][4])
{
 Bone *tmp;
 BoneList *list;
 float matriz1[4][4];
 float matriz2[4][4];
 float matriz[4][4];

 list=bone->hierarchy;
 	
 while(list != NULL)
 {
  tmp=list->hijo;
  
  // Calculamos las matrices
  matriz_unidad(matriz1);
  scale_matrix(tmp->xscale,tmp->yscale,tmp->zscale,matriz1);
  Quat_to_RotMatrix(&(tmp->current_rotation),matriz2);  // Get the rotation matrix
  multiplicar_matrices(matriz,matriz1,matriz2);
  translate_matrix(tmp->x,tmp->y,tmp->z,matriz);  
  multiplicar_matrices(tmp->m_transform,matriz,parent_matrix);  
  multiplicar_matrices(tmp->final_matrix,tmp->m_ref_inv,tmp->m_transform);    
  UpdateChildBoneMatrices(tmp,tmp->m_transform);  // Seguimos recursivamente con sus hijos  
  //DebugMatrices(tmp);

  
  list=list->next; // Pasamos al siguiente de la lista
 }
}

void Fantasy::UpdateBoneMatrices(world_type *world)
{
 int i;
 Bone *firstbone;
 float matriz1[4][4];
 float matriz2[4][4];
 BoneList *bonelist;
  
 for(i=0;i<world->number_of_objects;i++)
 {
 	if(world->obj[i].bonetree) // This object has a skin
 	{ 		
 	  bonelist=world->obj[i].bonetree;
          while(bonelist != NULL)
	  {
		firstbone=bonelist->hijo;	
 	
 		matriz_unidad(matriz1);
 		scale_matrix(firstbone->xscale,firstbone->yscale,firstbone->zscale,matriz1);
		Quat_to_RotMatrix(&(firstbone->current_rotation),matriz2);  // Get the rotation matrix
		multiplicar_matrices(firstbone->m_transform,matriz1,matriz2);
 		translate_matrix(firstbone->x,firstbone->y,firstbone->z,firstbone->m_transform); // And now the translation matrix
  		multiplicar_matrices(firstbone->final_matrix,firstbone->m_ref_inv,firstbone->m_transform); 
 		UpdateChildBoneMatrices(firstbone,firstbone->m_transform); 		    
		//DebugMatrices(firstbone); 

		bonelist=bonelist->next;
	  }
 	} 		
 }
}

float Fantasy::GetPos(char *object_name, float frame, int which)
{
 int i;
 object_type *obj;
 fuente *fuente;
 camara *camara;
 float xyz[3];
 
 if((which > 2) || (which < 0)) return 0.0f;
 	
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  if(!strcmp(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  {
  	obj= &(worlds[current_scene].obj[i]);
  	if(obj->numero_keys_path>1) // si tiene alg£n movimiento 
 	{ 
       		InterpolaPosicion(obj->numero_keys_path, 
      			 obj->tangentes_entrada,
      			 obj->tangentes_salida,
      			 obj->posiciones_keys,
      			 frame,
      			 &(xyz[0]),&(xyz[1]),&(xyz[2]));      		      
 	}
 	else
 	{
  		xyz[0] = obj->x;
  		xyz[1] = obj->y;
  		xyz[2] = obj->z;	
 	}  	
  } 	
 }

 for(i=0;i<worlds[current_scene].number_of_fuentes;i++)
 {
  if(!strcmp(object_name,worlds[current_scene].fuentes[i].nombre_fuente)) // Hemos encontrado la luz
  {
  	fuente= &(worlds[current_scene].fuentes[i]);
  	if(fuente->numero_keys_path>1) // si tiene alg£n movimiento 
 	{ 
       		InterpolaPosicion(fuente->numero_keys_path, 
      			 fuente->tangentes_entrada,
      			 fuente->tangentes_salida,
      			 fuente->posiciones_keys,
      			 frame,
      			 &(xyz[0]),&(xyz[1]),&(xyz[2]));      		      
 	}
 	else
 	{
  		xyz[0] = fuente->wx;
  		xyz[1] = fuente->wy;
  		xyz[2] = fuente->wz;	
 	}  	
  } 	
 } 

 for(i=0;i<worlds[current_scene].number_of_camaras;i++)
 {
  if(!strcmp(object_name,worlds[current_scene].camaras[i].nombre_camara)) // Hemos encontrado la camara
  {
  	camara= &(worlds[current_scene].camaras[i]);
  	if(camara->numero_keys_path>1) // si tiene alg£n movimiento 
 	{ 
       		InterpolaPosicion(camara->numero_keys_path, 
      			 camara->tangentes_entrada,
      			 camara->tangentes_salida,
      			 camara->posiciones_keys,
      			 frame,
      			 &(xyz[0]),&(xyz[1]),&(xyz[2]));      		      
 	}
 	else
 	{
  		xyz[0] = camara->wx;
  		xyz[1] = camara->wy;
  		xyz[2] = camara->wz;	
 	}  	
  }
  if(!strcmp(object_name,worlds[current_scene].camaras[i].nombre_camara_target)) // Hemos encontrado el target de camara
  {
  	camara= &(worlds[current_scene].camaras[i]);
  	if(camara->numero_keys_path>1) // si tiene alg£n movimiento 
 	{ 
       		InterpolaPosicion(camara->numero_keys_path_target, 
      			 camara->tangentes_entrada_target,
      			 camara->tangentes_salida_target,
      			 camara->posiciones_keys_target,
      			 frame,
      			 &(xyz[0]),&(xyz[1]),&(xyz[2]));      		      
 	}
 	else
 	{
  		xyz[0] = camara->wxt;
  		xyz[1] = camara->wyt;
  		xyz[2] = camara->wzt;	
 	}  	
  }
     	
 } 
 
 return xyz[which];
}


float Fantasy::GetScale(char *object_name, float frame, int which)
{
 int i;
 object_type *obj;
 float xyz[3];
 
 if((which > 2) || (which < 0)) return 0.0f;
 	
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  if(!strcmp(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  {
  	obj= &(worlds[current_scene].obj[i]);
   	if(obj->numero_keys_path_scale > 1)
 	{
  		InterpolaScale(obj->numero_keys_path_scale, 
    				obj->tangentes_entrada_scale,
      		 		obj->tangentes_salida_scale,
      		 		obj->scale_keys,
      		 		frame,
      			 	&(xyz[0]),&(xyz[1]),&(xyz[2]));      		      
 	}
 	else
 	{
  		xyz[0]  = obj->xscale;
  		xyz[1]  = obj->yscale;
  		xyz[2]  = obj->zscale;	
 	}	
  } 	
 }
 return xyz[which];
}


void Fantasy::SetPos(char *object_name, int which, float *xyz, int mode, float frame, int type)
{
//type 0:obj 1:light 2:camera 3:all	
//which 0:x 1:y 2:z 
//mode 0:Replace 1:Add 2:Multiply 
 int i;
 object_type *obj;
 Point tmp;//Para la interpolación
 fuente *fuente;
 camara *camara;
 
 if(type==0 || type==3)
 {
 	for(i=0;i<worlds[current_scene].number_of_objects;i++)
 	{
 		if(strmatch(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado un objeto 	
  		{
		  	obj= &(worlds[current_scene].obj[i]);
	  		switch(mode)
	  		{
		  		case 0://replace
	  					switch(which)
						{
							case 0:obj->x=xyz[0];break;
							case 1:obj->y=xyz[1];break;
							case 2:obj->z=xyz[2];break;
							default:break;
						}			
						break;
	  			case 1://add
	  		  			if(obj->numero_keys_path>1) // si tiene alg£n movimiento 
 						{ 
	
	  		     			InterpolaPosicion(obj->numero_keys_path, 
      			 			obj->tangentes_entrada,
      			 			obj->tangentes_salida,
      			 			obj->posiciones_keys,
      			 			frame,
      			 			&(tmp.x),&(tmp.y),&(tmp.z));   
      			 		}
      			 		else
      			 		{
	      			 		tmp.x  = obj->x;
  							tmp.y  = obj->y;
  							tmp.z  = obj->z;	
  						}
	      			 	
      			 		switch(which)
						{
							case 0:obj->x=tmp.x+xyz[0];break;
							case 1:obj->y=tmp.y+xyz[1];break;
							case 2:obj->z=tmp.z+xyz[2];break;
							default:break;
						}			
	  					break;
	  			case 2://multiply
	  		  			if(obj->numero_keys_path>1) // si tiene alg£n movimiento 
 						{ 
	
	  		     			InterpolaPosicion(obj->numero_keys_path, 
      			 			obj->tangentes_entrada,
      			 			obj->tangentes_salida,
      			 			obj->posiciones_keys,
      			 			frame,
      			 			&(tmp.x),&(tmp.y),&(tmp.z));   
      			 		}
      			 		else
      			 		{
	      			 		tmp.x  = obj->x;
  							tmp.y  = obj->y;
  							tmp.z  = obj->z;	
  						}
	      			 	 
      			 		switch(which)
						{
							case 0:obj->x=tmp.x*xyz[0];break;
							case 1:obj->x=tmp.y*xyz[1];break;
							case 2:obj->x=tmp.z*xyz[2];break;
							default:break;
						}			
	  					break;	
	  			default:break;
	  		}
  		} 	
 	}
 }

 if(type==1 || type==3)
 {
 	for(i=0;i<worlds[current_scene].number_of_fuentes;i++)
	{	
	 	if(strmatch(object_name,worlds[current_scene].fuentes[i].nombre_fuente)) // Hemos encontrado una fuente de luz
	  	{
	  		fuente= &(worlds[current_scene].fuentes[i]);
			switch(mode)
		  	{
	  			case 0://replace
	  					switch(which)
						{
							case 0:fuente->wx=xyz[0];break;
							case 1:fuente->wy=xyz[1];break;
							case 2:fuente->wz=xyz[2];break;
							default:break;
						}			
						break;
	  			case 1://add
	  		  			if(fuente->numero_keys_path>1) // si tiene alg£n movimiento 
 						{ 
	
	  			    		InterpolaPosicion(fuente->numero_keys_path, 
      			 			fuente->tangentes_entrada,
      			 			fuente->tangentes_salida,
      			 			fuente->posiciones_keys,
      			 			frame,
      			 			&(tmp.x),&(tmp.y),&(tmp.z));  
      			 		}
      			 		else
      			 		{
	      			 		tmp.x  = fuente->wx;
  							tmp.y  = fuente->wy;
  							tmp.z  = fuente->wz;	
  						}	  		
      			 		switch(which)
						{						
			  				case 0:fuente->wx=tmp.x+xyz[0];break;
	  						case 1:fuente->wy=tmp.y+xyz[1];break;
	  						case 2:fuente->wz=tmp.z+xyz[2];break;
	  					}
	  					break;
	  			case 2://multiply
		  		  		if(fuente->numero_keys_path>1) // si tiene alg£n movimiento 
 						{   		
		  		
		  			   		InterpolaPosicion(fuente->numero_keys_path, 
      			 			fuente->tangentes_entrada,
      			 			fuente->tangentes_salida,
      			 			fuente->posiciones_keys,
      			 			frame,
      			 			&(tmp.x),&(tmp.y),&(tmp.z));  
      			 		}	
      			 		else
      			 		{
	      			 		tmp.x  = fuente->wx;
  							tmp.y  = fuente->wy;
  							tmp.z  = fuente->wz;	
      					}
      			 		  			
	      			 	switch(which)
						{						
			  				case 0:fuente->wx=tmp.x*xyz[0];break;
	  						case 1:fuente->wy=tmp.y*xyz[1];break;
	  						case 2:fuente->wz=tmp.z*xyz[2];break;
	  					}
	  					break;
	  			default:break;
	  		}
					
	  	} 	
 	}
 }
 
 if(type==2 || type==3)
 {	
 	for(i=0;i<worlds[current_scene].number_of_camaras;i++)
 	{
	 	if(strmatch(object_name,worlds[current_scene].camaras[i].nombre_camara)) // Hemos encontrado una camara
	  	{
	  		camara= &(worlds[current_scene].camaras[i]);
			switch(mode)
	  		{
		  		case 0://Replace
	  					switch(which)
						{
							case 0:camara->wx=xyz[0];break;
							case 1:camara->wy=xyz[1];break;
							case 2:camara->wz=xyz[2];break;
							default:break;
						}			
						break;
	  			case 1://add
		  		  		if(camara->numero_keys_path>1) // si tiene alg£n movimiento 
 						{   		
		  		
		  			    	InterpolaPosicion(camara->numero_keys_path, 
      			 			camara->tangentes_entrada,
      			 			camara->tangentes_salida,
      			 			camara->posiciones_keys,
      			 			frame,
      			 			&(tmp.x),&(tmp.y),&(tmp.z));
      			 		}
      			 		else
      			 		{
	      			 		tmp.x  = camara->wx;
  							tmp.y  = camara->wy;
  							tmp.z  = camara->wz;	
      					}	  		
	      			 		  		
	  					switch(which)
						{						
			  				case 0:camara->wx=tmp.x+xyz[0];break;
	  						case 1:camara->wy=tmp.y+xyz[1];break;
	  						case 2:camara->wz=tmp.z+xyz[2];break;
	  					}
	  					break;
	  			case 2://multiply
		  		  		if(camara->numero_keys_path>1) // si tiene alg£n movimiento 
 						{   		
		  		
		  			    	InterpolaPosicion(camara->numero_keys_path, 
      			 			camara->tangentes_entrada,
      			 			camara->tangentes_salida,
      			 			camara->posiciones_keys,
      			 			frame,
      			 			&(tmp.x),&(tmp.y),&(tmp.z));
	      			 	}
      			 		else
      			 		{
	      			 		tmp.x  = camara->wx;
  							tmp.y  = camara->wy;
  							tmp.z  = camara->wz;	
      					}	  		
	      			 	
      			 		switch(which)
						{						
			  				case 0:camara->wx=tmp.x*xyz[0];break;
	  						case 1:camara->wy=tmp.y*xyz[1];break;
	  						case 2:camara->wz=tmp.z*xyz[2];break;
	  					}
	  					break;
	  			default:break;
	  		}		
  		} 
		
		if(strmatch(object_name,worlds[current_scene].camaras[i].nombre_camara_target)) // Hemos encontrado una camara
	  	{
	  		camara= &(worlds[current_scene].camaras[i]);
			switch(mode)
	  		{
		  		case 0://replace
	  					switch(which)
						{
							case 0:camara->wxt=xyz[0];break;
							case 1:camara->wyt=xyz[1];break;
							case 2:camara->wzt=xyz[2];break;
							default:break;
						}			
						break;
	  			case 1://add	  		
		  		  		if(camara->numero_keys_path_target>1) // si tiene alg£n movimiento 
 						{   		
		  		
	  			    		InterpolaPosicion(camara->numero_keys_path_target, 
      			 			camara->tangentes_entrada_target,
      			 			camara->tangentes_salida_target,
      			 			camara->posiciones_keys_target,
      			 			frame,
      			 			&(tmp.x),&(tmp.y),&(tmp.z));  	  		
      			 		}
	      			 	else
	      			 	{	
      			 			tmp.x  = camara->wxt;
  							tmp.y  = camara->wyt;
  							tmp.z  = camara->wzt;	
      					}	  
	      			 	      			 	
      			 		switch(which)
						{
							case 0:camara->wxt=tmp.x+xyz[0];break;
	  						case 1:camara->wyt=tmp.y+xyz[1];break;
	  						case 2:camara->wzt=tmp.z+xyz[2];break;
	  					}
	  					break;
	  			case 2://multiply
		  		  		if(camara->numero_keys_path_target>1) // si tiene alg£n movimiento 
 						{   		
		  		
	  			    		InterpolaPosicion(camara->numero_keys_path_target, 
      			 			camara->tangentes_entrada_target,
      			 			camara->tangentes_salida_target,
      			 			camara->posiciones_keys_target,
      			 			frame,
      			 			&(tmp.x),&(tmp.y),&(tmp.z));  	  		
      			 		}
      			 		else
      			 		{
	      			 		tmp.x  = camara->wxt;
  							tmp.y  = camara->wyt;
  							tmp.z  = camara->wzt;	
      					}	  
	      			 	      			 	
      			 		switch(which)
						{
							case 0:camara->wxt=tmp.x*xyz[0];break;
	  						case 1:camara->wyt=tmp.y*xyz[1];break;
	  						case 2:camara->wzt=tmp.z*xyz[2];break;
	  					}
	  					break;
	  			default:break;
	  		}	 	
  		}
 	}  
 }	
	 
}


void Fantasy::SetScale(char *object_name, int which, float *xyz, int mode, float frame)
{
//which 0:x 1:y 2:z
//mode 0:Add 1:Multiply 2:Replace
 int i;
 object_type *obj;
 Point tmp;//Para la interpolación

 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  	if(strmatch(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado un objeto 	
  	{
	  	obj= &(worlds[current_scene].obj[i]);
	  	switch(mode)
	  	{
	  		case 0://replace
	  				switch(which)
					{
						case 0:obj->xscale=xyz[0];break;
						case 1:obj->yscale=xyz[1];break;
						case 2:obj->zscale=xyz[2];break;
						default:break;
					}			
					break;
	  		case 1://add
	  		  		if(obj->numero_keys_path_scale>1) // si tiene alg£n movimiento 
 					{ 	  		
  						InterpolaScale(obj->numero_keys_path_scale, 
    					obj->tangentes_entrada_scale,
      		 			obj->tangentes_salida_scale,
      		 			obj->scale_keys,
      		 			frame,
      			 		&(tmp.x),&(tmp.y),&(tmp.z));   	  		
      			 		tmp.x/=1000;
      			 		tmp.y/=1000;
      			 		tmp.z/=1000;

      			 	}
      			 	else
      			 	{
      			 		tmp.x  = obj->xscale;
  						tmp.y  = obj->yscale;
  						tmp.z  = obj->zscale;	
      				}
      			 	
      			 	switch(which)
      			 	{
	  					case 0:obj->xscale=tmp.x+xyz[0];break;
	  					case 1:obj->yscale=tmp.y+xyz[1];break;
	  					case 2:obj->zscale=tmp.z+xyz[2];break;
	  				}
	  				break;
	  		case 2://multiply
	  		  		if(obj->numero_keys_path_scale>1) // si tiene alg£n movimiento 
 					{ 	  		
  						InterpolaScale(obj->numero_keys_path_scale, 
    					obj->tangentes_entrada_scale,
      		 			obj->tangentes_salida_scale,
      		 			obj->scale_keys,
      		 			frame,
      			 		&(tmp.x),&(tmp.y),&(tmp.z));   	  		
      			 		tmp.x/=1000;
      			 		tmp.y/=1000;
      			 		tmp.z/=1000;
      			 	}
      			 	else
      			 	{
      			 		tmp.x  = obj->xscale;
  						tmp.y  = obj->yscale;
  						tmp.z  = obj->zscale;	
      				}
      				
      			 	switch(which)
      			 	{      			 	
						case 0:obj->xscale=tmp.x*xyz[0];break;
		  				case 1:obj->yscale=tmp.y*xyz[1];break;
		  				case 2:obj->zscale=tmp.z*xyz[2];break;
		  			}		  				
	  				break;
	  		default:break;
	  	}		
 	}	
  } 	
  
}

void Fantasy::SetAnimateFlag(char *object_name, int flag, int type)
{
 //type  0:obj 1:light 2:camera 3:all	
 int i;
 object_type *obj;
 fuente *fuente;
 camara *camara;

 if(flag<0 || flag>1)
 {
 	flag=1;	
 }
 if(type==0 || type==3)
 {
 	for(i=0;i<worlds[current_scene].number_of_objects;i++)
 	{
	  	if(strmatch(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado un objeto 	
	  	{
	  		obj= &(worlds[current_scene].obj[i]);
	  		obj->animate=flag;
 		}	
  	} 	
 }

 if(type==1 || type==3)
 {
 	for(i=0;i<worlds[current_scene].number_of_fuentes;i++)
 	{
	  	if(strmatch(object_name,worlds[current_scene].fuentes[i].nombre_fuente)) // Hemos encontrado una fuente
	  	{
	  		fuente= &(worlds[current_scene].fuentes[i]);
	  		fuente->animate=flag;
 		}	
  	} 	  
 }

 if(type==2 || type==3)
 {
 	for(i=0;i<worlds[current_scene].number_of_camaras;i++)
 	{
	  	if(strmatch(object_name,worlds[current_scene].camaras[i].nombre_camara)) // Hemos encontrado una Camara
	  	{
	    	camara= &(worlds[current_scene].camaras[i]);
	  		camara->animate=flag;
 		}	
 		if(strmatch(object_name,worlds[current_scene].camaras[i].nombre_camara_target)) // Hemos encontrado un Target
 		{
	 		camara= &(worlds[current_scene].camaras[i]);
	  		camara->animate_target=flag;
		}
  	} 	  
  }
}


void Fantasy::SetMaterial(char *object_name, char *material_name)
{
 int i;
   
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  	if(strmatch(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  	{
	  	worlds[current_scene].obj[i].num_material=worlds[current_scene].mats.FindMaterial(material_name);
  	}
 }	
}


void Fantasy::SetLightColor(char *light_name, float r, float g, float b)
{
 int i;
   
 for(i=0;i<worlds[current_scene].number_of_fuentes;i++)
 {
  	if(strmatch(light_name,worlds[current_scene].fuentes[i].nombre_fuente)) // Hemos encontrado el objeto
 	{
 		worlds[current_scene].fuentes[i].r=r;
 		worlds[current_scene].fuentes[i].g=g;
 		worlds[current_scene].fuentes[i].b=b;
	}
 }
 
}

void Fantasy::SetLightMult(char *light_name, float value)
{
 int i;
   
 for(i=0;i<worlds[current_scene].number_of_fuentes;i++)
 {
  	if(strmatch(light_name,worlds[current_scene].fuentes[i].nombre_fuente)) // Hemos encontrado el objeto
 	{
 		worlds[current_scene].fuentes[i].multiplicador=value;
	}
 }
 	
}

void Fantasy::SetFov(char *camera_name, float value)
{
 int i;
   
 for(i=0;i<worlds[current_scene].number_of_camaras;i++)
 {
  	if(strmatch(camera_name,worlds[current_scene].camaras[i].nombre_camara)) // Hemos encontrado el objeto
 	{
 		worlds[current_scene].camaras[i].fov=value;
	}
 }	
}

void Fantasy::SetRoll(char *camera_name, float value)
{
 int i;
   
 for(i=0;i<worlds[current_scene].number_of_camaras;i++)
 {
  	if(strmatch(camera_name,worlds[current_scene].camaras[i].nombre_camara)) // Hemos encontrado el objeto
 	{
 		worlds[current_scene].camaras[i].inclinacion=value;
	}
 }	
}

float Fantasy::GetFov(char *camera_name)
{
 int i;
   
 for(i=0;i<worlds[current_scene].number_of_camaras;i++)
 {
  	if(!strcmp(camera_name,worlds[current_scene].camaras[i].nombre_camara)) // Hemos encontrado la camara
 	{
 		return worlds[current_scene].camaras[i].fov;
	}
 }		
 return 0.0f;
}

float Fantasy::GetRoll(char *camera_name)
{
 int i;
   
 for(i=0;i<worlds[current_scene].number_of_camaras;i++)
 {
  	if(!strcmp(camera_name,worlds[current_scene].camaras[i].nombre_camara)) // Hemos encontrado la camara
 	{
 		return worlds[current_scene].camaras[i].inclinacion;
	}
 }		
 return 0.0f;
}

void Fantasy::SetRotation(char *object_name, float x, float y, float z, float angle, int mode, float frame)
{
 int i;
 Quaternion tmp;
 object_type *obj; 
   
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  	if(strmatch(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
 	{ 		
 		Axis_to_Quat(x,y,z,angle,&tmp);
 		obj= &(worlds[current_scene].obj[i]);
 		switch(mode)
 		{
 			case 0://replace 					
 					worlds[current_scene].obj[i].current_rotation=tmp;
 					break; 
 			case 1://add
					if(obj->numero_keys_path_rotacion>1)
     				{
       					InterpolaRotacion(obj->numero_keys_path_rotacion, 
      					obj->tangentes_entrada_rotacion,
      					obj->tangentes_salida_rotacion,
      					obj->path_rotacion,
      					frame,
      					&(tmp));
     				}         
     				else
     				{
     					tmp.x=obj->current_rotation.x;
     					tmp.y=obj->current_rotation.y;
     					tmp.x=obj->current_rotation.z;
     					tmp.x=obj->current_rotation.w;
     				}
 			
 					obj->current_rotation.x=tmp.x;//UU, sucio sucio :)
 					obj->current_rotation.y=tmp.y;
 					obj->current_rotation.z=tmp.z;
 					obj->current_rotation.w=tmp.w;
 					break;
 		}
	}
 }			
	
}

void Fantasy::SetObjectAlpha(char *object_name, float alpha)
{
  int i;

  for(i=0;i<worlds[current_scene].number_of_objects;i++)
  {
  	if(strmatch(object_name,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  	{
	  	worlds[current_scene].obj[i].object_alpha=alpha;
  	}
 }		
}

// Debug levels
// 0:  DEBUG_LEVEL_ERROR  only errors are displayed
// 1:  DEBUG_LEVEL_WARNING warnings and errors (default)
// 2:  DEBUG_LEVEL_INFO    also informative things (some overhead)
// 3:  DEBUG_LEVEL_OVERDEBUG lots of debug (great overhead, usually disabled in the code)

extern int DEBUG_CURRENT_LEVEL;

void Fantasy::SetDebugLevel(int debuglevel)
{
 DEBUG_CURRENT_LEVEL = debuglevel;	
}

void Fantasy::SetSpecialFX(char *objname, int fxtype)
{
 int i;
 
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  if(strmatch(objname,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  {
        winview.SetSpecialFX(&(worlds[current_scene].obj[i]),fxtype);
  }
 }		
}

void Fantasy::SetFXParamu(char *objname, int index, unsigned int value)
{
 int i;

 if((index > 7) || index < 0) return; // As of now we will only allow 8 parameters
 
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  if(strmatch(objname,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  {
        worlds[current_scene].obj[i].fx.paramu[index] = value;
  }
 }			
}

void Fantasy::SetFXParamf(char *objname, int index, float value)
{
 int i;

 if((index > 7) || index < 0) return; // As of now we will only allow 8 parameters
 
 for(i=0;i<worlds[current_scene].number_of_objects;i++)
 {
  if(strmatch(objname,worlds[current_scene].obj[i].nombre_objeto)) // Hemos encontrado el objeto
  {
        worlds[current_scene].obj[i].fx.paramf[index] = value;
  }
 }			
}

void Fantasy::SetShaderParamf(char *mat_name, char *param_name, float value)
{
	
 int i;
 int j;

 if((i=worlds[current_scene].mats.FindMaterial(mat_name))!=-1)// Hemos encontrado el material
 {  		
	if(worlds[current_scene].mats.materiales[i].shader.params.used==0)//No parameters yet
	{
		worlds[current_scene].mats.materiales[i].shader.params.name[0]=new char[strlen(param_name)+1];	
  		strcpy(worlds[current_scene].mats.materiales[i].shader.params.name[0],param_name);	
  		worlds[current_scene].mats.materiales[i].shader.params.used++;	
	}
	else
	{
  		//Check if this param already exists
  		int exists=-1;
  		for(j=0;j<worlds[current_scene].mats.materiales[i].shader.params.used;j++)
  		{
	  		if(strcmp(worlds[current_scene].mats.materiales[i].shader.params.name[j],param_name)==0)//One already exists with the same name
  			{
  				exists=j;
  				break;
  			}
  		}
  		if(exists==-1)
  		{
  			if(worlds[current_scene].mats.materiales[i].shader.params.used<4)
  			{
  				int used=worlds[current_scene].mats.materiales[i].shader.params.used+1;
  				worlds[current_scene].mats.materiales[i].shader.params.name[used]=new char[strlen(param_name)+1];	
				strcpy(worlds[current_scene].mats.materiales[i].shader.params.name[used],param_name);	
				worlds[current_scene].mats.materiales[i].shader.params.used++;
			}
		}
		worlds[current_scene].mats.materiales[i].shader.params.paramf[exists]=value;

  	}                
 }
}