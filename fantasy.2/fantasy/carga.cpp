//#define MEGADEBUG
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#include "carga.h"
#include <packer/packer.h>
#include "material.h"
#include "mates.h"
#include "misc.h"

// Variables utilizadas durante la carga

static world_type *world;
static u3d_header header;
static char bigstring[512];
static u3d_material *materials=NULL;

#ifdef LINUX
 #define _strdup strdup
#endif

//-----------------------------------------------------------------------------
// Carga de la cabecera, con toda la informacin b�ica
//-----------------------------------------------------------------------------

int LoadHeader(PFILE *file)
{
 int i;
 char tmp[5];

 for(i=0;i<4;i++) header.signature[i] = pak_fgetc(file);
 header.signature[4]='\0';
 if(strcmp(header.signature,"UTO!"))
 {
  sprintf(FANTASY_DEBUG_STRING,"Invalid signature found\n");
  Debug(DEBUG_LEVEL_ERROR);
  return(-1);	
 }

 header.version_major = pak_fgetc(file);
 header.version_minor = pak_fgetc(file);

 for(i=0;i<4;i++) tmp[i] = pak_fgetc(file);
 tmp[4]='\0';
 if(strcmp(tmp,"MAIN"))
 {
  sprintf(FANTASY_DEBUG_STRING,"Main chunk not found\n");
  Debug(DEBUG_LEVEL_ERROR);
  return -1;
 }
 
 pak_read(&(header.first_frame),sizeof(int),1,file);
 pak_read(&(header.last_frame),sizeof(int),1,file);
 pak_read(&(header.framerate),sizeof(int),1,file);
 pak_read(&(header.delta_time),sizeof(int),1,file);
 pak_read(&(header.global_scale),sizeof(float),1,file);
 pak_read(&(header.nobjects),sizeof(int),1,file);
 pak_read(&(header.ncameras),sizeof(int),1,file);
 pak_read(&(header.nlights),sizeof(int),1,file); 
 pak_read(&(header.nmaterials),sizeof(int),1,file); 
 pak_read(&(header.ncontrollers),sizeof(int),1,file);
 pak_read(&(header.nbones),sizeof(int),1,file);
 
 world->number_of_objects = header.nobjects;
 world->number_of_camaras = header.ncameras;
 world->number_of_fuentes = header.nlights;
 world->number_of_materials = header.nmaterials;  
 world->number_of_bones = header.nbones;

 world->fuentes = (fuente *)malloc(sizeof(fuente)*world->number_of_fuentes);
 world->camaras=(camara *)malloc(sizeof(camara)*world->number_of_camaras);   
 world->obj = (object_type *)malloc(sizeof(object_type)*world->number_of_objects);
 if(header.nbones) 
 {
  world->bones = (Bone *)malloc(sizeof(Bone)*world->number_of_bones);
  sprintf(FANTASY_DEBUG_STRING,"There are bones there\n");
  Debug(DEBUG_LEVEL_OVERDEBUG);
 }
 else world->bones=NULL;

 materials = (u3d_material *)malloc(sizeof(u3d_material) * world->number_of_materials);
 
 if(!(world->fuentes) || !(world->camaras) || !(world->obj) || !(materials)) return -1;
 
 world->frame_inicial = header.first_frame;
 world->frame_final = header.last_frame;
 world->num_frames=world->frame_final-world->frame_inicial+1;   
  
 return 0; // Ok
}


//-----------------------------------------------------------------------------
// Carga de las mallas
//
// - Ignoramos los vertex color, si los hay
// - Hay que invertir las coordenadas V de las texturas en postprocesado
// - Hace falta postprocesar los objetos tras cargar todo el fichero, para sacar
//   el identificador REAL del material...
//-----------------------------------------------------------------------------

int LoadMeshes(PFILE *file)
{
 int i,j,k;
 object_type *curobj;
 char tmp[5];
 unsigned char flags;
 u3d_mesh mesh;
 int dummyint;
 float dummyfloat;

 for(i=0;i<4;i++) tmp[i] = pak_fgetc(file);
 tmp[4]='\0';
 if(strcmp(tmp,"MESH"))
 {
  sprintf(FANTASY_DEBUG_STRING,"Mesh chunk not found\n");
  Debug(DEBUG_LEVEL_ERROR);
  return -1;
 }
   
 for(i=0;i<header.nobjects;i++) 	
 {
  j=0;
  curobj = &(world->obj[i]);
  do
  {
   bigstring[j]	= pak_fgetc(file);
   j++;
  }while(bigstring[j-1]); 
 
  curobj->nombre_objeto = _strdup(bigstring);	

  	sprintf(FANTASY_DEBUG_STRING,"Loading Object %s \n",curobj->nombre_objeto);
	Debug(DEBUG_LEVEL_OVERDEBUG);


  pak_read(&(mesh.id),sizeof(int),1,file);
  pak_read(&(mesh.parentid),sizeof(int),1,file);
  pak_read(&(mesh.hidden),sizeof(unsigned char),1,file);  
  pak_read(&(mesh.position[0]),sizeof(float),1,file);
  pak_read(&(mesh.position[1]),sizeof(float),1,file);
  pak_read(&(mesh.position[2]),sizeof(float),1,file);   
  pak_read(&(mesh.rotation[0]),sizeof(float),1,file);
  pak_read(&(mesh.rotation[1]),sizeof(float),1,file);
  pak_read(&(mesh.rotation[2]),sizeof(float),1,file);   
  pak_read(&(mesh.rotation[3]),sizeof(float),1,file);    
  pak_read(&(mesh.scale[0]),sizeof(float),1,file);
  pak_read(&(mesh.scale[1]),sizeof(float),1,file);
  pak_read(&(mesh.scale[2]),sizeof(float),1,file);    
  pak_read(&(mesh.pivot[0]),sizeof(float),1,file);
  pak_read(&(mesh.pivot[1]),sizeof(float),1,file);
  pak_read(&(mesh.pivot[2]),sizeof(float),1,file);    
  curobj->id = mesh.id;	
  curobj->parentid = mesh.parentid;
    
  curobj->x = mesh.position[0];
  curobj->y = mesh.position[1];	
  curobj->z = mesh.position[2];    
    
  curobj->current_rotation.x = -mesh.rotation[0]; // Curioso esto de invertir las rotaciones
  curobj->current_rotation.y = -mesh.rotation[1];
  curobj->current_rotation.z = -mesh.rotation[2];  
  curobj->current_rotation.w = mesh.rotation[3]; 
  curobj->xscale = mesh.scale[0];
  curobj->yscale = mesh.scale[1];  
  curobj->zscale = mesh.scale[2];
  
  curobj->pivotx = mesh.pivot[0];
  curobj->pivoty = mesh.pivot[1];
  curobj->pivotz = mesh.pivot[2];  
        
  pak_read(&(mesh.flags),sizeof(int),1,file);
  pak_read(&(mesh.nverts),sizeof(float),1,file);
  pak_read(&(mesh.nfaces),sizeof(float),1,file);
  pak_read(&(mesh.nuvcoords),sizeof(float),1,file);
  pak_read(&(mesh.nvertexcolors),sizeof(float),1,file);

  curobj->number_of_vertices = mesh.nverts;
  curobj->number_of_polygons = mesh.nfaces;
  curobj->number_of_uv = mesh.nuvcoords;

  if(curobj->number_of_uv == 0)
  {
    sprintf(FANTASY_DEBUG_STRING,"WARNING: object %s has no texture coords\n",curobj->nombre_objeto);
    Debug(DEBUG_LEVEL_WARNING);	
  }
  

  // En caso de que el objeto sea de tipo skin, leemos la parte correspondiente

  if(mesh.flags & 4)
  {
	// If the object is a skin, then its initial position is not valid anymore...
	// Would this also be true for the rotation and scale????? Only time will tell
	
  	curobj->x = 0.0f;
  	curobj->y = 0.0f;	
  	curobj->z = 0.0f;    		  
  	
  	curobj->current_rotation.x = 0.0f;
  	curobj->current_rotation.y = 0.0f;
  	curobj->current_rotation.z = 0.0f;
  	curobj->current_rotation.w = 1.0f;
  	
	  
	pak_read(&flags,1,1,file);
	if(flags)	// Es una weighted skin, con mౠde un bone afectando a cada v豴ice
	{		
		int *NumBones;

		NumBones = (int *)malloc(sizeof(int) * curobj->number_of_vertices);
		curobj->BoneIds = (int *)malloc(4*sizeof(int) * curobj->number_of_vertices);
		curobj->BoneWeights = (float *)malloc(4*sizeof(float) * curobj->number_of_vertices);

		pak_read(NumBones,sizeof(int)*mesh.nverts,1,file); 	  

		for(k=0;k<curobj->number_of_vertices;k++)
		{
			// Como no siempre llegar�asta 4, lo hacemos en dos tandas
			// Primero los que s럥stଠen el fichero
			for(j=0;j<NumBones[k];j++)
			{
				pak_read(&(curobj->BoneIds[4*k+j]),sizeof(int),1,file); 	
			}
			// El resto se ponen a un valor 0
			for(j=NumBones[k];j<4;j++)
			{
				curobj->BoneIds[4*k+j] = 0;
			}
		}

		// Repetimos la jugada para los pesos
		for(k=0;k<curobj->number_of_vertices;k++)
		{
			// Como no siempre llegar�asta 4, lo hacemos en dos tandas
			// Primero los que s럥stଠen el fichero
			for(j=0;j<NumBones[k];j++)
			{
				pak_read(&(curobj->BoneWeights[4*k+j]),sizeof(float),1,file); 	  
			}
			// El resto se ponen a un valor 0
			for(j=NumBones[k];j<4;j++)
			{
				curobj->BoneWeights[4*k+j] = 0.0f;
			}
		}
		// Ya no nos hace falta mౠel n� de bones de cada v豴ice, as럱ue limpiamos esto
		free(NumBones);
	}
	else		// Mౠsencillo, 1 bone por v豴ice. Lo generalizamos a m�les
	{
		curobj->BoneIds = (int *)malloc(4*sizeof(int) * curobj->number_of_vertices);
		curobj->BoneWeights = (float *)malloc(4*sizeof(float) * curobj->number_of_vertices);

		for(k=0;k<curobj->number_of_vertices;k++)
		{
			// S򫭠tenemos un bone...

			pak_read(&(curobj->BoneIds[4*k]),sizeof(int),1,file); 	  
			
			// El resto se ponen a un valor 0
			curobj->BoneIds[4*k+1] = 0;
			curobj->BoneIds[4*k+2] = 0;
			curobj->BoneIds[4*k+3] = 0;
		}
		// Y los pesos, pues idem de lo mismo
		for(k=0;k<curobj->number_of_vertices;k++)
		{
			curobj->BoneWeights[4*k] = 1.0f;
			curobj->BoneWeights[4*k+1] = 0.0f;
			curobj->BoneWeights[4*k+2] = 0.0f;
			curobj->BoneWeights[4*k+3] = 0.0f;
		}
	}
  }
  else	// No hay skin. Para que quede limpio, inicializamos a NULL las variables necesarias
  {
	  curobj->BoneIds = NULL;
	  curobj->BoneWeights = NULL;
	  curobj->GLBoneWeights = NULL;
	  curobj->GLBoneIds = NULL;
	  curobj->bonetree = NULL;
	  curobj->BoneArray = NULL;
	  curobj->numbones=0;
  }


  // Reservamos memoria temporal para leer los arrays
  mesh.matids=NULL;
  if(mesh.nvertexcolors) curobj->ColorPointer=(float *)malloc(3*sizeof(float)*mesh.nvertexcolors);
   else curobj->ColorPointer=NULL;
  if(mesh.nfaces) mesh.matids=(int *)malloc(sizeof(int)*mesh.nfaces);
  
  curobj->TexCoordPointer = (float *)malloc(2 * curobj->number_of_uv * sizeof(float));
  curobj->VertexPointer = (float *)malloc(3 * curobj->number_of_vertices * sizeof(float));
  curobj->NormalPointer = (float *)malloc(3 * curobj->number_of_vertices * sizeof(float));

  // Asignamos memoria para el array de polgonos

  curobj->PolygonNormalPointer = (float *)malloc(3* curobj->number_of_polygons * sizeof(float));
  curobj->IndexPointer = (unsigned *)malloc(3* curobj->number_of_polygons * sizeof(unsigned));
  curobj->UVIndexPointer = (unsigned *)malloc(3* curobj->number_of_polygons * sizeof(unsigned));        
  curobj->EdgeFlagPointer= (unsigned char*)malloc(3* curobj->number_of_polygons * sizeof(unsigned char));
  if(mesh.nvertexcolors) curobj->ColorIndexPointer=(unsigned *)malloc(3* curobj->number_of_polygons * sizeof(unsigned));
   else curobj->ColorIndexPointer=NULL;
  
  // A los arrays efectivos para OpenGL se les reservar�memoria despu�
  curobj->GLTexCoordPointer = NULL;     
  curobj->GLVertexPointer = NULL;     
  curobj->GLNormalPointer = NULL;     
  curobj->GLIndexPointer = NULL;     
  curobj->GLColorPointer = NULL;
  
  // Ahora leemos los arrays. Lo hacemos a lo burro, todo junto
  
  pak_read(curobj->VertexPointer,3*sizeof(float)*mesh.nverts,1,file); 	  
  // Ojo, es posible que haya que invertir las coordenadas V de las texturas
  pak_read(curobj->TexCoordPointer,2*sizeof(float)*mesh.nuvcoords,1,file);  
  pak_read(curobj->ColorPointer,3*sizeof(float)*mesh.nvertexcolors,1,file);

  pak_read(curobj->IndexPointer,3*sizeof(int)*mesh.nfaces,1,file);
  pak_read(curobj->UVIndexPointer,3*sizeof(int)*mesh.nfaces,1,file);
  if(mesh.nvertexcolors) pak_read(curobj->ColorIndexPointer,3*sizeof(int)*mesh.nfaces,1,file);
  
  for(j=0;j<mesh.nfaces;j++)
  {
   pak_read(&flags,1,1,file);
   if(flags & 1) curobj->EdgeFlagPointer[3*j] = 1; else curobj->EdgeFlagPointer[3*j] = 0;
   if(flags & 2) curobj->EdgeFlagPointer[3*j+1] = 1; else curobj->EdgeFlagPointer[3*j+1] = 0;
   if(flags & 4) curobj->EdgeFlagPointer[3*j+2] = 1; else curobj->EdgeFlagPointer[3*j+2] = 0;
  }  

  pak_read(mesh.matids,sizeof(int)*mesh.nfaces,1,file);
  curobj->num_material = mesh.matids[0]; // OJO, ESTO ES TEMPORAL HASTA QUE SE POSTPROCESE
  
  
  // Para acabar leemos la bounding sphere
  if(mesh.flags & 1)
  {
  sprintf(FANTASY_DEBUG_STRING,"The object has a bounding sphere\n");
  Debug(DEBUG_LEVEL_OVERDEBUG);

	  
	  
   pak_read(&(curobj->bspherecenter[0]),sizeof(float),1,file);
   pak_read(&(curobj->bspherecenter[1]),sizeof(float),1,file);
   pak_read(&(curobj->bspherecenter[2]),sizeof(float),1,file);    
   pak_read(&(curobj->bsphereradius),sizeof(float),1,file);    
  }
  else
  {
   curobj->bspherecenter[0] = 0.0f;	
   curobj->bspherecenter[1] = 0.0f;
   curobj->bspherecenter[2] = 0.0f;
   curobj->bsphereradius=0.0f;
  }

  if(mesh.flags & 2)curobj->hint_dynamic=1;	// Este objeto va a tener updates...
  	else curobj->hint_dynamic=0;

  if(mesh.flags & 8) curobj->cast_shadows=1;	// this object should cast shadows
  	else curobj->cast_shadows=0;
  	
  // Al final del todo liberamos memoria
  if(mesh.matids)		free(mesh.matids);
  
  // Ponemos el resto de par�etros por defecto
  curobj->numero_keys_path=0;
  curobj->numero_keys_path_rotacion=0;
  curobj->numero_keys_path_scale=0;   
  curobj->posiciones_keys=NULL;
  curobj->path_rotacion=NULL;
  curobj->scale_keys=NULL;    
  curobj->VSpeed=0.0f;
  curobj->USpeed=0.0f;   
  curobj->VTranslate=0.0f;
  curobj->UTranslate=0.0f;
  curobj->tangentes_salida=NULL;
  curobj->tangentes_entrada=NULL;
  curobj->tangentes_salida_scale=NULL;
  curobj->tangentes_entrada_scale=NULL;
  curobj->tangentes_entrada_rotacion=NULL;
  curobj->tangentes_salida_rotacion=NULL;  
  curobj->normalize=0;
  curobj->updatevertices=0;
  curobj->visible=1;
  curobj->animate=1;
  curobj->vtx_shader_id=0xffffffff;
  curobj->render_mode=RENDER_NORMAL;    
  curobj->object_alpha = 1.0f;
  curobj->fx.id=0; // 0=no effect
  curobj->hierarchy=NULL;
  curobj->vbufinfo.usingvertexbuf=0;
  curobj->vbufinfo.vertexbufferid=0;
  curobj->vbufinfo.indexbufferid=0;
  curobj->vbufinfo.texbufferid=0;	
  curobj->vbufinfo.colorbufferid=0;
  curobj->vbufinfo.normalskip=0;
  curobj->bonetree=NULL;
 }	

 
 	sprintf(FANTASY_DEBUG_STRING,"Object %s has been succesfully loaded\n",curobj->nombre_objeto);
	Debug(DEBUG_LEVEL_OVERDEBUG);

 return 0;
}

//-----------------------------------------------------------------------------
// Carga de las camaras
//
// - Pendiente de ver qu�pasa con el roll
//-----------------------------------------------------------------------------

float getRollfromQuat(float x, float y, float z, float w)
{

 float q00, q11,q22,q33;
 float r11, r12, r13, r21, r31, r32, r33, tmp;
 
 q00=w*w;
 q11=x*x;
 q22=y*y;
 q33=z*z;
 
 r11=q00 + q11 - q22 - q33;
 r21=2*(x*y + w*z);
 r31=2*(x*z + w*y);
 r32=2*(y*z + w*x);
 r33=q00-q11-q22+q33;

 tmp=(float)fabs(r31);

 if(tmp>0.999999)
 {
		r12=2*(x*y + w*z);
		r13=2*(x*z + w*y);		
		return (float)atan2(-r12,-r31*r13);
 }
 else
 {
 	return (float)atan2(r21,r11);
 }
}



int LoadCameras(PFILE * file)
{
 int i,j;
 camara *curcam;
 char tmp[5];
 u3d_camera camera;	

 for(i=0;i<4;i++) tmp[i] = pak_fgetc(file);
 tmp[4]='\0';

 if(strcmp(tmp,"CAMS"))
 {
  sprintf(FANTASY_DEBUG_STRING,"Camera chunk not found, found %s instead\n",tmp);
  Debug(DEBUG_LEVEL_ERROR);
  return -1;
 }
 for(i=0;i<header.ncameras;i++)
 {
  j=0;
  curcam=&(world->camaras[i]);
  do
  {
   bigstring[j]	= pak_fgetc(file);
   j++;
  }while(bigstring[j-1]); 
 
  curcam->nombre_camara = _strdup(bigstring);	
  pak_read(&(camera.id),sizeof(int),1,file);
  pak_read(&(camera.parentid),sizeof(int),1,file);
  pak_read(&(camera.targetid),sizeof(int),1,file);    
  pak_read(&(camera.hidden),sizeof(unsigned char),1,file);  
  pak_read(&(camera.position[0]),sizeof(float),1,file);
  pak_read(&(camera.position[1]),sizeof(float),1,file);
  pak_read(&(camera.position[2]),sizeof(float),1,file);   
  pak_read(&(camera.rotation[0]),sizeof(float),1,file);
  pak_read(&(camera.rotation[1]),sizeof(float),1,file);
  pak_read(&(camera.rotation[2]),sizeof(float),1,file);   
  pak_read(&(camera.rotation[3]),sizeof(float),1,file);    
  pak_read(&(camera.scale[0]),sizeof(float),1,file);
  pak_read(&(camera.scale[1]),sizeof(float),1,file);
  pak_read(&(camera.scale[2]),sizeof(float),1,file);    
  pak_read(&(camera.pivot[0]),sizeof(float),1,file);
  pak_read(&(camera.pivot[1]),sizeof(float),1,file);
  pak_read(&(camera.pivot[2]),sizeof(float),1,file);    

  pak_read(&(camera.fov),sizeof(float),1,file);
  pak_read(&(camera.near_range),sizeof(float),1,file);
  pak_read(&(camera.far_range),sizeof(float),1,file);    
 
  curcam->id = camera.id;
  curcam->target_id = camera.targetid;
  curcam->wx = camera.position[0];
  curcam->wy = camera.position[1]; 
  curcam->wz = camera.position[2];
  curcam->fov = camera.fov;
  curcam->near_range = camera.near_range;
  curcam->far_range = camera.far_range;
  curcam->inclinacion = 0.0f;			// Ojito, est�pendiente de la consulta
  //curcam->inclinacion = (getRollfromQuat(camera.rotation[0],camera.rotation[1],camera.rotation[2],camera.rotation[3])*180.0f)/3.1415927f;
  curcam->animate=1;
  
  curcam->current_rotation.x = -camera.rotation[0]; // Curioso esto de invertir las rotaciones
  curcam->current_rotation.y = -camera.rotation[1];
  curcam->current_rotation.z = -camera.rotation[2];  
  curcam->current_rotation.w = camera.rotation[3]; 
  
  // Ponemos el resto de par�etros por defecto
  
  curcam->numero_keys_path=0;  
  curcam->posiciones_keys=NULL; 
  curcam->tangentes_salida=NULL;
  curcam->tangentes_entrada=NULL;
  curcam->numero_keys_rollpath=0;
  curcam->posiciones_rollkeys=NULL;
  curcam->tangentes_entrada_roll=NULL;
  curcam->tangentes_salida_roll=NULL; 
  curcam->numero_keys_path_target=0;  
  curcam->posiciones_keys_target=NULL; 
  curcam->tangentes_salida_target=NULL;
  curcam->tangentes_entrada_target=NULL;
  curcam->numero_keys_path_target_rotacion=0;
  curcam->path_target_rotacion=NULL; 
  curcam->tangentes_entrada_target_rotacion=NULL;
  curcam->tangentes_salida_target_rotacion=NULL;    
  curcam->numero_keys_fovpath=0; 
  curcam->posiciones_fovkeys=NULL; 
  curcam->tangentes_entrada_fov=NULL;
  curcam->tangentes_salida_fov=NULL;     
 } 
 
 return 0;
}

//-----------------------------------------------------------------------------
// Funcin auxiliar para localizar una c�ara a partir del target id
//
// - OJO! Asumimos que ya est� cargadas las c�aras, si no va a petar :) 
//-----------------------------------------------------------------------------

int FindCamera(int targetid)
{
 int i;
 
 for(i=0;i<world->number_of_camaras;i++)
 {
  if(world->camaras[i].target_id == targetid) 
    return i;	// C�ara localizada
 }	
 return -1; // No hay c�ara con ese target ??
}

//-----------------------------------------------------------------------------
// Carga de los camera targets
//
// - Tocamos la misma estructura de la anterior
//-----------------------------------------------------------------------------

int LoadCameraTargets(PFILE *file)
{
 int i,j;
 camara *curcam;
 char tmp[5];
 u3d_camera_target ctarget;	
 int parent;
 

 for(i=0;i<4;i++) tmp[i] = pak_fgetc(file);
 tmp[4]='\0';
 if(strcmp(tmp,"CTGT"))
 {
  sprintf(FANTASY_DEBUG_STRING,"Camera targets chunk not found\n");
  Debug(DEBUG_LEVEL_ERROR);
  return -1;	
 }

 for(i=0;i<header.ncameras;i++)
 {
  j=0;
  do
  {
   bigstring[j]	= pak_fgetc(file);
   j++;
  }while(bigstring[j-1]); 
 
  //ctarget.name = _strdup(bigstring);  	
  pak_read(&(ctarget.id),sizeof(int),1,file);
  pak_read(&(ctarget.parentid),sizeof(int),1,file);
  pak_read(&(ctarget.hidden),sizeof(unsigned char),1,file);  
  pak_read(&(ctarget.position[0]),sizeof(float),1,file);
  pak_read(&(ctarget.position[1]),sizeof(float),1,file);
  pak_read(&(ctarget.position[2]),sizeof(float),1,file);   
  pak_read(&(ctarget.rotation[0]),sizeof(float),1,file);
  pak_read(&(ctarget.rotation[1]),sizeof(float),1,file);
  pak_read(&(ctarget.rotation[2]),sizeof(float),1,file);   
  pak_read(&(ctarget.rotation[3]),sizeof(float),1,file);    
  pak_read(&(ctarget.scale[0]),sizeof(float),1,file);
  pak_read(&(ctarget.scale[1]),sizeof(float),1,file);
  pak_read(&(ctarget.scale[2]),sizeof(float),1,file);    
  pak_read(&(ctarget.pivot[0]),sizeof(float),1,file);
  pak_read(&(ctarget.pivot[1]),sizeof(float),1,file);
  pak_read(&(ctarget.pivot[2]),sizeof(float),1,file);    

  parent = FindCamera(ctarget.id);
  if(parent == -1) continue; // Este target no tiene a nadie asociado (?)
  
  curcam = &(world->camaras[parent]);
  curcam->nombre_camara_target=_strdup(bigstring);
  curcam->wxt = ctarget.position[0];
  curcam->wyt = ctarget.position[1];
  curcam->wzt = ctarget.position[2];
  curcam->animate_target=1;
 } 
 return 0;	
}

//-----------------------------------------------------------------------------
// Carga de las luces
//
//-----------------------------------------------------------------------------

int LoadLights(PFILE *file)
{
 int i,j;
 fuente *curfuente;
 char tmp[5];
 u3d_light light;
 
 for(i=0;i<4;i++) tmp[i] = pak_fgetc(file);
 tmp[4]='\0';
 if(strcmp(tmp,"LITE"))
 {
  sprintf(FANTASY_DEBUG_STRING,"Light chunk not found\n");
  Debug(DEBUG_LEVEL_ERROR);
  return -1;	
 }

 for(i=0;i<header.nlights;i++)
 {
  j=0;
  curfuente=&(world->fuentes[i]);
  do
  {
   bigstring[j]	= pak_fgetc(file);
   j++;
  }while(bigstring[j-1]); 
 
  curfuente->nombre_fuente = _strdup(bigstring);	
  pak_read(&(light.id),sizeof(int),1,file);
  pak_read(&(light.parentid),sizeof(int),1,file);
  pak_read(&(light.targetid),sizeof(int),1,file);
  pak_read(&(light.hidden),sizeof(unsigned char),1,file);  
  pak_read(&(light.position[0]),sizeof(float),1,file);
  pak_read(&(light.position[1]),sizeof(float),1,file);
  pak_read(&(light.position[2]),sizeof(float),1,file);   
  pak_read(&(light.rotation[0]),sizeof(float),1,file);
  pak_read(&(light.rotation[1]),sizeof(float),1,file);
  pak_read(&(light.rotation[2]),sizeof(float),1,file);   
  pak_read(&(light.rotation[3]),sizeof(float),1,file);    
  pak_read(&(light.scale[0]),sizeof(float),1,file);
  pak_read(&(light.scale[1]),sizeof(float),1,file);
  pak_read(&(light.scale[2]),sizeof(float),1,file);    
  pak_read(&(light.pivot[0]),sizeof(float),1,file);
  pak_read(&(light.pivot[1]),sizeof(float),1,file);
  pak_read(&(light.pivot[2]),sizeof(float),1,file);    

  pak_read(&(light.light_type),sizeof(unsigned int),1,file);
  pak_read(&(light.color[0]),sizeof(float),1,file);
  pak_read(&(light.color[1]),sizeof(float),1,file);
  pak_read(&(light.color[2]),sizeof(float),1,file);    
  pak_read(&(light.inicio_rango),sizeof(float),1,file);    
  pak_read(&(light.fin_rango),sizeof(float),1,file);      
  pak_read(&(light.atenuacion),sizeof(unsigned char),1,file);    
  pak_read(&(light.multiplicador),sizeof(float),1,file);      
  
  if(light.light_type == 1 || light.light_type == 2)
  {
  	pak_read(&(light.target_position[0]),sizeof(float),1,file);
  	pak_read(&(light.target_position[1]),sizeof(float),1,file);
  	pak_read(&(light.target_position[2]),sizeof(float),1,file);   
  	if(light.light_type == 2)
  	{
  		pak_read(&(light.cut_off),sizeof(float),1,file);      
  		pak_read(&(light.exponent),sizeof(float),1,file);    
		curfuente->cut_off = light.cut_off;
  		curfuente->exponent = light.exponent;  		  		
  	}
  }

  curfuente->numero_keys_direction = 0;
  curfuente->id = light.id;
  curfuente->target_id = light.targetid;
  curfuente->type = light.light_type;
  curfuente->wx = light.position[0];  
  curfuente->wy = light.position[1];
  curfuente->wz = light.position[2];
  curfuente->r = light.color[0];      
  curfuente->g = light.color[1];
  curfuente->b = light.color[2];  
  curfuente->inicio_rango = light.inicio_rango;
  curfuente->fin_rango = light.fin_rango;
  curfuente->multiplicador = light.multiplicador;
  curfuente->atenuacion = light.atenuacion;
  curfuente->animate = 1;
  curfuente->tx = light.target_position[0];  
  curfuente->ty = light.target_position[1];
  curfuente->tz = light.target_position[2];

  // Ponemos el resto de par�etros por defecto
  curfuente->numero_keys_path=0;
  curfuente->posiciones_keys=NULL;
  curfuente->tangentes_salida=NULL;
  curfuente->tangentes_entrada=NULL;
 } 
 
 return 0;	
}

//-----------------------------------------------------------------------------
// Carga de los bones
//
//-----------------------------------------------------------------------------
int LoadBones(PFILE *file)
{
 int i,j;
 char tmp[5];
 u3d_bone bone;
 Bone *curbone;

 for(i=0;i<4;i++) tmp[i] = pak_fgetc(file);
 tmp[4]='\0';

 if(strcmp(tmp,"BONE"))
 {
  sprintf(FANTASY_DEBUG_STRING,"Bone chunk not found\n");
  Debug(DEBUG_LEVEL_ERROR);
  return -1;
 }
 for(i=0;i<header.nbones;i++)
 {
  j=0;
  curbone=&(world->bones[i]);
  do
  {
   bigstring[j]	= pak_fgetc(file);
   j++;
  }while(bigstring[j-1]); 
 
  curbone->nombre = _strdup(bigstring);	
  pak_read(&(bone.id),sizeof(int),1,file);
  pak_read(&(bone.parentid),sizeof(int),1,file);
  pak_read(&(bone.hidden),sizeof(unsigned char),1,file);  
  pak_read(&(bone.position[0]),sizeof(float),1,file);
  pak_read(&(bone.position[1]),sizeof(float),1,file);
  pak_read(&(bone.position[2]),sizeof(float),1,file);   
  pak_read(&(bone.rotation[0]),sizeof(float),1,file);
  pak_read(&(bone.rotation[1]),sizeof(float),1,file);
  pak_read(&(bone.rotation[2]),sizeof(float),1,file);   
  pak_read(&(bone.rotation[3]),sizeof(float),1,file);    
  pak_read(&(bone.scale[0]),sizeof(float),1,file);
  pak_read(&(bone.scale[1]),sizeof(float),1,file);
  pak_read(&(bone.scale[2]),sizeof(float),1,file);    
  pak_read(&(bone.pivot[0]),sizeof(float),1,file);
  pak_read(&(bone.pivot[1]),sizeof(float),1,file);
  pak_read(&(bone.pivot[2]),sizeof(float),1,file);    
  
  curbone->id = bone.id;
  curbone->parentid = bone.parentid;
  curbone->numero_keys_path=0;
  curbone->posiciones_keys=NULL;
  curbone->tangentes_salida=NULL;
  curbone->tangentes_entrada=NULL;

  curbone->numero_keys_path_rotacion=0;
  curbone->path_rotacion=NULL; 
  curbone->tangentes_entrada_rotacion=NULL;
  curbone->tangentes_salida_rotacion=NULL;
  
  curbone->numero_keys_path_scale=0;
  curbone->scale_keys=NULL; 
  curbone->tangentes_entrada_scale=NULL;
  curbone->tangentes_salida_scale=NULL;  
  curbone->hierarchy=NULL;
  
  curbone->x = bone.position[0];
  curbone->y = bone.position[1];
  curbone->z = bone.position[2];
  
  curbone->xscale = bone.scale[0];
  curbone->yscale = bone.scale[1];
  curbone->zscale = bone.scale[2];
    
  curbone->current_rotation.x = -bone.rotation[0]; // Curioso esto de invertir las rotaciones
  curbone->current_rotation.y = -bone.rotation[1];
  curbone->current_rotation.z = -bone.rotation[2];  
  curbone->current_rotation.w = bone.rotation[3]; 
  
  // The matrices are calculated at runtime
  matriz_unidad(curbone->m_ref_inv);
  matriz_unidad(curbone->m_transform);
  matriz_unidad(curbone->final_matrix);

 }
	return 0;
}



//-----------------------------------------------------------------------------
// Carga de los materiales
//
//-----------------------------------------------------------------------------

int LoadMats(PFILE *file)
{
 int i,j;
 char tmp[5];	
 u3d_material *curmat;

 for(i=0;i<4;i++) tmp[i] = pak_fgetc(file);
 tmp[4]='\0';
 if(strcmp(tmp,"MATS"))
 {
  sprintf(FANTASY_DEBUG_STRING,"Material chunk not found\n");
  Debug(DEBUG_LEVEL_ERROR);
  return -1;	
 }
 
 for(i=0;i<header.nmaterials;i++)
 {
  j=0;
  curmat=&(materials[i]);
  do
  {
   bigstring[j]	= pak_fgetc(file);
   j++;
  }while(bigstring[j-1]); 
 
  curmat->name= _strdup(bigstring);	
  pak_read(&(curmat->id),sizeof(int),1,file);  
  curmat->matid = world->mats.FindMaterial(curmat->name);
 }
 
 return 0; // Ok
}

//-----------------------------------------------------------------------------
// Funciones auxiliares para localizar el path concreto entre todos los objetos
//
//-----------------------------------------------------------------------------
static info_pos_path **FindPosPath(int id,int nkeys)
{
 int i;
 
 for(i=0;i<world->number_of_objects;i++)
 {
  if(world->obj[i].id == id) 
  {
    world->obj[i].numero_keys_path = nkeys;
    return &(world->obj[i].posiciones_keys);
  }
 }
 for(i=0;i<world->number_of_fuentes;i++)
 {
  if(world->fuentes[i].id == id) 
  {
    world->fuentes[i].numero_keys_path = nkeys;  	
    return &(world->fuentes[i].posiciones_keys);
  }
  if(world->fuentes[i].target_id == id) 
  {
    world->fuentes[i].numero_keys_direction = nkeys;  	
    return &(world->fuentes[i].direction_keys);
  }  
 }
 for(i=0;i<world->number_of_camaras;i++)
 {
  if(world->camaras[i].id == id) 
  {
    world->camaras[i].numero_keys_path = nkeys;
    return &(world->camaras[i].posiciones_keys);
  }
  if(world->camaras[i].target_id == id) 
  {
    world->camaras[i].numero_keys_path_target = nkeys;
    return &(world->camaras[i].posiciones_keys_target);
  }
 }
 for(i=0;i<world->number_of_bones;i++)
 {
	if(world->bones[i].id == id)
	{
		world->bones[i].numero_keys_path = nkeys;
		return &(world->bones[i].posiciones_keys);
	}
 }
 return NULL;
}

static info_rot_path **FindRotPath(int id,int nkeys)
{
 int i;
 
 for(i=0;i<world->number_of_objects;i++)
 {
  if(world->obj[i].id == id) 
  {
    world->obj[i].numero_keys_path_rotacion = nkeys;
    return &(world->obj[i].path_rotacion);
  } 
 }
 for(i=0;i<world->number_of_camaras;i++)
 {
  if(world->camaras[i].target_id == id) 
  {
    world->camaras[i].numero_keys_path_target_rotacion = nkeys;
    return &(world->camaras[i].path_target_rotacion);
  } 
 } 
 for(i=0;i<world->number_of_bones;i++)
 {
	if(world->bones[i].id == id)
	{
		world->bones[i].numero_keys_path_rotacion = nkeys;
		return &(world->bones[i].path_rotacion);
	}
 }
 return NULL;
}

static info_scale_path **FindScalePath(int id,int nkeys)
{
 int i;
 
 for(i=0;i<world->number_of_objects;i++)
 {
  if(world->obj[i].id == id) 
  {
    world->obj[i].numero_keys_path_scale = nkeys;
    return &(world->obj[i].scale_keys);
  } 
 }
 for(i=0;i<world->number_of_bones;i++)
 {
	if(world->bones[i].id == id)
	{
		world->bones[i].numero_keys_path_scale = nkeys;
		return &(world->bones[i].scale_keys);
	}
 } 
 return NULL;
}

static info_fov_path **FindFovPath(int id,int nkeys)
{
 int i;
 
 for(i=0;i<world->number_of_camaras;i++)
 {
  if(world->camaras[i].id == id) 
  {
    world->camaras[i].numero_keys_fovpath = nkeys;
    return &(world->camaras[i].posiciones_fovkeys);
  }
 }
 return NULL;
}

static info_roll_path **FindRollPath(int id,int nkeys)
{
 int i;
 
 for(i=0;i<world->number_of_camaras;i++)
 {
  if(world->camaras[i].id == id) 
  {
    world->camaras[i].numero_keys_rollpath = nkeys;
    return &(world->camaras[i].posiciones_rollkeys);
  }
 }
 return NULL;
}

//-----------------------------------------------------------------------------
// Carga de los controladores
//
//-----------------------------------------------------------------------------
int LoadControllers(PFILE *file)
{
 int i,j;
 char tmp[5];	
 u3d_controller ctrl;
 unsigned char flags;
 

 for(i=0;i<4;i++) tmp[i] = pak_fgetc(file);
 tmp[4]='\0';
 if(strcmp(tmp,"CTRL"))
 {
  sprintf(FANTASY_DEBUG_STRING,"Controller chunk not found, found %s instead\n",tmp);
  Debug(DEBUG_LEVEL_ERROR);
  return -1;	
 }
 for(i=0;i<header.ncontrollers;i++)
 {
  pak_read(&(ctrl.ctrl_type),sizeof(unsigned char),1,file); 
  pak_read(&(ctrl.id),sizeof(int),1,file); 
  pak_read(&(ctrl.owner_id),sizeof(int),1,file); 
  pak_read(&(ctrl.nkeyframes),sizeof(int),1,file);  	
  pak_read(&(ctrl.interp_scheme),sizeof(unsigned char),1,file); 

  if( (ctrl.interp_scheme != U3D_SCHEME_TCB) ||  (ctrl.ctrl_type == U3D_CTRL_UNKNOWN))
  {
   sprintf(FANTASY_DEBUG_STRING,"Unsupported controller... %d\n",ctrl.interp_scheme);
   Debug(DEBUG_LEVEL_WARNING);
   continue;
  }
  switch(ctrl.ctrl_type)
  {
   case U3D_CTRL_POSITION:{
			   info_pos_path **path;
			   int dummy;
			   float dummyf;
			   
			   path = FindPosPath(ctrl.owner_id,ctrl.nkeyframes);
			   if(!path)
			   {
			    sprintf(FANTASY_DEBUG_STRING,"Warning: corrupt u3d file. No object owns a position path, but it was supposed to be %d\n",ctrl.owner_id);
   			    Debug(DEBUG_LEVEL_ERROR);

		            // Skip controller
			    for(j=0;j<ctrl.nkeyframes;j++)
			    {
				pak_read(&dummy,sizeof(int),1,file);  // Time
			        pak_read(&dummyf,sizeof(float),1,file); // position
			        pak_read(&dummyf,sizeof(float),1,file); 
			        pak_read(&dummyf,sizeof(float),1,file); 
			        pak_read(&flags,sizeof(unsigned char),1,file);  // Flags 
			        if(flags & 1) pak_read(&dummyf,sizeof(float),1,file); 
			        if(flags & 2) pak_read(&dummyf,sizeof(float),1,file); 
			        if(flags & 4) pak_read(&dummyf,sizeof(float),1,file); 			        
	        		if(flags & 8) pak_read(&dummyf,sizeof(float),1,file); 			        
	        		if(flags & 16) pak_read(&dummyf,sizeof(float),1,file); 			        
			    }				
			    continue; // Next iteration
			   }
			   *path = (info_pos_path *)malloc(sizeof(info_pos_path)*ctrl.nkeyframes);
			   if(!*path) return -1;

			   for(j=0;j<ctrl.nkeyframes;j++)
			   {
			     pak_read(&dummy,sizeof(int),1,file);  // Time
			     (*path)[j].num_frame = dummy / header.delta_time;
			     pak_read(&((*path)[j].wx),sizeof(float),1,file); // position
			     pak_read(&((*path)[j].wy),sizeof(float),1,file); 
			     pak_read(&((*path)[j].wz),sizeof(float),1,file); 
			     
			     pak_read(&flags,sizeof(unsigned char),1,file);  // Flags 
			     if(flags & 1) pak_read(&((*path)[j].spflags.tension),sizeof(float),1,file); 
			      else (*path)[j].spflags.tension = 0.0f;
			     if(flags & 2) pak_read(&((*path)[j].spflags.continuity),sizeof(float),1,file); 
			      else (*path)[j].spflags.continuity = 0.0f;
			     if(flags & 4) pak_read(&((*path)[j].spflags.bias),sizeof(float),1,file); 
			      else (*path)[j].spflags.bias = 0.0f;
			     if(flags & 8) pak_read(&(dummyf),sizeof(float),1,file); // ease in
			     if(flags & 16) pak_read(&(dummyf),sizeof(float),1,file); // ease out
			   }			   			   
			  }
   			  break;
   case U3D_CTRL_ROTATION: {
			    info_rot_path **path;		    
		            int dummy;
			    float dummyf;
			   
			    path = FindRotPath(ctrl.owner_id,ctrl.nkeyframes);			    
 			    if(!path) 			   
			    {
			     sprintf(FANTASY_DEBUG_STRING,"Warning: corrupt u3d file. No object owns a rotation path, but it was supposed to be %d\n",ctrl.owner_id);
   			     Debug(DEBUG_LEVEL_ERROR);

			     // Skip controller
			     for(j=0;j<ctrl.nkeyframes;j++)
			     {
				pak_read(&dummy,sizeof(int),1,file);  // Time
			        pak_read(&dummyf,sizeof(float),1,file); // quaternio
			        pak_read(&dummyf,sizeof(float),1,file); 
			        pak_read(&dummyf,sizeof(float),1,file); 
			        pak_read(&dummyf,sizeof(float),1,file); 
			        pak_read(&flags,sizeof(unsigned char),1,file);  // Flags 
			        if(flags & 1) pak_read(&dummyf,sizeof(float),1,file); 
			        if(flags & 2) pak_read(&dummyf,sizeof(float),1,file); 
			        if(flags & 4) pak_read(&dummyf,sizeof(float),1,file); 			        
	        		if(flags & 8) pak_read(&dummyf,sizeof(float),1,file); 			        
	        		if(flags & 16) pak_read(&dummyf,sizeof(float),1,file); 			        
			     }				
			     continue; // Next iteration
			    }
			    *path = (info_rot_path *)malloc(sizeof(info_rot_path)*ctrl.nkeyframes);
			    if(!*path) return -1;
	
			    for(j=0;j<ctrl.nkeyframes;j++)
			    {
			     pak_read(&dummy,sizeof(int),1,file);  // Time
			     (*path)[j].num_frame = dummy / header.delta_time;
			     pak_read(&((*path)[j].quat.x),sizeof(float),1,file); // quaternio
			     pak_read(&((*path)[j].quat.y),sizeof(float),1,file); 
			     pak_read(&((*path)[j].quat.z),sizeof(float),1,file); 
			     pak_read(&((*path)[j].quat.w),sizeof(float),1,file); 
		
			     (*path)[j].quat.x = -(*path)[j].quat.x;
			     (*path)[j].quat.y = -(*path)[j].quat.y;
			     (*path)[j].quat.z = -(*path)[j].quat.z;
					     
			     pak_read(&flags,sizeof(unsigned char),1,file);  // Flags 
			     if(flags & 1) pak_read(&((*path)[j].spflags.tension),sizeof(float),1,file); 
			      else (*path)[j].spflags.tension = 0.0f;
			     if(flags & 2) pak_read(&((*path)[j].spflags.continuity),sizeof(float),1,file); 
			      else (*path)[j].spflags.continuity = 0.0f;			      
			      
			     if(flags & 4) pak_read(&((*path)[j].spflags.bias),sizeof(float),1,file); 
			      else (*path)[j].spflags.bias = 0.0f;
			     if(flags & 8) pak_read(&(dummyf),sizeof(float),1,file); // ease in
			     if(flags & 16) pak_read(&(dummyf),sizeof(float),1,file); // ease out			     
			    }			   			   
			   }
   			   break;
   case U3D_CTRL_SCALE: {
			 info_scale_path **path;
			 
 			 int dummy;
			 float dummyf;
			   
			 path = FindScalePath(ctrl.owner_id,ctrl.nkeyframes);			 
 		         if(!path)
			 {
			  sprintf(FANTASY_DEBUG_STRING,"Warning: corrupt u3d file. No object owns a scale path, but it was supposed to be %d\n",ctrl.owner_id);
   			  Debug(DEBUG_LEVEL_ERROR);
		          // Skip controller
			  for(j=0;j<ctrl.nkeyframes;j++)
			  {
				pak_read(&dummy,sizeof(int),1,file);  // Time
			        pak_read(&dummyf,sizeof(float),1,file); // scale
			        pak_read(&dummyf,sizeof(float),1,file); 
			        pak_read(&dummyf,sizeof(float),1,file); 
			        pak_read(&dummyf,sizeof(float),1,file); // 4 unused values :)
			        pak_read(&dummyf,sizeof(float),1,file); 
			        pak_read(&dummyf,sizeof(float),1,file); 
			        pak_read(&dummyf,sizeof(float),1,file); 			        			  
			        pak_read(&flags,sizeof(unsigned char),1,file);  // Flags 
			        if(flags & 1) pak_read(&dummyf,sizeof(float),1,file); 
			        if(flags & 2) pak_read(&dummyf,sizeof(float),1,file); 
			        if(flags & 4) pak_read(&dummyf,sizeof(float),1,file); 			        
	        		if(flags & 8) pak_read(&dummyf,sizeof(float),1,file); 			        
	        		if(flags & 16) pak_read(&dummyf,sizeof(float),1,file); 			        
			  }				
			  continue; // Next iteration   			     			   			  
			 }
			 *path = (info_scale_path *)malloc(sizeof(info_scale_path)*ctrl.nkeyframes);
			 if(!*path) return -1;

			 for(j=0;j<ctrl.nkeyframes;j++)
			 {
			   pak_read(&dummy,sizeof(int),1,file);  // Time
			   (*path)[j].num_frame = dummy / header.delta_time;
			   pak_read(&((*path)[j].scalex),sizeof(float),1,file); // scale
			   pak_read(&((*path)[j].scaley),sizeof(float),1,file); 
			   pak_read(&((*path)[j].scalez),sizeof(float),1,file); 
			   
			   pak_read(&(dummyf),sizeof(float),1,file); // 4 unused values :)
			   pak_read(&(dummyf),sizeof(float),1,file);
			   pak_read(&(dummyf),sizeof(float),1,file);
			   pak_read(&(dummyf),sizeof(float),1,file);
			   
			   pak_read(&flags,sizeof(unsigned char),1,file);  // Flags 
			   if(flags & 1) pak_read(&((*path)[j].spflags.tension),sizeof(float),1,file); 
			    else (*path)[j].spflags.tension = 0.0f;
			   if(flags & 2) pak_read(&((*path)[j].spflags.continuity),sizeof(float),1,file); 
			    else (*path)[j].spflags.continuity = 0.0f;
			   if(flags & 4) pak_read(&((*path)[j].spflags.bias),sizeof(float),1,file); 
			    else (*path)[j].spflags.bias = 0.0f;
			   if(flags & 8) pak_read(&(dummyf),sizeof(float),1,file); // ease in
			   if(flags & 16) pak_read(&(dummyf),sizeof(float),1,file); // ease out			   
			 }			   			   			 
			}
   				break;
   case U3D_CTRL_FOV:   {
			 info_fov_path **path;
 			 int dummy;
			 float dummyf;
			   
			 path = FindFovPath(ctrl.owner_id,ctrl.nkeyframes);			 
 		         if(!path)
			 {
			  sprintf(FANTASY_DEBUG_STRING,"Warning: corrupt u3d file. No camera owns a fov path, but it was supposed to be %d\n",ctrl.owner_id);
   			  Debug(DEBUG_LEVEL_ERROR);
			  // Skip controller
			  for(j=0;j<ctrl.nkeyframes;j++)
			  {
				pak_read(&dummy,sizeof(int),1,file);  // Time
			        pak_read(&dummyf,sizeof(float),1,file); // fov	        			  
			        pak_read(&flags,sizeof(unsigned char),1,file);  // Flags 
			        if(flags & 1) pak_read(&dummyf,sizeof(float),1,file); 
			        if(flags & 2) pak_read(&dummyf,sizeof(float),1,file); 
			        if(flags & 4) pak_read(&dummyf,sizeof(float),1,file); 			        
	        		if(flags & 8) pak_read(&dummyf,sizeof(float),1,file); 			        
	        		if(flags & 16) pak_read(&dummyf,sizeof(float),1,file); 			        
			  }				
			  continue; // Next iteration   
			 }
			 *path = (info_fov_path *)malloc(sizeof(info_fov_path)*ctrl.nkeyframes);
			 if(!*path) return -1;

			 for(j=0;j<ctrl.nkeyframes;j++)
			 {
			   pak_read(&dummy,sizeof(int),1,file);  // Time
			   (*path)[j].num_frame = dummy / header.delta_time;
			   pak_read(&((*path)[j].fov),sizeof(float),1,file); // fov
		           pak_read(&flags,sizeof(unsigned char),1,file);  // Flags 
			   if(flags & 1) pak_read(&((*path)[j].spflags.tension),sizeof(float),1,file); 
			    else (*path)[j].spflags.tension = 0.0f;
			   if(flags & 2) pak_read(&((*path)[j].spflags.continuity),sizeof(float),1,file); 
			    else (*path)[j].spflags.continuity = 0.0f;
			   if(flags & 4) pak_read(&((*path)[j].spflags.bias),sizeof(float),1,file); 
			    else (*path)[j].spflags.bias = 0.0f;
			   if(flags & 8) pak_read(&(dummyf),sizeof(float),1,file); // ease in
			   if(flags & 16) pak_read(&(dummyf),sizeof(float),1,file); // ease out			 
			 }
			}
   			break;
   case U3D_CTRL_ROLL:  {
			 info_roll_path **path;
 			 int dummy;
			 float dummyf;
			   
			 path = FindRollPath(ctrl.owner_id,ctrl.nkeyframes);			 
 		         if(!path)
			 {
			  sprintf(FANTASY_DEBUG_STRING,"Warning: corrupt u3d file. No camera owns a roll path, but it was supposed to be %d\n",ctrl.owner_id);
   			  Debug(DEBUG_LEVEL_ERROR);
			  // Skip controller
			  for(j=0;j<ctrl.nkeyframes;j++)
			  {
				pak_read(&dummy,sizeof(int),1,file);  // Time
			        pak_read(&dummyf,sizeof(float),1,file); // roll	        			  
			        pak_read(&flags,sizeof(unsigned char),1,file);  // Flags 
			        if(flags & 1) pak_read(&dummyf,sizeof(float),1,file); 
			        if(flags & 2) pak_read(&dummyf,sizeof(float),1,file); 
			        if(flags & 4) pak_read(&dummyf,sizeof(float),1,file); 			        
	        		if(flags & 8) pak_read(&dummyf,sizeof(float),1,file); 			        
	        		if(flags & 16) pak_read(&dummyf,sizeof(float),1,file); 			        
			  }				
			  continue; // Next iteration   
			 }
			 *path = (info_roll_path *)malloc(sizeof(info_roll_path)*ctrl.nkeyframes);
			 if(!*path) return -1;

			 for(j=0;j<ctrl.nkeyframes;j++)
			 {
			   pak_read(&dummy,sizeof(int),1,file);  // Time
			   (*path)[j].num_frame = dummy / header.delta_time;
			   pak_read(&((*path)[j].roll),sizeof(float),1,file); // scale
			   pak_read(&flags,sizeof(unsigned char),1,file);  // Flags 
			   if(flags & 1) pak_read(&((*path)[j].spflags.tension),sizeof(float),1,file); 
			    else (*path)[j].spflags.tension = 0.0f;
			   if(flags & 2) pak_read(&((*path)[j].spflags.continuity),sizeof(float),1,file); 
			    else (*path)[j].spflags.continuity = 0.0f;
			   if(flags & 4) pak_read(&((*path)[j].spflags.bias),sizeof(float),1,file); 
			    else (*path)[j].spflags.bias = 0.0f;
			   if(flags & 8) pak_read(&(dummyf),sizeof(float),1,file); // ease in
			   if(flags & 16) pak_read(&(dummyf),sizeof(float),1,file); // ease out
			 }

			}
   				break;
  } 	
 }	

 return 0; // Ok
}



//-----------------------------------------------------------------------------
// - En el postprocesado de objetos se invierten las coordenadas Y y Z de los 
//   v�tices, y se localiza el identificador de material
//-----------------------------------------------------------------------------


int PostProcessObjects(void)
{
 int i,j;
 object_type *curobj;
 float tmp;
 
 if(!materials) return -1;
 
 for(i=0;i<header.nobjects;i++)
 {
  curobj = &(world->obj[i]);
/*  for(j=0;j<curobj->number_of_vertices;j++)	// Intercambiamos coordenadas Y y Z
  {
   tmp=curobj->VertexPointer[3*j+1];
   curobj->VertexPointer[3*j+1] = curobj->VertexPointer[3*j+2];
   curobj->VertexPointer[3*j+2] = tmp;
  }  */    
  curobj->num_material = materials[curobj->num_material].matid;  // Le agisnamos su verdadero material
  if(curobj->TexCoordPointer)
  {
   for(j=0;j<curobj->number_of_uv;j++) curobj->TexCoordPointer[2*j+1] = - curobj->TexCoordPointer[2*j+1];
  }

 } 
 
 // Limpiamos la estructura temporal de materiales
 
 for(i=0;i<header.nmaterials;i++) free(materials[i].name);
 free(materials);		
 materials=NULL;
 return 0; // Ok
}


void InsertSon(int index)
{
 int i;
 int parent_id = world->obj[index].parentid;
 ListaHijos *tmp;
 
 for(i=0;i<world->number_of_objects;i++)
 {
  if(world->obj[i].id == parent_id)	 //hemos encontrado el padre
  {
   if(world->obj[i].hierarchy == NULL) 
   {
    world->obj[i].hierarchy = (ListaHijos *)malloc(sizeof(ListaHijos));
    world->obj[i].hierarchy->hijo = &(world->obj[index]);
    world->obj[i].hierarchy->next = NULL;
   }
   else
   {
    tmp = world->obj[i].hierarchy;
    while(tmp->next != NULL) tmp=tmp->next;
    tmp->next = (ListaHijos *)malloc(sizeof(ListaHijos));
    tmp->next->hijo = &(world->obj[index]);
    tmp->next->next = NULL;
   }
   return; // No hay que seguir	
  }
 }  	
}

void BuildHierarchy(void)
{
 int i;
 
 for(i=0;i<world->number_of_objects;i++)
 {
  if(world->obj[i].parentid != -1) InsertSon(i);  	
 }	
}

//---------------------------------------------------------------------------
// Procesado de los bones tras la carga
// 
// 1. Asignar los bone ids seg� orden dentro del objeto
// 2. Crear el arbol de jerarqu쟠de bones
//---------------------------------------------------------------------------

// auxiliary function to know whether we already have this bone in the list

int BoneIncluded(int bone_number, int *BoneIds, int count)
{
 int i;

 for(i=0;i<count;i++)
 {
  if(bone_number == BoneIds[i]) return 1;
 }

 return 0;
}

// another stupid auxiliary function to run through an array

int FindBoneAssoc(int *array,int original_boneid,int numbones)
{
 int i;

 for(i=0;i<numbones;i++)
 {
  if(array[i] == original_boneid) return i;
 }
 return 0; // should never happen
}


// Bone tree making functions

void InsertBoneSon(int index)
{
 int i;
 int parent_id = world->bones[index].parentid;
 BoneList *tmp;
 
 for(i=0;i<world->number_of_bones;i++)
 {
  if(world->bones[i].id == parent_id)	 //hemos encontrado el padre
  {
   if(world->bones[i].hierarchy == NULL) 
   {
    world->bones[i].hierarchy = (BoneList *)malloc(sizeof(BoneList));
    world->bones[i].hierarchy->hijo = &(world->bones[index]);
    world->bones[i].hierarchy->next = NULL;
   }
   else
   {
    tmp = world->bones[i].hierarchy;
    while(tmp->next != NULL) tmp=tmp->next;
    tmp->next = (BoneList *)malloc(sizeof(BoneList));
    tmp->next->hijo = &(world->bones[index]);
    tmp->next->next = NULL;
   }
   return; // No hay que seguir	
  }
 }  	
}

void BuildBoneHierarchy(void)
{
 int i;
 
 for(i=0;i<world->number_of_bones;i++)
 {
  if(world->bones[i].parentid != 0) InsertBoneSon(i);  	
 }	
}



void mDebugMatrices(Bone *firstbone)
{
  sprintf (FANTASY_DEBUG_STRING, "Initial Bone %s. Position %f %f %f, Rotation %f %f %f %f\n",firstbone->nombre,firstbone->x,firstbone->y,firstbone->z,
  firstbone->current_rotation.x,firstbone->current_rotation.y,firstbone->current_rotation.z,firstbone->current_rotation.w);
  Debug(DEBUG_LEVEL_ERROR);
  sprintf (FANTASY_DEBUG_STRING, "m_transform:\n");
  Debug(DEBUG_LEVEL_ERROR);
  sprintf (FANTASY_DEBUG_STRING, "%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
  firstbone->m_transform[0][0],firstbone->m_transform[0][1],firstbone->m_transform[0][2],firstbone->m_transform[0][3],
  firstbone->m_transform[1][0],firstbone->m_transform[1][1],firstbone->m_transform[1][2],firstbone->m_transform[1][3],
  firstbone->m_transform[2][0],firstbone->m_transform[2][1],firstbone->m_transform[2][2],firstbone->m_transform[2][3],
  firstbone->m_transform[3][0],firstbone->m_transform[3][1],firstbone->m_transform[3][2],firstbone->m_transform[3][3]
  );
  Debug(DEBUG_LEVEL_ERROR);
  
  sprintf (FANTASY_DEBUG_STRING, "m_ref_inv:\n");
  Debug(DEBUG_LEVEL_ERROR);
  sprintf (FANTASY_DEBUG_STRING, "%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
  firstbone->m_ref_inv[0][0],firstbone->m_ref_inv[0][1],firstbone->m_ref_inv[0][2],firstbone->m_ref_inv[0][3],
  firstbone->m_ref_inv[1][0],firstbone->m_ref_inv[1][1],firstbone->m_ref_inv[1][2],firstbone->m_ref_inv[1][3],
  firstbone->m_ref_inv[2][0],firstbone->m_ref_inv[2][1],firstbone->m_ref_inv[2][2],firstbone->m_ref_inv[2][3],
  firstbone->m_ref_inv[3][0],firstbone->m_ref_inv[3][1],firstbone->m_ref_inv[3][2],firstbone->m_ref_inv[3][3]
  );
  Debug(DEBUG_LEVEL_ERROR);
    
  
  
  
  sprintf (FANTASY_DEBUG_STRING, "final_matrix:\n");
  Debug(DEBUG_LEVEL_ERROR);
  sprintf (FANTASY_DEBUG_STRING, "%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
  firstbone->final_matrix[0][0],firstbone->final_matrix[0][1],firstbone->final_matrix[0][2],firstbone->final_matrix[0][3],
  firstbone->final_matrix[1][0],firstbone->final_matrix[1][1],firstbone->final_matrix[1][2],firstbone->final_matrix[1][3],
  firstbone->final_matrix[2][0],firstbone->final_matrix[2][1],firstbone->final_matrix[2][2],firstbone->final_matrix[2][3],
  firstbone->final_matrix[3][0],firstbone->final_matrix[3][1],firstbone->final_matrix[3][2],firstbone->final_matrix[3][3]
  );
  Debug(DEBUG_LEVEL_ERROR);
	
}
void MultiplyBoneMatrices(Bone *bone, float parent_matrix[4][4])
{
 Bone *tmp;
 BoneList *list;
 float matriz[4][4];
 float matriz1[4][4];
 float matriz2[4][4];
 
 list=bone->hierarchy;
 	
 while(list != NULL)
 {
  tmp=list->hijo;
  
  // Calculamos las tres matrices
  
  matriz_unidad(matriz1);
  scale_matrix(tmp->xscale,tmp->yscale,tmp->zscale,matriz1);
/*
if(tmp->numero_keys_path_rotacion > 0)
{ 
 	tmp->current_rotation.x = tmp->path_rotacion[0].quat.x;
 	tmp->current_rotation.y = tmp->path_rotacion[0].quat.y;
 	tmp->current_rotation.z = tmp->path_rotacion[0].quat.z;
 	tmp->current_rotation.w = tmp->path_rotacion[0].quat.w;	

}*/

  Quat_to_RotMatrix(&(tmp->current_rotation),matriz2);  // Get the rotation matrix
  multiplicar_matrices(matriz,matriz1,matriz2);  
  translate_matrix(tmp->x,tmp->y,tmp->z,matriz);  
  multiplicar_matrices(tmp->m_transform,matriz,parent_matrix);    
  copiar_matrices(tmp->m_ref_inv,tmp->m_transform);
  Invert_Matrix(tmp->m_ref_inv);
  multiplicar_matrices(tmp->final_matrix,tmp->m_ref_inv,tmp->m_transform);  
  
  
  //mDebugMatrices(tmp);

  MultiplyBoneMatrices(tmp,tmp->m_transform);  // Seguimos recursivamente con sus hijos  
  list=list->next; // Pasamos al siguiente de la lista
 }
}



void CalcInitialBoneMatrices(void)
{
 int i;
 Bone *firstbone;
 BoneList *bonelist;
 float matriz1[4][4];
 float matriz2[4][4];
  
 for(i=0;i<world->number_of_objects;i++)
 {
 	if(world->obj[i].bonetree) // This object has a skin
 	{
 	  bonelist=world->obj[i].bonetree;
	  while(bonelist != NULL)
	  {
		firstbone=bonelist->hijo;
		  	
		// Aqu럣alculamos las tres matrices b಩cas

  		matriz_unidad(matriz1);
 		scale_matrix(firstbone->xscale,firstbone->yscale,firstbone->zscale,matriz1);
/*
if(firstbone->numero_keys_path_rotacion > 0)
{
 	firstbone->current_rotation.x = firstbone->path_rotacion[0].quat.x;
 	firstbone->current_rotation.y = firstbone->path_rotacion[0].quat.y;
 	firstbone->current_rotation.z = firstbone->path_rotacion[0].quat.z;
 	firstbone->current_rotation.w = firstbone->path_rotacion[0].quat.w;	
}
*/

		Quat_to_RotMatrix(&(firstbone->current_rotation),matriz2);  // Get the rotation matrix
		multiplicar_matrices(firstbone->m_transform,matriz1,matriz2); 
 
 		translate_matrix(firstbone->x,firstbone->y,firstbone->z,firstbone->m_transform); 		
 		copiar_matrices(firstbone->m_ref_inv,firstbone->m_transform);
		Invert_Matrix(firstbone->m_ref_inv);		
  		multiplicar_matrices(firstbone->final_matrix,firstbone->m_ref_inv,firstbone->m_transform);
 	//mDebugMatrices(firstbone);
 		MultiplyBoneMatrices(firstbone,firstbone->m_transform); 		 		
 		bonelist=bonelist->next;	
 	  }
 	} 	
 }		
}

Bone *FindBone(int boneid)
{
 int i;
 
 for(i=0;i<world->number_of_bones;i++)
 {
  if(world->bones[i].id == boneid) return &(world->bones[i]);
 }	
 
 return NULL;
}

void ProcessBones(void)
{
 int i,j;
 object_type *curobj;
 int numbones;
 int kkkk;
 int pares[28]; //y no admitimos mౠde 28 bones de momento
 Bone *tmpbone;
 // 

 for(i=0;i<world->number_of_objects;i++)
 {
   curobj=&(world->obj[i]);
   if(curobj->BoneIds == NULL) continue; // This object doesn't have a skeleton

   numbones = 0;
   for(j=0;j<4*curobj->number_of_vertices;j++)
   {
    if((curobj->BoneIds[j]!=0) && (!BoneIncluded(curobj->BoneIds[j],curobj->BoneIds,j)))
    {
	numbones++;
	pares[numbones-1] = curobj->BoneIds[j]; // Ese ser�u nuevo id dentro del objeto
	if(numbones >= 27) break;
    }
   }

   kkkk=numbones;

   for(j=0;j<kkkk;j++)
   {
    tmpbone=FindBone(pares[j]);
    while( (tmpbone->parentid != 0) && !BoneIncluded(tmpbone->parentid,pares,numbones) )
    {
     numbones ++;
     pares[numbones-1] = tmpbone->parentid;
     tmpbone = FindBone(tmpbone->parentid);	
    }   
   }

   /*for(j=0;j<numbones;j++)
   {
    sprintf(FANTASY_DEBUG_STRING,"Bone assoc: %d -> %d\n",j,pares[j]);
    Debug(DEBUG_LEVEL_ERROR);	
   }*/

   // Ya tenemos el n� de bones del objeto y las nuevas asociaciones. Ahora seguimos

   curobj->numbones = numbones;
   curobj->BoneArray = (Bone **) malloc(numbones * sizeof(Bone *));
   for(j=0;j<numbones;j++)
   {
    curobj->BoneArray[j] = FindBone(pares[j]);
    
   }

   for(j=0;j<4*curobj->number_of_vertices;j++)
   {
   	// El "3 *" viene por la forma de hacerlo en el vertex program. Las matrices de cada bone
   	// se ponen en tres vectores consecutivos, con lo que el 쭤ice de cada bone es el 쭤ice
   	// del primer vector. Por ejemplo
   	//
   	// program.local[0]: 1er vector de transformaci򬟤el bone 0   	
   	// program.local[1]: 2o 
   	// program.local[2]: 3o
   	// program.local[3]: 1er vector de transformaci򬟤el bone 1
   	// ...
   	
	if(curobj->BoneIds[j]) curobj->BoneIds[j] = 3 * FindBoneAssoc(pares,curobj->BoneIds[j],numbones);
   }

	// Ya tenemos los bone ids correctos. Ahora nos falta construir la jerarqu쟠de bones

   for(j=0;j<curobj->numbones;j++)
   {
	if(curobj->BoneArray[j]->parentid == 0) // This is the source of this skeleton
	{		
	 if(curobj->bonetree == NULL)
	 {
	  curobj->bonetree = (BoneList*)malloc(sizeof(BoneList));
	  curobj->bonetree->hijo=curobj->BoneArray[j];
	  curobj->bonetree->next=NULL;
	 }
	 else
	 {
	  BoneList *tmplist;
	  tmplist = curobj->bonetree;
	  while(tmplist->next != NULL) tmplist = tmplist->next;
	  tmplist->next = (BoneList*)malloc(sizeof(BoneList));
	  tmplist->next->hijo=curobj->BoneArray[j];
	  tmplist->next->next=NULL;	 	
	 }		
	}
   }   
 }
   // y ahora cada bone con su padre

   BuildBoneHierarchy();
   CalcInitialBoneMatrices();
}

//-----------------------------------------------------------------------------
// Inicio de todo...
//-----------------------------------------------------------------------------

int Load3DFile(world_type *mundo,char *nombre)
{
    PFILE *f;
    world=mundo;

    sprintf(FANTASY_DEBUG_STRING,"Opening file %s!\n", nombre);
    Debug(DEBUG_LEVEL_INFO);


    f = pak_open(nombre, "rb");
    if (f == NULL)
    {

        sprintf(FANTASY_DEBUG_STRING,"Can't open %s!\n", nombre);
	Debug(DEBUG_LEVEL_ERROR);
	return -1;
    }
    if(LoadHeader(f)) { pak_close(f); return -1;}
    if(LoadMeshes(f)) { pak_close(f); return -1;}
    if(LoadCameras(f)) { pak_close(f); return -1;}
    if(LoadCameraTargets(f)) { pak_close(f); return -1;}
    if(LoadLights(f)) { pak_close(f); return -1;}    
    if(LoadMats(f)) { pak_close(f); return -1;}
    if(LoadBones(f)) { pak_close(f); return -1;}
    if(LoadControllers(f)) { pak_close(f); return -1;}
    if(PostProcessObjects()) { pak_close(f); return -1;}
    pak_close(f);
    BuildHierarchy();    
    ProcessBones();
    return(0);
}




void InsertNode(VertexRemapper **node, int texid, int polygonindex)
{
 VertexRemapper *tmp;
	
 if((*node) == NULL)
 {
  *node=(VertexRemapper *)malloc(sizeof(VertexRemapper));
  (*node)->next = NULL;
  (*node)->texid = texid;
  (*node)->polygonindex = polygonindex;
  return;
 }
 
 tmp = *node;
 while(1)
 {
  if((tmp->texid) == texid) return;
  if(tmp->next == NULL)
  {
   tmp->next = (VertexRemapper *)malloc(sizeof(VertexRemapper));
   tmp->next->texid = texid;
   tmp->next->polygonindex = polygonindex;
   tmp->next->next = NULL;
   return;
  } 
  tmp = tmp->next;    
 }
}

void FreeNode(VertexRemapper **node)
{
 VertexRemapper *tmp,*tmp2;	
 
 tmp = *node;
 
 while(tmp !=NULL)
 {
  tmp2 = tmp;
  tmp = (tmp->next);
  free(tmp2);
 }	
}


void AdjustVertices(object_type *obj)
{ 
 VertexRemapper *tmp;
 int i,j;
 int new_number_of_vertices;
 
 obj->node = (VertexRemapper **) malloc(sizeof(VertexRemapper*) * obj->number_of_vertices);
 for(i=0; i < obj->number_of_vertices; i++) obj->node[i]=NULL;
 
 for(i=0; i < obj->number_of_vertices; i++)
 {
  // Para cada v�tice recorremos la lista de �dices en busca de caras a las que pertenezca
  // Una vez localizado, vemos a qu�coordenada uv se corresponde
  // Si no est�ya en la lista, lo aadimos
  for(j=0;j < 3*obj->number_of_polygons;j++)
  {
   if(obj->IndexPointer[j] == i) //Tenemos el v�tice
   {
    InsertNode(&(obj->node[i]),obj->UVIndexPointer[j],j);
   }
  } 	
 }
 
 // Ahora que ya tenemos insertados los nodos, primero
 new_number_of_vertices=0;
 
 for(i=0;i<obj->number_of_vertices;i++)
 {
  tmp = obj->node[i];
    
  while(tmp!=NULL)
  {
   tmp->newindex=new_number_of_vertices;		// Aqu�asigno el nuevo �dice
   new_number_of_vertices++;
   tmp=tmp->next;
  } 	
 }
 
 obj->GLnumber_of_vertices = new_number_of_vertices;
 
 // Ya sabemos el nmero efectivo de v�tices, ahora a reservar memoria

 obj->GLTexCoordPointer = (float *)malloc(2 * obj->GLnumber_of_vertices * sizeof(float));
 obj->GLVertexPointer = (float *)malloc(6 * obj->GLnumber_of_vertices * sizeof(float));
 obj->GLNormalPointer = obj->GLVertexPointer + 3 * obj->GLnumber_of_vertices;
 obj->GLIndexPointer = (unsigned *)malloc(3* obj->number_of_polygons * sizeof(unsigned));
 if(obj->ColorPointer) obj->GLColorPointer = (float *)malloc(3 * obj->GLnumber_of_vertices * sizeof(float));
 
 for(i=0,j=0;i<obj->number_of_vertices;i++)
 {
  tmp = obj->node[i];
    
  while(tmp!=NULL)
  {
   obj->GLVertexPointer[3*j] = obj->VertexPointer[3*i];
   obj->GLVertexPointer[3*j+1] = obj->VertexPointer[3*i+1];
   obj->GLVertexPointer[3*j+2] = obj->VertexPointer[3*i+2];
   obj->GLTexCoordPointer[2*j] = obj->TexCoordPointer[2*tmp->texid];
   obj->GLTexCoordPointer[2*j+1] = obj->TexCoordPointer[2*tmp->texid+1];
   
   tmp=tmp->next;
   j++;
  } 	
 }
 
 // If we have vertex colors we do the same...
 if(obj->ColorPointer)
 {
   for(i=0,j=0;i<obj->number_of_vertices;i++)
   {
    tmp = obj->node[i];
    while(tmp!=NULL)
    {
     obj->GLColorPointer[3*j]   = obj->ColorPointer[3*obj->ColorIndexPointer[tmp->polygonindex]];
     obj->GLColorPointer[3*j+1] = obj->ColorPointer[3*obj->ColorIndexPointer[tmp->polygonindex]+1];
     obj->GLColorPointer[3*j+2] = obj->ColorPointer[3*obj->ColorIndexPointer[tmp->polygonindex]+2];
     tmp=tmp->next;
     j++; 
    }
   }
 }
 
 // If we have bones we also have to duplicate the vertices...
 
 if(obj->BoneIds)
 {
  obj->GLBoneIds = (int *)malloc(4 * obj->GLnumber_of_vertices * sizeof(int));
  obj->GLBoneWeights = (float *)malloc(4 * obj->GLnumber_of_vertices * sizeof(float));
  for(i=0,j=0;i<obj->number_of_vertices;i++)
  {
   tmp = obj->node[i];
    
   while(tmp!=NULL)
   {
    obj->GLBoneIds[4*j]   = obj->BoneIds[4*i];
    obj->GLBoneIds[4*j+1] = obj->BoneIds[4*i+1];
    obj->GLBoneIds[4*j+2] = obj->BoneIds[4*i+2];
    obj->GLBoneIds[4*j+3] = obj->BoneIds[4*i+3];
    obj->GLBoneWeights[4*j]   = obj->BoneWeights[4*i];
    obj->GLBoneWeights[4*j+1] = obj->BoneWeights[4*i+1];
    obj->GLBoneWeights[4*j+2] = obj->BoneWeights[4*i+2];
    obj->GLBoneWeights[4*j+3] = obj->BoneWeights[4*i+3];
        
    tmp=tmp->next;
    j++;
   } 	
  }
 }


 
 
 
 for(i=0;i<3*obj->number_of_polygons;i++)
 {
  j=obj->IndexPointer[i];
  tmp = obj->node[j];
  while(tmp!=NULL)
  {
   if(tmp->texid == obj->UVIndexPointer[i]) 
   {
    obj->GLIndexPointer[i] = tmp->newindex;
   }
   tmp=tmp->next;
  } 
 }

 // Las normales de v�tice se remapean al calcularlas
}

