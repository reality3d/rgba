#define TEXTAN  32

#ifdef WIN32
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
#endif
#include <GL/gl.h>

#include <math.h>
#include <string.h>
#include <malloc.h>

#include "psys.h"
#include "math3d.h"
#include "radix.h"


//--- generate flare ---------------------------------------------------------

static void doflare( unsigned char *buffer, int xres, int yres )
{
    int     x, y;
    float   dx, dy, a;

    for( y=0; y<yres; y++ )
    for( x=0; x<xres; x++ )
        {
        dx = -1.0 + 2.0*(float)x/(float)xres;
        dy = -1.0 + 2.0*(float)y/(float)yres;

        a = dx*dx+dy*dy;
        a = exp( -a*2.7 );
        
        if( a>1.0 ) a=1.0;
        if( a<0.0 ) a=0.0;

        *buffer++ = 255;
        *buffer++ = 255;
        *buffer++ = 255;
        *buffer++ = 255.0*a;
        }
}

//--- psys! ------------------------------------------------------------------


void PSYS_Free( PSYS *psys )
{
    free( psys->p );
    free( psys->radix_buf );
    free( psys->radix_aux );

    glDeleteTextures( 1, &psys->tid );
}


int PSYS_Init( PSYS *psys, int n, float width )
{
    int             i;
    unsigned char   *text;

    psys->width     = width;
    psys->n         = n;
    psys->p         = (PART *)malloc( n*sizeof(PART) );
    psys->radix_buf = (unsigned int*)malloc( 2*n*sizeof(int) );
    psys->radix_aux = (unsigned int*)malloc( 2*n*sizeof(int) );
    text            = (unsigned char*)malloc( TEXTAN*TEXTAN*4 );

    if( !psys->p || !psys->radix_buf || !psys->radix_aux || !text )
        {
        return( 0 );
        }

    doflare( text, TEXTAN, TEXTAN );

    glGenTextures( 1, &psys->tid );
    glBindTexture( GL_TEXTURE_2D, psys->tid );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8,
                  TEXTAN, TEXTAN, 0, GL_RGBA, GL_UNSIGNED_BYTE, text );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    free( text );

    memset( psys->radix_buf, 0, 2*n*sizeof(int) );
    memset( psys->radix_aux, 0, 2*n*sizeof(int) );


    return( 1 );

}


void PSYS_Draw( PSYS *psys )
{
    float   w;
    int     i, n, m;
    PART    *p;
    float   z;
    float   mod[16];       // This will hold our modelview matrix
    float   inv[16];
    VEC     vec;


    w = psys->width;
    n = psys->n;


    /////////////

    glGetFloatv( GL_MODELVIEW_MATRIX, mod );
    MtxInvert( inv, mod );
    
    /////////////

    m = 0;
    for( i=0; i<n; i++ )
        {
        VecSub( &psys->p[i].pos, (VEC*)(inv+12), &vec );   // campos
        z = -VecDot( &vec, (VEC*)(inv+8) );                // camdir
        if( z>0.0 )
            {
            psys->radix_buf[2*m+0] = i;
            psys->radix_buf[2*m+1] = (int)(65535.0*z);
            m++;
            }
        }

    radixsort( psys->radix_buf, psys->radix_aux, m );

    //////////

    glEnable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );

    glBindTexture( GL_TEXTURE_2D, psys->tid );
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glDisable( GL_LIGHT0 );
    glDisable( GL_LIGHTING );


    glMultMatrixf( inv );

    for( i=0; i<m; i++ )
        {
        p = psys->p + psys->radix_buf[((m-1-i)<<1)];

//        glColor4fv( (float*)&p->col );

        VecSub( &p->pos, (VEC*)(inv+12), &vec );

        glPushMatrix();

        glTranslatef( VecDot( &vec, (VEC*)(inv+0) ),
                      VecDot( &vec, (VEC*)(inv+4) ),
                      VecDot( &vec, (VEC*)(inv+8) ) );

        glBegin( GL_QUADS );
            glTexCoord2i( 0, 1 ); glVertex3f( -w, +w, 0 );
            glTexCoord2i( 0, 0 ); glVertex3f( -w, -w, 0 );
            glTexCoord2i( 1, 0 ); glVertex3f( +w, -w, 0 );
            glTexCoord2i( 1, 1 ); glVertex3f( +w, +w, 0 );
        glEnd();

        glPopMatrix();
        }

    glDisable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );
}
