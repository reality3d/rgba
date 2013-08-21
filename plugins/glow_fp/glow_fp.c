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
#include "glow_fp.h"
#ifdef LINUX
#include <GL/glx.h>
#define wglGetProcAddress glXGetProcAddressARB
#endif

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



static char *CUT_VS="\
void main(void) \
{ \
 gl_TexCoord[0] = gl_MultiTexCoord0; \
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
}";

static char *CUT_FS="\
uniform sampler2D TexUnit0;\
uniform vec4 resta;\
uniform vec4 producto;\
void main(void)\
{\
 vec4 value;\
 value  = texture2D(TexUnit0, vec2(gl_TexCoord[0]));\
 gl_FragColor = clamp((value - resta),0.0,1.0) * producto;\
}";

static char *CUTALPHA_FS="\
uniform sampler2D TexUnit0;\
uniform float cut_alpha;\
void main(void)\
{\
 vec4 value;\
 value  = texture2D(TexUnit0, vec2(gl_TexCoord[0]));\
 if(value.a > cut_alpha) gl_FragColor = value;\
  else gl_FragColor = vec4(0.0,0.0,0.0,1.0);\
}";

static char *ADD_VS="\
void main(void) \
{ \
 gl_TexCoord[0] = gl_MultiTexCoord0; \
 gl_TexCoord[1] = gl_MultiTexCoord1; \
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
}";

// TODO: optimizar este shader. Ahora mismo es más efectivo hacerlo en dos pasadas
//       que en una sola (por lo del addormultiply!)

static char *ADD_FS="\
uniform sampler2D TexUnit0;\
uniform sampler2D TexUnit1;\
uniform vec4 producto;\
uniform float addormultiply;\
void main(void)\
{\
 vec4 pantalla=texture2D(TexUnit0, vec2(gl_TexCoord[0]));\
 vec4 blur=texture2D(TexUnit1, vec2(gl_TexCoord[1]))*producto;\
 gl_FragColor = pantalla*blur*addormultiply + ((pantalla+blur)*(1.0-addormultiply));\
}";


static char *ADDGLT_VS="\
void main(void) \
{ \
 gl_TexCoord[0] = gl_MultiTexCoord0; \
 gl_TexCoord[1] = gl_MultiTexCoord1; \
 gl_TexCoord[2] = gl_MultiTexCoord2; \
 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
}";

// TODO: optimizar este shader. Ahora mismo es más efectivo hacerlo en dos pasadas
//       que en una sola (por lo del addormultiply!)
// Coge sólo la componente r de la textura shared, para facilitarle la vida a r3d

static char *ADDGLT_FS="\
uniform sampler2D TexUnit0;\
uniform sampler2D TexUnit1;\
uniform sampler2D TexUnit2;\
uniform vec4 producto;\
uniform float addormultiply;\
void main(void)\
{\
 vec4 shared=texture2D(TexUnit2, vec2(gl_TexCoord[2]));\
 vec4 pantalla=texture2D(TexUnit0, vec2(gl_TexCoord[0]));\
 vec4 blur=texture2D(TexUnit1, vec2(gl_TexCoord[1]))*producto*shared.rrrr;\
 gl_FragColor = pantalla*blur*addormultiply + ((pantalla+blur)*(1.0-addormultiply));\
}";



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
static int GLOW_FP_InitShaders(GLOW_FP *g)
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
	printf( "glow_fp.epl:Error loading vertex shader\n" );
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
	printf( "glow_fp.epl:Error loading fragment shader\n" );
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
	printf( "glow_fp.epl:Error linking shader\n" );
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

 // Create second shader
 g->cut_vertexShader = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
 // Compile shader
 glShaderSourceARB( g->cut_vertexShader, 1, (const char **)&CUT_VS, NULL );
 glCompileShaderARB( g->cut_vertexShader);
 // Check for errors
 glGetObjectParameterivARB( g->cut_vertexShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->cut_vertexShader, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error loading vertex shader\n" );
	printf(errorString);	
	return 1;
 }

 // Create fragment shader
 g->cut_fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
 // Compile fragment shader
 glShaderSourceARB( g->cut_fragmentShader, 1, (const char **) &CUT_FS, NULL );
 glCompileShaderARB( g->cut_fragmentShader );
 glGetObjectParameterivARB( g->cut_fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->cut_fragmentShader, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error loading fragment shader\n" );
	printf(errorString);	
	return 1;
 }


 // Create a program object with the shaders 
 g->cut_programObj = glCreateProgramObjectARB();
 glAttachObjectARB( g->cut_programObj, g->cut_vertexShader );
 glAttachObjectARB( g->cut_programObj, g->cut_fragmentShader );
 // Link and check for errors
 glLinkProgramARB( g->cut_programObj );
 glGetObjectParameterivARB( g->cut_programObj, GL_OBJECT_LINK_STATUS_ARB, &wasok); 

 if( !wasok )
 {
	glGetInfoLogARB(g->cut_programObj, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error linking shader\n" );
	printf(errorString);	
	return 1;
 }

 // Last thing left is to get some parameters by name...
 
 g->cut_loc_TexUnit0 = glGetUniformLocationARB (g->cut_programObj,"TexUnit0");
 g->cut_loc_resta = glGetUniformLocationARB (g->cut_programObj,"resta");
 g->cut_loc_producto = glGetUniformLocationARB (g->cut_programObj,"producto");


 // Create third shader
 //g->cutalpha_vertexShader = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
 // Compile shader
 //glShaderSourceARB( g->cutalpha_vertexShader, 1, (const char **)&CUT_VS, NULL );
 //glCompileShaderARB( g->cutalpha_vertexShader);
 // Check for errors
 //glGetObjectParameterivARB( g->cutalpha_vertexShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 //if( !wasok )
 //{
//	glGetInfoLogARB(g->cutalpha_vertexShader, sizeof(errorString), NULL, errorString);
//	printf( "glow_fp.epl:Error loading vertex shader\n" );
//	printf(errorString);	
//	return 1;
// }

 // Create fragment shader
 g->cutalpha_fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
 // Compile fragment shader
 glShaderSourceARB( g->cutalpha_fragmentShader, 1, (const char **) &CUTALPHA_FS, NULL );
 glCompileShaderARB( g->cutalpha_fragmentShader );
 glGetObjectParameterivARB( g->cutalpha_fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->cutalpha_fragmentShader, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error loading fragment shader\n" );
	printf(errorString);	
	return 1;
 }


 // Create a program object with the shaders 
 g->cutalpha_programObj = glCreateProgramObjectARB();
 glAttachObjectARB( g->cutalpha_programObj, g->cut_vertexShader );
 glAttachObjectARB( g->cutalpha_programObj, g->cutalpha_fragmentShader );
 // Link and check for errors
 glLinkProgramARB( g->cutalpha_programObj );
 glGetObjectParameterivARB( g->cutalpha_programObj, GL_OBJECT_LINK_STATUS_ARB, &wasok); 

 if( !wasok )
 {
	glGetInfoLogARB(g->cutalpha_programObj, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error linking shader\n" );
	printf(errorString);	
	return 1;
 }

 // Last thing left is to get some parameters by name...
 
 g->cutalpha_loc_TexUnit0 = glGetUniformLocationARB (g->cutalpha_programObj,"TexUnit0");
 g->cutalpha_loc_cut_alpha= glGetUniformLocationARB (g->cut_programObj,"cut_alpha");
 
 // Create fourth shader
 g->add_vertexShader = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
 // Compile shader
 glShaderSourceARB( g->add_vertexShader, 1, (const char **)&ADD_VS, NULL );
 glCompileShaderARB( g->add_vertexShader);
 // Check for errors
 glGetObjectParameterivARB( g->add_vertexShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->add_vertexShader, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error loading vertex shader\n" );
	printf(errorString);	
	return 1;
 }

 // Create fragment shader
 g->add_fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
 // Compile fragment shader
 glShaderSourceARB( g->add_fragmentShader, 1, (const char **) &ADD_FS, NULL );
 glCompileShaderARB( g->add_fragmentShader );
 glGetObjectParameterivARB( g->add_fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->add_fragmentShader, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error loading fragment shader\n" );
	printf(errorString);	
	return 1;
 }


 // Create a program object with the shaders 
 g->add_programObj = glCreateProgramObjectARB();
 glAttachObjectARB( g->add_programObj, g->add_vertexShader );
 glAttachObjectARB( g->add_programObj, g->add_fragmentShader );
 // Link and check for errors
 glLinkProgramARB( g->add_programObj );
 glGetObjectParameterivARB( g->add_programObj, GL_OBJECT_LINK_STATUS_ARB, &wasok); 

 if( !wasok )
 {
	glGetInfoLogARB(g->add_programObj, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error linking shader\n" );
	printf(errorString);	
	return 1;
 }

 // Last thing left is to get some parameters by name...
 
 g->add_loc_TexUnit0 = glGetUniformLocationARB (g->add_programObj,"TexUnit0");
 g->add_loc_TexUnit1 = glGetUniformLocationARB (g->add_programObj,"TexUnit1");
 g->add_loc_producto = glGetUniformLocationARB (g->add_programObj,"producto");
 g->add_loc_addormultiply = glGetUniformLocationARB (g->add_programObj,"addormultiply");

 // Create fifthshader
 g->addGLT_vertexShader = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
 // Compile shader
 glShaderSourceARB( g->addGLT_vertexShader, 1, (const char **)&ADDGLT_VS, NULL );
 glCompileShaderARB( g->addGLT_vertexShader);
 // Check for errors
 glGetObjectParameterivARB( g->addGLT_vertexShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->addGLT_vertexShader, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error loading vertex shader\n" );
	printf(errorString);	
	return 1;
 }

 // Create fragment shader
 g->addGLT_fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
 // Compile fragment shader
 glShaderSourceARB( g->addGLT_fragmentShader, 1, (const char **) &ADDGLT_FS, NULL );
 glCompileShaderARB( g->addGLT_fragmentShader );
 glGetObjectParameterivARB( g->addGLT_fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->addGLT_fragmentShader, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error loading fragment shader\n" );
	printf(errorString);	
	return 1;
 }


 // Create a program object with the shaders 
 g->addGLT_programObj = glCreateProgramObjectARB();
 glAttachObjectARB( g->addGLT_programObj, g->addGLT_vertexShader );
 glAttachObjectARB( g->addGLT_programObj, g->addGLT_fragmentShader );
 // Link and check for errors
 glLinkProgramARB( g->addGLT_programObj );
 glGetObjectParameterivARB( g->addGLT_programObj, GL_OBJECT_LINK_STATUS_ARB, &wasok); 

 if( !wasok )
 {
	glGetInfoLogARB(g->addGLT_programObj, sizeof(errorString), NULL, errorString);
	printf( "glow_fp.epl:Error linking shader\n" );
	printf(errorString);	
	return 1;
 }

 // Last thing left is to get some parameters by name...
 
 g->addGLT_loc_TexUnit0 = glGetUniformLocationARB (g->addGLT_programObj,"TexUnit0");
 g->addGLT_loc_TexUnit1 = glGetUniformLocationARB (g->addGLT_programObj,"TexUnit1");
 g->addGLT_loc_TexUnit2 = glGetUniformLocationARB (g->addGLT_programObj,"TexUnit2");
 g->addGLT_loc_producto = glGetUniformLocationARB (g->addGLT_programObj,"producto");
 g->addGLT_loc_addormultiply = glGetUniformLocationARB (g->addGLT_programObj,"addormultiply");
 
 return 0; // Everything went ok
}
//----------------------------------------------------------------------------

GLOW_FP *GLOW_FP_Init( int xres, int yres, int r2tsizex, int r2tsizey )
{
    GLOW_FP            *g;

    while(r2tsizex > Epopeia_GetResX())
        r2tsizex /= 2;
    while(r2tsizey > Epopeia_GetResY())
        r2tsizey /= 2;


    g = (GLOW_FP*)malloc( sizeof(GLOW_FP) );
    if( !g )
        return( 0 );
    
    if( GLExtSupported("GL_ARB_shading_language_100") &&
        GLExtSupported("GL_ARB_shader_objects")&&
        GLExtSupported("GL_ARB_fragment_shader"))        
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
			!glMultiTexCoord2fARB || (ncoords <8))
         {
         #ifdef DEBUG
            printf( "glow_fp.epl:This card does not support GLslang extensions\n" );
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
	 g->cut_resta[0] = g->cut_resta[1] = g->cut_resta[2] =  0.8f;
	 g->cut_resta[3] = 0.0f;
	 g->producto[0] = g->producto[1] = g->producto[2] = 2.0f;
	 g->producto[3] = 1.0f;
	 g->cut_alpha = 0.5f;
	 g->algorithm=0;
	 g->blur_iterations=1;
	 g->add_or_multiply=0; // add
	 g->r2tsizex = r2tsizex;
	 g->r2tsizey = r2tsizey;
	 g->overbright_pre_or_post = 0; // ANTES del blur
	 

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
	 	 	
        }
    	else
        {
        #ifdef DEBUG
            printf( "glow_fp.epl:This card does not support GLslang extensions\n" );
            return( 0 );
        #else
            return( 0 );
        #endif
        }

    g->tid_shared=NULL;
    if(!GLOW_FP_InitShaders(g)) return( g );
     else return 0;
}


void GLOW_FP_End( GLOW_FP *g )
{
    if (g->tid_screen)
        CACHE_FreeTexture (g->tid_screen);    
    if(g->tid_small) TEX_Delete(g->tid_small );
    if(g->tid_shared) CACHE_FreeTexture(g->tid_shared);
    free( g );
}


static void GLOW_FP_InitialPass (GLOW_FP *g)
{
    // Zero pass (Cut to get highlights)

    glEnable( GL_TEXTURE_2D );
    TEX_Bind( g->tid_screen );
    TEX_SetFilter(GL_LINEAR, GL_LINEAR);
    TEX_SetWrap(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
    
    if(g->algorithm == 0) // resta + overbright
    {
     glUseProgramObjectARB( g->cut_programObj );       
     glUniform1iARB( g->cut_loc_TexUnit0, 0);
     glUniform4fARB( g->cut_loc_resta, g->cut_resta[0],g->cut_resta[1],g->cut_resta[2],g->cut_resta[3]);
     if (g->overbright_pre_or_post)
     	glUniform4fARB( g->cut_loc_producto, 1.0f,1.0f,1.0f,1.0f);
     else
     	glUniform4fARB( g->cut_loc_producto, g->producto[0],g->producto[1],g->producto[2],g->producto[3]);
    }
    else if (g->algorithm == 1)// alpha test
    {
     glUseProgramObjectARB( g->cutalpha_programObj );       
     glUniform1iARB( g->cutalpha_loc_TexUnit0, 0);    	
     glUniform1fARB( g->cutalpha_loc_cut_alpha, g->cut_alpha);
    }
    else if (g->algorithm == 2) // GLT texture defining blend factor
    {
     // The same shader used for algorithm 0 will be useful here, just substract 0 and go :)
     glUseProgramObjectARB( g->cut_programObj );       
     glUniform1iARB( g->cut_loc_TexUnit0, 0);
     glUniform4fARB( g->cut_loc_resta, 0.0f,0.0f,0.0f,0.0f);
     if (g->overbright_pre_or_post)
     	glUniform4fARB( g->cut_loc_producto, 1.0f,1.0f,1.0f,1.0f);
     else
     	glUniform4fARB( g->cut_loc_producto, g->producto[0],g->producto[1],g->producto[2],g->producto[3]);    	    	
    }
    
    glDisable(GL_BLEND);
    glBegin( GL_QUADS );
            glTexCoord2f(     0, g->oy ); glVertex2f( 0.0f, 1.0f);
            glTexCoord2f(     0,     0 ); glVertex2f( 0.0f, 0.0f);
            glTexCoord2f( g->ox,     0 ); glVertex2f( 1.0f, 0.0f);
            glTexCoord2f( g->ox, g->oy ); glVertex2f( 1.0f, 1.0f);
    glEnd();

    glUseProgramObjectARB( NULL );

    // Get the texture
    TEX_Bind( g->tid_small );
    glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE );
    glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->r2tsizex,g->r2tsizey);       
}

static void GLOW_FP_SecondPass (GLOW_FP *g, int numpass)
{
    int i;

    // First pass (horizontal blur)    
    TEX_Bind(g->tid_small);
    TEX_SetFilter(GL_NEAREST, GL_NEAREST);
    TEX_SetWrap(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
    
    glUseProgramObjectARB( g->programObj );       

    glUniform1fARB( g->loc_deltax, (1.0f / (float)(g->r2tsizex)));
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
// 5- Usar esa textura para el glow aditivo


void GLOW_FP_DoFrame( GLOW_FP *g )
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

    // 2. Viewport pequeño, proyección ortogonal
    glViewport (0, 0, g->r2tsizex, g->r2tsizey);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluOrtho2D(0.0,1.0,0.0,1.0);

    glDepthMask(GL_FALSE);   
    glDisable(GL_DEPTH_TEST);
    
    // 3. Activar shader y render de quad a pantalla completa
    GLOW_FP_InitialPass (g);
    
    for(i=0;i<g->blur_iterations;i++) GLOW_FP_SecondPass(g,i);

    // 5. Pintar todos los planos
    // Añadimos la textura de glow
    glViewport (viewport_actual[0], viewport_actual[1], viewport_actual[2], viewport_actual[3]);

    glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable( GL_TEXTURE_2D );
      TEX_Bind(g->tid_screen);
      TEX_SetFilter(GL_NEAREST, GL_NEAREST);
    glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable( GL_TEXTURE_2D );
      TEX_Bind(g->tid_small);
      TEX_SetFilter(GL_LINEAR, GL_LINEAR);
      TEX_SetWrap(GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE);
      
    if(g->algorithm == 2)
    {      
    	glActiveTextureARB(GL_TEXTURE2_ARB);
      	glEnable( GL_TEXTURE_2D );
      	TEX_Bind(g->tid_shared);
      	TEX_SetFilter(GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST);

    	glUseProgramObjectARB( g->addGLT_programObj );         
    	glUniform1iARB( g->addGLT_loc_TexUnit0, 0);
    	glUniform1iARB( g->addGLT_loc_TexUnit1, 1);
    	glUniform1iARB( g->addGLT_loc_TexUnit2, 2);
    	glUniform1fARB( g->addGLT_loc_addormultiply,(float)g->add_or_multiply);
    
    	if(g->overbright_pre_or_post)
     		glUniform4fARB( g->addGLT_loc_producto, g->producto[0],g->producto[1],g->producto[2],g->producto[3]);
    	else 
     		glUniform4fARB( g->addGLT_loc_producto, 1.0f,1.0f,1.0f,1.0f);              	    	

    	glBegin( GL_QUADS );
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0, g->oy);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0, 1.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,0, 0);
    	    glVertex2f( 0.0f, 1.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,0, 1.0f);
    	    glVertex2f( 0.0f, 0.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,g->ox, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1.0f, 0);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,1.0f, 1.0f);
    	    glVertex2f( 1.0f, 0.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE0_ARB,g->ox, g->oy);
    	    glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1.0f, 1.0f);
    	    glMultiTexCoord2fARB(GL_TEXTURE2_ARB,1.0f, 0);
    	    glVertex2f( 1.0f, 1.0f);
    	glEnd();


    }
    else
    {
    	glUseProgramObjectARB( g->add_programObj );         
    	glUniform1iARB( g->add_loc_TexUnit0, 0);
    	glUniform1iARB( g->add_loc_TexUnit1, 1);
    	glUniform1fARB( g->add_loc_addormultiply,(float)g->add_or_multiply);
    
    	if(g->overbright_pre_or_post)
     		glUniform4fARB( g->add_loc_producto, g->producto[0],g->producto[1],g->producto[2],g->producto[3]);
    	else 
     		glUniform4fARB( g->add_loc_producto, 1.0f,1.0f,1.0f,1.0f);              	    	

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
    
    glUseProgramObjectARB( NULL );       

    // dejarlo todo bien...
    if(g->algorithm == 2)
    {      
    	glActiveTextureARB(GL_TEXTURE2_ARB);
    	glDisable(GL_TEXTURE_2D);    
    }
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_2D);        
    glDisable( GL_BLEND );
    glDisable(GL_DEPTH_TEST);
    glShadeModel( GL_SMOOTH );
    glDepthMask(GL_TRUE);
}

void GLOW_FP_SetSharedGLTTexture(GLOW_FP *g, char *texname)
{
	 g->tid_shared = CACHE_LoadTexture(texname,0);	
}