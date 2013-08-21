#define DEBUG

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <malloc.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <string.h>
#ifdef LINUX
#include <GL/glx.h>
#define wglGetProcAddress glXGetProcAddressARB
#endif

#include <epopeia/epopeia_system.h>
#include "dof_fp.h"

// GL_ARB_shader_objects
static PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB  = NULL;
static PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB         = NULL;
static PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB     = NULL;
static PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB   = NULL;
static PFNGLSHADERSOURCEARBPROC         glShaderSourceARB         = NULL;
static PFNGLCOMPILESHADERARBPROC        glCompileShaderARB        = NULL;
static PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
static PFNGLATTACHOBJECTARBPROC         glAttachObjectARB         = NULL;
static PFNGLGETINFOLOGARBPROC           glGetInfoLogARB           = NULL;
static PFNGLLINKPROGRAMARBPROC          glLinkProgramARB          = NULL;
static PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB   = NULL;
static PFNGLUNIFORM4FARBPROC            glUniform4fARB            = NULL;
static PFNGLUNIFORM1IARBPROC            glUniform1iARB            = NULL;
static PFNGLUNIFORM1FARBPROC            glUniform1fARB            = NULL;
static PFNGLACTIVETEXTUREARBPROC   	glActiveTextureARB = NULL;
static PFNGLMULTITEXCOORD2FARBPROC	glMultiTexCoord2fARB = NULL;

static char *BLUR_VS="\
uniform float deltax; \
uniform float deltay; \
uniform float iteration;\
void main(void) \
{ \
 float salto;\
 salto=exp2(iteration);\
 gl_TexCoord[0] = gl_MultiTexCoord0 + vec4(deltax*salto,deltay*salto,0.0,1.0); \
 gl_TexCoord[4] = gl_MultiTexCoord0 - vec4(deltax*salto,deltay*salto,0.0,1.0); \
 gl_TexCoord[1] = gl_TexCoord[0] + vec4(deltax,deltay,0.0,1.0); \
 gl_TexCoord[2] = gl_TexCoord[1] + vec4(deltax,deltay,0.0,1.0); \
 gl_TexCoord[3] = gl_TexCoord[2] + vec4(deltax,deltay,0.0,1.0); \
 gl_TexCoord[5] = gl_TexCoord[4] - vec4(deltax,deltay,0.0,1.0); \
 gl_TexCoord[6] = gl_TexCoord[5] - vec4(deltax,deltay,0.0,1.0); \
 gl_TexCoord[7] = gl_TexCoord[6] - vec4(deltax,deltay,0.0,1.0); \
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
}";

static char *BLUR_FS="\
uniform sampler2D TexUnit0;\
uniform sampler2D TexUnit1;\
uniform sampler2D TexUnit2;\
uniform sampler2D TexUnit3;\
uniform sampler2D TexUnit4;\
uniform sampler2D TexUnit5;\
uniform sampler2D TexUnit6;\
uniform sampler2D TexUnit7;\
void main(void)\
{ \
 vec4 value;\
 value  = texture2D(TexUnit0, vec2(gl_TexCoord[0]));\
 value += texture2D(TexUnit1, vec2(gl_TexCoord[1]));\
 value += texture2D(TexUnit2, vec2(gl_TexCoord[2]));\
 value += texture2D(TexUnit3, vec2(gl_TexCoord[3]));\
 value += texture2D(TexUnit4, vec2(gl_TexCoord[4]));\
 value += texture2D(TexUnit5, vec2(gl_TexCoord[5]));\
 value += texture2D(TexUnit6, vec2(gl_TexCoord[6]));\
 value += texture2D(TexUnit7, vec2(gl_TexCoord[7]));\
 value.w = 1.0;\
 gl_FragColor = value * 0.1111;\
}";


static char *MIX_VS="\
void main(void) \
{ \
 gl_TexCoord[0] = gl_MultiTexCoord0; \
 gl_TexCoord[1] = gl_MultiTexCoord1; \
 gl_TexCoord[2] = gl_MultiTexCoord2; \
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
}";

static char *MIX_FS="\
uniform sampler2D TexUnit0;\
uniform sampler2D TexUnit1;\
uniform sampler2D TexUnit2;\
uniform float range;\
uniform float focus;\
void main(void)\
{\
 vec4 pantalla = texture2D(TexUnit0, vec2(gl_TexCoord[0]));\
 vec4 blur = texture2D(TexUnit1, vec2(gl_TexCoord[1]));\
 vec4 zbuff = texture2D(TexUnit2, vec2(gl_TexCoord[2]));\
 zbuff = clamp(zbuff - 0.999,0.0,1.0) * 1000.0;\
 gl_FragColor = mix(pantalla, blur, clamp(range * abs(focus - zbuff.a), 0.0, 1.0)); \
}";

static char *MIXALPHA_VS="\
void main(void) \
{ \
 gl_TexCoord[0] = gl_MultiTexCoord0; \
 gl_TexCoord[1] = gl_MultiTexCoord1; \
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
}";

static char *MIXALPHA_FS="\
uniform sampler2D TexUnit0;\
uniform sampler2D TexUnit1;\
uniform float range;\
uniform float focus;\
void main(void)\
{\
 vec4 pantalla = texture2D(TexUnit0, vec2(gl_TexCoord[0]));\
 vec4 blur = texture2D(TexUnit1, vec2(gl_TexCoord[1]));\
 gl_FragColor = mix(pantalla, blur, clamp(range * abs(focus - pantalla.a), 0.0, 1.0)); \
}";

static char *MIXSHARED_FS="\
uniform sampler2D TexUnit0;\
uniform sampler2D TexUnit1;\
uniform sampler2D TexUnit2;\
uniform float range;\
uniform float focus;\
void main(void)\
{\
 vec4 pantalla = texture2D(TexUnit0, vec2(gl_TexCoord[0]));\
 vec4 blur = texture2D(TexUnit1, vec2(gl_TexCoord[1]));\
 vec4 dist = texture2D(TexUnit2, vec2(gl_TexCoord[2]));\
 if (abs(dist.a - 0.5) < 0.1) \
  gl_FragColor = mix(pantalla, blur, clamp(range * abs(focus - dist.r), 0.0, 1.0)); \
 else\
  gl_FragColor = blur;\
}";

/*
 if(zbuff.a > 0.9) gl_FragColor=vec4(1.0,0.0,0.0,1.0);\
  else gl_FragColor=vec4(0.0,0.0,1.0,1.0);\
*/



//----------------------------------------------------------------------------

static int GLExtSupported(char *extension)
{
 const GLubyte *extensions = NULL;
 const GLubyte *start;
 GLubyte *where, *terminator;

 /* Extension names should not have spaces. */
 where = (GLubyte *) strchr(extension, ' ');
 if (where || *extension == '\0')
   return 0;

 extensions = glGetString(GL_EXTENSIONS);

// It takes a bit of care to be fool-proof about parsing the OpenGL extensions string. 
// Don't be fooled by sub-strings,etc. 

 start = extensions;
 for (;;) 
 {
  where = (GLubyte *) strstr((const char *) start, extension);
  if (!where)
    break;
  terminator = where + strlen(extension);
  if (where == start || *(where - 1) == ' ')
   if (*terminator == ' ' || *terminator == '\0')
    return 1; 
  start = terminator;
 }

 return 0;
}

//----------------------------------------------------------------------------
static int DOF_FP_InitShaders(DOF_FP *g)
{
 GLint wasok;
 char errorString[4096];

 
 // Create shader
 g->vertexShader = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
 // Compile shader
 glShaderSourceARB( g->vertexShader, 1, (const char **)&BLUR_VS, NULL );
 glCompileShaderARB( g->vertexShader);
 // Check for errors
 glGetObjectParameterivARB( g->vertexShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->vertexShader, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error loading vertex shader\n" );
	printf(errorString);	
	return 1;
 }

 // Create fragment shader
 g->fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
 // Compile fragment shader
 glShaderSourceARB( g->fragmentShader, 1, (const char **) &BLUR_FS, NULL );
 glCompileShaderARB( g->fragmentShader );
 glGetObjectParameterivARB( g->fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->fragmentShader, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error loading fragment shader\n" );
	printf(errorString);	
	return 1;
 }


 // Create a program object with the shaders 
 g->programObj = glCreateProgramObjectARB();
 glAttachObjectARB( g->programObj, g->vertexShader );
 glAttachObjectARB( g->programObj, g->fragmentShader );
 // Link and check for errors
 glLinkProgramARB( g->programObj );
 glGetObjectParameterivARB( g->programObj, GL_OBJECT_LINK_STATUS_ARB, &wasok); 

 if( !wasok )
 {
	glGetInfoLogARB(g->programObj, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error linking shader\n" );
	printf(errorString);	
	return 1;
 }

 // Last thing left is to get some parameters by name...
 
 g->loc_deltax = glGetUniformLocationARB (g->programObj,"deltax");
 g->loc_deltay= glGetUniformLocationARB (g->programObj,"deltay");
 g->loc_iteration= glGetUniformLocationARB (g->programObj,"iteration");
 g->loc_TexUnit[0] = glGetUniformLocationARB (g->programObj,"TexUnit0");
 g->loc_TexUnit[1] = glGetUniformLocationARB (g->programObj,"TexUnit1");
 g->loc_TexUnit[2] = glGetUniformLocationARB (g->programObj,"TexUnit2");
 g->loc_TexUnit[3] = glGetUniformLocationARB (g->programObj,"TexUnit3");
 g->loc_TexUnit[4] = glGetUniformLocationARB (g->programObj,"TexUnit4");
 g->loc_TexUnit[5] = glGetUniformLocationARB (g->programObj,"TexUnit5");
 g->loc_TexUnit[6] = glGetUniformLocationARB (g->programObj,"TexUnit6");
 g->loc_TexUnit[7] = glGetUniformLocationARB (g->programObj,"TexUnit7");
 
  // Create shader
 g->mix_vertexShader = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
 // Compile shader
 glShaderSourceARB( g->mix_vertexShader, 1, (const char **)&MIX_VS, NULL );
 glCompileShaderARB( g->mix_vertexShader);
 // Check for errors
 glGetObjectParameterivARB( g->mix_vertexShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->mix_vertexShader, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error loading vertex shader\n" );
	printf(errorString);	
	return 1;
 }

 // Create fragment shader
 g->mix_fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
 // Compile fragment shader
 glShaderSourceARB( g->mix_fragmentShader, 1, (const char **) &MIX_FS, NULL );
 glCompileShaderARB( g->mix_fragmentShader );
 glGetObjectParameterivARB( g->mix_fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->mix_fragmentShader, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error loading fragment shader\n" );
	printf(errorString);	
	return 1;
 }


 // Create a program object with the shaders 
 g->mix_programObj = glCreateProgramObjectARB();
 glAttachObjectARB( g->mix_programObj, g->mix_vertexShader );
 glAttachObjectARB( g->mix_programObj, g->mix_fragmentShader );
 // Link and check for errors
 glLinkProgramARB( g->mix_programObj );
 glGetObjectParameterivARB( g->mix_programObj, GL_OBJECT_LINK_STATUS_ARB, &wasok); 

 if( !wasok )
 {
	glGetInfoLogARB(g->mix_programObj, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error linking shader\n" );
	printf(errorString);	
	return 1;
 }

 // Last thing left is to get some parameters by name...
 
 g->loc_mix_focus    = glGetUniformLocationARB (g->mix_programObj,"focus");
 g->loc_mix_range    = glGetUniformLocationARB (g->mix_programObj,"range");
 g->loc_mix_TexUnit0 = glGetUniformLocationARB (g->mix_programObj,"TexUnit0");
 g->loc_mix_TexUnit1 = glGetUniformLocationARB (g->mix_programObj,"TexUnit1");
 g->loc_mix_TexUnit2 = glGetUniformLocationARB (g->mix_programObj,"TexUnit2");


  // Create third shader
 g->mixalpha_vertexShader = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
 // Compile shader
 glShaderSourceARB( g->mixalpha_vertexShader, 1, (const char **)&MIXALPHA_VS, NULL );
 glCompileShaderARB( g->mixalpha_vertexShader);
 // Check for errors
 glGetObjectParameterivARB( g->mixalpha_vertexShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->mixalpha_vertexShader, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error loading vertex shader\n" );
	printf(errorString);	
	return 1;
 }

 // Create fragment shader
 g->mixalpha_fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
 // Compile fragment shader
 glShaderSourceARB( g->mixalpha_fragmentShader, 1, (const char **) &MIXALPHA_FS, NULL );
 glCompileShaderARB( g->mixalpha_fragmentShader );
 glGetObjectParameterivARB( g->mixalpha_fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->mixalpha_fragmentShader, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error loading fragment shader\n" );
	printf(errorString);	
	return 1;
 }


 // Create a program object with the shaders 
 g->mixalpha_programObj = glCreateProgramObjectARB();
 glAttachObjectARB( g->mixalpha_programObj, g->mixalpha_vertexShader );
 glAttachObjectARB( g->mixalpha_programObj, g->mixalpha_fragmentShader );
 // Link and check for errors
 glLinkProgramARB( g->mixalpha_programObj );
 glGetObjectParameterivARB( g->mixalpha_programObj, GL_OBJECT_LINK_STATUS_ARB, &wasok); 

 if( !wasok )
 {
	glGetInfoLogARB(g->mixalpha_programObj, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error linking shader\n" );
	printf(errorString);	
	return 1;
 }

 // Last thing left is to get some parameters by name...
 
 g->loc_mixalpha_focus    = glGetUniformLocationARB (g->mixalpha_programObj,"focus");
 g->loc_mixalpha_range    = glGetUniformLocationARB (g->mixalpha_programObj,"range");
 g->loc_mixalpha_TexUnit0 = glGetUniformLocationARB (g->mixalpha_programObj,"TexUnit0");
 g->loc_mixalpha_TexUnit1 = glGetUniformLocationARB (g->mixalpha_programObj,"TexUnit1");


 // Create fourth shader
 // Create fragment shader
 g->mixshared_fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
 // Compile fragment shader
 glShaderSourceARB( g->mixshared_fragmentShader, 1, (const char **) &MIXSHARED_FS, NULL );
 glCompileShaderARB( g->mixshared_fragmentShader );
 glGetObjectParameterivARB( g->mixshared_fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->mixshared_fragmentShader, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error loading fragment shader\n" );
	printf(errorString);	
	return 1;
 }


 // Create a program object with the shaders 
 g->mixshared_programObj = glCreateProgramObjectARB();
 glAttachObjectARB( g->mixshared_programObj, g->mix_vertexShader );  // vertex shader is reused
 glAttachObjectARB( g->mixshared_programObj, g->mixshared_fragmentShader );
 // Link and check for errors
 glLinkProgramARB( g->mixshared_programObj );
 glGetObjectParameterivARB( g->mixshared_programObj, GL_OBJECT_LINK_STATUS_ARB, &wasok); 

 if( !wasok )
 {
	glGetInfoLogARB(g->mixshared_programObj, sizeof(errorString), NULL, errorString);
	printf( "dof_fp.epl:Error linking shader\n" );
	printf(errorString);	
	return 1;
 }

 // Last thing left is to get some parameters by name...
 
 g->loc_mixshared_focus    = glGetUniformLocationARB (g->mixshared_programObj,"focus");
 g->loc_mixshared_range    = glGetUniformLocationARB (g->mixshared_programObj,"range");
 g->loc_mixshared_TexUnit0 = glGetUniformLocationARB (g->mixshared_programObj,"TexUnit0");
 g->loc_mixshared_TexUnit1 = glGetUniformLocationARB (g->mixshared_programObj,"TexUnit1");
 g->loc_mixshared_TexUnit2 = glGetUniformLocationARB (g->mixshared_programObj,"TexUnit2");

 
 return 0; // Everything went ok
}
//----------------------------------------------------------------------------

DOF_FP *DOF_FP_Init( int xres, int yres, int r2tsizex, int r2tsizey )
{
    DOF_FP            *g;

    g = (DOF_FP*)malloc( sizeof(DOF_FP) );
    if( !g )
        return( 0 );

    g->blur_iterations = 1;
    g->focus = 0.1f;
    g->range = 3.0f;
    g->r2tsizex = r2tsizex;
    while(g->r2tsizex > Epopeia_GetResX())
        g->r2tsizex /= 2;
    g->r2tsizey = r2tsizey;
    while(g->r2tsizey > Epopeia_GetResY())
        g->r2tsizey /= 2;
    g->algorithm = 0;
    g->shared_depth_texture = NULL;
    

    if( GLExtSupported("GL_ARB_shading_language_100") &&
        GLExtSupported("GL_ARB_shader_objects") &&
        GLExtSupported("GL_ARB_fragment_shader") &&
        GLExtSupported("GL_ARB_depth_texture"))
        {
         // Extension initialization
         glCreateProgramObjectARB  = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
         glDeleteObjectARB         = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
         glUseProgramObjectARB     = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
         glCreateShaderObjectARB   = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
         glShaderSourceARB         = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
         glCompileShaderARB        = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
         glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
         glAttachObjectARB         = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
         glGetInfoLogARB           = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
         glLinkProgramARB          = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
         glGetUniformLocationARB   = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
         glUniform4fARB            = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
	 glUniform1iARB            = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
	 glUniform1fARB            = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
	 glActiveTextureARB	   = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
	 glMultiTexCoord2fARB	   = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
	 // We need at least 8 tex coord sets
	 // Every card supporting glslang I am aware of does have them
	 // All Geforce FX line, Ati Radeon 9500+, Ati Radeon X300-800 line
	 // but one never knows...
	 int ncoords;
	 glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB,&ncoords); 
	 
         if( !glCreateProgramObjectARB || !glDeleteObjectARB || !glUseProgramObjectARB ||
            !glCreateShaderObjectARB || !glCreateShaderObjectARB || !glCompileShaderARB || 
            !glGetObjectParameterivARB || !glAttachObjectARB || !glGetInfoLogARB || 
            !glLinkProgramARB || !glGetUniformLocationARB || !glUniform4fARB ||
			!glUniform1iARB ||!glUniform1fARB || !glActiveTextureARB ||
			!glMultiTexCoord2fARB || (ncoords<8))
         {
         #ifdef DEBUG
            printf( "dof_fp.epl:This card does not support GLslang extensions or ARB_depth_testure\n" );
            return( 0 );
         #else
            return( 0 );
         #endif
         }	
        	
        // Screen texture, to hold the full render to texture
        	
         g->xres = xres;
         g->yres = yres;
         g->ox = (float)xres/1024.0f;
         g->oy = (float)yres/1024.0f;

	 g->tid_screen = CACHE_LoadTexture("R2T_1024_1024",0);
	 if(!g->tid_screen)
	 {
	  // Creamos la textura, puesto que no existe
	  // Cuando pasemos a GLT2, 32 == GL_RGBA8
	  g->tid_screen = TEX_New (1024, 1024, 32, NULL, 0);
          if (!g->tid_screen) return 0;
          if (!CACHE_InsertTexture ("R2T_1024_1024", g->tid_screen)) return 0;	 	 	
	 } 

	 // Small texture, to hold the blurred texture
	  // Cuando pasemos a GLT2, 32 == GL_RGBA8
	  g->tid_small = TEX_New (g->r2tsizex, g->r2tsizey, 32, NULL, 0);
	  if (!g->tid_small) return 0;
	 
	
	 // Depth texture, used to hold the depth component
	 
	 glGenTextures(1, &g->tid_depth);
	 if( g->tid_depth<1 )
            return( 0 );
         glBindTexture(GL_TEXTURE_2D, g->tid_depth);
	 glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, 
	 	       GL_UNSIGNED_BYTE, NULL);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
    else
        {
        #ifdef DEBUG
            printf( "dof_fp.epl:This card does not support GLslang extensions or ARB_depth_testure\n" );
            return( 0 );            
        #else
            return( 0 );
        #endif
        }


    if(!DOF_FP_InitShaders(g)) return( g );
     else return 0;
}


void DOF_FP_End( DOF_FP *g )
{
    if (g->tid_screen)
        CACHE_FreeTexture (g->tid_screen);    
    if(g->tid_small) TEX_Delete(g->tid_small );
    if(g->shared_depth_texture)
    	CACHE_FreeTexture (g->shared_depth_texture);    
    glDeleteTextures( 1, &g->tid_depth );
    free( g );
}


static void DOF_FP_InitialPass (DOF_FP *g)
{
    // First pass   
    glEnable( GL_TEXTURE_2D );
    TEX_Bind( g->tid_screen );
    TEX_SetFilter(GL_LINEAR, GL_LINEAR);
    TEX_SetWrap(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
  
    glDisable(GL_BLEND);  
           
    glBegin( GL_QUADS );
            glTexCoord2f(     0, g->oy ); glVertex2f( 0.0f, 1.0f);
            glTexCoord2f(     0,     0 ); glVertex2f( 0.0f, 0.0f);
            glTexCoord2f( g->ox,     0 ); glVertex2f( 1.0f, 0.0f);
            glTexCoord2f( g->ox, g->oy ); glVertex2f( 1.0f, 1.0f);
    glEnd();

    // Copy to the small texture
    TEX_Bind( g->tid_small );
    glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE );
    glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->r2tsizex,g->r2tsizey);    
	
}

static void DOF_FP_SecondPass (DOF_FP *g, int numpass)
{
    int i;
	
    // First pass (horizontal blur)      
    TEX_Bind(g->tid_small);
    TEX_SetFilter(GL_NEAREST, GL_NEAREST);
    TEX_SetWrap(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
         
    glUseProgramObjectARB( g->programObj );       

    glUniform1fARB( g->loc_deltax, (1.0f / (float)(g->r2tsizex) ));
    glUniform1fARB( g->loc_deltay, 0.0 );
    glUniform1fARB (g->loc_iteration, (float) numpass);    	
    for(i=0;i<8;i++) glUniform1iARB( g->loc_TexUnit[i], 0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_SRC_ALPHA);
    
    glBegin( GL_QUADS );
         glTexCoord2f(    0, 1.0f ); glVertex2f( 0.0f, 1.0f);
         glTexCoord2f(    0,    0 ); glVertex2f( 0.0f, 0.0f);
         glTexCoord2f( 1.0f,    0 ); glVertex2f( 1.0f, 0.0f);
         glTexCoord2f( 1.0f, 1.0f ); glVertex2f( 1.0f, 1.0f);            
    glEnd();

    // Second pass (vertical) Now we start using the small texture

    glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE );
    glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->r2tsizex,g->r2tsizey);
	
    glUniform1fARB( g->loc_deltay, (1.0f / (float)(g->r2tsizey) ));
    glUniform1fARB( g->loc_deltax, 0.0 );
               
    glBegin( GL_QUADS );
         glTexCoord2f(    0, 1.0f ); glVertex2f( 0.0f, 1.0f);
         glTexCoord2f(    0,    0 ); glVertex2f( 0.0f, 0.0f);
         glTexCoord2f( 1.0f,    0 ); glVertex2f( 1.0f, 0.0f);
         glTexCoord2f( 1.0f, 1.0f ); glVertex2f( 1.0f, 1.0f);
    glEnd();    

    glUseProgramObjectARB( NULL );

    // Get the texture back
    glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->r2tsizex,g->r2tsizey);
    glDisable(GL_BLEND);
}

// El algoritmo es sencillo:
// 1- Renderizar a la textura grande (1024x1024) el contenido del viewport
// 2- Poner un viewport de 256x256 
// 3- Renderizar un quad a todo viewport usando la textura grande, haciendo blur mediante FP
// 4- Coger ese viewport en la textura pequeña (256x256)
// 5- Usar esa textura para las zonas con DOF, y la grande como textura fullscreen


void DOF_FP_DoFrame( DOF_FP *g )
{
    long    i;

    
    // disable
    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glDisable( GL_NORMALIZE );
    glDisable( GL_BLEND );
    glDisable( GL_TEXTURE_2D );
    glShadeModel( GL_FLAT );

    // 1. copiar a textura grande desde el color buffer
    glEnable( GL_TEXTURE_2D );
    TEX_Bind(g->tid_screen);
    glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE );
    TEX_SetEnvironment (GL_MODULATE);
       
    int viewport_actual[4];
    glGetIntegerv(GL_VIEWPORT,viewport_actual);

   
    if(viewport_actual[2] < g->xres || viewport_actual[3] < g->yres)
    {
 	g->ox = (float)viewport_actual[2]/1024.0f;
    	g->oy = (float)viewport_actual[3]/1024.0f;
	glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,viewport_actual[0],viewport_actual[1],viewport_actual[2],viewport_actual[3] );
    }
    else
    {   
     	glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->xres,g->yres );
 	g->ox = (float)g->xres/1024.0f;
    	g->oy = (float)g->yres/1024.0f;   
    }
    
    
    
    // Copiar la depth texture
    if(g->algorithm==0)
    {
    	glBindTexture( GL_TEXTURE_2D, g->tid_depth );
    	//glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE );    
    	if(viewport_actual[2] < g->xres || viewport_actual[3] < g->yres)
    	{
		glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,viewport_actual[0],viewport_actual[1],viewport_actual[2],viewport_actual[3] );
    	}
    	else
    	{   
	     	glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->xres,g->yres );
    	}	
    }

    // 2. Viewport para render a texture, proyección ortogonal
    glViewport (0, 0, g->r2tsizex, g->r2tsizey);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluOrtho2D(0.0,1.0,0.0,1.0);

    glDepthMask(GL_FALSE);   
    glDisable(GL_DEPTH_TEST);
    
    // 3. Activar shader y render de quad a pantalla completa
    DOF_FP_InitialPass (g);    
    for(i=0;i<g->blur_iterations;i++) DOF_FP_SecondPass(g,i);
    

    // 5. Pintar todos los planos
    glViewport (viewport_actual[0], viewport_actual[1], viewport_actual[2], viewport_actual[3]);
    
    glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable( GL_TEXTURE_2D );
      TEX_Bind(g->tid_screen);
      TEX_SetFilter(GL_NEAREST, GL_NEAREST);
    glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable( GL_TEXTURE_2D );
      TEX_Bind(g->tid_small);
      TEX_SetFilter(GL_LINEAR, GL_LINEAR);


    if(g->algorithm==0)
    {
      glActiveTextureARB(GL_TEXTURE2_ARB);
      glEnable( GL_TEXTURE_2D );
      glBindTexture( GL_TEXTURE_2D, g->tid_depth );
      glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_ALPHA);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB,GL_NONE);
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    	glUseProgramObjectARB( g->mix_programObj );       

    	glUniform1fARB( g->loc_mix_focus, g->focus);
    	glUniform1fARB( g->loc_mix_range, g->range);
    	glUniform1iARB( g->loc_mix_TexUnit0, 0);
    	glUniform1iARB( g->loc_mix_TexUnit1, 1);
    	glUniform1iARB( g->loc_mix_TexUnit2, 2);
    
     
    	glBegin( GL_QUADS );
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0, g->oy);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0, 1.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,0, g->oy); glVertex2f( 0.0f, 1.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,0, 0); glVertex2f( 0.0f, 0.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,g->ox, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1.0f, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,g->ox, 0);glVertex2f( 1.0f, 0.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,g->ox, g->oy);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1.0f, 1.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,g->ox, g->oy);glVertex2f( 1.0f, 1.0f);
    	glEnd();
    }
    else if(g->algorithm == 1)
    {
    	glUseProgramObjectARB( g->mixalpha_programObj );       

    	glUniform1fARB( g->loc_mixalpha_focus, g->focus);
    	glUniform1fARB( g->loc_mixalpha_range, g->range);
    	glUniform1iARB( g->loc_mixalpha_TexUnit0, 0);
    	glUniform1iARB( g->loc_mixalpha_TexUnit1, 1);    
     
    	glBegin( GL_QUADS );
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0, g->oy);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0, 1.0f);
    	    glVertex2f( 0.0f, 1.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0, 0);
    	    glVertex2f( 0.0f, 0.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,g->ox, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1.0f, 0);
    	    glVertex2f( 1.0f, 0.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,g->ox, g->oy);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1.0f, 1.0f);
    	    glVertex2f( 1.0f, 1.0f);
    	glEnd();    	
    }
    else if(g->algorithm == 2)
    {
      glActiveTextureARB(GL_TEXTURE2_ARB);
      glEnable( GL_TEXTURE_2D );
      if(g->shared_depth_texture)  TEX_Bind(g->shared_depth_texture);
      TEX_SetFilter(GL_NEAREST, GL_NEAREST);

      glUseProgramObjectARB( g->mixshared_programObj );       
      glUniform1fARB( g->loc_mixshared_focus, g->focus);
      glUniform1fARB( g->loc_mixshared_range, g->range);
      glUniform1iARB( g->loc_mixshared_TexUnit0, 0);
      glUniform1iARB( g->loc_mixshared_TexUnit1, 1);
      glUniform1iARB( g->loc_mixshared_TexUnit2, 2);
    
     
      glBegin( GL_QUADS );
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0, g->oy);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0, 1.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,0, g->oy); glVertex2f( 0.0f, 1.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,0, 0); glVertex2f( 0.0f, 0.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,g->ox, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1.0f, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,g->ox, 0);glVertex2f( 1.0f, 0.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,g->ox, g->oy);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1.0f, 1.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,g->ox, g->oy);glVertex2f( 1.0f, 1.0f);
      glEnd();
    }

    // dejarlo todo bien...
    glUseProgramObjectARB( NULL );       
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);        
    glDisable( GL_BLEND );
    glDisable(GL_DEPTH_TEST);
    glShadeModel( GL_SMOOTH );
    glDepthMask(GL_TRUE);
}

void DOF_FP_SetSharedDepthTexture(DOF_FP *g, char *texname)
{
	 g->shared_depth_texture = CACHE_LoadTexture(texname,0);	
}