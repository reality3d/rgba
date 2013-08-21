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
#include "glow.h"
#include "extens.h"

//----------------------------------------------------------------------------

GLOW *GLOW_Init( int xres, int yres )
{
    GLOW            *g;
    unsigned char   *text;

    g = (GLOW*)malloc( sizeof(GLOW) );
    if( !g )
        return( 0 );

    g->running = 0;

    if( EstaExtension("GL_EXT_texture_lod_bias") &&
        EstaExtension("GL_SGIS_generate_mipmap") )
        {
        g->xres = xres;
        g->yres = yres;
        g->ox = (float)xres/1024.0f;
        g->oy = (float)yres/1024.0f;
        g->pesos[0] = 1.0;
        g->pesos[1] = 0.0;
        g->pesos[2] = 0.0;
        g->pesos[3] = 0.0;
        g->pesos[4] = 0.0;
        g->pesos[5] = 0.0;
        g->pesos[6] = 0.0;
        g->pesos[7] = 0.0;
        g->running = 1;

        glGenTextures( 1, &g->tid );   
    
        if( g->tid<1 )
            return( 0 );

        text = (unsigned char*)malloc( 1024*1024*3 );
        if( !text )
            {
			glDeleteTextures( 1, &g->tid );
			return( 0 );
            }

        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, g->tid );

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		if( gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB8, 1024, 1024, GL_RGB, GL_UNSIGNED_BYTE, text ) )
            {
			glDeleteTextures( 1, &g->tid );
			free( text );
			return( 0 );
            }
		
		free( text );
        }
    else
        {
        #ifdef DEBUG
            printf( "glow.epl:: falta extension \"GL_EXT_texture_lod_bias\" o \"GL_SGIS_generate_mipmap\"...\n" );
        #else
            return( 0 );
        #endif
        }

    return( g );

}

void GLOW_SetPesos( GLOW *g,
                    float p0, float p1, float p2, float p3,
                    float p4, float p5, float p6, float p7 )
{
    g->pesos[0] = p0;
    g->pesos[1] = p1;
    g->pesos[2] = p2;
    g->pesos[3] = p3;
    g->pesos[4] = p4;
    g->pesos[5] = p5;
    g->pesos[6] = p6;
    g->pesos[7] = p7;
}

void GLOW_End( GLOW *g )
{
    glDeleteTextures( 1, &g->tid );

    free( g );
}


void GLOW_DoFrame( GLOW *g )
{
    long    i;

    // si no hay extensiones...
    if( !g->running ) return;
 
    // disable
    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_NORMALIZE );
    glDisable( GL_BLEND );
    glDisable( GL_TEXTURE_2D );
    glShadeModel( GL_FLAT );
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

    // set 2d
	//glViewport( 0, 0, g->xres, g->yres );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluOrtho2D( 0.0f, 1.0f, 0.0f, 1.0f );

    // copiar a textura desde el color buffer
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, g->tid );
    glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    
    
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

    // oscurecer octava 0
    glDisable( GL_TEXTURE_2D ); //iqnew
    glEnable( GL_BLEND );
    glBlendFunc( GL_ZERO, GL_SRC_ALPHA );
    glColor4f( 0.0f,0.0f,0.0f,g->pesos[0]);
    glBegin( GL_QUADS );
        glVertex2f( 0.0f, 1.0f );
        glVertex2f( 0.0f, 0.0f );
        glVertex2f( 1.0f, 0.0f );
        glVertex2f( 1.0f, 1.0f );
    glEnd();

    // resto de octavas
    glEnable( GL_TEXTURE_2D );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );
    for( i=1; i<8; i++ )
        {
        glTexEnvi( GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, i );
		
        glColor4f( 1.0f, 1.0f, 1.0f, g->pesos[i] );
        glBegin( GL_QUADS );
            glTexCoord2f(     0, g->oy ); glVertex2f( 0.0f, 1.0f );
            glTexCoord2f(     0,     0 ); glVertex2f( 0.0f, 0.0f );
            glTexCoord2f( g->ox,     0 ); glVertex2f( 1.0f, 0.0f );
            glTexCoord2f( g->ox, g->oy ); glVertex2f( 1.0f, 1.0f );
        glEnd();
        }

    // dejarlo todo bien...
	glTexEnvi( GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0 );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );
	glShadeModel( GL_SMOOTH );
}
