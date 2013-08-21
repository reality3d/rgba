#ifndef _FLAME_H_
#define _FLAME_H_

typedef struct
{
    unsigned char  *mascara;
    long           paleta[256];
    float          ttable[256];
    int            ancho;
    int            octavas;
}FLAME;


long FLAME_Init( FLAME *flame, int an, int sem );
void FLAME_DoFrame( long *buffer, FLAME *flame, float time1, float time2 );
void FLAME_Free( FLAME *flame );

void FLAME_SetColor( FLAME *flame,
                     int i1, int a1, int r1, int g1, int b1,
                     int i2, int a2, int r2, int g2, int b2,
                     int i3, int a3, int r3, int g3, int b3,
                     int i4, int a4, int r4, int g4, int b4 );

#endif
