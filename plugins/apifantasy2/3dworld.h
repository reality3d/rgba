#ifndef WORLD_H
#define WORLD_H

#include "quatern.h"
#include "material.h"

// Tipos de render para cada objeto (por defecto es RENDER_NORMAL)

#define RENDER_NORMAL 		0
#define RENDER_WIRE   		1
#define RENDER_FLAT   		2
#define RENDER_NORMALWIRE	3
#define RENDER_CARTOON		4


struct point
{
 float x;
 float y;
 float z;
};
typedef struct point Point;
typedef struct point Vector;

struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};
typedef struct Vector4 Vector4;

struct plane
{
 float a,b,c,d;
};
typedef struct plane Plane;

struct VertexRemapper
{
	int texid;
	int newindex;
	struct VertexRemapper *next;
};
typedef struct VertexRemapper VertexRemapper;



struct object_type;

struct ListaHijos
{
 struct object_type *hijo;
 struct ListaHijos *next;	
};
typedef struct ListaHijos ListaHijos;


struct vertex_buffer
{
	unsigned char usingvertexbuf;	
	unsigned int vertexbufferid;		// 0 por defecto; incluye vertices y normales
	unsigned int indexbufferid;		// 0 por defecto
	unsigned int texbufferid;		// 0 por defecto
	int normalskip;		// 0 por defecto
};

struct spline_flags_type
{
    float tension;
    float continuity;
    float bias;
};
typedef struct spline_flags_type spline_flags_type;


struct info_pos_path 
{
 int num_frame;            // nmero de frame en que estamos
 float wx,wy,wz;           // posicin del objeto
 spline_flags_type spflags;
};
typedef struct info_pos_path info_pos_path;

struct info_scale_path 
{
 int num_frame;            // nmero de frame en que estamos
 float scalex,scaley,scalez;           // escalado en cada eje
 spline_flags_type spflags;
};
typedef struct info_scale_path info_scale_path;


struct info_rot_path
{
    int num_frame;            // nmero de frame en que estamos
    Quaternion quat;          // Quaternio que define la rotacin
    spline_flags_type spflags;
};
typedef struct info_rot_path info_rot_path;

struct info_roll_path
{
    int num_frame;		// nmero de frame en que estamos
    float roll;
    spline_flags_type spflags;
};
typedef struct info_roll_path info_roll_path;

struct info_fov_path
{
    int num_frame;		// nmero de frame en que estamos
    float fov;
    spline_flags_type spflags;
};
typedef struct info_fov_path info_fov_path;

struct fuente {
   char *nombre_fuente; // nombre de la fuente
   int id;		// Identificador de la fuente
   float wx,wy,wz;         // Posicion de la fuente
   float r,g,b;      // Intensidades en las tres bandas
   int numero_keys_path;
   info_pos_path *posiciones_keys;
   Vector *tangentes_salida;
   Vector *tangentes_entrada;
   float inicio_rango;
   float fin_rango;
   float multiplicador;
   int atenuacion; // 1: se utiliza atenuacion; 0: no se utiliza
   int animate;	 // la animamos?
};

typedef struct fuente fuente;

struct camara {
  char *nombre_camara;  // nombre de la camara
  char *nombre_camara_target;  // nombre del target de la camara
  int id;		// Identificador de la camara
  int target_id;	// Identificador de objeto del target
  float wx,wy,wz;    // posicin de la cmara
  float wxt,wyt,wzt; // posicin del objetivo
  float inclinacion; // inclinacion de la camara (coincide con angz)
  float fov;         // campo de visin de la cmara 
  float near_range;
  float far_range;   // rango cercano y lejano de visin (usado para la niebla)
  int animate;	 // animamos la camara?
  int animate_target;	 // animamos el target?

  int numero_keys_path;  
  info_pos_path *posiciones_keys; //info de cada posicin clave del path (key)
  Vector *tangentes_salida;	  // Necesario para el cï¿½culo de las trayectorias
  Vector *tangentes_entrada;

  int numero_keys_rollpath;
  info_roll_path *posiciones_rollkeys; //info de cada posicin clave del path (key)
  float *tangentes_entrada_roll;
  float *tangentes_salida_roll;
  
  int numero_keys_path_target;  
  info_pos_path *posiciones_keys_target; //info de cada posicin clave del path (key)
  Vector *tangentes_salida_target;
  Vector *tangentes_entrada_target;

  int numero_keys_fovpath; 
  info_fov_path *posiciones_fovkeys; //info de cada posicin clave del path (key)
  float *tangentes_entrada_fov;
  float *tangentes_salida_fov;     
} ;


typedef struct camara camara;

struct Bone;
struct BoneList
{
 struct Bone *hijo;
 struct BoneList *next;
};
typedef struct BoneList BoneList;

struct Bone
{
	char *nombre;
	int id;
	int parentid;
   	float  x, y, z;               // Coordenadas del origen local del bone
   	float xscale, yscale, zscale; // bone scale
	Quaternion current_rotation; // Used during keyframing, to know the current rotation
	
	float m_ref_inv[4][4];    // inverse transform of the bone’s reference posture transform 
	float m_transform[4][4];  // affine transform matrix (during runtime, it's the current transform * the parent transform)
	float final_matrix[4][4]; // for each frame, m_ref_inv * m_transform (to be fed to the vertex shader)
	
	// Nos falta su información de animación

	int numero_keys_path;  
	info_pos_path *posiciones_keys; //info de cada posicin clave del path (key)
	Vector *tangentes_salida;	  // Necesario para el cï¿½culo de las trayectorias
	Vector *tangentes_entrada;

	int numero_keys_path_rotacion;
	info_rot_path *path_rotacion; // keys del path de rotacion  
	Quaternion *tangentes_entrada_rotacion;
    	Quaternion *tangentes_salida_rotacion;
   
   	int numero_keys_path_scale;
	info_scale_path *scale_keys;  // keys del path de escalado
	Vector *tangentes_salida_scale;
  	Vector *tangentes_entrada_scale;

	BoneList *hierarchy;
};
typedef struct Bone Bone;



struct object_type {
   char *nombre_objeto;     // nombre del objeto 
   int id;	    // Identificador
   int parentid;    // Identificador del padre
   float  x, y, z;               // Coordenadas mundiales del origen local del objeto
   float pivotx,pivoty,pivotz; // Pivot point
   float xscale, yscale, zscale;
   Quaternion current_rotation; // Used during keyframing, to know the current rotation
   int number_of_vertices;     // Nmero de vrtices
   int number_of_polygons;     // Nmero de polgonos
   int number_of_uv;		// Numero de coordenadas UV
   float *TexCoordPointer;     // Array de coordenadas de textura (2*n_uv)
   float *VertexPointer;       // Array de vrtices (3*n_vertices)
   float *NormalPointer;       // Array de normales de vrtice (3*n_vertices)
   float *PolygonNormalPointer;// Array de normales de polgono (3*n_polygons)
   unsigned *IndexPointer;     // Array de ndices (3*n_polygons)
   unsigned *UVIndexPointer;
   unsigned char *EdgeFlagPointer; // array de flags de visibilidad de cada arista (para wire)

   // Aadido para almacenar los vï¿½tices efectivos para OpenGL
   
   float *GLTexCoordPointer;     
   float *GLVertexPointer;
   float *GLNormalPointer;
   unsigned *GLIndexPointer;
   struct VertexRemapper **node;
   int GLnumber_of_vertices;

   float bspherecenter[3];	//  Bounding sphere                                
   float bsphereradius;     
   int num_material;            // Material del objeto -> Vamos a asumir que un objeto slo va a tener un material
   int numero_keys_path;
   int numero_keys_path_rotacion;
   int numero_keys_path_scale;
   info_pos_path *posiciones_keys;//info de cada posicin clave del path (key)
   info_rot_path *path_rotacion; // keys del path de rotacion
   info_scale_path *scale_keys;  // keys del path de escalado

   struct vertex_buffer vbufinfo;	// Informacin por si usamos vertex buffers
	
   
   float VSpeed;		// Velocidad de cambio de las coordenadas de textura
   float USpeed;   
   float VTranslate;
   float UTranslate;
   
   // Aadidas para el calculo de la interpolacion en "real time"
  Vector *tangentes_salida;
  Vector *tangentes_entrada;
  Vector *tangentes_salida_scale;
  Vector *tangentes_entrada_scale;
  Quaternion *tangentes_entrada_rotacion;
  Quaternion *tangentes_salida_rotacion;
  
   
  // Interaccin con otros plugins
  int normalize; // Hay que recalcular las normales?
  int updatevertices; // Hay que actualizar los vertices?
  int visible;   // Es visible el objeto?  
  int animate;	 // lo animamos?
  unsigned char hint_dynamic; // Va a ser modificado a menudo?
  unsigned char cast_shadows; // should this object cast shadows?
  // Vertex shaders
  unsigned int vtx_shader_id;
  // Tipo de render
  int render_mode;  
  // Alpha value for the object (to make it dissapear)
  float object_alpha;

  ListaHijos *hierarchy;

  // Variables para los bones

  int *BoneIds;   // id del bone asociado (4 * n_vertices, porque como máximo hay 4 bones/vertice)
  float *BoneWeights; // pesos asociados (4 * n_vertices)
  int *GLBoneIds;
  float *GLBoneWeights;

  BoneList *bonetree; // pointer to the source of the bone tree
  Bone **BoneArray; // Pointers to the bones
  int numbones;     // Number of bones associated to this object
};
typedef struct object_type object_type;

struct world_type {
   int number_of_objects;
   int number_of_fuentes;
   int number_of_camaras;
   int number_of_materials;
   int number_of_bones;
   fuente *fuentes;
   camara *camaras;
   object_type * obj;
   Bone *bones;
   Material mats;
   int frame_inicial;
   int frame_final;
   int num_frames;
   int current_camera;
};

#endif
