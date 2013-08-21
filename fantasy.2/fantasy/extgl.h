#ifndef EXTGL_H
#define EXTGL_H

#ifdef WIN32
 #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

// Some constants

#define UNSUPPORTED_GFX_CARD 	0
#define PATH_DX7		1
#define PATH_DX8		2
#define PATH_DX9		3

//Our function pointers.
extern PFNGLLOCKARRAYSEXTPROC   	 f_glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC 	 f_glUnlockArraysEXT;
extern PFNGLCLIENTACTIVETEXTUREARBPROC 	 f_glClientActiveTextureARB;
extern PFNGLACTIVETEXTUREARBPROC   	 f_glActiveTextureARB;
extern PFNGLGENPROGRAMSARBPROC           f_glGenProgramsARB;
extern PFNGLDELETEPROGRAMSARBPROC        f_glDeleteProgramsARB;
extern PFNGLBINDPROGRAMARBPROC           f_glBindProgramARB;
extern PFNGLPROGRAMSTRINGARBPROC         f_glProgramStringARB;
extern PFNGLPROGRAMENVPARAMETER4FARBPROC f_glProgramEnvParameter4fARB;
extern PFNGLBLENDEQUATIONEXTPROC	 f_glBlendEquationEXT;
extern PFNGLBINDBUFFERARBPROC		 f_glBindBufferARB;
extern PFNGLDELETEBUFFERSARBPROC         f_glDeleteBuffersARB;
extern PFNGLGENBUFFERSARBPROC            f_glGenBuffersARB;
extern PFNGLBUFFERDATAARBPROC            f_glBufferDataARB;
extern PFNGLMAPBUFFERARBPROC             f_glMapBufferARB;
extern PFNGLUNMAPBUFFERARBPROC           f_glUnmapBufferARB;
extern PFNGLVERTEXATTRIBPOINTERARBPROC  	f_glVertexAttribPointerARB;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC  	f_glEnableVertexAttribArrayARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC	f_glDisableVertexAttribArrayARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC	f_glProgramLocalParameter4fARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC  f_glCreateProgramObjectARB  ;
extern PFNGLDELETEOBJECTARBPROC         f_glDeleteObjectARB         ;
extern PFNGLUSEPROGRAMOBJECTARBPROC     f_glUseProgramObjectARB     ;
extern PFNGLCREATESHADEROBJECTARBPROC   f_glCreateShaderObjectARB   ;
extern PFNGLSHADERSOURCEARBPROC         f_glShaderSourceARB         ;
extern PFNGLCOMPILESHADERARBPROC        f_glCompileShaderARB        ;
extern PFNGLGETOBJECTPARAMETERIVARBPROC f_glGetObjectParameterivARB ;
extern PFNGLATTACHOBJECTARBPROC         f_glAttachObjectARB         ;
extern PFNGLGETINFOLOGARBPROC           f_glGetInfoLogARB           ;
extern PFNGLLINKPROGRAMARBPROC          f_glLinkProgramARB          ;
extern PFNGLGETUNIFORMLOCATIONARBPROC   f_glGetUniformLocationARB   ;
extern PFNGLUNIFORM4FARBPROC            f_glUniform4fARB            ;
extern PFNGLUNIFORM3FARBPROC            f_glUniform3fARB            ;
extern PFNGLUNIFORM1IARBPROC            f_glUniform1iARB            ;
extern PFNGLUNIFORM1FARBPROC            f_glUniform1fARB            ;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

extern unsigned char EXTGL_compiled_vertex_arrays;
extern unsigned char EXTGL_multitexturing;
extern unsigned char EXTGL_vertex_shaders;
extern int EXTGL_number_of_texture_units;
extern unsigned char EXTGL_blend_substract_supported;
extern unsigned char EXTGL_texture_env_combine_supported; 
extern unsigned char EXTGL_texture_env_dot3_supported; 
extern unsigned char EXTGL_texture_cube_map_supported; 
extern unsigned char EXTGL_vertex_buffer_objects;
extern unsigned char EXTGL_shading_language;
extern unsigned char EXTGL_render_path;
void InitGLExtensions(void);
#endif
