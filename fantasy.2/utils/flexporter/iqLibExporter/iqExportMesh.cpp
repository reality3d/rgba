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


	int nfaces;
	int nquads;
	int i,j,k,l;
    char *usado;

	nfaces = Mesh->mNbFaces;
	usado = (char *)malloc(nfaces);
	if(!usado) return false;
	for(i=0;i<nfaces;i++) usado[i]=0;

    // Para encontrar los quads nos basamos en las propiedades de visibilidad

	const MAXFace* f = Mesh->mFaces;
	const NakedPoint* v = Mesh->mVerts;

	if(!f || !v) return false;
	nquads = 0;
	for(i=0;i<nfaces;i++)
	{
	for(j=0;j<3;j++)
	 {
		int mask = 1 << j;

		if((  (f[i].EdgeVis & mask) ==0) && (usado[i] == 0)) // Esta arista no es visible...

		if(  (f[i].EdgeVis & mask) ==0) 
		{
			int vert1,vert2;	// Estos son los vértices de la arista

			vert1 = f[i].VRef[j];
			vert2 = f[i].VRef[(j+1)%3];
			for(k=i+1;k<nfaces;k++)
			{
				int resultado = ComparteVertices(vert1,vert2,&(f[i]));
				if(resultado != -1)
				{
					usado[i] = usado[k] = 1;
					nquads++;
					break;
				}
		}
	  }
	 }


	}

	// Si quedan triángulos, los convertimos a quads de una forma un tanto burda
	for(i=0;i<nfaces;i++) 
	{
		if(!usado[i])
		{
			nquads++;
		}
	}

	/*
	// Empezamos la juerga con los vértices

	if(Mesh->mVerts)
	{

		for(udword i=0;i<Mesh->mNbVerts;i++)
		{
			mGeomObjects.Store(v[i].x);
			mGeomObjects.Store(v[i].y);
			mGeomObjects.Store(v[i].z);
		}
	}

	// Seguimos con las coordenadas de mapeo

	if(Mesh->mTVerts)
	{
		const NakedPoint* v = Mesh->mTVerts;
		for(udword i=0;i<Mesh->mNbTVerts;i++)
		{
			mGeomObjects.Store(v[i].x); // Ignoramos la coordenada W de los mapeos
			mGeomObjects.Store(v[i].y);
			// mGeomObjects.Store(v[i].z);	
		}
	}

	
	// Seguimos con las caras

	if(Mesh->mFaces)
	{
		const MAXFace* f = Mesh->mFaces;

		// Primero la asociación entre vértices y caras
		for(udword i=0;i<Mesh->mNbFaces;i++)
		{
				mGeomObjects.Store(f[i].VRef[0]);
				mGeomObjects.Store(f[i].VRef[1]);
				mGeomObjects.Store(f[i].VRef[2]);

		}
		// Primero la asociación entre vértices y coordenadas de textura
		for(udword i=0;i<Mesh->mNbFaces;i++)
		{
				mGeomObjects.Store(f[i].TRef[0]);
				mGeomObjects.Store(f[i].TRef[1]);
				mGeomObjects.Store(f[i].TRef[2]);

		}
		// Después los flags de visibilidad de cada cara (wireframe chachi y esas cosas)
		for(udword i=0;i<Mesh->mNbFaces;i++)
		{
				mGeomObjects.Store(f[i].EdgeVis);
		}
		// Y luego el identificador de material de cada cara 
	    // para el fantasy debe ser el mismo para todas, pero por si acaso lo exporto

		for(udword i=0;i<Mesh->mNbFaces;i++)
		{
				mGeomObjects.Store(f[i].MatID);
		}
	}


*/
	mGeomObjects.StoreASCII("numvertices ").StoreASCII(Mesh->mNbVerts).StoreASCII("\n");
	mGeomObjects.StoreASCII("numquads ").StoreASCII(nquads).StoreASCII("\n");
	mGeomObjects.StoreASCII("numtexcoords ").StoreASCII(Mesh->mNbTVerts).StoreASCII("\n");

// Empezamos con los vertices

	mGeomObjects.StoreASCII("\n; vertices [x y z]\n");
	for(udword i=0;i<Mesh->mNbVerts;i++)
			mGeomObjects.StoreASCII("%f %f %f\n",v[i].x,v[i].y,v[i].z);

// Seguimos con coordenadas de textura

	const NakedPoint *uvcoords = Mesh->mTVerts;
	mGeomObjects.StoreASCII("\n; uv [u v]\n");
	for(udword i=0;i<Mesh->mNbTVerts;i++)
			mGeomObjects.StoreASCII("%f %f\n",uvcoords[i].x,uvcoords[i].y); 

// Nos ponemos con los quads, incluyendo sus coordenadas de textura

	mGeomObjects.StoreASCII("\n; Caras [v1 v2 v3 v4 uv1 uv2 uv3 uv4]\n");
	for(i=0;i<nfaces;i++) usado[i]=0;

    // Para encontrar los quads nos basamos en las propiedades de visibilidad

	for(i=0;i<nfaces;i++)
	{
	for(j=0;j<3;j++)
	 {
		int mask = 1 << j;

		if((  (f[i].EdgeVis & mask) ==0) && (usado[i] == 0)) // Esta arista no es visible...

		if(  (f[i].EdgeVis & mask) ==0) 
		{
			int vert1,vert2;	// Estos son los vértices de la arista

			vert1 = f[i].VRef[j];
			vert2 = f[i].VRef[(j+1)%3];
			for(k=i+1;k<nfaces;k++)
			{
				int resultado = ComparteVertices(vert1,vert2,&(f[i]));
				if(resultado != -1)
				{
					usado[i] = usado[k] = 1;
					for(l=0;l<=j;l++)
					{
						mGeomObjects.StoreASCII("%d ",f[i].VRef[l]);
					}

					mGeomObjects.StoreASCII("%d ",NoIncluido(vert1,vert2,&(f[k])));
					for(l=j+1;l<3;l++)
					{
						mGeomObjects.StoreASCII("%d ",f[i].VRef[l]);
					}
		
					// Coordenadas de textura
					for(l=0;l<=j;l++)
					{
						mGeomObjects.StoreASCII("%d ",f[i].TRef[l]);
					}

					mGeomObjects.StoreASCII("%d ",f[k].TRef[NoIncluido_Texcoord(vert1,vert2,&(f[k]))]);
					for(l=j+1;l<3;l++)
					{
						mGeomObjects.StoreASCII("%d ",f[i].TRef[l]);
					}
			
					mGeomObjects.StoreASCII("\n");
					break;
				}
		}
	  }
	 }


	}

	// Si quedan triángulos, los convertimos a quads de una forma un tanto burda
	for(i=0;i<nfaces;i++) 
	{
		if(!usado[i])
		{
			mGeomObjects.StoreASCII("%d %d %d %d ",f[i].VRef[0],f[i].VRef[1],f[i].VRef[2],f[i].VRef[2]);
			mGeomObjects.StoreASCII("%d %d %d %d ",f[i].TRef[0],f[i].TRef[1],f[i].TRef[2],f[i].TRef[2]);
			mGeomObjects.StoreASCII("\n");
		}
	}

	free(usado);
	
	
	
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
