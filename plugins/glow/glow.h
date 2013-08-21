#ifndef _GLOW_H_
#define _GLOW_H_

typedef struct
{
    int             xres, yres;
    unsigned int    tid;
    float           ox, oy;

    int             running;
    float           pesos[8];
}GLOW;

GLOW *GLOW_Init( int xres, int yres );
void GLOW_End( GLOW *glow );
void GLOW_DoFrame( GLOW *glow );

void GLOW_SetPesos( GLOW *g,
                    float p0, float p1, float p2, float p3,
                    float p4, float p5, float p6, float p7 );

#endif
