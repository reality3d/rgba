#ifndef GLSL_H
#define GLSL_H

#ifdef WINDOZE
#include <windows.h>
#endif

#include <GL/gl.h>
#ifdef WINDOZE
 #include <GL/glext.h>
#else
 #include <GL/glext.h> 
#endif

#include "extgl.h"

#define GLSL_VERTEX_SHADER 1
#define GLSL_FRAGMENT_SHADER 2

void GLSL_LoadShader(GLuint *,char *, int );
void GLSL_CompileAndLinkShader(GLuint *,GLuint,GLuint);
void GLSL_GetUniforms(material *);
void GLSL_SelectShader(material *);
void GLSL_DeleteShaders(material *);

#endif