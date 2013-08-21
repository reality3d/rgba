#ifndef _TURB_H_
#define _TURB_H_



void  TURB_Init( int sr );
float TURB_Turbulence2d( float *table, float x, float y, long octaves );
float noise2d(float x, float y );
float noise1d(float x);


static float   vtable[256];

#endif
