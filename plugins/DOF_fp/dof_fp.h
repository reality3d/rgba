#ifndef _DOF_FP_H_
#define _DOF_FP_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <glt/glt.h>

typedef struct
{
    int             xres, yres;
    TTexture        *tid_screen;	 // Textura del viewport completo
    TTexture        *tid_small;   // small blurred texture
    TTexture	    *shared_depth_texture;  // Shared depth texture via GLT
    int 	    r2tsizex, r2tsizey;      // Size of the small blurred texture
    unsigned int    tid_depth;   // Depth buffer texture (1024x1024)
    float           ox, oy;
    int             blur_iterations; // Número de iteraciones de blur
    int algorithm;		// 0: leer el zbuffer; 1: distancia almacenada en el alpha; 2: shared depth texture via GLT
    float focus;
    float range;
    
// Shader variables
    GLhandleARB programObj;
    GLhandleARB vertexShader;
    GLhandleARB fragmentShader;    
    
    GLuint loc_deltax;
    GLuint loc_deltay;
    GLuint loc_iteration;
    GLuint loc_TexUnit[8];


    GLhandleARB mix_programObj;
    GLhandleARB mix_vertexShader;
    GLhandleARB mix_fragmentShader;  
    GLuint loc_mix_TexUnit0;
    GLuint loc_mix_TexUnit1;
    GLuint loc_mix_TexUnit2;    
    GLuint loc_mix_focus;
    GLuint loc_mix_range;
    
    GLhandleARB mixalpha_programObj;
    GLhandleARB mixalpha_vertexShader;
    GLhandleARB mixalpha_fragmentShader;  
    GLuint loc_mixalpha_TexUnit0;
    GLuint loc_mixalpha_TexUnit1;
    GLuint loc_mixalpha_focus;
    GLuint loc_mixalpha_range;    
    
    GLhandleARB mixshared_programObj;
    GLhandleARB mixshared_fragmentShader;  
    GLuint loc_mixshared_TexUnit0;
    GLuint loc_mixshared_TexUnit1;
    GLuint loc_mixshared_TexUnit2;    
    GLuint loc_mixshared_focus;
    GLuint loc_mixshared_range;
    
}DOF_FP;

DOF_FP *DOF_FP_Init( int xres, int yres, int r2tsizex, int r2tsizey );
void DOF_FP_End( DOF_FP *dof );
void DOF_FP_DoFrame( DOF_FP *dof );
void DOF_FP_SetSharedDepthTexture(DOF_FP *g, char *texname);

#endif
