#ifndef _TURB_H_
#define _TURB_H_

void  kkTURB_Init( float *table, int sr );

float kkTURB_Turbulence3d( float *table, float x, float y, float z, long octaves );

#endif
