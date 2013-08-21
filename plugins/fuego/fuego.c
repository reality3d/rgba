#include <malloc.h>     // malloc(), free()
#include <stdlib.h>     // srand(), rand()  
#include <assert.h>     // assert()

#ifdef WIN32
 #include <windows.h>
#endif
#include <GL/gl.h>
#include <glt/glt.h>


#include "fuego.h"
#include "flame.h"


void FUEGO_Start( FUEGO *f )
{
}

void FUEGO_Stop( FUEGO *f )
{
}

FUEGO *FUEGO_Init( int sem, int ancho, char *texture_id )
{
    int     i;
    FUEGO   *f;

    f = (FUEGO*)malloc( sizeof(FUEGO) );
    assert( f!=NULL );


    f->an = ancho;

    f->velo1 = 1.0;
    f->velo2 = 1.0;

    f->text = (long*)malloc( ancho*ancho*4 );
    assert( f->text!=NULL );

    
    i = FLAME_Init( &f->flame, ancho, sem );
    assert( i );

    f->tid = TEX_New (ancho, ancho, 32, f->text, TEX_BUILD_MIPMAPS);
    assert (f->tid != NULL);
    TEX_SetFilter (GL_LINEAR, GL_LINEAR);
    TEX_SetWrap   (GL_CLAMP, GL_CLAMP);
    CACHE_InsertTexture (texture_id, f->tid);

    /*
    glGenTextures( 1, &f->tid );

    glBindTexture( GL_TEXTURE_2D, f->tid );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    glTexImage2D( GL_TEXTURE_2D, 0, 4, ancho, ancho, 0,
                  GL_RGBA, GL_UNSIGNED_BYTE, f->text );
*/

    return( f );
}




void FUEGO_Draw( FUEGO *fuego, float t )
{
    FLAME_DoFrame( fuego->text, &fuego->flame, t*fuego->velo1, t*fuego->velo2 );
    TEX_Bind (fuego->tid);
    TEX_UpdateData (fuego->tid, fuego->text, TEX_BUILD_MIPMAPS);
/*

    glBindTexture( GL_TEXTURE_2D, fuego->tid );
    glTexImage2D(  GL_TEXTURE_2D, 0,
                   GL_RGBA8, fuego->an, fuego->an, 0,
                   GL_RGBA, GL_UNSIGNED_BYTE, fuego->text );

*/
}

void FUEGO_End( FUEGO *fuego )
{
    FLAME_Free( &fuego->flame );
    //glDeleteTextures( 1, &fuego->tid );
    CACHE_FreeTexture (fuego->tid);
    free( fuego->text );
    free( fuego );
}


void FUEGO_SetColor( FUEGO *f,
                     int i1, int a1, int r1, int g1, int b1,
                     int i2, int a2, int r2, int g2, int b2,
                     int i3, int a3, int r3, int g3, int b3,
                     int i4, int a4, int r4, int g4, int b4 )
{
    FLAME_SetColor( &f->flame,  i1, a1, r1, g1, b1,
                                i2, a2, r2, g2, b2,
                                i3, a3, r3, g3, b3,
                                i4, a4, r4, g4, b4 );
}

void FUEGO_SetSpeed( FUEGO *f, float v1, float v2 )
{
    f->velo1 = v1;
    f->velo2 = v2;
}
