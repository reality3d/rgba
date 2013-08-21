#include <math.h>
#include <malloc.h>
#include <stdlib.h>

#include "flame.h"
#include "turb.h"


void FLAME_DoFrame( long *buffer, FLAME *flame, float time1, float time2 )
{
    int             i, j;
    signed int      kk;
    unsigned char   *ptr;
    float           x, y, f;
    float           s1, s2;
        
    time1 *= .04;
    time2 *= .04;

    s1 = (float)flame->ancho*9.0/64.0;
    s2 = 4.0/(float)flame->ancho;

    ptr = flame->mascara;

                       
    for( j=0; j<flame->ancho; j++ )
        {
        y = j*s2;
        for( i=0; i<flame->ancho; i++ )
            {
            x = i*s2;

            f = -1.0+2.0*kkTURB_Turbulence3d( flame->ttable, x, y+time1, time2*.5, flame->octavas );
            kk = i + f*s1;
        
            if( kk<0 ) kk=0;
            else if( kk>=(flame->ancho-1) ) kk = flame->ancho-1;

            *buffer++ = flame->paleta[ ptr[kk] ];
            }
        ptr += flame->ancho;
        }
}


static void hace_mascara( unsigned char *mascara, int ancho )
{
    int             i, j;
    float           x, y, t, h, b;
    unsigned int    color;
    int             kk ;

    for( j=0; j<ancho; j++ )
    for( i=0; i<ancho; i++ )
        {
        y =  1.0-(float)j/(float)(ancho-1);
        t = -2.0 + i*(4.0/(float)(ancho-1));
        t *=.75;

        t = fabs(t);
        if( t>1 ) t=1;
        t = 1-t;

        if( y>.1 )
            t *= 1.0 - (y-.1)/(1.0-.1);
        else
            t *= pow( (y-.0)/(.1-.0), .3 );

        color = t*255.0;
        if( color>255 ) color=255;
        *mascara++ = 255-color;
        }

}


typedef struct
{
    int index;
    int a, r, g, b;
}COLOR;

static long get_color( COLOR *colores, int index )
{
    int r, g, b, a;
    int i;

    for( i=0; i<(4-1); i++ )
        if( index>=colores[i].index && index<colores[i+1].index )
            break;

    r = colores[i].r + (index-colores[i].index)*(colores[i+1].r-colores[i].r)/(colores[i+1].index-colores[i].index);
    g = colores[i].g + (index-colores[i].index)*(colores[i+1].g-colores[i].g)/(colores[i+1].index-colores[i].index);
    b = colores[i].b + (index-colores[i].index)*(colores[i+1].b-colores[i].b)/(colores[i+1].index-colores[i].index);
    a = colores[i].a + (index-colores[i].index)*(colores[i+1].a-colores[i].a)/(colores[i+1].index-colores[i].index);

    return( (a<<24)|(b<<16)|(g<<8)|r );
}

static void hace_pal( long *paleta, COLOR *colores )
{
    int i;
    for( i=0; i<256; i++ )
        paleta[i] = get_color( colores, i );
}


static COLOR colores[4] =
{

{   0,  255, 255,237,183 },
{  92,  255, 242,205, 57 },
{ 180,  255, 211,121, 29 },
{ 256,    0, 185, 49,  4 }

};

long FLAME_Init( FLAME *flame, int an, int sem )
{
    flame->ancho = an;

    flame->mascara = (unsigned char*)malloc( an*an );
    if( !flame->mascara )
        return( 0 );

    hace_mascara( flame->mascara, an );
    hace_pal( flame->paleta, colores );


    kkTURB_Init( flame->ttable, sem );

    flame->octavas=0;
    while( !(an&1) )
        {
        flame->octavas++;
        an>>=1;
        }


    return( 1 );
}

void FLAME_Free( FLAME *flame )
{
    free( flame->mascara );
}


void FLAME_SetColor( FLAME *flame,
                     int i1, int a1, int r1, int g1, int b1,
                     int i2, int a2, int r2, int g2, int b2,
                     int i3, int a3, int r3, int g3, int b3,
                     int i4, int a4, int r4, int g4, int b4 )
{
    COLOR col[4];

    col[0].index = i1;
    col[0].a = a1;
    col[0].r = r1;
    col[0].g = g1;
    col[0].b = b1;
    col[1].index = i2;
    col[1].a = a2;
    col[1].r = r2;
    col[1].g = g2;
    col[1].b = b2;
    col[2].index = i3;
    col[2].a = a3;
    col[2].r = r3;
    col[2].g = g3;
    col[2].b = b3;
    col[3].index = i4;
    col[3].a = a4;
    col[3].r = r4;
    col[3].g = g4;
    col[3].b = b4;


    hace_pal( flame->paleta, col );
}

