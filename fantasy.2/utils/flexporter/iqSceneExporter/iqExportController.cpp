// Precompiled Header
#include "Stdafx.h"



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Controller export method.
 *	This method is called once for each exported controller.
 *	\param		controller		[in] a structure filled with current controller information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool iqFormat::ExportController(const ControllerDescriptor& controller)
{
	// Controller data
	ControllerData* cdata = controller.mData;

	// Primero vemos qué tipo de controller es, y lo almacenamos
	if(!strcmp(controller.mField,"POSITION"))
	{
		WriteTitle("Spline posicion",&mControllers);
		mControllers.StoreASCII("tipospline posicion\n");
	}
	else if (!strcmp(controller.mField,"ROTATION")) 
	{
	  if(cdata->mType==FLX_CTRL_FLOAT) 
	  {
	  	WriteTitle("Spline roll",&mControllers);
	  	mControllers.StoreASCII("tipospline roll\n");	
	  }
	  else 
	  {
		WriteTitle("Spline rotacion - quaternios",&mControllers);
		mControllers.StoreASCII("tipospline rotacion\n");
	  }
	}
	else return false;

	mControllers.StoreASCII("propietario %d\n",controller.mOwnerID);	// Nodo al que pertenece el controlador

	if(cdata->mMode!=CTRL_KEYFRAMES) // No válido, debemos usar keyframes
	{
		printf("Error: MUST use keyframes, no samples allowed");
		return false;
	}

	// Keyframing path
	KeyframeData* kdata = (KeyframeData*)cdata;
	mControllers.StoreASCII("nkeyframes %d\n",kdata->mNbKeyframes);	// Número de keyframes

	switch(kdata->mScheme)
	{
		case FLX_SCHEME_LINEAR:		// Interpolación lineal, no se usa de momento...
							return false;
							break;


		case FLX_SCHEME_TCB:		// Interpolación por splines, esta es la que se usa...
		{
			if(!strcmp(controller.mField,"POSITION"))
			{
 			 // TCB Point keyframes
			 TCBPointKey* Keys = (TCBPointKey*)kdata->mKeyframes;
			 mControllers.StoreASCII("; keys [tiempo x y z]\n");
			 for(udword i=0;i<kdata->mNbKeyframes;i++)
			 {
				const TCBPointKey& CurKey = Keys[i];
				mControllers.StoreASCII("%d %f %f %f\n",CurKey.mTime,
									CurKey.mVal.x,
									CurKey.mVal.y,
									CurKey.mVal.z);
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
					mControllers.StoreASCII("%d %f\n",CurKey.mTime,
									CurKey.mVal);
				}
			 }
			 else
			 {			 	
			  // TCB Rot keyframes
			  TCBRotKey* Keys = (TCBRotKey*)kdata->mKeyframes;
			  mControllers.StoreASCII("; keys [tiempo x y z w]\n");
			  for(udword i=0;i<kdata->mNbKeyframes;i++)
			  {
				const TCBRotKey& CurKey = Keys[i];
				mControllers.StoreASCII("%d %f %f %f %f\n",CurKey.mTime,
									CurKey.mVal.x,
									CurKey.mVal.y,
									CurKey.mVal.z,
									CurKey.mVal.w);
			  }
			 }
			}
		}
		break;
			
		case FLX_SCHEME_BEZIER:
		{
			printf("Bezier schema not supported yet\n");
			return false;
		}
		break;
	}

	numsplines++;
	return true;
}