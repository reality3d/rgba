///////////////////////////////////////////////////////////////////////
// VECTOR 3D  -  epsilum
///////////////////////////////////////////////////////////////////////
#ifndef __VECTOR_H__
#define __VECTOR_H__

#ifdef __cplusplus
extern "C" {
#endif


//
// Use FPU inline ASM
//
#define VECTOR_USE_FPU


typedef struct
{
    float  x, y, z, w;
} TVector;


void  VECT_Set (TVector *v, float x, float y, float z);
void  VECT_Add (TVector *dst, TVector *u, TVector *v);
void  VECT_Inc (TVector *u, TVector *v);
void  VECT_Sub (TVector *dst, TVector *u, TVector *v);
void  VECT_Scale (TVector *dst, TVector *src, float factor);
float VECT_Norm (TVector *u);
float VECT_DotProduct (TVector *u, TVector *v);
void  VECT_CrossProduct (TVector *dst, TVector *u, TVector *v);
void  VECT_Copy (TVector *dst, TVector *src);
void  VECT_Print (TVector *u);


#ifdef __cplusplus
};
#endif

#endif // __VECTOR_H__
