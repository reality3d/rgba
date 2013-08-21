// Precompiled Header
#include "Stdafx.h"



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Mesh export method.
 *	This method is called once for each exported mesh.
 *	\param		mesh		[in] a structure filled with current mesh information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Esta función auxiliar devuelve -1 si una cara no incluye los dos vértices especificados
// Si los comparte, devuelve el número de la arista en la que aparecen

static int ComparteVertices(int vert1, int vert2, const MAXFace* f)
{
 int i,j;
 int mayor;

 for(i=0;i<3;i++)
 {
	for(j=0;j<3;j++)
	{
		   if((vert1 == f->VRef[i]) && (vert2 == f->VRef[j])) 
		   {
	  	    // Es un método algo rupestre de saber cuál es la arista, pero creo que funciona :)
			mayor = i;
			if(j > mayor) mayor = j;
			if(mayor == 1) return 0; // Los vértices son el 0 y el 1
			else if((i == 0)  || (j == 0)) return 2; // Son el 0 y el 2
			else return 1; // Son el 1 y el 2
		   }
	}
 }

 return -1;
}

static int NoIncluido(int vert1, int vert2, const MAXFace* f)
{
 int i;

 for(i=0;i<3;i++)
 {
   if((f->VRef[i] != vert1) && (f->VRef[i] != vert2))
	   return f->VRef[i];
 }
 // return -1; //mmmm... Esto es un error
 return vert1;  // Esto puede generar algún artefacto, pero no se me ocurre cómo hacerlo si no :)
}

static int NoIncluido_Texcoord(int vert1, int vert2, const MAXFace* f)
{
 int i;

 for(i=0;i<3;i++)
 {
   if((f->VRef[i] != vert1) && (f->VRef[i] != vert2))
	   return i;
 }

 return -1; //mmmm... Esto es un error
}

bool iqFormat::ExportMesh(const MeshDescriptor& mesh)
{
	int flags = 0;


	if(mesh.mIsTarget)	// En realidad no es un objeto, sino un target de una cámara
	{
		return ExportCameraTarget(mesh);
	}

	WriteTitle("objeto",&mGeomObjects);

	ExportBasicInfo(&mesh, &mGeomObjects,false);

	const MAXNativeMesh* Mesh = mesh.mOriginalMesh; // Obtenemos la malla de MAX

	mGeomObjects.StoreASCII("numvertices ").StoreASCII("0").StoreASCII("\n");
	mGeomObjects.StoreASCII("numquads ").StoreASCII("0").StoreASCII("\n");
	mGeomObjects.StoreASCII("numtexcoords ").StoreASCII("0").StoreASCII("\n");
	mGeomObjects.StoreASCII("numtexcoords2 ").StoreASCII("0").StoreASCII("\n");


	
	
	return true;
}

// De un camera target tan solo hay que exportar la info básica, que tiene todo lo necesario
// (posición, node id...)

bool iqFormat::ExportCameraTarget(const MeshDescriptor &ctarget)
{
	int i;

	//WriteTitle("Cameratarget",&mCameraTargets);
	//ExportBasicInfo(&ctarget, &mCameraTargets, false);
	for(i=0;i<numcamaras;i++)
	{
		if(camaras[i].targetid == ctarget.mObjectID)
		{
 			camaras[i].pos_tgt[0] = ctarget.mPrs.mPosition.x;
			camaras[i].pos_tgt[1] = ctarget.mPrs.mPosition.y;
			camaras[i].pos_tgt[2] = ctarget.mPrs.mPosition.z;
			camaras[i].rot_tgt[0] = ctarget.mPrs.mRotation.x;
			camaras[i].rot_tgt[1] = ctarget.mPrs.mRotation.y;
			camaras[i].rot_tgt[2] = ctarget.mPrs.mRotation.z;
			camaras[i].rot_tgt[3] = ctarget.mPrs.mRotation.w;
			camaras[i].scale_tgt[0] = ctarget.mPrs.mScale.x;
			camaras[i].scale_tgt[1] = ctarget.mPrs.mScale.y;
			camaras[i].scale_tgt[2] = ctarget.mPrs.mScale.z;
			return true;
		}
	}
	// No lo hemos encontrado, luego se ha exportado el target antes que la camara
	camaras[numcamaras].targetid = ctarget.mObjectID;
	camaras[numcamaras].pos_tgt[0] = ctarget.mPrs.mPosition.x;
	camaras[numcamaras].pos_tgt[1] = ctarget.mPrs.mPosition.y;
	camaras[numcamaras].pos_tgt[2] = ctarget.mPrs.mPosition.z;
	camaras[numcamaras].rot_tgt[0] = ctarget.mPrs.mRotation.x;
	camaras[numcamaras].rot_tgt[1] = ctarget.mPrs.mRotation.y;
	camaras[numcamaras].rot_tgt[2] = ctarget.mPrs.mRotation.z;
	camaras[numcamaras].rot_tgt[3] = ctarget.mPrs.mRotation.w;
	camaras[numcamaras].scale_tgt[0] = ctarget.mPrs.mScale.x;
	camaras[numcamaras].scale_tgt[1] = ctarget.mPrs.mScale.y;
	camaras[numcamaras].scale_tgt[2] = ctarget.mPrs.mScale.z;

	numcamaras++;
	return true;
}
