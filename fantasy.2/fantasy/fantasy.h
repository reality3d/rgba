#ifndef FANTASY_H
#define FANTASY_H

#include "3dworld.h"
#include "vista.h"
#include "textura.h"

class Fantasy
{
protected:
    world_type *worlds;
    int *polycount;
    int max_scenes;
    int num_scenes;
    int current_camera;
    int current_scene;
    float *previous_time;
    View winview;
    view_type curview;

    void prepara_pos_animacion(world_type *world);
    void preprocesa_rotaciones(world_type *world);
    void preprocesa_roll_and_fov(world_type *world);
    void UpdateBoneMatrices(world_type *world);
    void UpdateChildBoneMatrices(Bone *bone, float parent_matrix[4][4]);
public:
    Fantasy(int maxscenes); // N£mero m ximo de escenas a cargar
    ~Fantasy();
    int Load(char *anim_filename,char *mat_filename);
    void SetScene(int scene);
    void SetCamera(int camera);
    void SetDynLighting(int dynlighting);
    void DrawFrame(float frame,float tiempo);
    void SetFog(int yesno,int fog_type,float fog_r,float fog_g,float fog_b,float density,int fog_invert);
    void SetGlobalIlum(float ilum);
    void SetGlobalAlpha(float alpha);
    void SetZBufferClear(int yesno);
    void CleanupTextures(int scene);
    void SetUChange(char *object_name, float speed);
    void SetVChange(char *object_name, float speed);
    void LoadVertexProgram(char *objname, char * filename);
    void SetRenderMode(char *object_name, int render_mode);
    world_type *GetWorldPointer(void);
    void *GetAPI(void);
    float GetPos(char *object_name, float frame, int which);
    float GetScale(char *object_name, float frame, int which);
    void SetPos(char *object_name, int which, float *xyz, int mode, float frame, int type);
    void SetScale(char *object_name, int which, float *xyz, int mode, float frame);
    void SetAnimateFlag(char *object_name, int flag, int type);
    void SetMaterial(char *object_name, char *material_name);
    void SetLightColor(char *light_name, float r, float g, float b);
    void SetLightMult(char *light_name, float value);
    void SetFov(char *camera_name, float value);
    void SetRoll(char *camera_name, float value);
    float GetFov(char *camera_name);
    float GetRoll(char *camera_name);
    void SetRotation(char *object_name, float x, float y, float z, float angle, int mode, float frame);
    void SetObjectAlpha(char *object_name, float alpha);
    void SetDebugLevel(int debuglevel);
    void SetSpecialFX(char *objname, int fxtype);
    void SetFXParamu(char *objname, int index, unsigned int value);
    void SetFXParamf(char *objname, int index, float value);
    void SetShaderParamf(char *mat_name, char *param_name, float value);	


    
    unsigned char render_path;
};

#endif
