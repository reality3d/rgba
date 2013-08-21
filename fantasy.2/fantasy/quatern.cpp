#include <math.h>

#include "mates.h"
#include "quatern.h"

// Producto de quaternios, necesario para traducir de rotaciones relativas
// a absolutas

void Prod_Quat(Quaternion *q1, Quaternion *q2, Quaternion *result)
{
    Vector v1,v2;
    Vector temp1,temp2,temp3;
    float producto;

    v1.x=q1->x;    v1.y=q1->y;    v1.z=q1->z;
    v2.x=q2->x;    v2.y=q2->y;    v2.z=q2->z;

    producto = Prod_esc(&v1,&v2);
    result->w = (q1->w * q2->w)  - producto;

    temp1.x = q2->w * q1->x;
    temp1.y = q2->w * q1->y;
    temp1.z = q2->w * q1->z;
    temp2.x = q1->w * q2->x;
    temp2.y = q1->w * q2->y;
    temp2.z = q1->w * q2->z;
    Prod_vec(&v1,&v2,&temp3);

    result->x = temp1.x + temp2.x + temp3.x;
    result->y = temp1.y + temp2.y + temp3.y;
    result->z = temp1.z + temp2.z + temp3.z;
}

float Prod_Esc_Quat(Quaternion *q1, Quaternion *q2)
{
  return(q1->x*q2->x + q1->y*q2->y + q1->z*q2->z + q1->w*q2->w);
}

// t es el tiempo, entre 0 y 1

void Slerp(Quaternion *q1, Quaternion *q2, float t, Quaternion *result)
{
    float a;
    float const1,const2;

    a = Prod_Esc_Quat(q1,q2);
    a = acos (a);

    if(a < 0.00001f) // hay que evitar divisiones por 0, luego hacemos interpolación lineal
    {
// ¿Por qué no me funciona esto?

/*     const1 = 1 - t ;
     const2 = t;*/ 


     result->x = q2->x;
     result->y = q2->y; 
     result->z = q2->z;
     result->w = q2->w;
     return;
    }
    else
    {
     const1 = (sin((1-t)*a) / sin(a));
     const2 = (sin(t*a) / sin (a));
    }
  
    result->x = (q1->x * const1) + (q2->x * const2);
    result->y = (q1->y * const1) + (q2->y * const2);
    result->z = (q1->z * const1) + (q2->z * const2);
    result->w = (q1->w * const1) + (q2->w * const2);

}


// Atenci¢n: aqu¡ estoy haciendo una chapucilla, al invertir el signo del
// quaternio. Con esto creo que estoy corrigiendo un bug en otro sitio...

void Axis_to_Quat(float x, float y, float z, float angle, Quaternion *result)
{
    angle/=2.0f;

    result->x = x * sin(angle);
    result->y = y * sin(angle);
    result->z = z * sin(angle);
    result->w = cos(angle);
}

// Traduce un quaternio en una matriz de rotaci¢n 4x4

void Quat_to_RotMatrix(Quaternion *q1, float matrix[4][4])
{
    matrix[0][0] = 1.0f - 2.0f * (q1->y*q1->y + q1->z*q1->z);
    matrix[0][1] = 2.0f * (q1->x*q1->y - q1->w*q1->z);
    matrix[0][2] = 2.0f * (q1->x*q1->z + q1->w*q1->y);
    matrix[0][3] = 0.0f;

    matrix[1][0] = 2.0f * (q1->x*q1->y + q1->w*q1->z);
    matrix[1][1] = 1.0f - 2.0f * (q1->x*q1->x + q1->z*q1->z);
    matrix[1][2] = 2.0f * (q1->y*q1->z - q1->w*q1->x);
    matrix[1][3] = 0.0f;

    matrix[2][0] = 2.0f * (q1->x*q1->z - q1->w*q1->y);
    matrix[2][1] = 2.0f * (q1->y*q1->z + q1->w*q1->x);
    matrix[2][2] = 1.0f - 2.0f * (q1->x*q1->x + q1->y*q1->y);
    matrix[2][3] = 0.0f;

    matrix[3][0] = 0.0f;
    matrix[3][1] = 0.0f;
    matrix[3][2] = 0.0f;
    matrix[3][3] = 1.0f;
}
