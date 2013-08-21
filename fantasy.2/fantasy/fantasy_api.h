#ifndef FANTASY_API_H
#define FANTASY_API_H

#include <epopeia/epopeia_system.h>
#include "3dworld.h"


class FantasyAPI
{
	
public:
	world_type *world;

	FantasyAPI(world_type *current_world);
	~FantasyAPI();
	
// Funciones misceláneas		
	EPOPEIA_PLUGIN_API 	int GetActiveCamera(void);
// Funciones de gestión de objetos	
	EPOPEIA_PLUGIN_API int  CopyObject(char *original_object, char *dst_object);
	EPOPEIA_PLUGIN_API void CopyQuaternion(int original_object, int dst_object);
	EPOPEIA_PLUGIN_API void SetPosition(int objindex,float x,float y,float z);
	EPOPEIA_PLUGIN_API void SetScale(int objindex,float scalex,float scaley,float scalez);
	EPOPEIA_PLUGIN_API void SetRotation(int objindex, Quaternion *rotation);
	EPOPEIA_PLUGIN_API void GetPosition(int objindex,float frame,float *x,float *y,float *z);
	EPOPEIA_PLUGIN_API void GetRotation(int objindex,float frame,Quaternion *q);
	EPOPEIA_PLUGIN_API void GetScale(int objindex,float frame,float *scalex,float *scaley,float *scalez);
	EPOPEIA_PLUGIN_API void Normalize(int objindex);
	EPOPEIA_PLUGIN_API void SetAnimateFlag(int objindex, int flag);
	EPOPEIA_PLUGIN_API void SetMaterial(int objindex, char *matname);
	EPOPEIA_PLUGIN_API int  FindObject(char *name);
	EPOPEIA_PLUGIN_API void UpdateObject(object_type *obj,unsigned char flags);
// Funciones de gestión de quaternions
	EPOPEIA_PLUGIN_API void API_Axis_to_Quat(float x, float y, float z, float angle, Quaternion *result);
	EPOPEIA_PLUGIN_API void API_Quat_to_RotMatrix(Quaternion *q1, float matrix[4][4]);
	EPOPEIA_PLUGIN_API void API_Quat_to_Axis(Quaternion *q, float *x, float *y, float *z, float *angle);
};

#endif