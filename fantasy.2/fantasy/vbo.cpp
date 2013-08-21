// Functions to implement the vertex_buffer_object extension in the engine
// It's just better to take them to another file, since this is growing :)

#ifdef WINDOZE
 #include <windows.h>
 #include <stdio.h>
 #include <GL/gl.h>
 #include <GL/glext.h>
#else
 #include <GL/gl.h>
 #include <GL/glx.h>
 #include <GL/glext.h>
#endif

#include "3dworld.h"
#include "mates.h"
#include "extgl.h"
#include "misc.h"
#include "vbo.h"

void UploadBuffers(object_type *obj)
{
    int mode;
    int error;

    glGetError(); // limpio el ltimo error (por si acaso :)

    if(obj->hint_dynamic) mode = GL_STREAM_DRAW_ARB;
     else mode = GL_STATIC_DRAW_ARB;
    
    obj->vbufinfo.usingvertexbuf=1;
    
    f_glGenBuffersARB( 1, &(obj->vbufinfo.vertexbufferid));
    f_glGenBuffersARB( 1, &(obj->vbufinfo.indexbufferid));
    f_glGenBuffersARB( 1, &(obj->vbufinfo.texbufferid));
    if(obj->ColorPointer)
     f_glGenBuffersARB( 1, &(obj->vbufinfo.colorbufferid));

        
    f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, obj->vbufinfo.vertexbufferid);
    f_glBufferDataARB( GL_ARRAY_BUFFER_ARB, 6 * obj->GLnumber_of_vertices * sizeof(float),
    			obj->GLVertexPointer, mode);

    error=glGetError();    
    if(error)
    {
     sprintf (FANTASY_DEBUG_STRING, "Código de error al subir vertex buffer %d\n",error);
     Debug(DEBUG_LEVEL_WARNING);
     obj->vbufinfo.usingvertexbuf=0;
     return;
    }
 
    obj->vbufinfo.normalskip=3*obj->GLnumber_of_vertices*sizeof(float);

    f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, obj->vbufinfo.texbufferid);
    f_glBufferDataARB( GL_ARRAY_BUFFER_ARB, 2 * obj->GLnumber_of_vertices * sizeof(float),
    			obj->GLTexCoordPointer, GL_STREAM_DRAW_ARB); 
 
    error=glGetError();    
    if(error)
    {
     sprintf (FANTASY_DEBUG_STRING, "Código de error al subir texcoord buffer %d\n",error);
     Debug(DEBUG_LEVEL_WARNING);
     obj->vbufinfo.usingvertexbuf=0;
     return;
    }
    			
    if(obj->ColorPointer)
    {
    	f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, obj->vbufinfo.colorbufferid);
    	f_glBufferDataARB( GL_ARRAY_BUFFER_ARB, 3 * obj->GLnumber_of_vertices * sizeof(float),
    			obj->GLColorPointer, GL_STATIC_DRAW_ARB);

    	error=glGetError();    
    	if(error)
    	{
     		sprintf (FANTASY_DEBUG_STRING, "Código de error al subir color buffer %d\n",error);
     		Debug(DEBUG_LEVEL_WARNING);
     		obj->vbufinfo.usingvertexbuf=0;
     		return;
    	}			
    }	
    			
    f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, obj->vbufinfo.indexbufferid);
    f_glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 3* obj->number_of_polygons*sizeof(unsigned),
    			obj->GLIndexPointer, GL_STATIC_DRAW_ARB);
    			
    error=glGetError();    
    if(error)
    {
     sprintf (FANTASY_DEBUG_STRING, "Código de error al subir index buffer %d\n",error);
     Debug(DEBUG_LEVEL_WARNING);
     obj->vbufinfo.usingvertexbuf=0;
     return;
    }    			
    			
    f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, NULL );
    f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, NULL );
    
    free(obj->GLVertexPointer);
    free(obj->GLTexCoordPointer);
    free(obj->GLIndexPointer);
    obj->GLVertexPointer=NULL;
    obj->GLTexCoordPointer=NULL;
    obj->GLNormalPointer=NULL;
    obj->GLIndexPointer=NULL;
    if(obj->ColorPointer)
    {
    	free(obj->GLColorPointer);
    	obj->GLColorPointer=NULL;
    }
}

// Flag:
// 	Bit 0: update vertices
// 	Bit 1: update normals



void UpdateBuffers(object_type *obj,unsigned char flags)
{
   int i,j;
   VertexRemapper *tmp;   
 
   if(obj->vbufinfo.usingvertexbuf)
   {
   	if(flags & UPDATE_VERTICES)
   	{
   		f_glBindBufferARB(GL_ARRAY_BUFFER_ARB,obj->vbufinfo.vertexbufferid);
    		obj->GLVertexPointer = (float *)f_glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
   		for(i=0,j=0;i<obj->number_of_vertices;i++)
   		{
		  	tmp = obj->node[i];
	    		while(tmp!=NULL)
	  		{
	   			obj->GLVertexPointer[3*j] = obj->VertexPointer[3*i];
   				obj->GLVertexPointer[3*j+1] = obj->VertexPointer[3*i+1];
	   			obj->GLVertexPointer[3*j+2] = obj->VertexPointer[3*i+2];
   				tmp=tmp->next;
   				j++;
  			} 	
   		}   	
   		f_glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
       		f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, NULL );
   		obj->GLVertexPointer=NULL;
	}
	if(flags & UPDATE_NORMALS)
	{


   		f_glBindBufferARB(GL_ARRAY_BUFFER_ARB,obj->vbufinfo.vertexbufferid);
    		obj->GLVertexPointer = (float *)f_glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
    		obj->GLNormalPointer = obj->GLVertexPointer + 3 * obj->GLnumber_of_vertices;
    		AssignGLNormals(obj);
   		f_glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
    		f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, NULL );
    		obj->GLVertexPointer = NULL;
    		obj->GLNormalPointer = NULL;		
	}
    }
}

void DestroyBuffers(object_type *obj)
{
    	f_glBindBufferARB( GL_ARRAY_BUFFER_ARB, NULL );
    	f_glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, NULL );
	
	if(obj->vbufinfo.usingvertexbuf)
	{
    		obj->vbufinfo.usingvertexbuf=0;
    		f_glDeleteBuffersARB( 1, &(obj->vbufinfo.vertexbufferid));
    		f_glDeleteBuffersARB( 1, &(obj->vbufinfo.indexbufferid));
    		f_glDeleteBuffersARB( 1, &(obj->vbufinfo.texbufferid));
	}
}
