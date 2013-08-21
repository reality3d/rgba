#ifndef VISTA_H
#define VISTA_H

#include "3dworld.h"
#include "raster.h"

// Los siguientes parámetros van a controlar el funcionamiento del sistema de vista + renderizado


struct view_parameters
{
 int dyn_lighting;   // 0: Sin luces; 1: Con luces (defecto)
 float globalIlum;  // iluminación global de la escena (crossfades!)
 float globalAlpha; // alpha global de la escena
};

typedef struct view_parameters view_parameters;


struct view_type 
{
  float wx,wy,wz;    // posici¢n de la c mara
  float wxt,wyt,wzt; // posici¢n del objetivo
  float inclinacion; // inclinacion de la camara (coincide con angz)
  float fov;         // campo de visi¢n de la c mara (no se usa por ahora, ya que es fijo de 90 grados)
  float near_range,far_range; // Rango de la camara (de momento solo usado para el fog)

  int fog;	// 0: no fog; 1: fog
  int fog_type; // 0: GL_EXP; 1: GL_EXP2; 2: GL_LINEAR
  float fog_r,fog_g,fog_b; // Color de la niebla
  float fog_density; // Densidad de la niebla
  int fog_invert; // 0: fog normal; 1: fog invertido
  int using_cam; // 0: not using a camera; 1: using a camera
};


class View {
private:  
   world_type *world;
   Raster *raster;
 
   void alignview (world_type *, view_type);
//   void update (object_type * ,view_type);
   void draw_objects(void);


public:
    View();
    ~View();
   void setworld (world_type *wrld, int polycnt);
   void display (view_type curview);
   void SetGlobalAlpha(float alpha);
   void SetZBufferClear(int yesno);
   void SetDynLighting(int dynlighting);
   void LoadVertexProgram(object_type *obj,char *filename);
   void DisplaceTexcoords(object_type *obj);
   void SetSpecialFX(object_type *obj,int fxtype);
   view_parameters params;
  
};
#endif
