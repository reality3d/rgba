//--------------------------------------------------------------------------//
// iq . 2003 . code for the Paradise 64 kb intro by RGBA                    //
//--------------------------------------------------------------------------//

#ifndef _TURB_H_
#define _TURB_H_

extern "C"
{

void  OTROTURB_New( float *table, int sr );

float OTROTURB_Turbulence3d( float *table, float x, float y, float z, long octaves );
float OTROTURB_Turbulence4d( float *table, float x, float y, float z, float w, long octaves );

}

#endif
