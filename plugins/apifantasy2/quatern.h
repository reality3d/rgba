#ifndef QUATERNION_H
#define QUATERNION_H

struct Quaternion
{
    float x,y,z,w;
};
typedef struct Quaternion Quaternion;

void Prod_Quat(Quaternion *q1, Quaternion *q2, Quaternion *result);
float Prod_Esc_Quat(Quaternion *q1, Quaternion *q2);
void Slerp(Quaternion *q1, Quaternion *q2, float t, Quaternion *result);
void Axis_to_Quat(float x, float y, float z, float angle, Quaternion *result);
void Quat_to_RotMatrix(Quaternion *q1, float matrix[4][4]);
#endif
