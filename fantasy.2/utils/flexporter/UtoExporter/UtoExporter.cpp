///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	This file contains the Void Format exporter plug-in for Flexporter.
 *	\file		VoidExporter.cpp
 *	\author		Pierre Terdiman
 *	\date		May, 6, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include "Stdafx.h"

static UtoFormat		gUtoExporter;		// Gloabl exporter instance
static ExportSettings	gSettings;			// Global export settings

#define U3D_VERSION_MAJOR	0
#define U3D_VERSION_MINOR	9

// FLEXPORTER Identification Callbacks

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Gives a brief exporter description. That string is displayed inside MAX, in the Options Panel.
 *	\relates	VoidFormat
 *	\fn			ExporterDescription()
 *	\return		a description string
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* ExporterDescription()
{ 
	return "Utopian's Exporter";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Returns the format's extension. That string is displayed inside MAX, in the Options Panel.
 *	\relates	VoidFormat
 *	\fn			FormatExtension()
 *	\return		an extension string
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* FormatExtension()
{
	return "u3d";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Returns the one and only exporter instance.
 *	\relates	VoidFormat
 *	\fn			GetExporter()
 *	\return		pointer to the global exporter instance.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ExportFormat* GetExporter()
{
	return &gUtoExporter;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Returns the default exportation settings for this format. This is the right place to initialize the default settings for your format.
 *	\relates	VoidFormat
 *	\fn			GetDefaultSettings()
 *	\return		pointer to the global ExportSettings instance.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ExportSettings* GetDefaultSettings()
{

	// General settings
	gSettings.mExportWholeScene				= true;
	gSettings.mExportVisTrack				= false;
	gSettings.mExportHiddenNodes			= false;
	gSettings.mAutoUnload					= false;
	gSettings.mAutoLoad						= false;
	gSettings.mGlobalScale					= 1.0f;
	gSettings.mLocale						= FLX_LOCALE_ENGLISH;
	gSettings.mExportPHYSIQUE				= true;
	gSettings.mExportSKIN					= true;
	gSettings.mExportBIPED					= true;
	gSettings.mAlwaysExportMotion			= false;
	// Mesh settings
	gSettings.mExpUVW						= true;
	gSettings.mDiscardW						= true;
	gSettings.mExpVTXColor					= true;	// Va a exportar los vertex color, si los hay
	gSettings.mUseSmgrp						= false;
	gSettings.mRemoveScaling				= true; // Bye bye scaling
	gSettings.mConvertToD3D					= true;
	gSettings.mAbsolutePRS					= false;
	gSettings.mConvexHull					= false;
	gSettings.mBoundingSphere				= true;
	gSettings.mInertiaTensor				= false;
	gSettings.mEdgeVis						= true; // Para que exporte la visibilidad de los ejes
	gSettings.mMakeManifold					= false;
	gSettings.mExpTargetNodes				= true;
	gSettings.mDiscardInstances				= false;
	// Consolidation settings
	gSettings.mConsolidateMesh				= false;
	gSettings.mComputeFaceNormals			= false;
	gSettings.mComputeVertexNormals			= false;  
	gSettings.mExportNormalInfo				= false; 
	gSettings.mWeightNormalsByArea			= false;
	gSettings.mCacheOptimize				= false;
	// Material settings
	gSettings.mForceAmbient					= false;
	gSettings.mForceDiffuse					= false;
	gSettings.mForceSpecular				= false;
	gSettings.mForceFilter					= false;
	gSettings.mExpMtlCtrl					= false;
	gSettings.mMaterialSharing				= false;
	// Texture settings
	gSettings.mOpacityInAlpha				= false;
	gSettings.mTexnameOnly					= true;
	gSettings.mKeepAmbientTexture			= false;
	gSettings.mKeepDiffuseTexture			= true;
	gSettings.mKeepSpecularTexture			= false;
	gSettings.mKeepShininessTexture			= false;
	gSettings.mKeepShiningStrengthTexture	= false;
	gSettings.mKeepSelfIllumTexture			= false;
	gSettings.mKeepOpacityTexture			= true;
	gSettings.mKeepFilterTexture			= false;
	gSettings.mKeepBumpTexture				= false;
	gSettings.mKeepReflexionTexture			= false;
	gSettings.mKeepRefractionTexture		= false;
	gSettings.mKeepDisplacementTexture		= false;
	gSettings.mTexMaxSize					= 0;
	gSettings.mFlipHorizontal				= false;
	gSettings.mFlipVertical					= false;
	gSettings.mQuantizeTextures				= false;
	// Camera settings
	gSettings.mExpCameras					= true;
	gSettings.mExpFOVCtrl					= true;
	gSettings.mExpCamCtrl					= false;
	// Light settings
	gSettings.mExpLights					= true;
	gSettings.mExpLitCtrl					= true;	
	gSettings.mExpProjMaps					= false;
	gSettings.mComputeVtxColors				= false;
	gSettings.mComputeShadows				= false;
	gSettings.mColorSmoothing				= false;
	// Animation settings
	gSettings.mSingleFrame					= false; // Export full animation
	gSettings.mSampling						= false;
	gSettings.mSamplingRate					= 1;
	// Shape settings
	gSettings.mExpShapes					= true;
	gSettings.mShapeToMesh					= true;
	// Helper settings
	gSettings.mExpHelpers					= true;
	// IK settings
	gSettings.mExportIK						= false;


	return &gSettings;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Returns the enabled/disabled status for all settings. This is the right place to disable options your own format doesn't support.
 *	\relates	VoidFormat
 *	\fn			GetEnabledSettings()
 *	\return		pointer to a global Enabled instance.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Enabled* GetEnabledSettings()
{
	static Enabled Settings;

// Aquí habría que meter todos aquellos settings que realmente queremos que no se puedan tocar

	Settings.mOpacityInAlpha	= false;
	Settings.mTexnameOnly		= false;
	Settings.mFlipHorizontal	= false;
	Settings.mFlipVertical		= false;
	Settings.mQuantizeTextures	= false;

	// Consolidation settings
	Settings.mConsolidateMesh				= false;
	Settings.mComputeFaceNormals			= false;
	Settings.mComputeVertexNormals			= false;  
	Settings.mExportNormalInfo				= false; 
	Settings.mWeightNormalsByArea			= false;
	Settings.mCacheOptimize				= false;


	return &Settings;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Flexporter callback.
 *	Returns the FLEXPORTER SDK Version.
 *	\relates	VoidFormat
 *	\fn			Version()
 *	\return		FLEXPORTER_VERSION
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Version()
{
	return FLEXPORTER_VERSION;
}









///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	A void exporter plug-in for Flexporter..
 *	\class		VoidFormat
 *	\author		Pierre Terdiman
 *	\version	1.0
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Constructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UtoFormat::UtoFormat()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Destructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UtoFormat::~UtoFormat()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Plug-in initialization method.
 *	This method is called once before each export. When this method is called, the mSettings and mFilename members of the base format are valid.
 *	\param		motion		[in] true for pure motion files.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool UtoFormat::Init(bool motion)
{
	// Primero ponemos el magic (UTO!)

	mGeneral.Store((char)'U').Store((char)'T').Store((char)'O').Store((char)'!');
	mGeneral.Store((unsigned char)U3D_VERSION_MAJOR);
	mGeneral.Store((unsigned char)U3D_VERSION_MINOR);
	
	// Y luego vamos metiendo cada parte de información
	
	mGeneral		.Store("MAIN"); // "MAIN" es la firma del chunk, simplemente.
	mGeomObjects	.Store("MESH");
	mCameras		.Store("CAMS");
	mCameraTargets	.Store("CTGT");
	mBones			.Store("BONE");	nBones = 0;
	mLights			.Store("LITE");
//	mTexmaps		.Store("TEXMCHUNK").Store((udword)CHUNK_TEXM_VER);
	mMaterials		.Store("MATS");
	mControllers	.Store("CTRL");
	nObjects = 0;


	mIsMotionFile	= motion; // Y eto pa que coño e?

	return true;


	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Scene export method.
 *	This method is called once to export the scene global information.
 *	\param		maininfo	[in] main info structure
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool UtoFormat::SetSceneInfo(const MainDescriptor& maininfo)
{

	// Save time info for later
	CopyMemory(&mTimeInfo, &maininfo.mTime, sizeof(MAXTimeInfo));

	// Export time info
	mGeneral.Store((long)(mTimeInfo.mStartTime / mTimeInfo.mDeltaTime));	// First frame
	mGeneral.Store((long)(mTimeInfo.mEndTime / mTimeInfo.mDeltaTime));		// Last frame
	mGeneral.Store(mTimeInfo.mFrameRate);									// Framerate
	mGeneral.Store((long)mTimeInfo.mDeltaTime);								// Para calcular cosas luego
	mGeneral.Store((float)maininfo.mGlobalScale);							// Global scale (sirve de algo?)

	return true;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	End of export notification.
 *	This method is called once all nodes have been exported. This is a convenient place to free all used ram, etc.
 *	\param		stats		[in] a structure filled with some export statistics.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool UtoFormat::EndExport(const StatsDescriptor& stats)
{

	// Complete general chunk
		mGeneral
			.Store(nObjects)		// Número de objetos
			.Store(stats.mNbCameras)			// Número de cámaras
			.Store(stats.mNbLights)				// Número de luces
			.Store(stats.mNbMaterials)			// Número de materiales
			.Store(stats.mNbControllers)		// Número de controllers
			.Store(nBones);						// Numero de bones

	 /*sprintf(EXPORTER_DEBUG_STRING,"General. Obj: %d, cameras: %d, lights: %d, materials: %d, controllers: %d, bones: %d\n",
		 nObjects,
		 stats.mNbCameras,
		 stats.mNbLights,
		 stats.mNbMaterials,
		 stats.mNbControllers,
		 nBones);
	 Debug(1);*/


//			.Store(stats.mNbTexmaps)


	// Export to disk
	{
		// Open output file
		udword fp = CreateFile(mFilename, FILE_WRITE, FILE_CREATE_ALWAYS);
		if(fp!=-1)
		{
			// Write data to disk
			void* Data;
				// The "General" array must be exported first, since it may include the format's signature.
											{ Data = mGeneral			.Collapse();	if(Data)	WriteFile(fp, Data, mGeneral		.GetOffset());}
//				udword TotalNb = stats.mNbGeomObjects + stats.mNbDerivedObjects;
				udword TotalNb = stats.mNbGeomObjects - stats.mNbCameras;
				if(TotalNb)					{ Data = mGeomObjects		.Collapse();	if(Data)	WriteFile(fp, Data, mGeomObjects	.GetOffset());}
				if(stats.mNbCameras)		
				{ 
					Data = mCameras			.Collapse();	if(Data)	WriteFile(fp, Data, mCameras		.GetOffset());
					Data = mCameraTargets	.Collapse();	if(Data)	WriteFile(fp, Data, mCameraTargets	.GetOffset());
				}

				if(stats.mNbLights)			{ Data = mLights			.Collapse();	if(Data)	WriteFile(fp, Data, mLights			.GetOffset());}
//				if(stats.mNbTexmaps)		{ Data = mTexmaps			.Collapse();	if(Data)	WriteFile(fp, Data, mTexmaps		.GetOffset());}
				if(stats.mNbMaterials)		{ Data = mMaterials			.Collapse();	if(Data)	WriteFile(fp, Data, mMaterials		.GetOffset());}
				/*if(nBones)*/					{ Data = mBones				.Collapse();	if(Data)	WriteFile(fp, Data, mBones			.GetOffset());}
				if(stats.mNbControllers)	{ Data = mControllers		.Collapse();	if(Data)	WriteFile(fp, Data, mControllers	.GetOffset());}
				

			// Close output file
			CloseFile(fp);
		}
	}
		// Free everything for next call
		mGeneral		.Clean();
		mGeomObjects	.Clean();
		mCameras		.Clean();
		mCameraTargets.Clean();
		mBones.Clean();
		mLights			.Clean();
		mShapes			.Clean();
		mHelpers		.Clean();
		mTexmaps		.Clean();
		mMaterials		.Clean();
		mControllers	.Clean();
		mMotion			.Clean();
		nBones = 0;
		nObjects = 0;

	return true;
}



bool UtoFormat::ExportBasicInfo(const ObjectDescriptor* obj, CustomArray* array, bool isCamera)
{
	// Checkings
	if(!array || !obj) return false;

	// Export name
	array	->StoreASCII((const char*)obj->mName);
	array	->Store((unsigned char)0);
	// Export database management information
	array	->Store(obj->mObjectID);
	array	->Store(obj->mParentID);
	if(isCamera)	array	->Store(obj->mTargetID);
	array	->Store(obj->mIsHidden);

	// Export common properties
	array	->Store(obj->mPrs.mPosition.x);	// Position
	array	->Store(obj->mPrs.mPosition.y);
	array	->Store(obj->mPrs.mPosition.z);

	array	->Store(obj->mPrs.mRotation.x);	// Rotation (quaternion)
	array	->Store(obj->mPrs.mRotation.y);
	array	->Store(obj->mPrs.mRotation.z);
	array	->Store(obj->mPrs.mRotation.w);

	array	->Store(obj->mPrs.mScale.x);	// Scale
	array	->Store(obj->mPrs.mScale.y);
	array	->Store(obj->mPrs.mScale.z);


	// Export user-defined properties
	//if(obj->mUserProps)	array->StoreASCII("\nUser props:   ").StoreASCII((const char*)obj->mUserProps);


	// Export pivot

	
	array	->Store(obj->mPivot.mWorldPivot.mPosition.x);
	array	->Store(obj->mPivot.mWorldPivot.mPosition.y);
	array	->Store(obj->mPivot.mWorldPivot.mPosition.z);


// ¿¿¿Qué es esto de rotación y escala del pivot???
//			.StoreASCII("\nPivot rotation:   ")	.StoreASCII(obj->mPivot.mWorldPivot.mRotation.x)	.StoreASCII("  ")
//												.StoreASCII(obj->mPivot.mWorldPivot.mRotation.y)	.StoreASCII("  ")
//												.StoreASCII(obj->mPivot.mWorldPivot.mRotation.z)	.StoreASCII("  ")
//												.StoreASCII(obj->mPivot.mWorldPivot.mRotation.w)
//			.StoreASCII("\nPivot scale:   ")	.StoreASCII(obj->mPivot.mWorldPivot.mScale.x)		.StoreASCII("  ")
//												.StoreASCII(obj->mPivot.mWorldPivot.mScale.y)		.StoreASCII("  ")
//												.StoreASCII(obj->mPivot.mWorldPivot.mScale.z);

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Simple debug function
 *	This method is called after storing the debug text on EXPORTER_DEBUG_STRING
 *	\param		debuglevel.		Debug level. If the debug level is lower than the minimum level, the message will be logged to disk
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEBUG_CURRENT_LEVEL 0

void UtoFormat::Debug(int debuglevel)
{
 FILE *p;

 if(debuglevel <= DEBUG_CURRENT_LEVEL)
 {
    p=fopen("debug_exporter.txt","a");
    if(!p) return;
    fputs(EXPORTER_DEBUG_STRING,p);
    fclose(p);
 } 
}
