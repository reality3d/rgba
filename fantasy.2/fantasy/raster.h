#ifndef RASTER_H
#define RASTER_H

#ifdef WINDOZE
 #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include "3dworld.h"

class Raster
{
protected: 
    int fog;
    int clearZbuffer;
    float globalAlpha;
    int dyn_lighting;
    unsigned int cartoon_texture;
    float CameraPosition[3];

    void SetRenderMode(struct object_type *obj);
    void SetPostRender(struct object_type *obj);
    void SetVertexProgram(unsigned int vtx_shader_id);    
    void SoftSkinObject(struct object_type *obj);
    void DrawObjectHalo(struct object_type *obj,float globalIlum,struct world_type *world);
    void DrawObjectNormal(struct object_type *obj,float globalIlum,struct world_type *world);
    void DrawObjectDOF(struct object_type *obj,float globalIlum,struct world_type *world);

public:

    Raster();
    ~Raster();
    void InitRender(void);
    void FinRender(void);
    void SetCameraPos(float eyex,float eyey,float eyez,float focusx,float focusy,float focusz,float upx,float upy,float upz);
    void DrawObject(struct object_type *obj,float globalIlum,struct world_type *world);
    void SetFog(int fog_type,float fog_r,float fog_g,float fog_b,float density,float near_range,float far_range,int fog_invert);
    void SetFov(float fov);
    void SetGlobalAlpha(float alpha);
    void SetZBufferClear(int yesno);
    void SetDynLighting(int dynlighting);
    void ProcessLights(struct world_type *world);   
    void LoadVertexProgram(unsigned int *vtx_shader_id,char *filename);
    void DisplaceTexcoords(object_type *obj);
    void SetSpecialFX(object_type *obj, int fxtype);
};

// Definitions for object special effects

#define FX_NONE 0
#define FX_HALO 1
#define FX_DOF	2

#endif
