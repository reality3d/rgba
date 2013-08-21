#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "material.h"
#include "glsl.h"
#include "packer/packer.h"
#include "misc.h"

static char errorString[4096];

void GLSL_LoadShader(GLuint *shader_id,char *filename, int type)
{
    PFILE *f;
    int filesize;
    int bytes;
    char * buffer;
    int wasok;

    if(!EXTGL_shading_language)
    {
     *shader_id=0;
     sprintf (FANTASY_DEBUG_STRING, "Sorry, this card does not support GLSL\n");
     Debug(DEBUG_LEVEL_WARNING);
     return;
    } 

    f = pak_open(filename, "rb");
    if (f == NULL)
    {
     sprintf (FANTASY_DEBUG_STRING, "Shader file %s not found\n",filename);
     Debug(DEBUG_LEVEL_ERROR);
     *shader_id=0;     
     return;
    }

    // Find file size.
    pak_fseek(f, 0, SEEK_END);
    filesize = pak_ftell(f);
    pak_fseek(f, 0, SEEK_SET);

    buffer = (char *)malloc(filesize*sizeof(char)+1);
    if(!buffer) return;
    // Leemos el shader en memoria y lo almacenamos
    bytes = pak_read(buffer,1,filesize,f);
    pak_close(f);
    buffer[filesize*sizeof(char)] = '\0';

    // Create shader
    if(type == GLSL_VERTEX_SHADER)
       *shader_id = f_glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
    else if (type == GLSL_FRAGMENT_SHADER)
       *shader_id = f_glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
    
    // Compile shader
    f_glShaderSourceARB( *shader_id, 1, (const char **)&buffer, NULL );
    f_glCompileShaderARB( *shader_id);
    // Check for errors
    f_glGetObjectParameterivARB( *shader_id, GL_OBJECT_COMPILE_STATUS_ARB,&wasok);
    if( !wasok )
    {
	f_glGetInfoLogARB(*shader_id, sizeof(errorString), NULL, errorString);
        sprintf (FANTASY_DEBUG_STRING, "Error compiling shader %s\n",filename);
        Debug(DEBUG_LEVEL_ERROR);	
        sprintf (FANTASY_DEBUG_STRING, "%s\n",errorString);
        Debug(DEBUG_LEVEL_ERROR);	
	*shader_id=0;
	return;
    }
    free(buffer);
}

void GLSL_CompileAndLinkShader(GLuint *program_id,GLuint vtx_shader_id,GLuint pix_shader_id)
{
 int wasok;
 
 if(!EXTGL_shading_language)
 {
     return; // No glsl supported
 } 

	
 // Create a program object with the shaders 
 *program_id = f_glCreateProgramObjectARB();
 if(vtx_shader_id) f_glAttachObjectARB( *program_id, vtx_shader_id );
 if(pix_shader_id) f_glAttachObjectARB( *program_id, pix_shader_id );
 // Link and check for errors
 f_glLinkProgramARB( *program_id );
 f_glGetObjectParameterivARB( *program_id, GL_OBJECT_LINK_STATUS_ARB, &wasok); 

 if( !wasok )
 {
	f_glGetInfoLogARB(*program_id, sizeof(errorString), NULL, errorString);
        sprintf (FANTASY_DEBUG_STRING, "Error linking shader: \n");
        Debug(DEBUG_LEVEL_ERROR);	
        sprintf (FANTASY_DEBUG_STRING, "%s\n",errorString);
        Debug(DEBUG_LEVEL_ERROR);	
	return;
 }		
}

void GLSL_GetUniforms(material *mat)
{
 if(!EXTGL_shading_language)
 {
     return; // No glsl supported
 } 

 mat->shader.loc_TexUnit[0] = f_glGetUniformLocationARB (mat->shader.programObj,"TexUnit0");
 mat->shader.loc_TexUnit[1] = f_glGetUniformLocationARB (mat->shader.programObj,"TexUnit1");
 mat->shader.loc_TexUnit[2] = f_glGetUniformLocationARB (mat->shader.programObj,"TexUnit2");
 mat->shader.loc_TexUnit[3] = f_glGetUniformLocationARB (mat->shader.programObj,"TexUnit3");
 mat->shader.loc_TexUnit[4] = f_glGetUniformLocationARB (mat->shader.programObj,"TexUnit4");
 mat->shader.loc_TexUnit[5] = f_glGetUniformLocationARB (mat->shader.programObj,"TexUnit5");
 mat->shader.loc_TexUnit[6] = f_glGetUniformLocationARB (mat->shader.programObj,"TexUnit6");
 mat->shader.loc_TexUnit[7] = f_glGetUniformLocationARB (mat->shader.programObj,"TexUnit7");    
 mat->shader.loc_Camera  = f_glGetUniformLocationARB (mat->shader.programObj,"Camera");
 mat->shader.loc_NLights = f_glGetUniformLocationARB (mat->shader.programObj,"NLights");
}

void GLSL_SelectShader(material *mat)
{
 int i;
 
 if(!EXTGL_shading_language)
 {
     return; // No glsl supported
 } 

 f_glUseProgramObjectARB( mat->shader.programObj );       
 for(i=0;i<8;i++)
	f_glUniform1iARB( mat->shader.loc_TexUnit[i], i );
 f_glUniform1iARB( mat->shader.loc_NLights, mat->shader.nlights );
 f_glUniform3fARB( mat->shader.loc_Camera, mat->shader.Camera[0], mat->shader.Camera[1], mat->shader.Camera[2]);
 for(i=0;i<mat->shader.params.used;i++) //Custom parameters 	
 {
 	f_glUniform1fARB(f_glGetUniformLocationARB (mat->shader.programObj,mat->shader.params.name[i]),mat->shader.params.paramf[i]);
 }
}


void GLSL_DeleteShaders(material *mat)
{

 if(!EXTGL_shading_language)
 {
     return; // No glsl supported
 } 	
}