#define DEBUG
#define BOXBLUR 0
#define GENBLUR 1

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
#include "blurfp.h"

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
uniform float coeff0; \
uniform float coeff1; \
uniform float coeff2; \
uniform float coeff3; \
uniform float coeff4; \
uniform float coeff5; \
uniform float coeff6; \
uniform float coeff7; \
uniform float coeff8; \
void main(void)\
{ \
 vec4 value;\
 value  = texture2D(TexUnit0, vec2(gl_TexCoord[0])) * coeff1;\
 value += texture2D(TexUnit1, vec2(gl_TexCoord[1])) * coeff2;\
 value += texture2D(TexUnit2, vec2(gl_TexCoord[2])) * coeff3;\
 value += texture2D(TexUnit3, vec2(gl_TexCoord[3])) * coeff4;\
 value += texture2D(TexUnit4, vec2(gl_TexCoord[4])) * coeff5;\
 value += texture2D(TexUnit5, vec2(gl_TexCoord[5])) * coeff6;\
 value += texture2D(TexUnit6, vec2(gl_TexCoord[6])) * coeff7;\
 value += texture2D(TexUnit7, vec2(gl_TexCoord[7])) * coeff8;\
 value.w = coeff0;\
 gl_FragColor = value;\
}";

static char *BOXBLUR_FS="\
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
int BlurFP_InitShaders(BlurFP *g)
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
	printf( "blurfp.epl:Error loading vertex shader\n" );
	printf(errorString);	
	return 1;
 }

 // Create fragment shader
 g->fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
 // Compile fragment shader
 if(g->type == BOXBLUR)
  glShaderSourceARB( g->fragmentShader, 1, (const char **) &BOXBLUR_FS, NULL );
 else
  glShaderSourceARB( g->fragmentShader, 1, (const char **) &BLUR_FS, NULL );
  
 glCompileShaderARB( g->fragmentShader );
 glGetObjectParameterivARB( g->fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
 if( !wasok )
 {
	glGetInfoLogARB(g->fragmentShader, sizeof(errorString), NULL, errorString);
	printf( "blurfp.epl:Error loading fragment shader\n" );
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
	printf( "blurfp.epl:Error linking shader\n" );
	printf(errorString);	
	return 1;
 }

 // Last thing left is to get some parameters by name...
 
 g->loc_deltax = glGetUniformLocationARB (g->programObj,"deltax");
 g->loc_deltay= glGetUniformLocationARB (g->programObj,"deltay");
 g->loc_TexUnit[0] = glGetUniformLocationARB (g->programObj,"TexUnit0");
 g->loc_TexUnit[1] = glGetUniformLocationARB (g->programObj,"TexUnit1");
 g->loc_TexUnit[2] = glGetUniformLocationARB (g->programObj,"TexUnit2");
 g->loc_TexUnit[3] = glGetUniformLocationARB (g->programObj,"TexUnit3");
 g->loc_TexUnit[4] = glGetUniformLocationARB (g->programObj,"TexUnit4");
 g->loc_TexUnit[5] = glGetUniformLocationARB (g->programObj,"TexUnit5");
 g->loc_TexUnit[6] = glGetUniformLocationARB (g->programObj,"TexUnit6");
 g->loc_TexUnit[7] = glGetUniformLocationARB (g->programObj,"TexUnit7");
 g->loc_iteration = glGetUniformLocationARB (g->programObj,"iteration");  
 
 if(g->type==GENBLUR)
 {
  g->loc_coeff[0] = glGetUniformLocationARB (g->programObj,"coeff0");
  g->loc_coeff[1] = glGetUniformLocationARB (g->programObj,"coeff1");
  g->loc_coeff[2] = glGetUniformLocationARB (g->programObj,"coeff2");
  g->loc_coeff[3] = glGetUniformLocationARB (g->programObj,"coeff3");
  g->loc_coeff[4] = glGetUniformLocationARB (g->programObj,"coeff4");
  g->loc_coeff[5] = glGetUniformLocationARB (g->programObj,"coeff5");
  g->loc_coeff[6] = glGetUniformLocationARB (g->programObj,"coeff6");  
  g->loc_coeff[7] = glGetUniformLocationARB (g->programObj,"coeff7");  
  g->loc_coeff[8] = glGetUniformLocationARB (g->programObj,"coeff8");   
 }


 return 0; // Everything went ok
}

//----------------------------------------------------------------------------

BlurFP *BlurFP_Init( int type, int r2tsize, int xres, int yres )
{
    BlurFP *g;
    unsigned char   *text;

    g = (BlurFP*)malloc( sizeof(BlurFP) );
    if( !g )
        return( 0 );

    g->type = type;
    g->small_tex_size = r2tsize;   

    if( GLExtSupported("GL_ARB_shading_language_100") &&
        GLExtSupported("GL_ARB_shader_objects") &&
        GLExtSupported("GL_ARB_fragment_shader")
        )
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
			!glUniform1iARB ||!glUniform1fARB ||(ncoords < 8))
         {
         #ifdef DEBUG
            printf( "blurfp.epl:This card does not support GLslang extensions\n" );
            return( 0 );
         #else
            return( 0 );
         #endif
         }	

 	 // Texture creation and so...
 	 // This should be changed by some calls to GLT, or maybe use rectangle texture
 	 // extensions...
        	
         g->xres = xres;
         g->yres = yres;
         g->ox = (float)xres/1024.0f;
         g->oy = (float)yres/1024.0f;

         glGenTextures( 1, &g->tid );
    
         if( g->tid<1 )
            return( 0 );

         text = (unsigned char*)malloc( 1024*1024*4 );
         if( !text )
         {
			glDeleteTextures( 1, &g->tid );
			return( 0 );
         }

         glEnable( GL_TEXTURE_2D );
         glBindTexture( GL_TEXTURE_2D, g->tid );

         glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
         glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
         glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE );
         glTexImage2D (GL_TEXTURE_2D,0,GL_RGBA8,1024,1024,0,GL_RGBA,GL_UNSIGNED_BYTE,text);


	 if(r2tsize)
	 {
          glGenTextures( 1, &g->tid_small);   
          if( g->tid_small<1 )
            return( 0 );

          glEnable( GL_TEXTURE_2D );
          glBindTexture( GL_TEXTURE_2D, g->tid_small);

          glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
          glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE );
          glTexImage2D (GL_TEXTURE_2D,0,GL_RGBA8,r2tsize,r2tsize,0,GL_RGBA,GL_UNSIGNED_BYTE,text);
	 }
	 else
	   g->tid_small=0;

	 free( text );	
        }
    else
        {
        #ifdef DEBUG
		printf( "blurfp.epl:This card does not support GLslang extensions\n" );
		return( 0 );
        #else
            return( 0 );
        #endif
        }

    g->iterations = 1; // By default we just do it once
    g->coeff[0] = 0.1111f; 	// Default coeffs (Box blur)
    g->coeff[1] = 0.1111f; 
    g->coeff[2] = 0.1111f; 
    g->coeff[3] = 0.1111f; 
    g->coeff[4] = 0.1111f; 
    g->coeff[5] = 0.1111f; 
    g->coeff[6] = 0.1111f; 
    g->coeff[7] = 0.1111f; 
    g->coeff[8] = 0.1111f; 

    if (!BlurFP_InitShaders(g)) return( g );
     else return NULL;
}


void BlurFP_End( BlurFP *g )
{
    glDeleteTextures( 1, &g->tid );
    if(g->tid_small > 0) glDeleteTextures( 1, &g->tid_small );
    free( g );
}

static void BlurFP_InitialPass (BlurFP *g)
{
    int i;
	
    // First pass   
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, g->tid );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    	    		
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

  
    glDisable(GL_BLEND);  
           
    glBegin( GL_QUADS );
            glTexCoord2f(     0, g->oy ); glVertex2f( 0.0f, 1.0f);
            glTexCoord2f(     0,     0 ); glVertex2f( 0.0f, 0.0f);
            glTexCoord2f( g->ox,     0 ); glVertex2f( 1.0f, 0.0f);
            glTexCoord2f( g->ox, g->oy ); glVertex2f( 1.0f, 1.0f);
    glEnd();

    // Copy to the small texture
    glBindTexture( GL_TEXTURE_2D, g->tid_small );
    glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE );
    glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->small_tex_size,g->small_tex_size);    
}

static void BlurFP_SecondPass (BlurFP *g, int numpass)
{
    int i;

    // First pass (horizontal blur)    
    glBindTexture( GL_TEXTURE_2D, g->tid_small );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUseProgramObjectARB( g->programObj );       

    glUniform1fARB( g->loc_deltax, 1.0f / (float)g->small_tex_size );
    glUniform1fARB( g->loc_deltay, 0.0 );
    for(i=0;i<8;i++) glUniform1iARB( g->loc_TexUnit[i], 0);
    glUniform1fARB (g->loc_iteration, (float) numpass);    	
    if(g->type == GENBLUR)
    {
     for(i=0;i<9;i++) glUniform1fARB( g->loc_coeff[i], g->coeff[i]);
    }
    
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
    glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->small_tex_size,g->small_tex_size);
	
    glUniform1fARB( g->loc_deltay, 1.0f / (float)g->small_tex_size );
    glUniform1fARB( g->loc_deltax, 0.0 );
               
    glBegin( GL_QUADS );
         glTexCoord2f(    0, 1.0f ); glVertex2f( 0.0f, 1.0f);
         glTexCoord2f(    0,    0 ); glVertex2f( 0.0f, 0.0f);
         glTexCoord2f( 1.0f,    0 ); glVertex2f( 1.0f, 0.0f);
         glTexCoord2f( 1.0f, 1.0f ); glVertex2f( 1.0f, 1.0f);
    glEnd();    

    glUseProgramObjectARB( NULL );

    // Get the texture back
    glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->small_tex_size,g->small_tex_size);	
}




void BlurFP_SinglePass(BlurFP *g, int viewport[4])
{
 int i,k ;
 
 for(k=0;k < g->iterations; k++)
 {           
    	
    // copiar a textura desde el color buffer
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, g->tid );
    glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    	   
    if(viewport[2] < g->xres || viewport[3] < g->yres)
    {
 	g->ox = (float)viewport[2]/1024.0f;
    	g->oy = (float)viewport[3]/1024.0f;
	glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,viewport[0],viewport[1],viewport[2],viewport[3] );
    }
    else
    {   
    	glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->xres,g->yres );
 	g->ox = (float)g->xres/1024.0f;
    	g->oy = (float)g->yres/1024.0f;   
    }
           
    // First pass (horizontal blur)    
    glUseProgramObjectARB( g->programObj );       

    glUniform1fARB( g->loc_deltax, 1.0f / 1024.0f );
    glUniform1fARB( g->loc_deltay, 0.0 );
    glUniform1fARB (g->loc_iteration, (float)(k));
    
    for(i=0;i<8;i++) glUniform1iARB( g->loc_TexUnit[i], 0);        	
    if(g->type == GENBLUR)
    {
     for(i=0;i<9;i++) glUniform1fARB( g->loc_coeff[i], g->coeff[i]);
    }
    	
    		
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_SRC_ALPHA);
	    
    glBegin( GL_QUADS );
        glTexCoord2f(     0, g->oy ); glVertex2f( 0.0f, 1.0f);
        glTexCoord2f(     0,     0 ); glVertex2f( 0.0f, 0.0f);
        glTexCoord2f( g->ox,     0 ); glVertex2f( 1.0f, 0.0f);
        glTexCoord2f( g->ox, g->oy ); glVertex2f( 1.0f, 1.0f);
    glEnd();

    // Render to texture a second time...
       
    if(viewport[2] < g->xres || viewport[3] < g->yres)
    {
 	g->ox = (float)viewport[2]/1024.0f;
    	g->oy = (float)viewport[3]/1024.0f;
	glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,viewport[0],viewport[1],viewport[2],viewport[3] );
    }
    else
    {   
     	glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->xres,g->yres );
 	g->ox = (float)g->xres/1024.0f;
    	g->oy = (float)g->yres/1024.0f;   
    }

    // Second pass (vertical blur)
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glUniform1fARB( g->loc_deltay, 1.0f / 1024.0f );
    glUniform1fARB( g->loc_deltax, 0.0 );
               
    glBegin( GL_QUADS );
         glTexCoord2f(     0, g->oy ); glVertex2f( 0.0f, 1.0f);
         glTexCoord2f(     0,     0 ); glVertex2f( 0.0f, 0.0f);
         glTexCoord2f( g->ox,     0 ); glVertex2f( 1.0f, 0.0f);
         glTexCoord2f( g->ox, g->oy ); glVertex2f( 1.0f, 1.0f);
    glEnd();    

    glUseProgramObjectARB( NULL );
 }	
}

void BlurFP_BlurDoFrame( BlurFP *g )
{
    int    i;
    int viewport[4];


    glGetIntegerv(GL_VIEWPORT,viewport);

    // disable
    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glDisable( GL_NORMALIZE );
    glDisable( GL_BLEND );
    glDisable( GL_TEXTURE_2D );
    glShadeModel( GL_FLAT );

    // set perspective
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    gluOrtho2D(0.0,1.0,0.0,1.0);

    if(g->small_tex_size == 0) // Fullscreen blur
    {
    	BlurFP_SinglePass(g, viewport);    		    	
    }
    else // smallscreen blur
    {
    	// 1. Copy the fullscreen texture and set the small viewport
    	
      	glEnable( GL_TEXTURE_2D );
    	glBindTexture( GL_TEXTURE_2D, g->tid );
    	glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE );
    	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );    	   
	if(viewport[2] < g->xres || viewport[3] < g->yres)
    	{
 		g->ox = (float)viewport[2]/1024.0f;
    		g->oy = (float)viewport[3]/1024.0f;
		glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,viewport[0],viewport[1],viewport[2],viewport[3] );
    	}
    	else
    	{   
     		glCopyTexSubImage2D( GL_TEXTURE_2D, 0,0,0,0,0,g->xres,g->yres );
 		g->ox = (float)g->xres/1024.0f;
    		g->oy = (float)g->yres/1024.0f;   
    	}  	
    	glViewport(0,0,g->small_tex_size,g->small_tex_size);

	// 2. Perform blur passes
    	
    	BlurFP_InitialPass(g);  
  	
    	for(i=0;i<g->iterations;i++)
    	 BlurFP_SecondPass(g,i);
 
        // 3. Paint the small texture fullscreen
        glViewport (viewport[0], viewport[1], viewport[2], viewport[3]);

        glBindTexture( GL_TEXTURE_2D, g->tid_small);
    	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	glDisable(GL_BLEND);
        glBegin( GL_QUADS );
            glTexCoord2f(     0, 1 ); glVertex2f( 0.0f, 1.0f);
            glTexCoord2f(     0, 0 ); glVertex2f( 0.0f, 0.0f);
            glTexCoord2f(     1, 0 ); glVertex2f( 1.0f, 0.0f);
            glTexCoord2f(     1, 1 ); glVertex2f( 1.0f, 1.0f);
        glEnd();    	
    }
    

        
    // dejarlo todo bien...
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );
    glDisable(GL_DEPTH_TEST);
    glShadeModel( GL_SMOOTH );
    glDepthMask(GL_TRUE);
}
