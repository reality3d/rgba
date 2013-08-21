#ifndef _BLURFP_H_
#define _BLURFP_H_


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>


typedef struct
{
    int type;	// Blur type. 0: box blur; 1: generic blur
    int             xres, yres; // screen size
    float           ox, oy;
    int iterations;
    float coeff[9];
    
    GLhandleARB programObj;
    GLhandleARB vertexShader;
    GLhandleARB fragmentShader;
    GLuint      tid;
    GLuint      tid_small;    
    int small_tex_size;
    
    // Shader uniforms
    GLuint loc_deltax;
    GLuint loc_deltay;
    GLuint loc_TexUnit[8];
    GLuint loc_coeff[9];    
    GLuint loc_iteration;  
                
}BlurFP;

BlurFP *BlurFP_Init( int type, int r2tsize, int xres, int yres );
void BlurFP_End( BlurFP *dof );
void BlurFP_BlurDoFrame( BlurFP *dof );


#endif
