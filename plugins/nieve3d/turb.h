#ifndef _TURB_H_
#define _TURB_H_

void  TURB_Init( float *table, int sr );

float TURB_Turbulence2d( float *table, float x, float y, long octaves );
float TURB_Turbulence4d( float *table, float x, float y, float z, float w, long octaves );

#endif
