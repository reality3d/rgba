#include "math3d.h"

#include <math.h>
#include <string.h>

void VecNorm( VEC * v )
{
    float   m, im;

    m = sqrt( v->x*v->x + v->y*v->y + v->z*v->z );
    im = 1.0 / m;
    v->x *= im;
    v->y *= im;
    v->z *= im;
}

void VecCross( VEC * va, VEC * vb, VEC * vc )
{
    vc->x = va->z*vb->y - va->y*vb->z;
    vc->y = va->x*vb->z - va->z*vb->x;
    vc->z = va->y*vb->x - va->x*vb->y;
}

void VecAdd( VEC * va, VEC * vb, VEC * vc )
{
    vc->x = va->x + vb->x;
    vc->y = va->y + vb->y;
    vc->z = va->z + vb->z;
}


void VecInc( VEC * va, VEC * vb )
{
    va->x += vb->x;
    va->y += vb->y;
    va->z += vb->z;
}

void VecSub( VEC * va, VEC * vb, VEC * vc )
{
    vc->x = va->x - vb->x;
    vc->y = va->y - vb->y;
    vc->z = va->z - vb->z;
}

void VecScale( VEC * va, float scale )
{
    va->x *= scale;
    va->y *= scale;
    va->z *= scale;
}

float VecDot( VEC * va, VEC * vb )
{
    float   m;

    m  = vb->x*va->x;
    m += vb->y*va->y;
    m += vb->z*va->z;

    return( m );
}


void MtxInvert( float *inv, float *mod )
{
    memset( inv, 0, 16*4 );
    inv[ 0] = mod[ 0];
    inv[ 1] = mod[ 4];
    inv[ 2] = mod[ 8];
    inv[ 4] = mod[ 1];
    inv[ 5] = mod[ 5];
    inv[ 6] = mod[ 9];
    inv[ 8] = mod[ 2];
    inv[ 9] = mod[ 6];
    inv[10] = mod[10];
    inv[12] = -( mod[0]*mod[12] + mod[1]*mod[13] + mod[ 2]*mod[14] );
    inv[13] = -( mod[4]*mod[12] + mod[5]*mod[13] + mod[ 6]*mod[14] );
    inv[14] = -( mod[8]*mod[12] + mod[9]*mod[13] + mod[10]*mod[14] );
    inv[15] = 1.0;
}
