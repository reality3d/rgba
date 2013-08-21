#ifndef _PSYS_H_
#define _PSYS_H_

#include "math3d.h"

typedef struct
{
    VEC     pos;
//    VEC     col;
//    float   al;
}PART;

typedef struct
{
    PART            *p;
    int             n;
    unsigned int    tid;
    unsigned int    *radix_buf;
    unsigned int    *radix_aux;
    float           width;
}PSYS;

void PSYS_Free( PSYS *psys );
int  PSYS_Init( PSYS *psys, int n, float width );
void PSYS_Draw( PSYS *psys );

#endif
