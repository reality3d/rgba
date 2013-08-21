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
#ifndef __IQEXPORTER_H__
#define __IQEXPORTER_H__

struct camara
{
 char *name;
 int id;
 int targetid;
 float pos[3];
 float rot[4];
 float scale[3];
 float pos_tgt[3];
 float rot_tgt[4];
 float scale_tgt[3];
 float fov;
 float roll;
};

	class iqFormat : public ExportFormat
	{
	public:
	// Constructor/Destructor
										iqFormat();
		virtual							~iqFormat();

	// Exporter init
		override(ExportFormat)	bool	Init(bool motion);

	// Main scene info
		override(ExportFormat)	bool	SetSceneInfo		(const MainDescriptor& maininfo);

	// Export loop
		override(ExportFormat)	bool	ExportCamera		(const CameraDescriptor& camera);
		override(ExportFormat)	bool	ExportCameraTarget	(const MeshDescriptor& ctarget);
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
								CustomArray		mLights;
								CustomArray		mShapes;
								CustomArray		mHelpers;
								CustomArray		mTexmaps;
								CustomArray		mMaterials;
								CustomArray		mControllers;
								CustomArray		mMotion;
	// Flags
								bool			mIsMotionFile;
								int				numsplines;
								int				numcamaras;
								struct camara   camaras[256];
		override(ExportFormat)	bool ExportBasicInfo(const ObjectDescriptor* obj, CustomArray* array,bool isCamera);
		override(ExportFormat)  void WriteTitle(const char *cadena, CustomArray *array);
		override(ExportFormat)  bool ExportAllCameras(CustomArray* array);
	};

#endif // __IQEXPORTER_H__
