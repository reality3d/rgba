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


class Raster;

struct point
{
 float x;
 float y;
 float z;
};
typedef struct point Point;
typedef struct point Vector;

struct plane
{
 float a,b,c,d;
};
typedef struct plane Plane;


struct spline_flags_type
{
    float tension;
    float continuity;
    float bias;
};
typedef struct spline_flags_type spline_flags_type;


struct info_pos_path 
{
 int num_frame;            // n£mero de frame en que estamos
 float wx,wy,wz;           // posici¢n del objeto
 float wxt,wyt,wzt;        // posici¢n del objetivo
 spline_flags_type spflags;
};
typedef struct info_pos_path info_pos_path;

struct info_scale_path 
{
 int num_frame;            // n£mero de frame en que estamos
 float scalex,scaley,scalez;           // escalado en cada eje
 spline_flags_type spflags;
};
typedef struct info_scale_path info_scale_path;


struct info_rot_path
{
    int num_frame;            // n£mero de frame en que estamos
    Quaternion quat;          // Quaternio que define la rotaci¢n
    spline_flags_type spflags;
};
typedef struct info_rot_path info_rot_path;

struct info_roll_path
{
    int num_frame;		// n£mero de frame en que estamos
    float roll;
    spline_flags_type spflags;
};
typedef struct info_roll_path info_roll_path;

struct info_fov_path
{
    int num_frame;		// n£mero de frame en que estamos
    float fov;
    spline_flags_type spflags;
};
typedef struct info_fov_path info_fov_path;

struct fuente {
   char nombre_fuente[30]; // nombre de la fuente
   float wx,wy,wz;         // Posicion de la fuente
   float Ir,Ig,Ib;      // Intensidades en las tres bandas
   float radio;    // radio de accion
   int numero_keys_path;
   info_pos_path *posiciones_keys;
   Vector *tangentes_salida;
   Vector *tangentes_entrada;
   float inicio_rango;
   float fin_rango;
   float multiplicador;
   int atenuacion; // 1: se utiliza atenuacion; 0: no se utiliza
};

typedef struct fuente fuente;

struct camara {
  char nombre_camara[30];  // nombre de la camara
  float wx,wy,wz;    // posici¢n de la c mara
  float wxt,wyt,wzt; // posici¢n del objetivo
  float inclinacion; // inclinacion de la camara (coincide con angz)
  float fov;         // campo de visi¢n de la c mara 
  float near_range;
  float far_range;   // rango cercano y lejano de visión (usado para la niebla)

  int numero_keys_path;  
  info_pos_path *posiciones_keys; //info de cada posici¢n clave del path (key)
  Vector *tangentes_salida;	  // Necesario para el cálculo de las trayectorias
  Vector *tangentes_entrada;

  int numero_keys_rollpath;
  info_roll_path *posiciones_rollkeys; //info de cada posici¢n clave del path (key)
  float *tangentes_entrada_roll;
  float *tangentes_salida_roll;
  
  int numero_keys_path_target;  
  info_pos_path *posiciones_keys_target; //info de cada posici¢n clave del path (key)
  Vector *tangentes_salida_target;
  Vector *tangentes_entrada_target;

  int numero_keys_fovpath; 
  info_fov_path *posiciones_fovkeys; //info de cada posici¢n clave del path (key)
  float *tangentes_entrada_fov;
  float *tangentes_salida_fov;     
} ;


typedef struct camara camara;

struct object_type {
   char nombre_objeto[30];     // nombre del objeto 
   int number_of_vertices;     // N£mero de v‚rtices
   int number_of_polygons;     // N£mero de pol¡gonos
   float  x, y, z;               // Coordenadas mundiales del origen local del objeto
   float pivotx,pivoty,pivotz; // Pivot point
   float xscale, yscale, zscale;
   int cast_shadows;           // ¨¨ El objeto va a lanzar sombras ??
   float *TexCoordPointer;     // Array de coordenadas de textura (2*n_vertices)
   float *VertexPointer;       // Array de v‚rtices (3*n_vertices)
   float *NormalPointer;       // Array de normales de v‚rtice (3*n_vertices)
   float *PolygonNormalPointer;// Array de normales de pol¡gono (3*n_polygons)
   unsigned *IndexPointer;     // Array de ¡ndices (3*n_polygons)
   int *SharedPolygons;   // Pol¡gonos que comparten aristas (3*n_polygons)
                               // Se usar  para calcular las sombras...   
   int num_material;            // Material del objeto -> Vamos a asumir que un objeto sólo va a tener un material
   int numero_keys_path;
   int numero_keys_path_rotacion;
   int numero_keys_path_scale;
   info_pos_path *posiciones_keys;//info de cada posici¢n clave del path (key)
   info_rot_path *path_rotacion; // keys del path de rotacion
   info_scale_path *scale_keys;  // keys del path de escalado
   Quaternion current_rotation; // Used during keyframing, to know the current rotation
   
   float VSpeed;		// Velocidad de cambio de las coordenadas de textura
   float USpeed;   
   
   // A¤adidas para el calculo de la interpolacion en "real time"
  Vector *tangentes_salida;
  Vector *tangentes_entrada;
  Vector *tangentes_salida_scale;
  Vector *tangentes_entrada_scale;
  Quaternion *tangentes_entrada_rotacion;
  Quaternion *tangentes_salida_rotacion;
  
  float mesh_matrix[4][4];
  
  // Interacción con otros plugins
  int normalize; // ¿Hay que recalcular las normales?
  int visible;   // ¿Es visible el objeto?  
  int animate;	 // lo animamos?
  // Vertex shaders
  unsigned int vtx_shader_id;
  // Tipo de render
  int render_mode;  
};
typedef struct object_type object_type;

struct world_type {
   int number_of_objects;
   int number_of_fuentes;
   int number_of_camaras;
   int number_of_materials;
   fuente *fuentes;
   camara *camaras;
   object_type * obj;
   Material mats;
   int frame_inicial;
   int frame_final;
   int num_frames;
   float tiempo_animacion;
   int current_camera;
};

#endif
