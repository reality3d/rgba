///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	This file contains the Void Format exporter plug-in for Flexporter.
 *	\file		VoidExporter.h
 *	\author		Pierre Terdiman
 *	\date		May, 6, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __UTOEXPORTER_H__
#define __UTOEXPORTER_H__

	class UtoFormat : public ExportFormat
	{
	public:
	// Constructor/Destructor
										UtoFormat();
		virtual							~UtoFormat();

	// Exporter init
		override(ExportFormat)	bool	Init(bool motion);

	// Main scene info
		override(ExportFormat)	bool	SetSceneInfo		(const MainDescriptor& maininfo);

	// Export loop
		override(ExportFormat)	bool	ExportCamera		(const CameraDescriptor& camera);
		override(ExportFormat)	bool	ExportCameraTarget	(const MeshDescriptor& ctarget);
		override(ExportFormat)	bool	ExportBone			(const MeshDescriptor& bone);
		override(ExportFormat)	bool	ExportController	(const ControllerDescriptor& controller);
		override(ExportFormat)	bool	ExportHelper		(const HelperDescriptor& helper);
		override(ExportFormat)	bool	ExportLight			(const LightDescriptor& light);
		override(ExportFormat)	bool	ExportMaterial		(const MaterialDescriptor& material);
		override(ExportFormat)	bool	ExportMesh			(const MeshDescriptor& mesh);
		override(ExportFormat)	bool	ExportMotion		(const MotionDescriptor& motion);
		override(ExportFormat)	bool	ExportShape			(const ShapeDescriptor& shape);
		override(ExportFormat)	bool	ExportTexture		(const TextureDescriptor& texture);
		override(ExportFormat)  bool	ExportSpaceWarp(const SpaceWarpDescriptor& space_warp);
	// End of export notification
		override(ExportFormat)	bool	EndExport			(const StatsDescriptor& stats);

	private:

	// De momento cogemos todo lo que cogen los exportadores por defecto (por si acaso)
	// Time info local copy
								MAXTimeInfo		mTimeInfo;
	// Export arrays
								CustomArray		mGeneral;
								CustomArray		mGeomObjects;
								CustomArray		mCameras;
								CustomArray		mCameraTargets;
								CustomArray		mBones;								
								CustomArray		mLights;
								CustomArray		mShapes;
								CustomArray		mHelpers;
								CustomArray		mTexmaps;
								CustomArray		mMaterials;
								CustomArray		mControllers;
								CustomArray		mMotion;
								int nBones;
								int nObjects;
	// Flags
								bool			mIsMotionFile;
		override(ExportFormat)	bool ExportBasicInfo(const ObjectDescriptor* obj, CustomArray* array,bool isCamera);

	// Cositas para debuggear

								char			EXPORTER_DEBUG_STRING[1024];
								void Debug(int debuglevel);
	};

#endif // __UTOEXPORTER_H__
