#ifndef _NIEVE3D_H_
#define _NIEVE3D_H_

#include "psys.h"

typedef struct
{
    float   vx, vy, vz;
    float   x1, y1, z1, x2, y2, z2;

    // private....

    float   vtable[256];
    PSYS    psys;
    float   dt;

}NIEVE3D;

NIEVE3D *NIEVE3D_Init( int num, float width, float x1, float y1, float z1,
                                             float x2, float y2, float z2 );

void    NIEVE3D_Draw( NIEVE3D *nieve, float t );
void    NIEVE3D_End( NIEVE3D *nieve );
void    NIEVE3D_Start( NIEVE3D *ni );

void    NIEVE3D_SetWind( NIEVE3D *ni, float x, float y, float z );

#endif
