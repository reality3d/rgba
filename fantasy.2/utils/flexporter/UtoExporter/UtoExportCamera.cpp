// Precompiled Header
#include "Stdafx.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Camera export method.
 *	This method is called once for each exported camera.
 *	\param		camera		[in] a structure filled with current camera information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool UtoFormat::ExportCamera(const CameraDescriptor& camera)
{

	// Export common information
	ExportBasicInfo(&camera, &mCameras,true);

	// Export camera parameters
	mCameras.Store(camera.mFOV);	// FOV


	// Cuál de estos dos es el que se interpreta como near range en 3ds? Parece que el segundo

	// Environment ranges
/*	mCameras
		.StoreASCII("\nENVIRONMENT RANGES:\n")
		.StoreASCII("\nNear range: ")				.StoreASCII(camera.mEnvNearRange)					// Near range
		.StoreASCII("\nFar range: ")				.StoreASCII(camera.mEnvFarRange)					// Far range
	// Environment display
		.StoreASCII("\nEnv display: ")				.StoreASCII(camera.mEnvDisplay)						// Environment display
		.StoreASCII("\n");*/

	// Export environment ranges for fog and similar...
	mCameras.Store(camera.mEnvNearRange);						// Near range
	mCameras.Store(camera.mEnvFarRange);						// Far range

	return true;
}