#ifndef _TURB_H_
#define _TURB_H_

#ifdef __cplusplus
extern "C" {
#endif


void  TURB_Init( int sr );

float TURB_Turbulence1d( float x, long octaves, long mode );
float TURB_Turbulence2d( float x, float y, long octaves, long mode );
float TURB_Turbulence3d( float x, float y, float z, long octaves, long modo );

#ifdef __cplusplus
};
#endif


#endif
