// Precompiled Header
#include "Stdafx.h"

#define U3D_CTRL_POSITION	(unsigned char)0
#define U3D_CTRL_ROTATION	(unsigned char)1
#define U3D_CTRL_SCALE		(unsigned char)2
#define U3D_CTRL_FOV		(unsigned char)3
#define U3D_CTRL_ROLL		(unsigned char)4
#define U3D_CTRL_UNKNOWN	(unsigned char)5

#define U3D_SCHEME_LINEAR	(unsigned char)0
#define U3D_SCHEME_TCB		(unsigned char)1
#define U3D_SCHEME_BEZIER	(unsigned char)2

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Controller export method.
 *	This method is called once for each exported controller.
 *	\param		controller		[in] a structure filled with current controller information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool UtoFormat::ExportController(const ControllerDescriptor& controller)
{
	// Controller data
	ControllerData* cdata = controller.mData;

	// Primero vemos qué tipo de controller es, y lo almacenamos
	if(!strcmp(controller.mField,"POSITION"))		mControllers.Store(U3D_CTRL_POSITION);
	else if (!strcmp(controller.mField,"ROTATION")) 
	{
	  if(cdata->mType==FLX_CTRL_FLOAT) 	mControllers.Store(U3D_CTRL_ROLL);
		else mControllers.Store(U3D_CTRL_ROTATION);
	}
	else if (!strcmp(controller.mField,"SCALE"))	mControllers.Store(U3D_CTRL_SCALE);
	else if (!strcmp(controller.mField,"FOVTRACK"))	mControllers.Store(U3D_CTRL_FOV);
	else 
	{
		mControllers.Store(U3D_CTRL_UNKNOWN);
		mControllers.Store(0xff);
		return true;
	}

	mControllers.Store(controller.mObjectID);	// Identificador del controlador
	mControllers.Store(controller.mOwnerID);	// Nodo al que pertenece el controlador

	if(cdata->mMode!=CTRL_KEYFRAMES) // No válido, debemos usar keyframes
	{
		printf("Error: MUST use keyframes, no samples allowed");
		return false;
	}

	// Keyframing path
	KeyframeData* kdata = (KeyframeData*)cdata;
	mControllers.Store(kdata->mNbKeyframes);	// Número de keyframes

	switch(kdata->mScheme)
	{
		case FLX_SCHEME_LINEAR:		// Interpolación lineal, no se usa de momento...
		{
			mControllers.Store(U3D_SCHEME_LINEAR);	// Ponemos el flag
			if(!strcmp(controller.mField,"POSITION"))
			{
			 // Linear Point keyframes
			 LinearPointKey* Keys = (LinearPointKey*)kdata->mKeyframes;
			 for(udword i=0;i<kdata->mNbKeyframes;i++)
			 {
				const LinearPointKey& CurKey = Keys[i];
				mControllers.Store(CurKey.mTime);		// Tiempo
				mControllers.Store(CurKey.mVal.x);		// Posición x,y,z
				mControllers.Store(CurKey.mVal.y);
				mControllers.Store(CurKey.mVal.z);
			 } 
			}
			else if(!strcmp(controller.mField,"ROTATION"))
			{
			 if(cdata->mType==FLX_CTRL_FLOAT)
			 {
				// Linear float keyframes
				LinearFloatKey* Keys = (LinearFloatKey*)kdata->mKeyframes;
				for(udword i=0;i<kdata->mNbKeyframes;i++)
				{
					const LinearFloatKey& CurKey = Keys[i];
					mControllers.Store(CurKey.mTime);		// Tiempo
					mControllers.Store(CurKey.mVal);		// Valor (flotante)
				}
			 }
			 else			 
			 {
				 // Linear Rot keyframes
				LinearRotKey* Keys = (LinearRotKey*)kdata->mKeyframes;

				for(udword i=0;i<kdata->mNbKeyframes;i++)
				{
					const LinearRotKey& CurKey = Keys[i];
					mControllers.Store(CurKey.mTime);		// Tiempo
					mControllers.Store(CurKey.mVal.x);		// Quaternio de rotación x,y,z,w
					mControllers.Store(CurKey.mVal.y);
					mControllers.Store(CurKey.mVal.z);
					mControllers.Store(CurKey.mVal.w);
				} 
			 }
			}
			else if(!strcmp(controller.mField,"SCALE"))			
			{
			 // Linear scale keyframes
			 LinearScaleKey* Keys = (LinearScaleKey*)kdata->mKeyframes;
			 for(udword i=0;i<kdata->mNbKeyframes;i++)
			 {
				const LinearScaleKey& CurKey = Keys[i];
				mControllers.Store(CurKey.mTime);		// Tiempo
				mControllers.Store(CurKey.s.x);			// Escala en x,y,z
				mControllers.Store(CurKey.s.y);
				mControllers.Store(CurKey.s.z);
				mControllers.Store(CurKey.q.x);			// No entiendo muy bien para qué es este quaternio...
				mControllers.Store(CurKey.q.y);
				mControllers.Store(CurKey.q.z);
				mControllers.Store(CurKey.q.w);
			 }
			}
			else if(!strcmp(controller.mField,"FOVTRACK"))			
			{
			 // Linear float keyframes
			 LinearFloatKey* Keys = (LinearFloatKey*)kdata->mKeyframes;
			 for(udword i=0;i<kdata->mNbKeyframes;i++)
			 {
				const LinearFloatKey& CurKey = Keys[i];
				mControllers.Store(CurKey.mTime);		// Tiempo
				mControllers.Store(CurKey.mVal);		// Valor (flotante)
			 }
			}


			else 
			{
				printf("Mierda, hay un problema\n");
				return false;
			}
		}
		break;


		case FLX_SCHEME_TCB:		// Interpolación por splines, esta es la que se usa...
		{
			unsigned char flags = 0;


			mControllers.Store(U3D_SCHEME_TCB);	// Ponemos el flag
			if(!strcmp(controller.mField,"POSITION"))
			{
 			 // TCB Point keyframes
			 TCBPointKey* Keys = (TCBPointKey*)kdata->mKeyframes;
			 for(udword i=0;i<kdata->mNbKeyframes;i++)
			 {
				const TCBPointKey& CurKey = Keys[i];
				flags=0;

				mControllers.Store(CurKey.mTime);		// Tiempo		
				mControllers.Store(CurKey.mVal.x);		// Posición x,y,z
				mControllers.Store(CurKey.mVal.y);
				mControllers.Store(CurKey.mVal.z);

				if(fabs(CurKey.mTens) > 0.0000000001f) flags |= 1;
				if(fabs(CurKey.mCont) > 0.0000000001f) flags |= 2;
				if(fabs(CurKey.mBias) > 0.0000000001f) flags |= 4;
				if(fabs(CurKey.mEaseIn) > 0.0000000001f) flags |= 8;
				if(fabs(CurKey.mEaseOut) > 0.0000000001f) flags |= 16;
				mControllers.Store(flags);		// Flags

				if(flags & 1)  mControllers.Store(CurKey.mTens);		// Tension
				if(flags & 2)  mControllers.Store(CurKey.mCont);		// Continuity
				if(flags & 4)  mControllers.Store(CurKey.mBias);		// Bias
				if(flags & 8)  mControllers.Store(CurKey.mEaseIn);		// Ease in
				if(flags & 16) mControllers.Store(CurKey.mEaseOut);	// Ease out
			 } 
			}
			else if(!strcmp(controller.mField,"ROTATION"))
			{
			 if(cdata->mType==FLX_CTRL_FLOAT)
			 {
				// TCB float keyframes
				TCBFloatKey* Keys = (TCBFloatKey*)kdata->mKeyframes;
				for(udword i=0;i<kdata->mNbKeyframes;i++)
				{
					const TCBFloatKey& CurKey = Keys[i];
					flags=0;

					mControllers.Store(CurKey.mTime);		// Tiempo
					mControllers.Store(CurKey.mVal);		// Valor (flotante)
				
			   	    if(fabs(CurKey.mTens) > 0.0000000001f) flags |= 1;
					if(fabs(CurKey.mCont) > 0.0000000001f) flags |= 2;
					if(fabs(CurKey.mBias) > 0.0000000001f) flags |= 4;
					if(fabs(CurKey.mEaseIn) > 0.0000000001f) flags |= 8;
					if(fabs(CurKey.mEaseOut) > 0.0000000001f) flags |= 16;
					mControllers.Store(flags);		// Flags

					if(flags & 1)  mControllers.Store(CurKey.mTens);		// Tension
					if(flags & 2)  mControllers.Store(CurKey.mCont);		// Continuity
					if(flags & 4)  mControllers.Store(CurKey.mBias);		// Bias
					if(flags & 8)  mControllers.Store(CurKey.mEaseIn);		// Ease in
					if(flags & 16) mControllers.Store(CurKey.mEaseOut);	// Ease out
				}
			 }
			 else
			 {				
				// TCB Rot keyframes
				 TCBRotKey* Keys = (TCBRotKey*)kdata->mKeyframes;
				 for(udword i=0;i<kdata->mNbKeyframes;i++)
				 {
					const TCBRotKey& CurKey = Keys[i];
					flags=0;

					mControllers.Store(CurKey.mTime);		// Tiempo				
	
					mControllers.Store(CurKey.mVal.x);		// Quaternio de rotación x,y,z,w
					mControllers.Store(CurKey.mVal.y);
					mControllers.Store(CurKey.mVal.z);
					mControllers.Store(CurKey.mVal.w);
	
					if(fabs(CurKey.mTens) > 0.0000000001f) flags |= 1;
					if(fabs(CurKey.mCont) > 0.0000000001f) flags |= 2;
					if(fabs(CurKey.mBias) > 0.0000000001f) flags |= 4;
					if(fabs(CurKey.mEaseIn) > 0.0000000001f) flags |= 8;
					if(fabs(CurKey.mEaseOut) > 0.0000000001f) flags |= 16;
					mControllers.Store(flags);		// Flags

					if(flags & 1)  mControllers.Store(CurKey.mTens);		// Tension
					if(flags & 2)  mControllers.Store(CurKey.mCont);		// Continuity
					if(flags & 4)  mControllers.Store(CurKey.mBias);		// Bias
					if(flags & 8)  mControllers.Store(CurKey.mEaseIn);		// Ease in
					if(flags & 16) mControllers.Store(CurKey.mEaseOut);	// Ease out
				}
			 }
			}
			else if(!strcmp(controller.mField,"SCALE"))			
			{
			 // TCB scale keyframes
			 TCBScaleKey* Keys = (TCBScaleKey*)kdata->mKeyframes;
			 for(udword i=0;i<kdata->mNbKeyframes;i++)
			 {
				const TCBScaleKey& CurKey = Keys[i];
				flags=0;

				mControllers.Store(CurKey.mTime);		// Tiempo
				
				mControllers.Store(CurKey.s.x);			// Escala en x,y,z
				mControllers.Store(CurKey.s.y);
				mControllers.Store(CurKey.s.z);
				mControllers.Store(CurKey.q.x);			// No entiendo muy bien para qué es este quaternio...
				mControllers.Store(CurKey.q.y);
				mControllers.Store(CurKey.q.z);
				mControllers.Store(CurKey.q.w);

				if(fabs(CurKey.mTens) > 0.0000000001f) flags |= 1;
				if(fabs(CurKey.mCont) > 0.0000000001f) flags |= 2;
				if(fabs(CurKey.mBias) > 0.0000000001f) flags |= 4;
				if(fabs(CurKey.mEaseIn) > 0.0000000001f) flags |= 8;
				if(fabs(CurKey.mEaseOut) > 0.0000000001f) flags |= 16;
				mControllers.Store(flags);		// Flags

				if(flags & 1)  mControllers.Store(CurKey.mTens);		// Tension
				if(flags & 2)  mControllers.Store(CurKey.mCont);		// Continuity
				if(flags & 4)  mControllers.Store(CurKey.mBias);		// Bias
				if(flags & 8)  mControllers.Store(CurKey.mEaseIn);		// Ease in
				if(flags & 16) mControllers.Store(CurKey.mEaseOut);	// Ease out
			 }
			}
			else if(!strcmp(controller.mField,"FOVTRACK"))			
			{
			 // TCB float keyframes
			 TCBFloatKey* Keys = (TCBFloatKey*)kdata->mKeyframes;
			 for(udword i=0;i<kdata->mNbKeyframes;i++)
			 {
				const TCBFloatKey& CurKey = Keys[i];
				flags=0;

				mControllers.Store(CurKey.mTime);		// Tiempo
				mControllers.Store(CurKey.mVal*180.0f/PI);		// Valor (flotante)

				if(fabs(CurKey.mTens) > 0.0000000001f) flags |= 1;
				if(fabs(CurKey.mCont) > 0.0000000001f) flags |= 2;
				if(fabs(CurKey.mBias) > 0.0000000001f) flags |= 4;
				if(fabs(CurKey.mEaseIn) > 0.0000000001f) flags |= 8;
				if(fabs(CurKey.mEaseOut) > 0.0000000001f) flags |= 16;
				mControllers.Store(flags);		// Flags

				if(flags & 1)  mControllers.Store(CurKey.mTens);		// Tension
				if(flags & 2)  mControllers.Store(CurKey.mCont);		// Continuity
				if(flags & 4)  mControllers.Store(CurKey.mBias);		// Bias
				if(flags & 8)  mControllers.Store(CurKey.mEaseIn);		// Ease in
				if(flags & 16) mControllers.Store(CurKey.mEaseOut);	// Ease out			 
			 }
			}
			else 
			{
				printf("Mierda, hay un problema\n");
				return false;
			}
		}
		break;
			
		case FLX_SCHEME_BEZIER:
		{
			printf("Bezier schema not supported yet\n");
			mControllers.Store(U3D_SCHEME_BEZIER);	// Ponemos el flag
			return false;
		}
		break;



	}

	return true;
}