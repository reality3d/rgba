#ifndef _FUEGO_H_
#define _FUEGO_H_


#include <glt/glt.h>
#include "flame.h"

typedef struct
{
    int     an;
    float   velo1;
    float   velo2;

    // private....

    FLAME   flame;
    //int     tid;
    TTexture *tid;
    long    *text;


}FUEGO;

FUEGO *FUEGO_Init( int sem, int ancho, char *texture_id );

void FUEGO_Draw( FUEGO *fuego, float t );
void FUEGO_End( FUEGO *fuego );
void FUEGO_Start( FUEGO *fuego );

void FUEGO_SetSpeed( FUEGO *f, float v1, float v2 );

void FUEGO_SetColor( FUEGO *f,
                     int i1, int a1, int r1, int g1, int b1,
                     int i2, int a2, int r2, int g2, int b2,
                     int i3, int a3, int r3, int g3, int b3,
                     int i4, int a4, int r4, int g4, int b4 );

#endif
