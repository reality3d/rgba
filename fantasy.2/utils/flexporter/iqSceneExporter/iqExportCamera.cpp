// Precompiled Header
#include "Stdafx.h"



float getRollfromQuat(float x, float y, float z, float w)
{
/* tan(yaw)   =  2(q1q2+q4q3)/ (q42 + q12 - q22- q32)      (5a)
   sin(pitch) = -2(q1q3-q4q2)                                              (5b)
   tan(roll)  =  2(q4q1+q2q3)/ (q4^2 - q1^2 - q2^2+ q3^2)        (5c) */
   
   /*
   q00=quat1\n^2
	q11=quat1\vec\x^2
	q22=quat1\vec\y^2
	q33=quat1\vec\z^2
	
	r11=q00 + q11 - q22 - q33
	r21=2*(quat1\vec\x*quat1\vec\y + quat1\n*quat1\vec\z)
	r31=2*(quat1\vec\x*quat1\vec\z + quat1\n*quat1\vec\y)
	r32=2*(quat1\vec\y*quat1\vec\z + quat1\n*quat1\vec\x)
	r33=q00-q11-q22+q33
	
	tmp=Abs(r31)
	
	If tmp>0.999999 Then
		r12=2*(quat1\vec\x*quat1\vec\y + quat1\n*quat1\vec\z)
		r13=2*(quat1\vec\x*quat1\vec\z + quat1\n*quat1\vec\y)
		
		result\x=radiansToDegrees(0)
		result\y=radiansToDegrees(-(Pi/2)*r31/tmp)
		result\z=radiansToDegrees(ATan2(-r12,-r31*r13))
	Else
		
		result\x=radiansToDegrees(ATan2(r32,r33))
		result\y=radiansToDegrees(ASin(-r31))
		result\z=radiansToDegrees(ATan2(r21,r11))
	EndIf
		
	Return result
*/

 float q00, q11,q22,q33;
 float r11, r12, r13, r21, r31, r32, r33, tmp;
 
 q00=w*w;
 q11=x*x;
 q22=y*y;
 q33=z*z;
 
 r11=q00 + q11 - q22 - q33;
 r21=2*(x*y + w*z);
 r31=2*(x*z + w*y);
 r32=2*(y*z + w*x);
 r33=q00-q11-q22+q33;

 tmp=(float)fabs(r31);

 if(tmp>0.999999)
 {
		r12=2*(x*y + w*z);
		r13=2*(x*z + w*y);
		
/*		result\x=radiansToDegrees(0)
		result\y=radiansToDegrees(-(Pi/2)*r31/tmp)*/
		return (float)atan2(-r12,-r31*r13);
 }
 else
 {
 	return (float)atan2(r21,r11);
 }
		
		/*result\x=radiansToDegrees(ATan2(r32,r33))
		result\y=radiansToDegrees(ASin(-r31))
		result\z=radiansToDegrees(ATan2(r21,r11))*/	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Camera export method.
 *	This method is called once for each exported camera.
 *	\param		camera		[in] a structure filled with current camera information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool iqFormat::ExportCamera(const CameraDescriptor& camera)
{
	int i;
	
	
	// Export common information

	for(i=0;i<numcamaras;i++)
	{
		if(camaras[i].targetid == camera.mTargetID)
		{
			camaras[i].name = strdup((const char *)camera.mName);
			camaras[i].id = camera.mObjectID;
			camaras[i].pos[0] = camera.mPrs.mPosition.x;
			camaras[i].pos[1] = camera.mPrs.mPosition.y;
			camaras[i].pos[2] = camera.mPrs.mPosition.z;
			camaras[i].rot[0] = camera.mPrs.mRotation.x;
			camaras[i].rot[1] = camera.mPrs.mRotation.y;
			camaras[i].rot[2] = camera.mPrs.mRotation.z;
			camaras[i].rot[3] = camera.mPrs.mRotation.w;
			camaras[i].scale[0] = camera.mPrs.mScale.x;
			camaras[i].scale[1] = camera.mPrs.mScale.y;
			camaras[i].scale[2] = camera.mPrs.mScale.z;		
			return true;
		}
	}
	// No lo hemos encontrado, luego se ha exportado la camara antes que el target
	camaras[numcamaras].name = strdup((const char *)camera.mName);
	camaras[numcamaras].id = camera.mObjectID;
	camaras[numcamaras].targetid = camera.mTargetID;
	camaras[numcamaras].pos[0] = camera.mPrs.mPosition.x;
	camaras[numcamaras].pos[1] = camera.mPrs.mPosition.y;
	camaras[numcamaras].pos[2] = camera.mPrs.mPosition.z;
	camaras[numcamaras].rot[0] = camera.mPrs.mRotation.x;
	camaras[numcamaras].rot[1] = camera.mPrs.mRotation.y;
	camaras[numcamaras].rot[2] = camera.mPrs.mRotation.z;
	camaras[numcamaras].rot[3] = camera.mPrs.mRotation.w;
	camaras[numcamaras].scale[0] = camera.mPrs.mScale.x;
	camaras[numcamaras].scale[1] = camera.mPrs.mScale.y;
	camaras[numcamaras].scale[2] = camera.mPrs.mScale.z;
	camaras[numcamaras].fov = (camera.mFOV*3.1415927f)/180.0f;
	camaras[numcamaras].roll = getRollfromQuat(camera.mPrs.mRotation.x,camera.mPrs.mRotation.y,camera.mPrs.mRotation.z,camera.mPrs.mRotation.w); // DE MOMENTO!
	numcamaras++;

	
  return true;

/*	
	// Export camera parameters
	mCameras.Store(camera.mFOV);	// FOV
	// Export clipping planes if needed
	mCameras.Store(camera.mNearClip);						// Near clip
	mCameras.Store(camera.mFarClip);						// Far clip
*/

}

bool iqFormat::ExportAllCameras(CustomArray* array)
{
	int i;

	for(i=0;i<numcamaras;i++)
	{
		WriteTitle("Camara",array);
		// Checkings
		if(!array) return false;

		// Export name
		array   ->StoreASCII("nombre ");
		array	->StoreASCII((const char*)camaras[i].name);
		array	->Store("\n");

		// Export database management information
		array-> StoreASCII("id "); array-> StoreASCII(camaras[i].id);	array	->Store("\n");
		array   ->StoreASCII("targetid "); 
		array	->StoreASCII(camaras[i].targetid);
		array	->Store("\n");


		array	->StoreASCII("position ").StoreASCII(camaras[i].pos[0])	.StoreASCII("  ")
											.StoreASCII(camaras[i].pos[1])	.StoreASCII("  ")
											.StoreASCII(camaras[i].pos[2])

			.StoreASCII("\nrotation ")	.StoreASCII(camaras[i].rot[0])	.StoreASCII("  ")
											.StoreASCII(camaras[i].rot[1])	.StoreASCII("  ")
											.StoreASCII(camaras[i].rot[2])	.StoreASCII("  ")
											.StoreASCII(camaras[i].rot[3])

			.StoreASCII("\nscale ")	.StoreASCII(camaras[i].scale[0])		.StoreASCII("  ")
											.StoreASCII(camaras[i].scale[1])		.StoreASCII("  ")
											.StoreASCII(camaras[i].scale[2])
			.StoreASCII("\nroll ") .StoreASCII(camaras[i].roll) 		.StoreASCII("\nFOV ") .StoreASCII(camaras[i].fov)
			.StoreASCII("\n");

		array	->StoreASCII("position_tgt ").StoreASCII(camaras[i].pos_tgt[0])	.StoreASCII("  ")
											.StoreASCII(camaras[i].pos_tgt[1])	.StoreASCII("  ")
											.StoreASCII(camaras[i].pos_tgt[2])

			.StoreASCII("\nrotation_tgt ")	.StoreASCII(camaras[i].rot_tgt[0])	.StoreASCII("  ")
											.StoreASCII(camaras[i].rot_tgt[1])	.StoreASCII("  ")
											.StoreASCII(camaras[i].rot_tgt[2])	.StoreASCII("  ")
											.StoreASCII(camaras[i].rot_tgt[3])

			.StoreASCII("\nscale_tgt ")	.StoreASCII(camaras[i].scale_tgt[0])		.StoreASCII("  ")
											.StoreASCII(camaras[i].scale_tgt[1])		.StoreASCII("  ")
											.StoreASCII(camaras[i].scale_tgt[2])
			.StoreASCII("\n");
	}
	return true;
}