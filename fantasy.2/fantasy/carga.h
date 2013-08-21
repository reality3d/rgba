#ifndef CARGA_H

#define CARGA_H
#include "3dworld.h"

extern int Load3DFile (world_type * world, char *filename);
extern void AdjustVertices(object_type *obj);

typedef struct
{
 char signature[5];
 unsigned char version_major;
 unsigned char version_minor;
 int first_frame;
 int last_frame;
 int framerate;
 int delta_time;
 float global_scale;
 int nobjects;
 int ncameras;
 int nlights;
 int nmaterials;
 int ncontrollers; 
 int nbones;
}u3d_header;
 
typedef struct
{
 char *name;
 int id;
 int parentid;
 int targetid;
 unsigned char hidden;
 float position[3];
 float rotation[4];
 float scale[3];
 float pivot[3];
 float fov;
 float near_range;
 float far_range;
}u3d_camera;

typedef struct
{
 char *name;
 int id;
 int parentid;
 unsigned char hidden;
 float position[3];
 float rotation[4];
 float scale[3];
 float pivot[3];
}u3d_camera_target;

typedef struct
{
 char *name;
 int id;
 int parentid;
 unsigned char hidden;
 float position[3];
 float rotation[4];
 float scale[3];
 float pivot[3];
}u3d_bone;


typedef struct
{
 char *name;
 int id;
 int parentid;
 unsigned char hidden;
 int flags;
 float position[3];
 float rotation[4];
 float scale[3];
 float pivot[3];
 int nverts;
 int nfaces;
 int nuvcoords;
 int nvertexcolors;
 float *vertexlist;
 float *uvlist;
 float *vertexcolors;
 int *faces;
 int *matids;
 float bspherecenter[3];
 float bsphereradius;
}u3d_mesh;

typedef struct
{
 char *name;
 int id;
 int parentid;
 int targetid;
 unsigned char hidden;
 float position[3];
 float rotation[4];
 float scale[3];
 float pivot[3];
 unsigned int light_type;
 float color[3];
 float inicio_rango;
 float fin_rango;
 unsigned char atenuacion;
 float multiplicador;
 float target_position[3];
 float cut_off;
 float exponent;
 
}u3d_light;

typedef struct
{
 char *name;
 int id;	// Identificador según el MAX
 int matid; 	// Identificador interno para el fantasy
}u3d_material;

#define U3D_CTRL_POSITION	(unsigned char)0
#define U3D_CTRL_ROTATION	(unsigned char)1
#define U3D_CTRL_SCALE		(unsigned char)2
#define U3D_CTRL_FOV		(unsigned char)3
#define U3D_CTRL_ROLL		(unsigned char)4
#define U3D_CTRL_UNKNOWN	(unsigned char)5

#define U3D_SCHEME_LINEAR	(unsigned char)0
#define U3D_SCHEME_TCB		(unsigned char)1
#define U3D_SCHEME_BEZIER	(unsigned char)2

typedef struct
{
 int key_number;
 int time;
 unsigned char flags;
 float vector[3];
 float quaternion[4];
 float tension, continuity,bias,easein,easeout;
}u3d_keys;

typedef struct
{
 unsigned char ctrl_type;	
 int id;
 int owner_id;
 int nkeyframes;
 unsigned char interp_scheme;
 u3d_keys *keys;
}u3d_controller;




#endif	
