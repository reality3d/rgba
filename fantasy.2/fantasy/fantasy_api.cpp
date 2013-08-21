#include "fantasy_api.h"
#include "mates.h"
#include "quatern.h"
#include "carga.h"
#include "extgl.h"
#include "vbo.h"
#include "interp.h"

#include <string.h>
#include <math.h>

#ifdef LINUX
#define _strdup strdup
#endif // LINUX

//---------------------------------------------------------
// Creacin del objeto API
//
//  current_world: puntero al mundo
//
//---------------------------------------------------------

FantasyAPI::FantasyAPI(world_type *current_world)
{
 world = current_world;	
}

//---------------------------------------------------------
// Destruccin del objeto API
//---------------------------------------------------------

FantasyAPI::~FantasyAPI()
{	
}

//---------------------------------------------------------
// Devuelve la cï¿½ara actual del mundo
//---------------------------------------------------------

EPOPEIA_PLUGIN_API int FantasyAPI::GetActiveCamera(void)
{
 return world->current_camera;	
}

//--------------------------------------------------------------------
// Copia un objeto y lo aade a la escena actual
//
// - original_object: nombre del objeto 
// - dst_object:      nombre del nuevo objeto 
//
// Devuelve un valor entero, correspondiente al identificador
// del nuevo objeto. Si no se puede crear, devuelve -1
//
// NOTES: if the object is part of a hierarchy or has bones, it just
//	  will not be copied, and the function will return -1
//--------------------------------------------------------------------		
		
EPOPEIA_PLUGIN_API int FantasyAPI::CopyObject(char *original_object, char *dst_object)
{
 int i;
 object_type *obj, *dstobj;
	
 obj = NULL;
	
 for(i=0;i<world->number_of_objects;i++)
 {
  if(!strcmp(original_object,world->obj[i].nombre_objeto)) 
    obj = &(world->obj[i]); // Hemos encontrado el objeto
 }	
 if(obj == NULL) return -1; // No hemos encontrado un objeto con ese nombre!!!!
 if(obj->parentid != -1) return -1; // This object is part of a hierarchy
 if(obj->BoneIds) return -1; // This object has a skin!


 // Inicialmente actualizamos la estructura del mundo
 world->number_of_objects++;
 world->obj = (object_type *)realloc(world->obj,world->number_of_objects*sizeof(object_type));
 dstobj = &(world->obj[world->number_of_objects - 1]);
 
 // Tenemos origen y destino, ahora a copiar como descosidos :)
 
 dstobj->nombre_objeto=_strdup(dst_object);  // Nombre 
 dstobj->id = 0; // id is not used anywhere in the engine, just when loading
 dstobj->parentid = -1;
 dstobj->number_of_vertices = obj->number_of_vertices;
 dstobj->number_of_polygons = obj->number_of_polygons;
 dstobj->number_of_uv = obj-> number_of_uv;
 dstobj->x = obj->x;
 dstobj->y = obj->y;
 dstobj->z = obj->z;  
 dstobj->pivotx = obj->pivotx;
 dstobj->pivoty = obj->pivoty;
 dstobj->pivotz = obj->pivotz;  
 dstobj->xscale = obj->xscale;  
 dstobj->yscale = obj->yscale;
 dstobj->zscale = obj->zscale;   
 dstobj->current_rotation.x = obj->current_rotation.x;
 dstobj->current_rotation.y = obj->current_rotation.y;
 dstobj->current_rotation.z = obj->current_rotation.z;
 dstobj->current_rotation.w = obj->current_rotation.w;    

// Reservamos memoria
 dstobj->TexCoordPointer = (float *)malloc(2 * dstobj->number_of_uv * sizeof(float));
 dstobj->VertexPointer = (float *)malloc(3 * dstobj->number_of_vertices * sizeof(float));
 dstobj->NormalPointer = (float *)malloc(3 * dstobj->number_of_vertices * sizeof(float)); 
 dstobj->PolygonNormalPointer = (float *)malloc(3* dstobj->number_of_polygons * sizeof(float));
 dstobj->IndexPointer = (unsigned *)malloc(3* dstobj->number_of_polygons * sizeof(unsigned));
 dstobj->UVIndexPointer = (unsigned *)malloc(3* dstobj->number_of_polygons * sizeof(unsigned));        
 dstobj->EdgeFlagPointer= (unsigned char*)malloc(3* dstobj->number_of_polygons * sizeof(unsigned char));
  
 if(obj->ColorPointer) 
 {
 	dstobj->ColorPointer = (float *)malloc(3 * dstobj->number_of_vertices * sizeof(float)); 
 	dstobj->ColorIndexPointer = (unsigned *)malloc(3* dstobj->number_of_polygons * sizeof(unsigned));        
 }
 else 
 	dstobj->ColorPointer = NULL;
  
 // A los arrays efectivos para OpenGL se les reservarï¿½memoria despuï¿½
 dstobj->GLTexCoordPointer = NULL;     
 dstobj->GLVertexPointer = NULL;     
 dstobj->GLNormalPointer = NULL;     
 dstobj->GLIndexPointer = NULL;   
 dstobj->GLColorPointer = NULL;
    
// Copiamos los arrays

 memcpy(dstobj->TexCoordPointer,obj->TexCoordPointer,2 * dstobj->number_of_uv * sizeof(float));
 memcpy(dstobj->VertexPointer,obj->VertexPointer, 3 * dstobj->number_of_vertices * sizeof(float));
 memcpy(dstobj->NormalPointer,obj->NormalPointer, 3 * dstobj->number_of_vertices * sizeof(float));
 memcpy(dstobj->PolygonNormalPointer,obj->PolygonNormalPointer, 3 * dstobj->number_of_vertices * sizeof(float)); 
 memcpy(dstobj->IndexPointer,obj->IndexPointer, 3* dstobj->number_of_polygons * sizeof(unsigned));
 memcpy(dstobj->UVIndexPointer, obj->UVIndexPointer,3* dstobj->number_of_polygons * sizeof(unsigned));
 memcpy(dstobj->EdgeFlagPointer, obj->EdgeFlagPointer, 3* dstobj->number_of_polygons * sizeof(unsigned char));
 if(dstobj->ColorPointer)
 {
 	memcpy(dstobj->ColorPointer,obj->ColorPointer, 3 * dstobj->number_of_vertices * sizeof(float));
 	memcpy(dstobj->ColorIndexPointer, obj->ColorIndexPointer,3* dstobj->number_of_polygons * sizeof(unsigned));
 }

// Seguimos

 dstobj->num_material = obj->num_material;
 dstobj->bspherecenter[0] = obj->bspherecenter[0];
 dstobj->bspherecenter[1] = obj->bspherecenter[1];
 dstobj->bspherecenter[2] = obj->bspherecenter[2];
 dstobj->bsphereradius = obj->bsphereradius;
 
 dstobj->numero_keys_path = obj->numero_keys_path;
 dstobj->numero_keys_path_rotacion = obj->numero_keys_path_rotacion;
 dstobj->numero_keys_path_scale = obj->numero_keys_path_scale;
   
 dstobj->posiciones_keys=(info_pos_path *)malloc(dstobj->numero_keys_path*sizeof(info_pos_path));   
 dstobj->path_rotacion=(info_rot_path *)malloc(dstobj->numero_keys_path_rotacion*sizeof(info_rot_path));   
 dstobj->scale_keys=(info_scale_path *)malloc(dstobj->numero_keys_path_scale*sizeof(info_scale_path));
 
 memcpy(dstobj->posiciones_keys,obj->posiciones_keys,dstobj->numero_keys_path*sizeof(info_pos_path));
 memcpy(dstobj->path_rotacion,obj->path_rotacion,dstobj->numero_keys_path_rotacion*sizeof(info_rot_path));
 memcpy(dstobj->scale_keys,obj->scale_keys,dstobj->numero_keys_path_scale*sizeof(info_scale_path));


 dstobj->USpeed = obj->USpeed;
 dstobj->VSpeed = obj->VSpeed; 
 dstobj->UTranslate = obj->UTranslate;
 dstobj->VTranslate = obj->VTranslate;

 if(dstobj->numero_keys_path)
 {
  dstobj->tangentes_entrada = (Vector *)malloc(dstobj->numero_keys_path*sizeof(Vector));
  dstobj->tangentes_salida = (Vector *)malloc(dstobj->numero_keys_path*sizeof(Vector));
  memcpy(dstobj->tangentes_entrada, obj->tangentes_entrada,dstobj->numero_keys_path*sizeof(Vector));
  memcpy(dstobj->tangentes_salida, obj->tangentes_salida,dstobj->numero_keys_path*sizeof(Vector));
 }
 else
 {
  dstobj->tangentes_entrada = NULL;
  dstobj->tangentes_salida = NULL;
 }
 
 if(dstobj->numero_keys_path_scale)
 {
  dstobj->tangentes_entrada_scale = (Vector *)malloc(dstobj->numero_keys_path_scale*sizeof(Vector));
  dstobj->tangentes_salida_scale = (Vector *)malloc(dstobj->numero_keys_path_scale*sizeof(Vector)); 
  memcpy(dstobj->tangentes_entrada_scale, obj->tangentes_entrada_scale,dstobj->numero_keys_path_scale*sizeof(Vector));
  memcpy(dstobj->tangentes_salida_scale, obj->tangentes_salida_scale,dstobj->numero_keys_path_scale*sizeof(Vector));  	
 }
 else
 {
  dstobj->tangentes_entrada_scale = NULL;
  dstobj->tangentes_salida_scale = NULL;
 }

 if(dstobj->numero_keys_path_rotacion)
 {
  dstobj->tangentes_entrada_rotacion = (Quaternion *)malloc(dstobj->numero_keys_path_rotacion*sizeof(Quaternion));
  dstobj->tangentes_salida_rotacion = (Quaternion *)malloc(dstobj->numero_keys_path_rotacion*sizeof(Quaternion));
  memcpy(dstobj->tangentes_entrada_rotacion, obj->tangentes_entrada_rotacion,dstobj->numero_keys_path_rotacion*sizeof(Quaternion));
  memcpy(dstobj->tangentes_salida_rotacion, obj->tangentes_salida_rotacion,dstobj->numero_keys_path_rotacion*sizeof(Quaternion));  	
 }
 else
 {
  dstobj->tangentes_entrada_rotacion = NULL;
  dstobj->tangentes_salida_rotacion = NULL;
 }

 
// Finalizamos
  
 dstobj->normalize = obj->normalize;
 dstobj->updatevertices = obj->updatevertices;
 dstobj->visible = obj->visible;
 dstobj->animate = obj->animate;
 dstobj->hint_dynamic = obj->hint_dynamic;
 dstobj->cast_shadows = obj->cast_shadows;
 dstobj->vtx_shader_id = obj->vtx_shader_id;
 dstobj->render_mode = obj->render_mode;
 dstobj->object_alpha = obj->object_alpha;
 dstobj->fx.id = obj->fx.id;
 
 for(i=0;i<8;i++)
 {
  dstobj->fx.paramf[i] = obj->fx.paramf[i];
  dstobj->fx.paramu[0] = obj->fx.paramu[0];
 }
 dstobj->hierarchy=NULL;
 dstobj->vbufinfo.usingvertexbuf=0;
 dstobj->vbufinfo.vertexbufferid=0;
 dstobj->vbufinfo.indexbufferid=0;
 dstobj->vbufinfo.texbufferid=0;	
 dstobj->vbufinfo.normalskip=0;
 dstobj->BoneIds = NULL;
 dstobj->BoneWeights = NULL;
 dstobj->GLBoneWeights = NULL;
 dstobj->GLBoneIds = NULL;
 dstobj->bonetree = NULL;
 dstobj->BoneArray = NULL;
 dstobj->numbones=0;

// Ahora es el  momento de ajustar vértices y subir al buffer object, si aplica.
 AdjustVertices(dstobj);
 calcula_normales_poligono(dstobj); // Esto es necesario?
 calcula_normales_vertice(dstobj);
 if(EXTGL_vertex_buffer_objects)
 {
 	UploadBuffers(dstobj);	           
 }
   
// Devolvemos el identificador de objeto
  return world->number_of_objects - 1;	
}
		

//--------------------------------------------------------------------
// Copia el quaternio de rotacion de un objeto a otro
//
// - original_object: identificador del objeto origen
// - dst_object:      identificador del objeto destino
//--------------------------------------------------------------------				
EPOPEIA_PLUGIN_API void FantasyAPI::CopyQuaternion(int original_object, int dst_object)
{
 object_type *obj, *dstobj;
 
 obj = &(world->obj[original_object]);
 dstobj = &(world->obj[dst_object]);

 dstobj->current_rotation.x = obj->current_rotation.x;
 dstobj->current_rotation.y = obj->current_rotation.y;
 dstobj->current_rotation.z = obj->current_rotation.z;
 dstobj->current_rotation.w = obj->current_rotation.w;    	
}

//--------------------------------------------------------------------
// Sita un objeto en el mundo
//
// - objindex: 	identificador del objeto 
// - x,y,z:	posicion
//--------------------------------------------------------------------				
EPOPEIA_PLUGIN_API void FantasyAPI::SetPosition(int objindex,float x,float y,float z)
{
 object_type * obj;
 
 obj = &(world->obj[objindex]);
 obj->x = x;
 obj->y = y;
 obj->z = z;
}

//--------------------------------------------------------------------
// Cambia la escala de un objeto
//
// - objindex: 	identificador del objeto 
// - scalex,scaley,scalez: nueva escala
//--------------------------------------------------------------------				

EPOPEIA_PLUGIN_API void FantasyAPI::SetScale(int objindex,float scalex,float scaley,float scalez)
{
 object_type * obj;
 
 obj = &(world->obj[objindex]);
 obj->xscale = scalex;
 obj->yscale = scaley;
 obj->zscale = scalez;		
}

//--------------------------------------------------------------------
// Establece un nuevo quaternio de rotacin para un objeto
//
// - objindex: 	identificador del objeto 
// - rotacion: 	puntero a un quaternio con la nueva rotacin
//--------------------------------------------------------------------				

EPOPEIA_PLUGIN_API void FantasyAPI::SetRotation(int objindex, Quaternion *rotation)
{
 object_type *obj;
 
 obj = &(world->obj[objindex]);

 obj->current_rotation.x = rotation->x;
 obj->current_rotation.y = rotation->y;
 obj->current_rotation.z = rotation->z;
 obj->current_rotation.w = rotation->w;    		
}

//--------------------------------------------------------------------
// Recalcula las normales de un objeto
//
// - objindex: 	identificador del objeto 
//--------------------------------------------------------------------				

EPOPEIA_PLUGIN_API void FantasyAPI::Normalize(int objindex)
{
 object_type *obj;
 
 obj = &(world->obj[objindex]);  
 obj->normalize=1;
}

//--------------------------------------------------------------------
// Cambia el material de un objeto
//
// - objindex: 	identificador del objeto 
// - matname:	nombre del material
//--------------------------------------------------------------------				


EPOPEIA_PLUGIN_API void FantasyAPI::SetMaterial(int objindex, char *matname)
{
 object_type * obj;

 obj = &(world->obj[objindex]); 
 obj->num_material = world->mats.FindMaterial(matname);	
}

//--------------------------------------------------------------------
// Devuelve el identificador de un objeto, a partir de su nombre
//
// - name: nombre del objeto (max 29 chars)
//
// Si no localiza el objeto, devuelve -1 como identificador
//--------------------------------------------------------------------				

EPOPEIA_PLUGIN_API int FantasyAPI::FindObject(char *name)
{
 int i;
 
 for(i=0;i<world->number_of_objects;i++)
 {
  if(!strcmp(name,world->obj[i].nombre_objeto)) 
    return i; // Objeto encontrado
 }	

 return -1; // Ese objeto no existe (?)
}

//--------------------------------------------------------------------
// Return the position of an object, according to its keyframing info
//
// - objindex: Object number
// - frame: frame where the keyframing info is to be retrieved
// - x,y,z: coordinates (passed as a references)
//--------------------------------------------------------------------				

EPOPEIA_PLUGIN_API void FantasyAPI::GetPosition(int objindex,float frame,float *x,float *y,float *z)
{
 object_type *obj;

 obj = &(world->obj[objindex]);
 if(obj->numero_keys_path>1) // si tiene alg£n movimiento 
 { 
       InterpolaPosicion(obj->numero_keys_path, 
      			 obj->tangentes_entrada,
      			 obj->tangentes_salida,
      			 obj->posiciones_keys,
      			 frame,
      			 x,y,z);      		      
 }
 else
 {
  *x = obj->x;
  *y = obj->y;
  *z = obj->z;	
 }	
}

//--------------------------------------------------------------------
// Return the rotation of an object, according to its keyframing info
//
// - objindex: Object number
// - frame: frame where the keyframing info is to be retrieved
// - q: quaternion where the rotation will be stored 
//--------------------------------------------------------------------				

EPOPEIA_PLUGIN_API void FantasyAPI::GetRotation(int objindex,float frame,Quaternion *q)
{
 object_type *obj;

 obj = &(world->obj[objindex]);
 if(obj->numero_keys_path_rotacion>1) // si tiene alg£n movimiento 
 { 
 	InterpolaRotacion(obj->numero_keys_path_rotacion, 
      			  obj->tangentes_entrada_rotacion,
      			  obj->tangentes_salida_rotacion,
      			  obj->path_rotacion,
      			  frame,
      			  q);
 }
 else
 {
  q->x = obj->current_rotation.x;
  q->y = obj->current_rotation.y;
  q->z = obj->current_rotation.z;	
  q->w = obj->current_rotation.w;	
 }	
}

//--------------------------------------------------------------------
// Return the scale of an object, according to its keyframing info
//
// - objindex: Object number
// - frame: frame where the keyframing info is to be retrieved
// - scalex,scaley,scalez: scale (passed as a reference)
//--------------------------------------------------------------------				

EPOPEIA_PLUGIN_API void FantasyAPI::GetScale(int objindex,float frame,float *scalex,float *scaley,float *scalez)
{
 object_type *obj;

 obj = &(world->obj[objindex]);
 if(obj->numero_keys_path_scale > 1)
 {
  InterpolaScale(obj->numero_keys_path_scale, 
    		 obj->tangentes_entrada_scale,
      		 obj->tangentes_salida_scale,
      		 obj->scale_keys,
      		 frame,
      		 scalex,scaley,scalez);            
 }
 else
 {
  *scalex = obj->xscale;
  *scaley = obj->yscale;
  *scalez = obj->zscale;	
 }	
}


//--------------------------------------------------------------------
// Set the animate flag on an object
//
// - objindex: Object number
// - flag: 0 -> object will be animated manually; 
//         1 -> object animated by fantasy
//--------------------------------------------------------------------				

EPOPEIA_PLUGIN_API void FantasyAPI::SetAnimateFlag(int objindex, int flag)
{
 object_type *obj;

 obj = &(world->obj[objindex]);
 obj->animate = flag;
}

//--------------------------------------------------------------------
// Update the vertex buffer objects associated with the object
//
// - obj: Object pointer
// - flags: Bit mask
//	   Bit 0 -> update vertices
//         Bit 1 -> update normals
//--------------------------------------------------------------------				

EPOPEIA_PLUGIN_API void FantasyAPI::UpdateObject(object_type *obj,unsigned char flags)
{
	if(flags & 1)	obj->updatevertices=1;
	if(flags & 2)   obj->normalize=1;
}



//--------------------------------------------------------------------
// Las dos siguientes funciones sirven exclusivamente para exponer
// funcionalidad de quaternions
//--------------------------------------------------------------------				

EPOPEIA_PLUGIN_API void FantasyAPI::API_Axis_to_Quat(float x, float y, float z, float angle, Quaternion *result)
{
 	Axis_to_Quat(x,y,z,angle,result);
}

EPOPEIA_PLUGIN_API void FantasyAPI::API_Quat_to_RotMatrix(Quaternion *q1, float matrix[4][4])
{
	Quat_to_RotMatrix(q1,matrix);	
}

EPOPEIA_PLUGIN_API void FantasyAPI::API_Quat_to_Axis(Quaternion *q, float *x, float *y, float *z, float *angle)
{
	float temp;
	
	// Perform a simple inverse calculation of the quaternion
	
 	*angle = acos(q->w);
 	temp = sqrt(1 - q->w * q->w); // sin^2 + cos^2 = 1
 	if(fabs(temp) < 0.001f) temp=1.0f;
 	*x = q->x / temp;
 	*y = q->y / temp;
 	*z = q->z / temp;
}