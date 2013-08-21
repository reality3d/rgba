#ifndef _GLOW_FP_H_
#define _GLOW_FP_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <glt/glt.h>
#include <string.h>

typedef struct
{
    int             xres, yres;
    TTexture        *tid_screen;	 // Textura del viewport completo
    TTexture        *tid_small;   // small blurred texture
    TTexture	    *tid_shared;  // shared GLT texture defining blend factors
    int		    r2tsizex, r2tsizey; // Size of the small blurred texture
    float           ox, oy;
    int             blur_iterations; // Número de iteraciones de blur
    int algorithm;		// 0: resta + overbright; 1: alpha values; 2: textura GLT  
    int add_or_multiply;	// 0: add; 1: multiply  
    float cut_resta[4];		// Para el algoritmo de resta + overbright
    float cut_alpha;            // Para el algoritmo de alpha values
    float producto[4];      // Para todos los algoritmos
    int overbright_pre_or_post; // Hacer el overbright antes o después del blur; 0: antes; 1: después
    
// Shader variables
    GLhandleARB programObj;
    GLhandleARB vertexShader;
    GLhandleARB fragmentShader;

    GLhandleARB cut_programObj;
    GLhandleARB cut_vertexShader;
    GLhandleARB cut_fragmentShader;

    GLhandleARB cutalpha_programObj;
//    GLhandleARB cutalpha_vertexShader;
    GLhandleARB cutalpha_fragmentShader;
    
    GLhandleARB add_programObj;
    GLhandleARB add_vertexShader;
    GLhandleARB add_fragmentShader;

    GLhandleARB addGLT_programObj;
    GLhandleARB addGLT_vertexShader;
    GLhandleARB addGLT_fragmentShader;
        
    GLuint loc_deltax;
    GLuint loc_deltay;
    GLuint loc_iteration;
    GLuint loc_TexUnit[8];
    
    GLuint cut_loc_TexUnit0;
    GLuint cut_loc_resta;
    GLuint cut_loc_producto;

    GLuint cutalpha_loc_TexUnit0;
    GLuint cutalpha_loc_cut_alpha;
    
    GLuint add_loc_TexUnit0;
    GLuint add_loc_TexUnit1;
    GLuint add_loc_producto;    
    GLuint add_loc_addormultiply;    

    GLuint addGLT_loc_TexUnit0;
    GLuint addGLT_loc_TexUnit1;
    GLuint addGLT_loc_TexUnit2;
    GLuint addGLT_loc_producto;    
    GLuint addGLT_loc_addormultiply;    
}GLOW_FP;

GLOW_FP *GLOW_FP_Init( int xres, int yres, int r2tsizex, int r2tsizey );
void GLOW_FP_End( GLOW_FP *glow );
void GLOW_FP_DoFrame( GLOW_FP *glow);
void GLOW_FP_SetSharedGLTTexture(GLOW_FP *g, char *texname);

#endif
