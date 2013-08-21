///////////////////////////////////////////////////////////////////////
// VECTOR 3D  -  epsilum
///////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <math.h>
#include "vector.h"

#if defined(LINUX) || defined(MINGW32)
#undef VECTOR_USE_FPU
#endif

void VECT_Set (TVector *v, float x, float y, float z)
{
#ifndef VECTOR_USE_FPU 
    v->x = x;
    v->y = y;
    v->z = z;
#else
    __asm
    {
        MOV    EDI, v              // []
        FLD    DWORD PTR z         // [z]
        FLD    DWORD PTR y         // [y][z]
        FLD    DWORD PTR x         // [x][y][z]
        FSTP   DWORD PTR [EDI+0]   // [y][z]
        FSTP   DWORD PTR [EDI+4]   // [z]
        FSTP   DWORD PTR [EDI+8]   // []
    }
#endif // VECTOR_USE_FPU
}

void VECT_Copy (TVector *dst, TVector *src)
{
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

void VECT_Add (TVector *dst, TVector *u, TVector *v)
{
#ifndef VECTOR_USE_FPU 
    dst->x = u->x + v->x;
    dst->y = u->y + v->y;
    dst->z = u->z + v->z;
#else
    __asm
    {
        MOV    EAX, dst
        MOV    EDI, u
        MOV    ESI, v
        FLD    DWORD PTR [EDI+0]    // [u.x]
        FADD   DWORD PTR [ESI+0]    // [u.x+v.x]
        FLD    DWORD PTR [EDI+4]    // [u.y] [u.x+v.x]
        FADD   DWORD PTR [ESI+4]    // [u.y+v.y] [u.x+v.x]
        FLD    DWORD PTR [EDI+8]    // [u.z] [u.y+v.y] [u.x+v.x]
        FADD   DWORD PTR [ESI+8]    // [u.z+v.z] [u.y+v.y] [u.x+v.x]
        FSTP   DWORD PTR [EAX+8]    // [u.y+v.y] [u.x+v.x]
        FSTP   DWORD PTR [EAX+4]    // [u.x+v.x]
        FSTP   DWORD PTR [EAX+0]    // []
    }
#endif // VECTOR_USE_FPU
}

// u += v
void VECT_Inc (TVector *u, TVector *v)
{
#ifndef VECTOR_USE_FPU 
    u->x += v->x;
    u->y += v->y;
    u->z += v->z;
#else
    __asm
    {
        MOV    EDI, u
        MOV    ESI, v
        FLD    DWORD PTR [EDI+0]    // [u.x]
        FADD   DWORD PTR [ESI+0]    // [u.x+v.x]
        FSTP   DWORD PTR [EDI+0]    // []
        FLD    DWORD PTR [EDI+4]    // [u.y]
        FADD   DWORD PTR [ESI+4]    // [u.y+v.y]
        FSTP   DWORD PTR [EDI+4]    // []
        FLD    DWORD PTR [EDI+8]    // [u.z]
        FADD   DWORD PTR [ESI+8]    // [u.z+v.z]
        FSTP   DWORD PTR [EDI+8]    // []
    }
#endif // VECTOR_USE_FPU
}



void VECT_Sub (TVector *dst, TVector *u, TVector *v)
{
#ifndef VECTOR_USE_FPU 
    dst->x = u->x - v->x;
    dst->y = u->y - v->y;
    dst->z = u->z - v->z;
#else
    __asm
    {
        MOV    EAX, dst             // EAX = dst
        MOV    EDI, u               // EDI = u
        MOV    ESI, v               // ESI = v
        FLD    DWORD PTR [EDI+0]    // [u.x]
        FSUB   DWORD PTR [ESI+0]    // [u.x-v.x]
        FLD    DWORD PTR [EDI+4]    // [u.y] [u.x-v.x]
        FSUB   DWORD PTR [ESI+4]    // [u.y-v.y] [u.x-v.x]
        FLD    DWORD PTR [EDI+8]    // [u.z] [u.y-v.y] [u.x-v.x]
        FSUB   DWORD PTR [ESI+8]    // [u.z-v.z] [u.y-v.y] [u.x-v.x]
        FSTP   DWORD PTR [EAX+8]    // [u.y-v.y] [u.x-v.x]
        FSTP   DWORD PTR [EAX+4]    // [u.x-v.x]
        FSTP   DWORD PTR [EAX+0]    // []
    }
#endif // VECTOR_USE_FPU
}


void VECT_Scale (TVector *dst, TVector *src, float factor)
{
#ifndef VECTOR_USE_FPU 
    dst->x = factor * src->x;
    dst->y = factor * src->y;
    dst->z = factor * src->z;
#else
    __asm
    {
        MOV    EDI, dst              // EDI = dst
        MOV    ESI, src              // ESI = src
        FLD    DWORD PTR factor      // [factor]
        FLD    st(0)                 // [factor] [factor]
        FLD    st(0)                 // [factor] [factor] [factor]
        FMUL   DWORD PTR [ESI+0]     // [src.x * factor] [factor] [factor]
        FSTP   DWORD PTR [EDI+0]     // [factor] [factor]
        FMUL   DWORD PTR [ESI+4]     // [src.y * factor] [factor]
        FSTP   DWORD PTR [EDI+4]     // [factor]
        FMUL   DWORD PTR [ESI+8]     // [src.z * factor]
        FSTP   DWORD PTR [EDI+8]     // []
    }
#endif // VECTOR_USE_FPU
}


//
// Vector Norm
//
// ||u|| = sqrt (xý + yý + zý)
//
float VECT_Norm (TVector *u)
{
#ifndef VECTOR_USE_FPU 
    return sqrt (u->x * u->x + u->y * u->y + u->z * u->z);
#else
    __asm
    {
        MOV    EDI, u               // EDI = u
        FLD    DWORD PTR [EDI+0]    // [x]
        FLD    st(0)                // [x] [x]
        FMUL                        // [x*x]
        FLD    DWORD PTR [EDI+4]    // [y] [x*x]
        FLD    st(0)                // [y] [y] [x*x]
        FMUL                        // [y*y] [x*x]
        FLD    DWORD PTR [EDI+8]    // [z] [y*y] [x*x]
        FLD    st(0)                // [z] [z] [y*y] [x*x]
        FMUL                        // [z*z] [y*y] [x*x]
        FADD                        // [z*z+y*y] [x*x]
        FADD                        // [z*z+y*y+x*x]
        FSQRT                       // [sqrt(z*z+y*y+x*x)]
        // Result [8087]
    }
#endif // VECTOR_USE_FPU
}


void VECT_Normalize (TVector *u)
{
#ifndef VECTOR_USE_FPU 
    float inorm;

    inorm = 1.0f / sqrt (u->x * u->x + u->y * u->y + u->z * u->z);
    u->x *= inorm;
    u->y *= inorm;
    u->z *= inorm;
#else
    __asm
    {
        MOV    EDI, u               // EDI = u
        FLD    DWORD PTR [EDI+0]    // [x]
        FLD    st(0)                // [x] [x]
        FMUL                        // [x*x]
        FLD    DWORD PTR [EDI+4]    // [y] [x*x]
        FLD    st(0)                // [y] [y] [x*x]
        FMUL                        // [y*y] [x*x]
        FLD    DWORD PTR [EDI+8]    // [z] [y*y] [x*x]
        FLD    st(0)                // [z] [z] [y*y] [x*x]
        FMUL                        // [z*z] [y*y] [x*x]
        FADD                        // [z*z+y*y] [x*x]
        FADD                        // [z*z+y*y+x*x]
        FSQRT                       // [sqrt(z*z+y*y+x*x)]
        FLD1                        // [1.0] [sqrt(z*z+y*y+x*x)]
        FDIVR                       // [1.0/sqrt(z*z+y*y+x*x)]
        FLD    DWORD PTR [EDI+0]    // [x] [1.0/sqrt(z*z+y*y+x*x)]
        FMUL   st(0), st(1)         // [x*1.0/sqrt(z*z+y*y+x*x)] [1.0/sqrt(z*z+y*y+x*x)]
        FSTP   DWORD PTR [EDI+0]    // [1.0/sqrt(z*z+y*y+x*x)]
        FLD    DWORD PTR [EDI+4]    // [y] [1.0/sqrt(z*z+y*y+x*x)]
        FMUL   st(0), st(1)         // [y*1.0/sqrt(z*z+y*y+x*x)] [1.0/sqrt(z*z+y*y+x*x)]
        FSTP   DWORD PTR [EDI+4]    // [1.0/sqrt(z*z+y*y+x*x)]
        FMUL   DWORD PTR [EDI+8]    // [z*1.0/sqrt(z*z+y*y+x*x)]
        FSTP   DWORD PTR [EDI+8]    // []
    }
#endif // VECTOR_USE_FPU

}


// 
// Scalar (dot) product
// uúv = u.x*v.x + u.y*v.y + u.z*v.z
//
float VECT_DotProduct (TVector *u, TVector *v)
{
#ifndef VECTOR_USE_FPU 
    return u->x*v->x + u->y*v->y + u->z*v->z;
#else
    __asm
    {
        MOV    EDI, u              // EDI = u
        MOV    ESI, v              // ESI = v
        FLD    DWORD PTR [EDI+0]   // [u.x]
        FMUL   DWORD PTR [ESI+0]   // [u.x*v.x]
        FLD    DWORD PTR [EDI+4]   // [u.y] [u.x*v.x]
        FMUL   DWORD PTR [ESI+4]   // [u.y*v.y] [u.x*v.x]
        FADD                       // [u.y*v.y+u.x*v.x]
        FLD    DWORD PTR [EDI+8]   // [u.z] [u.y*v.y+u.x*v.x]
        FMUL   DWORD PTR [ESI+8]   // [u.z*v.z] [u.y*v.y+u.x*v.x]
        FADD                       // [u.z*v.z+u.y*v.y+u.x*v.x]
        // result [8087]
    }
#endif // VECTOR_USE_FPU

}


//
// Vector (cross) product
//
// | i  j  k  |
// | u1 u2 u3 | = (u2v3 - u3v2)i + (u3v1 - u1v3)y + (u1v2 - u2v1)z
// | v1 v2 v3 |
//
// NOTA: "dst" puede ser tanto "u" como "v"
//
void VECT_CrossProduct (TVector *dst, TVector *u, TVector *v)
{
#ifndef VECTOR_USE_FPU 
    float x, y, z;
    x = u->y * v->z - u->z * v->y;
    y = u->z * v->x - u->x * v->z;
    z = u->x * v->y - u->y * v->x;

    dst->x = x;
    dst->y = y;
    dst->z = z;
#else
    __asm
    {
        MOV    EAX, dst           // EAX = dst
        MOV    EDI, u             // EDI = u
        MOV    ESI, v             // ESI = v
        FLD    DWORD PTR [EDI+8]  // [u.z]
        FMUL   DWORD PTR [ESI+4]  // [u.z * v.y]
        FLD    DWORD PTR [EDI+4]  // [u.y] [u.z * v.y]
        FMUL   DWORD PTR [ESI+8]  // [u.y * v.z] [u.z * v.y]
        FSUBR                     // [u.y * v.z - u.z * v.y]

        FLD    DWORD PTR [EDI+0]  // [u.x] [u.y * v.z - u.z * v.y]
        FMUL   DWORD PTR [ESI+8]  // [u.x * v.z] [u.y * v.z - u.z * v.y]
        FLD    DWORD PTR [EDI+8]  // [u.z] [u.x * v.z] [u.y * v.z - u.z * v.y]
        FMUL   DWORD PTR [ESI+0]  // [u.z * v.x] [u.x * v.z] [u.y * v.z - u.z * v.y]
        FSUBR                     // [u.z * v.x - u.x * v.z] [u.y * v.z - u.z * v.y]

        FLD    DWORD PTR [EDI+4]  // [u.y] [u.z * v.x - u.x * v.z] [u.y * v.z - u.z * v.y]
        FMUL   DWORD PTR [ESI+0]  // [u.y * v.x] [u.z * v.x - u.x * v.z] [u.y * v.z - u.z * v.y]
        FLD    DWORD PTR [EDI+0]  // [u.x] [u.y * v.x] [u.z * v.x - u.x * v.z] [u.y * v.z - u.z * v.y]
        FMUL   DWORD PTR [ESI+4]  // [u.x * v.y] [u.y * v.x] [u.z * v.x - u.x * v.z] [u.y * v.z - u.z * v.y]
        FSUBR                     // [u.x * v.y - u.y * v.x] [u.z * v.x - u.x * v.z] [u.y * v.z - u.z * v.y]

        FSTP   DWORD PTR [EAX+8]  // [u.z * v.x - u.x * v.z] [u.y * v.z - u.z * v.y]
        FSTP   DWORD PTR [EAX+4]  // [u.y * v.z - u.z * v.y]
        FSTP   DWORD PTR [EAX+0]  // []
    }
#endif // VECTOR_USE_FPU
}


void VECT_Print (TVector *u)
{
    printf ("VECT 0x%X = (%3.2f, %3.2f, %3.2f)\n",
            u,
            u->x,
            u->y,
            u->z);
}
