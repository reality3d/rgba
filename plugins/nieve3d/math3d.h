#ifndef _MATH3D_H_
#define _MATH3D_H_


typedef struct
{
    float   x, y, z;
}VEC;


void  VecNorm( VEC *v );
void  VecCross( VEC *va, VEC *vb, VEC *vc );
void  VecAdd( VEC *va, VEC *vb, VEC *vc );
void  VecSub( VEC *va, VEC *vb, VEC *vc );
void  VecInc( VEC *va, VEC *vb );
void  VecScale( VEC *va, float scale );
float VecDot( VEC *va, VEC *vb );
void  MtxInvert( float *inv, float *mod );

#endif
