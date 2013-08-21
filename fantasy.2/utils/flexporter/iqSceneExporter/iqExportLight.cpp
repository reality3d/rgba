
// Precompiled Header
#include "Stdafx.h"


#define U3D_LTYPE_OMNI		0
#define U3D_LTYPE_TSPOT		1
#define U3D_LTYPE_DIR		2
#define U3D_LTYPE_FSPOT		3
#define U3D_LTYPE_TDIR		4


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Light export method.
 *	This method is called once for each exported light.
 *	\param		light		[in] a structure filled with current light information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool iqFormat::ExportLight(const LightDescriptor& light)
{
	WriteTitle("Luz",&mLights);
	ExportBasicInfo(&light, &mLights,false);
/*
	if (light.mLightType==FLX_LTYPE_OMNI)					mLights.Store((unsigned char)U3D_LTYPE_OMNI);	// Omni (la única soportada por el fantasy)
	else if (light.mLightType==FLX_LTYPE_TSPOT)				mLights.Store((unsigned char)U3D_LTYPE_TSPOT);	// targeted spot light
	else if (light.mLightType==FLX_LTYPE_DIR)				mLights.Store((unsigned char)U3D_LTYPE_DIR);	// directional light
	else if (light.mLightType==FLX_LTYPE_FSPOT)				mLights.Store((unsigned char)U3D_LTYPE_FSPOT);	// free spot light
	else if (light.mLightType==FLX_LTYPE_TDIR)				mLights.Store((unsigned char)U3D_LTYPE_TDIR);	// targeted directional light

	mLights.Store(light.mColor.x);						// Color
	mLights.Store(light.mColor.y);						// Color
	mLights.Store(light.mColor.z);						// Color
	mLights.Store(light.mAttenStart);					//  atten start
	mLights.Store(light.mAttenEnd);						//  atten end
	mLights.Store(light.mUseAtten);						// Use atten
	mLights.Store(light.mIntensity);					// Intensity (multiplier)
*/
	mLights.StoreASCII("color %f %f %f\n",light.mColor.x,light.mColor.y,light.mColor.z);
	mLights.StoreASCII("atenuacion %d %f %f\n", light.mUseAtten,light.mAttenStart,light.mAttenEnd);
	mLights.StoreASCII("intensidad %f\n",light.mIntensity);

	return true;
}
