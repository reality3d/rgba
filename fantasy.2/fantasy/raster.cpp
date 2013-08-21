#ifdef WINDOZE
 #include <windows.h>
#endif
// Definition of GL extensions
#ifdef WINDOZE
 #include <GL/gl.h>
 #include <GL/glext.h>
#else
 #include <GL/gl.h>
 #include <GL/glx.h>
 #include <GL/glext.h>
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>


#include "raster.h"
#include "misc.h"
#include "textura.h"
#include "mates.h"
#include "packer/packer.h"
#include "extgl.h"

// #define ENABLE_HARDWARE_SKINNING
//#define MEGADEBUG

static GLuint TextureUnits[32]={GL_TEXTURE0_ARB,GL_TEXTURE1_ARB,GL_TEXTURE2_ARB,GL_TEXTURE3_ARB,GL_TEXTURE4_ARB,GL_TEXTURE5_ARB,GL_TEXTURE6_ARB,GL_TEXTURE7_ARB,
GL_TEXTURE8_ARB,GL_TEXTURE9_ARB,GL_TEXTURE10_ARB,GL_TEXTURE11_ARB,GL_TEXTURE12_ARB,GL_TEXTURE13_ARB,GL_TEXTURE14_ARB,GL_TEXTURE15_ARB,
GL_TEXTURE16_ARB,GL_TEXTURE17_ARB,GL_TEXTURE18_ARB,GL_TEXTURE19_ARB,GL_TEXTURE20_ARB,GL_TEXTURE21_ARB,GL_TEXTURE22_ARB,GL_TEXTURE23_ARB,
GL_TEXTURE24_ARB,GL_TEXTURE25_ARB,GL_TEXTURE26_ARB,GL_TEXTURE27_ARB,GL_TEXTURE28_ARB,GL_TEXTURE29_ARB,GL_TEXTURE30_ARB,GL_TEXTURE31_ARB
};

void Raster::InitRender(void)
{

 if(clearZbuffer)
 {
  glClear(GL_DEPTH_BUFFER_BIT);
 }

 // Preparamos la matriz

 glMatrixMode(GL_PROJECTION); // Matriz de proyeccin
 glLoadIdentity();

 // Perspectiva con un fov de 45, near clipping plane a 0.1 y far a 100

 // OJO!!!! Esto hay que hablarlo

// gluPerspective(45.0f,4.0f/3.0f,0.1f,10000000000000.0f);
 gluPerspective(45.0f,4.0f/3.0f,0.1f,1000.0f);

 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();

 glDepthFunc(GL_LEQUAL); // Tipo de test que vamos a hacer
 glEnable(GL_DEPTH_TEST); // Habilitamos el testeo de profundidad
 glShadeModel(GL_SMOOTH); // Sombreado suavizado
 glCullFace(GL_BACK);
 glEnable(GL_CULL_FACE);
 if(EXTGL_multitexturing)
 {
  f_glActiveTextureARB(TextureUnits[0]);
 }

 glEnable(GL_TEXTURE_2D); // Habilitamos el texture mapping

 // Si el alpha de la escena es menor que 1, estamos en un fade, por lo que es necesario
 // habilitar siempre el blending, de forma que los objetos opacos tambiï¿½ usen el alpha

 if(globalAlpha < 0.99f)
 {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
 }

 // Ponemos parï¿½etros internos

 fog = 0;
  // Tenemos que normalizar automï¿½icamente las normales
 if(dyn_lighting)	// Al producirse los artefactos slo en la iluminacin (creo), podemos
 			// deshabilitarlo si no usamos luces dinï¿½icas
 {
  glEnable(GL_NORMALIZE);
 }

 //if(render_mode == RENDER_WIRE)
//   glPolygonMode(GL_FRONT,GL_LINE);
}

void Raster::SetCameraPos(float eyex,float eyey,float eyez,float focusx,float focusy,float focusz,float upx,float upy,float upz)
{
    CameraPosition[0] = eyex;
    CameraPosition[1] = eyey;
    CameraPosition[2] = -eyez;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyex,eyey,-eyez,focusx,focusy,-focusz,upx,upy,upz);
    glScalef(1.0f,1.0f,-1.0f);  // Asï¿½le metemos la z inversa
}


void Raster::SetFog(int fog_type,float fog_r,float fog_g,float fog_b,float density,float near_range,float far_range,int fog_invert)
{
 float fogColor[4];

 fogColor[0] = fog_r;
 fogColor[1] = fog_g;
 fogColor[2] = fog_b;
 fogColor[3] = 1.0f;

 switch(fog_type)
 {
  case 0 : glFogi(GL_FOG_MODE, GL_EXP);
	   break;
  case 1 : glFogi(GL_FOG_MODE, GL_EXP2);
	   break;
  case 2 : glFogi(GL_FOG_MODE, GL_LINEAR);
	   break;
 }

 glFogfv(GL_FOG_COLOR, fogColor);					// Set Fog Color
 glFogf(GL_FOG_DENSITY, density);
 glHint(GL_FOG_HINT, GL_DONT_CARE);
// glHint(GL_FOG_HINT, GL_FASTEST);
 if(fog_invert)
 {
  glFogf(GL_FOG_START, far_range);
  glFogf(GL_FOG_END, near_range);
 }
 else
 {
  glFogf(GL_FOG_START, near_range);
  glFogf(GL_FOG_END, far_range);
 }
 glEnable(GL_FOG);

 fog=1;
}

void Raster::SetFov(float fov)
{
 glMatrixMode(GL_PROJECTION); // Matriz de proyeccin
 glLoadIdentity();

 gluPerspective(fov,4.0f/3.0f,0.1f,10000.0f);
}

void Raster::SetGlobalAlpha(float alpha)
{
 globalAlpha = alpha;
}

void Raster::SetZBufferClear(int yesno)
{
  clearZbuffer = yesno;
}

void Raster::DrawObjectHalo(struct object_type *obj,float globalIlum, struct world_type *world)
{
   int i;
   int restoreblend;



   restoreblend=glIsEnabled(GL_BLEND);

    // El halo no debe tocar el zbuffer
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    // Primero cargamos el vertex program asociado al halo
    SetVertexProgram(obj->fx.paramu[0]); // Ahí está el vtx_shader_id
    f_glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0,
                                      obj->fx.paramf[4],
                                      obj->fx.paramf[4],
                                      obj->fx.paramf[4],
                                      obj->fx.paramf[4]);


    if(obj->BoneIds) // Al usar ya un vertex program tenemos que utilizar soft skinning
    {
   	SoftSkinObject(obj);   
    }  
    
    SetRenderMode(obj);
    
    if(dyn_lighting) glEnable(GL_COLOR_MATERIAL);
    glColor4f(obj->fx.paramf[0],obj->fx.paramf[1],obj->fx.paramf[2],obj->fx.paramf[3]);   
      
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    if(obj->vbufinfo.usingvertexbuf)
    {  
     f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, obj->vbufinfo.vertexbufferid);
     glVertexPointer(3,GL_FLOAT,0,BUFFER_OFFSET(0));
     glNormalPointer(GL_FLOAT,0,BUFFER_OFFSET(obj->vbufinfo.normalskip));
    }
    else
    {
     glVertexPointer(3,GL_FLOAT,0,obj->GLVertexPointer);
     glNormalPointer(GL_FLOAT,0,obj->GLNormalPointer);
    }
   
   if(EXTGL_multitexturing)
   {
       for(i=0;i<EXTGL_number_of_texture_units;i++)
       {
           f_glClientActiveTextureARB(TextureUnits[i]);
       	   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
           glDisable(GL_TEXTURE_2D);    
       }
   }
   else
   {
       glDisable(GL_TEXTURE_2D);
   }

    if(EXTGL_compiled_vertex_arrays)
    {
       f_glLockArraysEXT(0,obj->GLnumber_of_vertices);
    }

    if(obj->vbufinfo.usingvertexbuf)
    {
       f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, obj->vbufinfo.indexbufferid);
       glDrawElements(GL_TRIANGLES,3*obj->number_of_polygons,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
    }
    else
    {
       glDrawElements(GL_TRIANGLES,3*obj->number_of_polygons,GL_UNSIGNED_INT,obj->GLIndexPointer);
    }
   SetPostRender(obj);

    if(EXTGL_compiled_vertex_arrays)
    {
       f_glUnlockArraysEXT();
    }

   if(obj->vbufinfo.usingvertexbuf)
   {
	 f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, NULL );
	 f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, NULL );
   }
   
   glDepthMask(GL_TRUE);	
   if(restoreblend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
}


void Raster::DrawObjectDOF(struct object_type *obj,float globalIlum, struct world_type *world)
{
   int i;

   // Primero cargamos el vertex program asociado al DOF
   SetVertexProgram(obj->fx.paramu[0]); // Ahí está el vtx_shader_id
   f_glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0,
                                      obj->fx.paramf[0],
                                      obj->fx.paramf[0],
                                      obj->fx.paramf[0],
                                      obj->fx.paramf[0]);
                                      
   if(obj->BoneIds) // Al usar ya un vertex program tenemos que utilizar soft skinning
   {
  	SoftSkinObject(obj);   
   }  
          
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    if(obj->vbufinfo.usingvertexbuf)
    {  
     f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, obj->vbufinfo.vertexbufferid);
     glVertexPointer(3,GL_FLOAT,0,BUFFER_OFFSET(0));
    }
    else
    {
     glVertexPointer(3,GL_FLOAT,0,obj->GLVertexPointer);
    }
   
   if(EXTGL_multitexturing)
   {
       for(i=0;i<EXTGL_number_of_texture_units;i++)
       {
           f_glClientActiveTextureARB(TextureUnits[i]);
       	   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
           glDisable(GL_TEXTURE_2D);    
       }
   }
   else
   {
       glDisable(GL_TEXTURE_2D);
   }

    if(EXTGL_compiled_vertex_arrays)
    {
       f_glLockArraysEXT(0,obj->GLnumber_of_vertices);
    }

    if(obj->vbufinfo.usingvertexbuf)
    {
       f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, obj->vbufinfo.indexbufferid);
       glDrawElements(GL_TRIANGLES,3*obj->number_of_polygons,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
    }
    else
    {
       glDrawElements(GL_TRIANGLES,3*obj->number_of_polygons,GL_UNSIGNED_INT,obj->GLIndexPointer);
    }
   
    if(EXTGL_compiled_vertex_arrays)
    {
       f_glUnlockArraysEXT();
    }

   if(obj->vbufinfo.usingvertexbuf)
   {
	 f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, NULL );
	 f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, NULL );
   }
   
}



void Raster::DrawObjectNormal(struct object_type *obj,float globalIlum, struct world_type *world)
{
    int i;
    
          
    // WARNING THIS IS A BAD HACK!!!
    // Test this...
    world->mats.materiales[obj->num_material].shader.Camera[0] = CameraPosition[0];
    world->mats.materiales[obj->num_material].shader.Camera[1] = CameraPosition[1];
    world->mats.materiales[obj->num_material].shader.Camera[2] = CameraPosition[2];
    world->mats.materiales[obj->num_material].shader.nlights = world->number_of_fuentes;
    
    if((obj->fx.id == FX_HALO) || (obj->fx.id == FX_DOF))
      // Since the halo and DOF effects are modifying the texture state, we need to reselect the 
      // material regardless what material was there before
      world->mats.SelectMaterial(obj->num_material,1); 
    else
      // Seleccionamos el material del objeto
      world->mats.SelectMaterial(obj->num_material,0);
      
    if((obj->object_alpha < 0.999999f) && world->mats.materiales[obj->num_material].opaque)
    {
      glEnable(GL_BLEND);              
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);	
    } 

    // Primero cargamos el vertex program, si corresponde
    SetVertexProgram(obj->vtx_shader_id);

    // If the object has a skin, we should now pass all the parameters to the vertex program
    //
#ifdef ENABLE_HARDWARE_SKINNING
    if(obj->BoneIds) // the object has a skin
    {
     	if(EXTGL_vertex_shaders && (obj->vtx_shader_id!=0xffffffff))
     	{
     	 Bone *tempbone;

     	 f_glEnableVertexAttribArrayARB(6);
     	 f_glEnableVertexAttribArrayARB(7);
     	 f_glVertexAttribPointerARB(6,4,GL_FLOAT,GL_FALSE,0,obj->GLBoneWeights);
     	 f_glVertexAttribPointerARB(7,4,GL_INT,GL_FALSE,0,obj->GLBoneIds);

     	 for(i=0;i<obj->numbones;i++)
     	 {
     	 	tempbone = obj->BoneArray[i];
     	 	f_glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 3*i,
                                      tempbone->final_matrix[0][0],
                                      tempbone->final_matrix[1][0],
                                      tempbone->final_matrix[2][0],
                                      tempbone->final_matrix[3][0]);
     	 	f_glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 3*i+1,
                                      tempbone->final_matrix[0][1],
                                      tempbone->final_matrix[1][1],
                                      tempbone->final_matrix[2][1],
                                      tempbone->final_matrix[3][1]);
     	 	f_glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 3*i+2,
                                      tempbone->final_matrix[0][2],
                                      tempbone->final_matrix[1][2],
                                      tempbone->final_matrix[2][2],
                                      tempbone->final_matrix[3][2]);
     	 }
     	 for(i=obj->numbones;i<28;i++)
     	 {
     	 	f_glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 3*i, 0.0f,0.0f,0.0f,0.0f);
     	 	f_glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 3*i+1, 0.0f,0.0f,0.0f,0.0f);
     	 	f_glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 3*i+2, 0.0f,0.0f,0.0f,0.0f);
     	 }
     	}
     	else  // Try software fallback
     	{		
     		SoftSkinObject(obj);
     	}
    }
#else
  if(obj->BoneIds) // the object has a skin
  {
     if(obj->fx.id != FX_HALO) SoftSkinObject(obj); // Otherwise the skinning was done previously
  }
#endif
    // Despuï¿½ preparamos el modo de render del objeto
    SetRenderMode(obj);

    if(EXTGL_multitexturing)
    {
     f_glActiveTextureARB(TextureUnits[0]);
    }

   // Si hay luces dinï¿½icas tengo que hacer esta chapucilla, porque al
   // habilitar GL_LIGHTING no hace caso al glColor (?)

   if(dyn_lighting)
   {
    glEnable(GL_COLOR_MATERIAL);
    glColor4f(globalIlum,globalIlum,globalIlum,globalAlpha*obj->object_alpha);   
   }
   else
   {
    glColor4f(globalIlum,globalIlum,globalIlum,globalAlpha*obj->object_alpha);
   }
   
   if(obj->ColorPointer) glEnableClientState(GL_COLOR_ARRAY);
    else glDisableClientState(GL_COLOR_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);

   if(obj->vbufinfo.usingvertexbuf)
   {
    f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, obj->vbufinfo.vertexbufferid);
    glVertexPointer(3,GL_FLOAT,0,BUFFER_OFFSET(0));
    glNormalPointer(GL_FLOAT,0,BUFFER_OFFSET(obj->vbufinfo.normalskip));
   }
   else
   {
    glVertexPointer(3,GL_FLOAT,0,obj->GLVertexPointer);
    glNormalPointer(GL_FLOAT,0,obj->GLNormalPointer);
   }
   
   if(obj->ColorPointer)
   {
   	if(obj->vbufinfo.usingvertexbuf)
   	{
    		f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, obj->vbufinfo.colorbufferid);
    		glColorPointer(3,GL_FLOAT,0,BUFFER_OFFSET(0));    
   	}
   	else
   	{
    		glColorPointer(3,GL_FLOAT,0,obj->GLColorPointer);
   	}   	   	
   }

   if(EXTGL_multitexturing)
   {
       for(i=0;i<EXTGL_number_of_texture_units;i++)
       {
           f_glClientActiveTextureARB(TextureUnits[i]);
       	   glEnableClientState(GL_TEXTURE_COORD_ARRAY);       	   
           if(obj->vbufinfo.usingvertexbuf)
           {
            f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, obj->vbufinfo.texbufferid);
            glTexCoordPointer(2,GL_FLOAT,0,BUFFER_OFFSET(0));
           }
           else
           {
            glTexCoordPointer(2,GL_FLOAT,0,obj->GLTexCoordPointer);
           }
       }
   }
   else
   {
       glTexCoordPointer(2,GL_FLOAT,0,obj->GLTexCoordPointer);
   }


   if(EXTGL_compiled_vertex_arrays)
   {
       f_glLockArraysEXT(0,obj->GLnumber_of_vertices);
   }


   if(obj->vbufinfo.usingvertexbuf)
   {
       f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, obj->vbufinfo.indexbufferid);
       glDrawElements(GL_TRIANGLES,3*obj->number_of_polygons,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
   }
   else
   {
       glDrawElements(GL_TRIANGLES,3*obj->number_of_polygons,GL_UNSIGNED_INT,obj->GLIndexPointer);
   }

   SetPostRender(obj);


   if(EXTGL_compiled_vertex_arrays)
   {
       f_glUnlockArraysEXT();
   }

  if(obj->vbufinfo.usingvertexbuf)
   {
	 f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, NULL );
	 f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, NULL );
  }
  
#ifdef ENABLE_HARDWARE_SKINNING  
  
  if(EXTGL_vertex_shaders && (obj->vtx_shader_id!=0xffffffff))
  {
     	 f_glDisableVertexAttribArrayARB(6);
     	 f_glDisableVertexAttribArrayARB(7);
  }
#endif
  if((obj->object_alpha < 0.999999f) && world->mats.materiales[obj->num_material].opaque)
  {
      glDisable(GL_BLEND);                
  } 
	
}

void Raster::DrawObject(struct object_type *obj,float globalIlum, struct world_type *world)
{

    float rotmat[4][4];
    ListaHijos *tmp;

    #ifdef MEGADEBUG
   	sprintf (FANTASY_DEBUG_STRING, " * Drawing object %s\n",obj->nombre_objeto);
   	Debug(DEBUG_LEVEL_OVERDEBUG);
    #endif

    // Preparamos las matrices de transformacin del objeto

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(obj->x, obj->y, obj->z);

    // Obtenemos la matriz de rotacion a partir del quaternio

     Quat_to_RotMatrix(&(obj->current_rotation),rotmat);
     glMultMatrixf(&rotmat[0][0]);
    
     glScalef(obj->xscale, obj->yscale, obj->zscale);
/*        
     float matri[16];
     glGetFloatv(GL_MODELVIEW_MATRIX,matri);
     sprintf (FANTASY_DEBUG_STRING, "Object %s\n Trans %f %f %f Rot %f %f %f %f\n Scale %f %f %f \n",obj->nombre_objeto,
     obj->x,obj->y,obj->z,obj->current_rotation.x,obj->current_rotation.y,obj->current_rotation.z,obj->current_rotation.w,
     obj->xscale,obj->yscale,obj->zscale);
     Debug(DEBUG_LEVEL_INFO);
     sprintf (FANTASY_DEBUG_STRING, "%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
     matri[0],matri[1],matri[2],matri[3],matri[4],matri[5],matri[6],matri[7],matri[8],matri[9],matri[10],matri[11],matri[12],matri[13],matri[14],matri[15]);
     Debug(DEBUG_LEVEL_INFO);
  */  	
    tmp = obj->hierarchy;
    while(tmp!=NULL)
    {
     DrawObject(tmp->hijo,globalIlum,world);
     tmp = tmp->next;
    }


/*
 Cuando haya jerarquï¿½s, el sistema serï¿½ sencillo: se coge al padre de la jerarquï¿½, se hace
 el pushmatrix, se aplican sus transformaciones y se dibuja. Luego, para cada hijo se aplican
 sus transformaciones y se dibuja (sin pushmatrix). Finalmente, se hace el popmatrix y se pasa
 a la siguiente jerarquï¿½.
*/

    switch(obj->fx.id)
    {
    	case FX_NONE: // No effect, draw normal object
    		DrawObjectNormal(obj,globalIlum,world);
    		break;
    	case FX_HALO: // Halo effect also with normal object
    		DrawObjectHalo(obj,globalIlum,world);
    		DrawObjectNormal(obj,globalIlum,world);    			
    		break;    		
    	case FX_DOF:  // The DOF effect is performed in two passes: 
    		      // 1 for the alpha depth texture creation, 1 for the normal drawing
    		      // The pass is defined by the paramu[1] variable
    		if(obj->fx.paramu[1]) // Normal pass
    		{
    		 DrawObjectNormal(obj,globalIlum,world);
    		 obj->fx.paramu[1]=0;
    		}
    		else // Depth pass
    		{
    		 DrawObjectDOF(obj,globalIlum,world);
    		 obj->fx.paramu[1]=1;
    		}    		
    		break;    		          		          	
    }	 
 glMatrixMode(GL_MODELVIEW);
 // Dejamos las matrices de transformacin como estaban (todo esto habrï¿½que modificarlo cuando se aadan jerarquias)
 glPopMatrix();
}


Raster::Raster()
{
 float cartoonTexture[32]={0.2,0.2,0.2,0.2,0.3,0.3,0.3,0.4,0.4,0.4,0.4,0.5,0.5,0.5,0.6,0.6,0.6,0.7,0.7,0.7,0.8,0.8,0.8,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
 globalAlpha = 1.0f;
 clearZbuffer = 1;
 InitGLExtensions();
 dyn_lighting=1;

 // Cargamos la textura 1D usada para el cartoon rendering

 glGenTextures (1, &cartoon_texture);
 glBindTexture (GL_TEXTURE_1D, cartoon_texture);
 glTexImage1D (GL_TEXTURE_1D, 0, GL_LUMINANCE, 32, 0, GL_LUMINANCE , GL_FLOAT, cartoonTexture);

}

Raster::~Raster()
{
}

// Con esta funcin devolvemos todo a la normalidad

void Raster::FinRender(void)
{
    int i;

 glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
 glDisable(GL_LIGHTING);
 glDisable(GL_BLEND);
 glDepthMask(GL_TRUE);
 glDepthFunc(GL_LEQUAL); // Tipo de test que vamos a hacer
// glEnable(GL_DEPTH_TEST); // Habilitamos el testeo de profundidad
 glCullFace(GL_BACK);
// glEnable(GL_CULL_FACE);

 glDisable(GL_DEPTH_TEST);
 glDisable(GL_CULL_FACE);

 if(EXTGL_multitexturing)
 {
     for(i=0;i<EXTGL_number_of_texture_units;i++)
     {
         f_glActiveTextureARB(TextureUnits[i]);
         glDisable(GL_TEXTURE_2D);
         glDisable(GL_TEXTURE_GEN_S);
         glDisable(GL_TEXTURE_GEN_T);
         glDisable(GL_TEXTURE_GEN_R);
         glDisable(GL_TEXTURE_CUBE_MAP_ARB);
   	 glDisableClientState(GL_TEXTURE_COORD_ARRAY);
     }
 }
 f_glActiveTextureARB(TextureUnits[0]);

 glDisableClientState(GL_COLOR_ARRAY);
 glDisableClientState(GL_VERTEX_ARRAY);
 glDisableClientState(GL_NORMAL_ARRAY);


 if(fog)
     glDisable(GL_FOG);


 if(EXTGL_vertex_shaders)
 {
   glDisable( GL_VERTEX_PROGRAM_ARB );
   glDisable(GL_COLOR_SUM_ARB);
 }
 
 if(EXTGL_shading_language)
 {
     f_glUseProgramObjectARB( NULL );       
 } 
 
 if(EXTGL_blend_substract_supported)
 {
  f_glBlendEquationEXT(GL_FUNC_ADD_EXT);
 }

 glDisable(GL_COLOR_MATERIAL);
 glDisable(GL_NORMALIZE);
// glDisable(GL_STENCIL_TEST);
 glColor4f(1.0f,1.0f,1.0f,1.0f);
 glMatrixMode(GL_MODELVIEW);
}

// Inicio del procesamiento de luces. De momento suponemos materiales por defecto
// Para el futuro, hay que tener en cuenta mï¿½ cosas de las luces (luces direccionales, cono de luz)
// asï¿½como de los materiales (shininess, specular, emission)


static GLuint lights[8]={GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};

void Raster::ProcessLights(struct world_type *world)
{

  int i;
  fuente *F;
  GLfloat position[4];
  GLfloat target_position[4];
  GLfloat diffuse[4];
  GLfloat specular[4];

  // Tenemos luz, asï¿½que la habilitamos
  glEnable(GL_LIGHTING);
  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);

  for(i=0;((i<world->number_of_fuentes) && (i<8));i++)
  {


   F = &(world->fuentes[i]);  // Cojamos sus parï¿½etros
   position[0] = F->wx; position[1] = F->wy; position[2] = F->wz;
   
   F->type == 1 ? position [3] = 0.0 : position [3] = 1.0;
   
   diffuse[0] = 1.2*F->r*F->multiplicador ; diffuse[1]= 1.2*F->g*F->multiplicador; diffuse[2] = 1.2*F->b*F->multiplicador; diffuse[3] = 1.0;
   specular[0] = F->r*F->multiplicador; specular[1] = F->g*F->multiplicador; specular[2] = F->b*F->multiplicador; specular[3] = 1.0;
   
   glLightfv(lights[i],GL_POSITION,position);
   glLightfv(lights[i],GL_DIFFUSE,diffuse);
   glLightfv(lights[i],GL_SPECULAR,specular);

   if(F->type == 1 || F->type == 2)
   {
  	Vector direccion;
   	direccion.x =  F->tx - F->wx;
   	direccion.y =  F->ty - F->wy;
   	direccion.z =  F->tz - F->wz;
   	Normaliza(&direccion);
   	target_position[0] = direccion.x;
   	target_position[1] = direccion.y;
   	target_position[2] = direccion.z;
   	target_position[3] = 1.0;   	

   	if(F->type == 2)
   	{
		glLightfv(lights[i], GL_SPOT_DIRECTION, target_position); /* set the direction of GL_LIGHT0 */
		glLightf(lights[i], GL_SPOT_CUTOFF, F->cut_off); /* set the cutoff angle */
		glLightf(lights[i], GL_SPOT_EXPONENT, int(F->exponent)); 
	}
   }
   glLightf(lights[i],GL_CONSTANT_ATTENUATION, 1.0f );
   if(F->atenuacion)
   {
    glLightf(lights[i],GL_LINEAR_ATTENUATION, 4.0f / (F->fin_rango - F->inicio_rango));
   }
   else
   {
   	 glLightf(lights[i],GL_LINEAR_ATTENUATION, 0.0f);
   }

   glEnable(lights[i]);
  }
  for(i=world->number_of_fuentes;i<8;i++) glDisable(lights[i]);
}

void Raster::SetDynLighting(int dynlighting)
{
 dyn_lighting = dynlighting;
}

void Raster::SetVertexProgram(unsigned int vtx_shader_id)
{
 if(EXTGL_vertex_shaders)
 {
  if(vtx_shader_id != 0xffffffff)
  {
   f_glBindProgramARB( GL_VERTEX_PROGRAM_ARB, vtx_shader_id);
   glEnable( GL_VERTEX_PROGRAM_ARB );
   glEnable(GL_COLOR_SUM_ARB);
  }
  else
  {
   glDisable( GL_VERTEX_PROGRAM_ARB );
   glDisable(GL_COLOR_SUM_ARB);
  }
 }
}

void Raster::LoadVertexProgram(unsigned int *vtx_shader_id,char *filename)
{
    PFILE *f;
    int filesize;
    int bytes;
    char * buffer;

    if(!EXTGL_vertex_shaders)
    {
     *vtx_shader_id=0xffffffff;
     sprintf (FANTASY_DEBUG_STRING, "Sorry, this card does not support vertex programs\n");
     Debug(DEBUG_LEVEL_WARNING);
     return;
    } 

    f = pak_open(filename, "rb");
    if (f == NULL)
    {
     sprintf (FANTASY_DEBUG_STRING, "Shader file %s not found\n",filename);
     Debug(DEBUG_LEVEL_ERROR);
     *vtx_shader_id=0xffffffff;     
     return;
    }

    // Find file size.
    pak_fseek(f, 0, SEEK_END);
    filesize = pak_ftell(f);
    pak_fseek(f, 0, SEEK_SET);

    buffer = (char *)malloc(filesize*sizeof(char)+1);
    if(!buffer) return;
    // Leemos el shader en memoria y lo almacenamos
    bytes = pak_read(buffer,1,filesize,f);
    pak_close(f);
    buffer[filesize*sizeof(char)] = '\0';

    f_glGenProgramsARB(1,vtx_shader_id);
    f_glBindProgramARB( GL_VERTEX_PROGRAM_ARB, *vtx_shader_id );

    glGetError(); // limpio el ltimo error (por si acaso :)

    f_glProgramStringARB( GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB,
                        strlen(buffer), buffer );

    if(glGetError())
    {
    	int error;
    	error=-1;
    	glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB,&error);
     sprintf (FANTASY_DEBUG_STRING, "Syntax error in vertex shader: %d %s. %s\n",error,filename,glGetString(GL_PROGRAM_ERROR_STRING_ARB));
     Debug(DEBUG_LEVEL_WARNING);
     sprintf(FANTASY_DEBUG_STRING,"%s\n",&(buffer[error]));
     Debug(DEBUG_LEVEL_WARNING);
     *vtx_shader_id=0xffffffff;
    }


    free(buffer);
}

// Habilita diferentes opciones segn el tipo de render del objeto

void Raster::SetRenderMode(object_type *obj)
{
	switch (obj->render_mode)
	{
	 case RENDER_NORMAL: glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	 		     glShadeModel(GL_SMOOTH);
//	 		     glDisable(GL_STENCIL_TEST);
	 			break; // No hace nada mï¿½, opciones por defecto
	 case RENDER_WIRE:   glPolygonMode(GL_FRONT,GL_LINE);
	 		     glShadeModel(GL_FLAT);
//	 		     glDisable(GL_STENCIL_TEST);
	 		     glEnable(GL_LINE_SMOOTH);


	 		     break;
	 case RENDER_FLAT:   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	 		     glShadeModel(GL_FLAT);
    			     if(EXTGL_multitexturing)
    			     {
     				f_glActiveTextureARB(TextureUnits[0]);
    			     }
	 		     glDisable(GL_TEXTURE_2D);
//	 		     glDisable(GL_STENCIL_TEST);
	 			break;
	 case RENDER_NORMALWIRE:
	 		     glPolygonMode(GL_FRONT,GL_LINE);
	 		     glLineWidth(0.5f);
	 		     glShadeModel(GL_FLAT);
    			     if(EXTGL_multitexturing)
    			     {
     				f_glActiveTextureARB(TextureUnits[0]);
    			     }
	 		     glDisable(GL_TEXTURE_2D);
//	 		     glStencilFunc(GL_ALWAYS,1,0xFFFFFFFF); // Siempre va a pasar el stencil
	 		     //glStencilOp(GL_KEEP,GL_KEEP,GL_INCR); // Si pasa, pon a 1
//	 		     glEnable(GL_STENCIL_TEST);
	 		     break;
	 case RENDER_CARTOON:
	     		     if(EXTGL_multitexturing)
    			     {
     				f_glActiveTextureARB(TextureUnits[0]);
    			     }
	 		     glEnable(GL_TEXTURE_1D);
	 		     glDisable(GL_TEXTURE_2D);
	 		     glBindTexture(GL_TEXTURE_1D,cartoon_texture);
			     glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	                     glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	 		     // Esto es todo lo que hay que hacer a este nivel. Del resto
	 		     // se encarga el vertex shader (y el postprocesado para marcar
	 		     // las lineas)
	 			break;
	}
}

void Raster::SetPostRender(object_type *obj)
{
	switch (obj->render_mode)
	{
          case RENDER_NORMALWIRE:
          			 glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	 		         glShadeModel(GL_SMOOTH);
	 		         if(EXTGL_multitexturing)
    			         {
     				  f_glActiveTextureARB(TextureUnits[0]);
    			         }
	 		         glEnable(GL_TEXTURE_2D);
//          			 glStencilFunc(GL_EQUAL,0,0xFFFFFFFF); // Solo si no hay linea pasa el test
          			 //glStencilOp(GL_ZERO,GL_KEEP,GL_KEEP); // Si falla el test, lo pone a 0
          			 if(obj->vbufinfo.usingvertexbuf)
   				 {
       					f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, obj->vbufinfo.indexbufferid);
       					glDrawElements(GL_TRIANGLES,3*obj->number_of_polygons,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
   				 }
   				 else
	 				 glDrawElements(GL_TRIANGLES,3*obj->number_of_polygons,GL_UNSIGNED_INT,obj->GLIndexPointer);
 				 break;
 	 case RENDER_CARTOON: glEnable (GL_BLEND);									// Enable Blending
		              glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);		// Set The Blend Mode

		              glPolygonMode (GL_BACK, GL_LINE);						// Draw Backfacing Polygons As Wireframes
		              glLineWidth (3.0f);								// Set The Line Width
		       	      glCullFace (GL_FRONT);									// Don't Draw Any Front-Facing Polygons
 		              glDepthFunc (GL_LEQUAL);								// Change The Depth Mode
			      glColor3f(0.0f,0.0f,0.0f);
        		      if(obj->vbufinfo.usingvertexbuf)
   			      {
       				f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, obj->vbufinfo.indexbufferid);
       				glDrawElements(GL_TRIANGLES,3*obj->number_of_polygons,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
   			      }
   			      else
		 	      	glDrawElements(GL_TRIANGLES,3*obj->number_of_polygons,GL_UNSIGNED_INT,obj->GLIndexPointer);

		 	      glDepthFunc (GL_LESS);									// Reset The Depth-Testing Mode
		              glCullFace (GL_BACK);									// Reset The Face To Be Culled
		              glPolygonMode (GL_BACK, GL_FILL);						// Reset Back-Facing Polygon Drawing Mode
		              glDisable (GL_BLEND);
		              glColor4f(1.0f,1.0f,1.0f,1.0f);
 			      break;

	}
}

// Function to perform software skinning in software
// SLOWER!!!!!!!

void Raster::SoftSkinObject(struct object_type *obj)
{
  int i,j,k;
  int counter;
  Vector4 v1;
  Vector4 vtemp;
  Vector vres;		// Final skinned vertex
  VertexRemapper *tmp;   
  
  Vector n1;
  Vector ntemp;
  Vector nres;		// Final skinned normal
  
  
  // There are two paths: one if using vbo and one if not
   if(obj->vbufinfo.usingvertexbuf)
   {
   	f_glBindBufferARB(GL_ARRAY_BUFFER_ARB,obj->vbufinfo.vertexbufferid);
    	obj->GLVertexPointer = (float *)f_glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
    	obj->GLNormalPointer = obj->GLVertexPointer + 3 * obj->GLnumber_of_vertices;
   }
  
   counter=0;
   for(i=0,k=0;i<obj->number_of_vertices;i++)
   {
	  tmp = obj->node[i];  		  
     	  vtemp.x = obj->VertexPointer[3*i];
   	  vtemp.y = obj->VertexPointer[3*i+1];
   	  vtemp.z = obj->VertexPointer[3*i+2];
   	  vtemp.w = 1.0f;    
   	  vres.x=vres.y=vres.z=0.0f;
     	
     	  ntemp.x = obj->NormalPointer[3*i];
     	  ntemp.y = obj->NormalPointer[3*i+1];
     	  ntemp.z = obj->NormalPointer[3*i+2];
     	  nres.x=nres.y=nres.z=0.0f;
     
          // Actual skinning
     
   	  for(j=0;j<4;j++)
   	  {
    	 	Mult_Vector4_Matrix(&vtemp, obj->BoneArray[(obj->BoneIds[counter])/3]->final_matrix, &v1);
    	 	
   		vres.x += v1.x * obj->BoneWeights[counter];
   		vres.y += v1.y * obj->BoneWeights[counter];
   		vres.z += v1.z * obj->BoneWeights[counter];
   		
   		Mult_Vector_Matrix3(&ntemp, obj->BoneArray[(obj->BoneIds[counter])/3]->final_matrix, &n1);
   		nres.x += n1.x * obj->BoneWeights[counter];
   		nres.y += n1.y * obj->BoneWeights[counter];
   		nres.z += n1.z * obj->BoneWeights[counter];   		   	
   		counter++;	
   			
   	  }	  	
   	  // Upload buffers (whow, what a mess ;)

	  while(tmp!=NULL)
	  {
	   	obj->GLVertexPointer[3*k]   = vres.x;
   		obj->GLVertexPointer[3*k+1] = vres.y;
	   	obj->GLVertexPointer[3*k+2] = vres.z;
	   	
	   	obj->GLNormalPointer[3*k]   = nres.x;
   		obj->GLNormalPointer[3*k+1] = nres.y;
   		obj->GLNormalPointer[3*k+2] = nres.z;
	   	
   		tmp=tmp->next;
   		k++;
  	  } 	   
   }	
   if(obj->vbufinfo.usingvertexbuf)
   {
   	f_glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
      	f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, NULL );
   	obj->GLVertexPointer=NULL;
   	obj->GLNormalPointer = NULL;
   }

}

// Function to perform texture offset

void Raster::DisplaceTexcoords(object_type *obj)
{
  int j;
  int counter;

  // There are two paths: one if using vbo and one if not
  
   if(obj->vbufinfo.usingvertexbuf)
   {
   	f_glBindBufferARB(GL_ARRAY_BUFFER_ARB,obj->vbufinfo.texbufferid);
    	obj->GLTexCoordPointer = (float *)f_glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);    	
   }

   counter=0;
   for(j=0;j<obj->GLnumber_of_vertices;j++)
   {
       obj->GLTexCoordPointer[counter++] += obj->UTranslate;       
       obj->GLTexCoordPointer[counter++] += obj->VTranslate;       
   }       


   if(obj->vbufinfo.usingvertexbuf)
   {
   	f_glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
      	f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, NULL );
   	obj->GLTexCoordPointer = NULL;
   }
	
}

void Raster::SetSpecialFX(object_type *obj, int fxtype)
{
 obj->fx.id = fxtype;
 
 switch(obj->fx.id)
 {
  case FX_HALO: LoadVertexProgram(&(obj->fx.paramu[0]),"shaders/halo.txt");
  		obj->fx.paramf[0]=0.0f;	 // Red component of the halo color
  		obj->fx.paramf[1]=1.0f;  // Green component of the halo color
  		obj->fx.paramf[2]=0.0f;  // Blue component of the halo color
  		obj->fx.paramf[3]=0.5f;  // Alpha component of the halo color
  		obj->fx.paramf[4]=0.3f;  // Halo size
  		break;
  case FX_DOF:  LoadVertexProgram(&(obj->fx.paramu[0]),"shaders/fx_dof.txt");
  		obj->fx.paramu[1] = 0;  // we are in the first pass (depth creation pass)
  		obj->fx.paramf[0]= 0.0035f; // scale to be applied to the distance
  		break;
 }	
}