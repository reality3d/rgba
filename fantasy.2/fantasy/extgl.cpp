#ifdef WIN32
 #include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glext.h>

#ifdef LINUX
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

#include <stdio.h>
#include <string.h>

#include "misc.h"

// These are all the boolean parameters showing which extensions are supported

unsigned char EXTGL_compiled_vertex_arrays=0;
unsigned char EXTGL_multitexturing=0;
unsigned char EXTGL_vertex_shaders=0;
int EXTGL_number_of_texture_units=1;
unsigned char EXTGL_blend_substract_supported=0;
unsigned char EXTGL_texture_env_combine_supported=0; 
unsigned char EXTGL_texture_env_dot3_supported=0; 
unsigned char EXTGL_texture_cube_map_supported=0; 
unsigned char EXTGL_vertex_buffer_objects=0;
unsigned char EXTGL_shading_language=0;

#define UNSUPPORTED_GFX_CARD 	0
#define PATH_DX7		1
#define PATH_DX8		2
#define PATH_DX9		3


unsigned char EXTGL_render_path = UNSUPPORTED_GFX_CARD;


// These are the functions

PFNGLLOCKARRAYSEXTPROC   	 	f_glLockArraysEXT= NULL;
PFNGLUNLOCKARRAYSEXTPROC 	 	f_glUnlockArraysEXT= NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC  	f_glClientActiveTextureARB= NULL;
PFNGLACTIVETEXTUREARBPROC   	 	f_glActiveTextureARB = NULL;
PFNGLGENPROGRAMSARBPROC          	f_glGenProgramsARB        = NULL;
PFNGLDELETEPROGRAMSARBPROC       	f_glDeleteProgramsARB     = NULL;
PFNGLBINDPROGRAMARBPROC          	f_glBindProgramARB        = NULL;
PFNGLPROGRAMSTRINGARBPROC         	f_glProgramStringARB      = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC 	f_glProgramEnvParameter4fARB = NULL;
PFNGLBLENDEQUATIONEXTPROC	  	f_glBlendEquationEXT = NULL;
PFNGLBINDBUFFERARBPROC           f_glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC        f_glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC           f_glGenBuffersARB = NULL;
PFNGLBUFFERDATAARBPROC           f_glBufferDataARB = NULL;
PFNGLMAPBUFFERARBPROC            f_glMapBufferARB = NULL;
PFNGLUNMAPBUFFERARBPROC          f_glUnmapBufferARB = NULL;
PFNGLVERTEXATTRIBPOINTERARBPROC  f_glVertexAttribPointerARB = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC  	f_glEnableVertexAttribArrayARB = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	f_glDisableVertexAttribArrayARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC	f_glProgramLocalParameter4fARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC  f_glCreateProgramObjectARB  = NULL;
PFNGLDELETEOBJECTARBPROC         f_glDeleteObjectARB         = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC     f_glUseProgramObjectARB     = NULL;
PFNGLCREATESHADEROBJECTARBPROC   f_glCreateShaderObjectARB   = NULL;
PFNGLSHADERSOURCEARBPROC         f_glShaderSourceARB         = NULL;
PFNGLCOMPILESHADERARBPROC        f_glCompileShaderARB        = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC f_glGetObjectParameterivARB = NULL;
PFNGLATTACHOBJECTARBPROC         f_glAttachObjectARB         = NULL;
PFNGLGETINFOLOGARBPROC           f_glGetInfoLogARB           = NULL;
PFNGLLINKPROGRAMARBPROC          f_glLinkProgramARB          = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC   f_glGetUniformLocationARB   = NULL;
PFNGLUNIFORM4FARBPROC            f_glUniform4fARB            = NULL;
PFNGLUNIFORM3FARBPROC            f_glUniform3fARB            = NULL;
PFNGLUNIFORM1IARBPROC            f_glUniform1iARB            = NULL;
PFNGLUNIFORM1FARBPROC            f_glUniform1fARB            = NULL;




// Taken straight from an nVidia document :)

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



// We will use this function to initialize those OpenGL extensions we need

void InitGLExtensions(void)
{
    EXTGL_compiled_vertex_arrays=0;
    EXTGL_multitexturing=0;
    EXTGL_vertex_shaders=0;
    
// First, the Compiled Vertex Array extension

    if(GLExtSupported("GL_EXT_compiled_vertex_array"))
    {

#ifdef WIN32
        f_glLockArraysEXT= (PFNGLLOCKARRAYSEXTPROC)wglGetProcAddress("glLockArraysEXT");
        f_glUnlockArraysEXT= (PFNGLUNLOCKARRAYSEXTPROC)wglGetProcAddress("glUnlockArraysEXT");
#endif
#ifdef LINUX
        f_glLockArraysEXT= (PFNGLLOCKARRAYSEXTPROC)glXGetProcAddressARB((const GLubyte *)"glLockArraysEXT");
	f_glUnlockArraysEXT= (PFNGLUNLOCKARRAYSEXTPROC)glXGetProcAddressARB((const GLubyte *)"glUnlockArraysEXT");
#endif
 	if(f_glLockArraysEXT == NULL  ||  f_glUnlockArraysEXT == NULL)
        {
            EXTGL_compiled_vertex_arrays = 0;
        }
        else
            EXTGL_compiled_vertex_arrays=1;
    }

    
// Then some others (multitexturing, texture_env_combine...)
    if(GLExtSupported("GL_ARB_multitexture"))
    {
#ifdef WIN32
        f_glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC)  wglGetProcAddress("glActiveTextureARB");
        f_glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
#endif
#ifdef LINUX
        f_glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC)  glXGetProcAddressARB((const GLubyte *)"glActiveTextureARB");
        f_glClientActiveTextureARB= (PFNGLCLIENTACTIVETEXTUREARBPROC)glXGetProcAddressARB((const GLubyte *)"glClientActiveTextureARB");
#endif
        
        if(f_glActiveTextureARB == NULL || f_glClientActiveTextureARB == NULL)
            EXTGL_multitexturing = 0;
        else
        {
            glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &EXTGL_number_of_texture_units);
            EXTGL_multitexturing = 1;
        }
    }

// Now, vertex shaders support

    if(GLExtSupported("GL_ARB_vertex_program"))
    {
    	#ifdef WIN32
	f_glGenProgramsARB           = (PFNGLGENPROGRAMSARBPROC)wglGetProcAddress("glGenProgramsARB");
	f_glDeleteProgramsARB        = (PFNGLDELETEPROGRAMSARBPROC)wglGetProcAddress("glDeleteProgramsARB");
	f_glBindProgramARB           = (PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB");
	f_glProgramStringARB         = (PFNGLPROGRAMSTRINGARBPROC)wglGetProcAddress("glProgramStringARB");
	f_glProgramEnvParameter4fARB = (PFNGLPROGRAMENVPARAMETER4FARBPROC)wglGetProcAddress("glProgramEnvParameter4fARB");
	f_glVertexAttribPointerARB   = (PFNGLVERTEXATTRIBPOINTERARBPROC) wglGetProcAddress("glVertexAttribPointerARB");
	f_glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC) wglGetProcAddress("glEnableVertexAttribArrayARB");
	f_glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) wglGetProcAddress("glDisableVertexAttribArrayARB");
	f_glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC) wglGetProcAddress("glProgramLocalParameter4fARB");	
	
	
	
	#endif
	#ifdef LINUX
	f_glGenProgramsARB           = (PFNGLGENPROGRAMSARBPROC)glXGetProcAddressARB((const GLubyte *)"glGenProgramsARB");
	f_glDeleteProgramsARB        = (PFNGLDELETEPROGRAMSARBPROC)glXGetProcAddressARB((const GLubyte *)"glDeleteProgramsARB");
	f_glBindProgramARB           = (PFNGLBINDPROGRAMARBPROC)glXGetProcAddressARB((const GLubyte *)"glBindProgramARB");
	f_glProgramStringARB         = (PFNGLPROGRAMSTRINGARBPROC)glXGetProcAddressARB((const GLubyte *)"glProgramStringARB");
        f_glProgramEnvParameter4fARB = (PFNGLPROGRAMENVPARAMETER4FARBPROC)glXGetProcAddressARB((const GLubyte *)"glProgramEnvParameter4fARB");
        // Enlar añadio ARB al final de los siguientes nombres de funcion. Sino da un unresolved en algunas ocasiones
	f_glVertexAttribPointerARB   = (PFNGLVERTEXATTRIBPOINTERARBPROC) glXGetProcAddressARB((const GLubyte *)"glVertexAttribPointerARB");
	f_glEnableVertexAttribArrayARB = (PFNGLENABLEVERTEXATTRIBARRAYARBPROC) glXGetProcAddressARB((const GLubyte *)"glEnableVertexAttribArrayARB");
	f_glDisableVertexAttribArrayARB = (PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) glXGetProcAddressARB((const GLubyte *)"glDisableVertexAttribArrayARB");
	f_glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC) glXGetProcAddressARB((const GLubyte *)"glProgramLocalParameter4fARB");

	#endif

	if( !f_glGenProgramsARB || !f_glDeleteProgramsARB || !f_glBindProgramARB || 
            !f_glProgramStringARB || !f_glProgramEnvParameter4fARB || !f_glVertexAttribPointerARB ||
            !f_glEnableVertexAttribArrayARB || !f_glDisableVertexAttribArrayARB || !f_glProgramLocalParameter4fARB)
	{
          EXTGL_vertex_shaders=0;	
	}
	else 
	{
		EXTGL_vertex_shaders=1;
	}
    }
 
    if(GLExtSupported("GL_EXT_blend_subtract"))
    {
     #ifdef WIN32
       f_glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)wglGetProcAddress("glBlendEquationEXT");
     #endif
     #ifdef LINUX
       f_glBlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)glXGetProcAddressARB((const GLubyte *)"glBlendEquationEXT");
     #endif
       if(!f_glBlendEquationEXT) 
       {
       	EXTGL_blend_substract_supported = 0;
       }
       else
       {
         EXTGL_blend_substract_supported=1;
       }
    }                


    if(GLExtSupported("GL_ARB_vertex_buffer_object"))
    {
	#ifdef WIN32
		f_glBindBufferARB           = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
        	f_glDeleteBuffersARB        = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
        	f_glGenBuffersARB           = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
        	f_glBufferDataARB           = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
        	f_glMapBufferARB            = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
        	f_glUnmapBufferARB          = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");
	#endif
        #ifdef LINUX
		f_glBindBufferARB           = (PFNGLBINDBUFFERARBPROC)glXGetProcAddressARB((const GLubyte *)"glBindBufferARB");
        	f_glDeleteBuffersARB        = (PFNGLDELETEBUFFERSARBPROC)glXGetProcAddressARB((const GLubyte *)"glDeleteBuffersARB");
        	f_glGenBuffersARB           = (PFNGLGENBUFFERSARBPROC)glXGetProcAddressARB((const GLubyte *)"glGenBuffersARB");
        	f_glBufferDataARB           = (PFNGLBUFFERDATAARBPROC)glXGetProcAddressARB((const GLubyte *)"glBufferDataARB");
        	f_glMapBufferARB            = (PFNGLMAPBUFFERARBPROC)glXGetProcAddressARB((const GLubyte *)"glMapBufferARB");
        	f_glUnmapBufferARB          = (PFNGLUNMAPBUFFERARBPROC)glXGetProcAddressARB((const GLubyte *)"glUnmapBufferARB");
	#endif
	if(!f_glBindBufferARB || !f_glDeleteBuffersARB || !f_glGenBuffersARB || !f_glBufferDataARB
	   || !f_glMapBufferARB ||!f_glUnmapBufferARB)
	{
	   EXTGL_vertex_buffer_objects=0;	
	}
	else EXTGL_vertex_buffer_objects=1;

    }
 
    // GLslang extensions
 
    if(GLExtSupported("GL_ARB_shading_language_100") && GLExtSupported("GL_ARB_shader_objects") 
        && GLExtSupported("GL_ARB_fragment_shader"))
    {
	#ifdef WIN32        	
         	f_glCreateProgramObjectARB  = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
         	f_glDeleteObjectARB         = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
         	f_glUseProgramObjectARB     = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
         	f_glCreateShaderObjectARB   = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
         	f_glShaderSourceARB         = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
         	f_glCompileShaderARB        = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
         	f_glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
         	f_glAttachObjectARB         = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
         	f_glGetInfoLogARB           = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
         	f_glLinkProgramARB          = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
         	f_glGetUniformLocationARB   = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
         	f_glUniform4fARB            = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
		f_glUniform3fARB            = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress("glUniform3fARB");         	
	 	f_glUniform1iARB            = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
	 	f_glUniform1fARB            = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
	#endif
        #ifdef LINUX        	
         	f_glCreateProgramObjectARB  = (PFNGLCREATEPROGRAMOBJECTARBPROC)glXGetProcAddressARB((const GLubyte *)"glCreateProgramObjectARB");
         	f_glDeleteObjectARB         = (PFNGLDELETEOBJECTARBPROC)glXGetProcAddressARB((const GLubyte *)"glDeleteObjectARB");
         	f_glUseProgramObjectARB     = (PFNGLUSEPROGRAMOBJECTARBPROC)glXGetProcAddressARB((const GLubyte *)"glUseProgramObjectARB");
         	f_glCreateShaderObjectARB   = (PFNGLCREATESHADEROBJECTARBPROC)glXGetProcAddressARB((const GLubyte *)"glCreateShaderObjectARB");
         	f_glShaderSourceARB         = (PFNGLSHADERSOURCEARBPROC)glXGetProcAddressARB((const GLubyte *)"glShaderSourceARB");
         	f_glCompileShaderARB        = (PFNGLCOMPILESHADERARBPROC)glXGetProcAddressARB((const GLubyte *)"glCompileShaderARB");
         	f_glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)glXGetProcAddressARB((const GLubyte *)"glGetObjectParameterivARB");
         	f_glAttachObjectARB         = (PFNGLATTACHOBJECTARBPROC)glXGetProcAddressARB((const GLubyte *)"glAttachObjectARB");
         	f_glGetInfoLogARB           = (PFNGLGETINFOLOGARBPROC)glXGetProcAddressARB((const GLubyte *)"glGetInfoLogARB");
         	f_glLinkProgramARB          = (PFNGLLINKPROGRAMARBPROC)glXGetProcAddressARB((const GLubyte *)"glLinkProgramARB");
         	f_glGetUniformLocationARB   = (PFNGLGETUNIFORMLOCATIONARBPROC)glXGetProcAddressARB((const GLubyte *)"glGetUniformLocationARB");
         	f_glUniform4fARB            = (PFNGLUNIFORM4FARBPROC)glXGetProcAddressARB((const GLubyte *)"glUniform4fARB");
         	f_glUniform3fARB            = (PFNGLUNIFORM3FARBPROC)glXGetProcAddressARB((const GLubyte *)"glUniform3fARB");
	 	f_glUniform1iARB            = (PFNGLUNIFORM1IARBPROC)glXGetProcAddressARB((const GLubyte *)"glUniform1iARB");
	 	f_glUniform1fARB            = (PFNGLUNIFORM1FARBPROC)glXGetProcAddressARB((const GLubyte *)"glUniform1fARB");
        
        #endif
        if( !f_glCreateProgramObjectARB || !f_glDeleteObjectARB || !f_glUseProgramObjectARB ||
            !f_glCreateShaderObjectARB || !f_glCreateShaderObjectARB || !f_glCompileShaderARB || 
            !f_glGetObjectParameterivARB || !f_glAttachObjectARB || !f_glGetInfoLogARB || 
            !f_glLinkProgramARB || !f_glGetUniformLocationARB || !f_glUniform4fARB || !f_glUniform3fARB ||
			!f_glUniform1iARB ||!f_glUniform1fARB )
         {
		EXTGL_shading_language=0;        
	 }
	 else EXTGL_shading_language=1;
    }
 
 
 EXTGL_texture_env_combine_supported=GLExtSupported("GL_ARB_texture_env_combine");
 EXTGL_texture_env_dot3_supported=GLExtSupported("GL_ARB_texture_env_dot3");
 EXTGL_texture_cube_map_supported=GLExtSupported("GL_ARB_texture_cube_map");
 EXTGL_blend_substract_supported=GLExtSupported("GL_EXT_blend_subtract");
 EXTGL_multitexturing = GLExtSupported("GL_ARB_multitexture");    

 if(EXTGL_vertex_buffer_objects && EXTGL_vertex_shaders && (EXTGL_number_of_texture_units >=4)
    && EXTGL_blend_substract_supported)
 {
  EXTGL_render_path = PATH_DX8;	
 }
 // Voy a ser un poco menos exigente con el render path básico: le quito la necesidad de vertex buffer objects, que si
 // no va a ser imposible hacer nada con mi 7500 del 

 //else if (EXTGL_compiled_vertex_arrays && (EXTGL_number_of_texture_units >=2) && EXTGL_texture_env_combine_supported
 //   && EXTGL_texture_env_dot3_supported && EXTGL_texture_cube_map_supported && EXTGL_vertex_buffer_objects)
 else if (EXTGL_compiled_vertex_arrays && (EXTGL_number_of_texture_units >=2) && EXTGL_texture_env_combine_supported
    && EXTGL_texture_env_dot3_supported && EXTGL_texture_cube_map_supported )
 {
  EXTGL_render_path = PATH_DX7;
 }


 /*printf("Compiled vertex arrays: %d\n",EXTGL_compiled_vertex_arrays);
 printf("Multitexturing: %d\n",EXTGL_multitexturing);
 printf("Texture units: %d\n",EXTGL_number_of_texture_units);
 printf("Vertex shaders: %d\n",EXTGL_vertex_shaders);
 printf("Blend subtract: %d\n",EXTGL_blend_substract_supported);
 printf("Texture env combine: %d\n",EXTGL_texture_env_combine_supported); 
 printf("Dot3 bump mapping: %d\n",EXTGL_texture_env_dot3_supported); 
 printf("Cube mapping: %d\n",EXTGL_texture_cube_map_supported); 
 printf("Vertex Buffer Objects: %d\n",EXTGL_vertex_buffer_objects);
 printf("Render path: %d\n",EXTGL_render_path);
 
 int kk;
 
 glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &kk);
 
 printf("Max number of vertex attribs: %d\n",kk);*/

  
}
