#define USEGLT

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <malloc.h>		// malloc(), free()
#include <string.h>		// memset()
#include <math.h>       // sinf(), sqr(), log(), fmod()
#include <stdlib.h>     // rand()

#ifdef USEGLT
#include <glt/glt.h>
#else
#include <stdio.h>
#endif


typedef struct
{
    #ifdef USEGLT
    TTexture        *tid;
    #else
    unsigned int    tid;
    #endif
    int             mode;

    int             f_ya;
    float           f_to;
    float           freq;
}PORTADA;

//-------------------------------------------------

void PORTADA_Delete( void *vme )
{
    PORTADA  *me = (PORTADA*)vme;
	int		i;

	if( !me ) return;

    #ifdef USEGLT

    CACHE_FreeTexture( me->tid );

    #else

    if( me->tid ) glDeleteTextures( 1, &me->tid );

    #endif

    free( me );
}

#ifndef USEGLT
static int TGA_Carga32( char *nombre, long *buffer, int xres, int yres )
{
    FILE            *fp;
    int             i, a;
    unsigned char   *ptr = (unsigned char*)buffer;


    fp = fopen( nombre, "rb" );
    if( !fp )
        return( 0 );

    fseek( fp, 44, SEEK_SET );

    while( yres-- )
        {
        for( i=0; i<xres; i++ )
            {
            fread( ptr+0, 1, 1, fp );
            fread( ptr+3, 1, 1, fp );
            fread( ptr+2, 1, 1, fp );
            fread( ptr+1, 1, 1, fp );

            //ptr[3] = rand()&255;
            ptr += 4;
            }
        }

    fclose( fp );

    return( 1 );
}
#endif

void *PORTADA_New( void )
{
    PORTADA *me;
    long    *buffer;

    me = (PORTADA*)malloc( sizeof(PORTADA) );
	if( !me )
		return( 0 );

    memset( me, 0, sizeof(PORTADA) );



    #ifdef USEGLT

    me->tid = CACHE_LoadTexture( "2d/portada.png", 0&TEX_SWAP_RB );
    if( !me->tid )
        return( 0 );

    if( me->tid->width !=1024 ) {PORTADA_Delete( me ); return( 0 );}
    if( me->tid->height!=1024 ) {PORTADA_Delete( me ); return( 0 );}

    TEX_SetFilter( GL_LINEAR, GL_LINEAR );
    TEX_SetWrap  ( GL_CLAMP, GL_CLAMP );
    //TEX_SetEnvironment( GL_DECAL );
    TEX_SetEnvironment( GL_BLEND );

    //CACHE_InsertTexture( "portada.png", f->tid );

    #else

    buffer = (long*)malloc( 1024*1024*4 );
    if( !buffer )
        {
        PORTADA_Delete( me );
        return( 0 );
        }


    if( !TGA_Carga32( "2d/portada.tga", buffer, 1024, 1024 ) )
        {
        PORTADA_Delete( me );
        return( 0 );
        }

    // sube textura 2d
    glGenTextures( 1, &me->tid );
    if( !me->tid )
        {
        PORTADA_Delete( me );
        return( 0 );
        }


    glBindTexture( GL_TEXTURE_2D, me->tid );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    free( buffer );

    #endif

	return( me );
}


#ifdef USEGLT

#define GFX2TXTX(a) ((float)(a)/1024.0f)
#define GFX2TXTY(a) ((float)(a)/1024.0f)

#else

#define GFX2TXTX(a) ((float)(a)/1024.0f)
#define GFX2TXTY(a) ((1024.0f-(float)(a))/1024.0f)

#endif

void PORTADA_DoFrame( void *vme, float t )
{
    int i;
    PORTADA *me = (PORTADA*)vme;
    float   x1, x2, s, a, y1, y2;
    float   w = 158.0f;
    float   h = 335.0f;

	if( !me ) return;


    if( me->f_ya )
        {
        me->f_ya = 0;
        me->f_to = t;
        }

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glOrtho( 0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f );

    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_NORMALIZE );
    glShadeModel( GL_SMOOTH );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );


    #if 0
    glClearColor(0,0,1,0);
    glClear( GL_COLOR_BUFFER_BIT );
    glColor3f( 1,1,1);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    #endif

    glEnable( GL_TEXTURE_2D );

    #ifdef USEGLT
    TEX_Bind( me->tid );
    #else
    glBindTexture( GL_TEXTURE_2D, me->tid );
    #endif

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );


    if( !me->mode )
        {
        a = .5f-.5f*cosf(6.2831*(t-me->f_to)*me->freq);
        a = a*a;
        a = a*a;
        a = a*a;
        s = 1.0f+0.05f*a;
        x1 =  4.0f*a;
        x2 = -4.0f;
        a *= .06f;
        a = -a;
        y1 = 0.0f*(-1.0f+2.0f*(float)(rand()&255)/255.0f);
        y2 = 0.0f*(-1.0f+2.0f*(float)(rand()&255)/255.0f);
        }
    else
        {
        a = .5f-.5f*cosf(6.2831*(t-me->f_to)*me->freq);
        a = a*a;
        a = a*a;
        a = a*a;
        s = 1.0f+0.01f*a;
        y1 = a*20.0f*(-1.0f+2.0f*(float)(rand()&255)/255.0f);
        y2 = a*20.0f*(-1.0f+2.0f*(float)(rand()&255)/255.0f);
        x1 = (a*5.0f)+a*15.0f*(-1.0f+2.0f*(float)(rand()&255)/255.0f);
        x2 = (-a*5.0f)+a*15.0f*(-1.0f+2.0f*(float)(rand()&255)/255.0f);
        a = 0.0f;
        }

    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f);

    glBegin( GL_QUADS );
        // dibuja la cara
        glTexCoord2f( GFX2TXTX(224),     GFX2TXTY(424) );     glVertex2f(   0.0f,   0.0f );
        glTexCoord2f( GFX2TXTX(224),     GFX2TXTY(424+600) ); glVertex2f(   0.0f, 600.0f );
        glTexCoord2f( GFX2TXTX(224+800), GFX2TXTY(424+600) ); glVertex2f( 800.0f, 600.0f );
        glTexCoord2f( GFX2TXTX(224+800), GFX2TXTY(424) );     glVertex2f( 800.0f,   0.0f );

        // casco derecho
        glTexCoord2f( GFX2TXTX(47),     GFX2TXTY(327    ) );  glVertex2f( x1+526+0,                       y1 ); 
        glTexCoord2f( GFX2TXTX(47),     GFX2TXTY(327+335) );  glVertex2f( x1+526+(0*cosf(a)-h*sinf(a))*s, y1+(0*sinf(a)+h*cosf(a))*s );
        glTexCoord2f( GFX2TXTX(47+158), GFX2TXTY(327+335) );  glVertex2f( x1+526+(w*cosf(a)-h*sinf(a))*s, y1+(w*sinf(a)+h*cosf(a))*s );
        glTexCoord2f( GFX2TXTX(47+158), GFX2TXTY(327    ) );  glVertex2f( x1+526+(w*cosf(a)-0*sinf(a))*s, y1+(w*sinf(a)+0*cosf(a))*s );

        a = -a;
        // casco izquierdo
        glTexCoord2f( GFX2TXTX(40),     GFX2TXTY(682    ) );  glVertex2f( w+x2+117+(-w*cosf(a)-0*sinf(a))*s,y2+(-w*sinf(a)+0*cosf(a))*s );
        glTexCoord2f( GFX2TXTX(40),     GFX2TXTY(682+335) );  glVertex2f( w+x2+117+(-w*cosf(a)-h*sinf(a))*s,y2+(-w*sinf(a)+h*cosf(a))*s );
        glTexCoord2f( GFX2TXTX(40+158), GFX2TXTY(682+335) );  glVertex2f( w+x2+117+(+0*cosf(a)-h*sinf(a))*s,y2+( 0*sinf(a)+h*cosf(a))*s );
        glTexCoord2f( GFX2TXTX(40+158), GFX2TXTY(682    ) );  glVertex2f( w+x2+117+(+0*cosf(a)-0*sinf(a))*s,y2+( 0*sinf(a)+0*cosf(a))*s );
    glEnd();


    glDisable( GL_BLEND );
    glDisable( GL_TEXTURE_2D );
}


void PORTADA_SetMode( void *vme, int mode )
{
    PORTADA *me = (PORTADA*)vme;

	if( !me ) return;

    me->mode = mode;
}

void PORTADA_SetFreq( void *vme, float freq )
{
    PORTADA *me = (PORTADA*)vme;

	if( !me ) return;

    me->freq = freq;
    me->f_ya = 1;
}
