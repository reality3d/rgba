#ifndef MATH_H

#define MATH_H
#include "3dworld.h"
void copiar_matrices (float dest[4][4], float source[4][4]) ;
void matriz_unidad(float matrix[4][4]) ;
void translate_matrix (float xt, float yt,float zt,float matrix[4][4]);
void scale_matrix (float xs, float ys, float zs,float matrix[4][4]);
void SwapYZMatrix(float matrix[4][4]);
void Mult_Vector_Matrix(Vector *v1, float matrix[4][4], Vector *result);
void Mult_Vector4_Matrix(Vector4 *v1, float matrix[4][4], Vector4 *result);
void Mult_Vector_Matrix3(Vector *v1, float matrix[4][4], Vector *result);
void Transpose(float matrix[4][4]);
void calcula_normales_poligono(object_type *obj);
void calcula_normales_vertice(object_type *obj);
float Prod_esc(Vector *v1,Vector *v2);
void Prod_vec(Vector *v1, Vector *v2, Vector *result);
float Norma(Vector *v1);
void Normaliza(Vector *v1);
void Prod_esc_vector(float escalar,Vector *v);
void CalcPlane(Point v[3], Plane *plane);
void Invert_Matrix(float m[4][4]);
void AssignGLNormals(object_type *obj);
void multiplicar_matrices (float result[4][4], float mat1[4][4], float mat2[4][4]);
#endif	/*  */

