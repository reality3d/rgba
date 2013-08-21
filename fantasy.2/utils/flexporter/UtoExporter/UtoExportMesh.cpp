// Precompiled Header
#include "Stdafx.h"

// Some helper functions

static void Quat_to_RotMatrix(float x, float y, float z, float w, float matrix[4][4])
{
    matrix[0][0] = 1.0f - 2.0f * (y*y + z*z);
    matrix[0][1] = 2.0f * (x*y - w*z);
    matrix[0][2] = 2.0f * (x*z + w*y);
    matrix[0][3] = 0.0f;

    matrix[1][0] = 2.0f * (x*y + w*z);
    matrix[1][1] = 1.0f - 2.0f * (x*x + z*z);
    matrix[1][2] = 2.0f * (y*z - w*x);
    matrix[1][3] = 0.0f;

    matrix[2][0] = 2.0f * (x*z - w*y);
    matrix[2][1] = 2.0f * (y*z + w*x);
    matrix[2][2] = 1.0f - 2.0f * (x*x + y*y);
    matrix[2][3] = 0.0f;

    matrix[3][0] = 0.0f;
    matrix[3][1] = 0.0f;
    matrix[3][2] = 0.0f;
    matrix[3][3] = 1.0f;
}

struct uto_vector
{
 float x,y,z;
};
typedef struct uto_vector uto_vector;

static void Mult_Vector_Matrix3(uto_vector *v1, float matrix[4][4], uto_vector *result)
{
 float lx,ly,lz;

 lx = v1->x; 
 ly = v1->y;
 lz = v1->z;

 result->x = ( lx * matrix[0][0] + ly * matrix[1][0] 
                 + lz * matrix[2][0]) ;

 result->y = ( lx * matrix[0][1] + ly * matrix[1][1] 
                 + lz * matrix[2][1]) ;
     
 result->z = ( lx * matrix[0][2] + ly * matrix[1][2] 
                 + lz * matrix[2][2]) ;    
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Mesh export method.
 *	This method is called once for each exported mesh.
 *	\param		mesh		[in] a structure filled with current mesh information.
 *	\return		true if success.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define U3D_HAS_BOUNDING_SPHERE	(1)
#define U3D_DYNAMIC_OBJECT	(2)
#define U3D_SKINNED_OBJECT	(4)
#define U3D_CAST_SHADOWS	(8)

#define U3D_SIMPLE_SKIN		(unsigned char)(0)
#define U3D_WEIGHTED_SKIN	(unsigned char)(1)

bool UtoFormat::ExportMesh(const MeshDescriptor& mesh)
{
	int flags = 0;


	if(mesh.mIsTarget)	// En realidad no es un objeto, sino un target de una cámara
	{
		return ExportCameraTarget(mesh);
	}

	if(mesh.mIsSkeleton)	// Tampoco es un objeto, es un bone (espero no cagarla con esto)
	{
		return ExportBone(mesh);
	}
	else if(mesh.mUserProps) // Forma alternativa de definir un bone
	{
		if(strstr((char *)mesh.mUserProps,"bone"))
			return ExportBone(mesh);
	}

	nObjects++;
	ExportBasicInfo(&mesh, &mGeomObjects,false);

	const MAXNativeMesh* Mesh = mesh.mOriginalMesh; // Obtenemos la malla de MAX

	if(Mesh->mFlags&MESH_BOUNDINGSPHERE)	flags |= U3D_HAS_BOUNDING_SPHERE;
	if(mesh.mUserProps)
	{
		if(strstr((char *)mesh.mUserProps,"dynamic"))
			flags |= U3D_DYNAMIC_OBJECT;
	
	}
	if(mesh.mIsSkin)	// This is a skin!
	{
	 flags |= U3D_SKINNED_OBJECT; 	
	}
	if(mesh.mCastShadows)
	{
	 flags |= U3D_CAST_SHADOWS; // This object will cast shadows
	}


	mGeomObjects.Store(flags);				// Almacenamos los flags
	mGeomObjects.Store(Mesh->mNbVerts);		// Número de vértices
	mGeomObjects.Store(Mesh->mNbFaces);		// Número de caras
	mGeomObjects.Store(Mesh->mNbTVerts);	// Número de coordenadas de mapeo
	mGeomObjects.Store(Mesh->mNbCVerts);	// Número de vertex colors

	sprintf(EXPORTER_DEBUG_STRING,"%d verts, %d faces\n",Mesh->mNbVerts,Mesh->mNbFaces);
	Debug(1);



	if(flags & U3D_SKINNED_OBJECT) // Exportamos toda la información de skin
	{
		if(!Mesh->mBonesNb) // Hay un solo bone por vertice
		{
			mGeomObjects.Store(U3D_SIMPLE_SKIN);
						
			if(Mesh->mBonesLocalID)  // ID del bone
			{
			  for(udword i=0;i<Mesh->mNbVerts;i++)
			  {
			   mGeomObjects.Store(Mesh->mBonesLocalID[i]);
 			  }
 			}
						
			const NakedPoint* v = Mesh->mOffsetVectors;
			for(udword i=0;i<Mesh->mNbVerts;i++)  // Los vértices como offset vectors (?)
			{
			 mGeomObjects.Store(v[i].x);
			 mGeomObjects.Store(v[i].y);
			 mGeomObjects.Store(v[i].z);
			}
		}
		else // Multiples bones por vertice
		{
			sprintf(EXPORTER_DEBUG_STRING,"with multiple bones per vertex\n");
			Debug(1);
			
			mGeomObjects.Store(U3D_WEIGHTED_SKIN);			
			// Store the number of bones per vertex			
			udword Sum = 0;  // bone-vertex associations
			for(udword i=0;i<Mesh->mNbVerts;i++)
			{
				mGeomObjects.Store(Mesh->mBonesNb[i]);
				Sum+=Mesh->mBonesNb[i];
			}			
			// Bone id of bones associated to each vertex		
			if(Mesh->mBonesLocalID)
			{
				for(udword i=0;i<Sum;i++)
					mGeomObjects.Store(Mesh->mBonesLocalID[i]);
			}
			else
			{
				sprintf(EXPORTER_DEBUG_STRING,"WARNING: no BonesLocalID\n");
				Debug(1);
			}

			// Weights				
			if(Mesh->mWeights)
			{
				for(udword i=0;i<Sum;i++)
					mGeomObjects.Store(Mesh->mWeights[i]);
			}
			else
			{
				sprintf(EXPORTER_DEBUG_STRING,"WARNING: no BonesWeights\n");
				Debug(1);
			}
			// And now the vertices. Since they are stored as offset vectors,
			// and there is one offset vector per bone-vertex association, I 
			// need to guess how the hell this is transformed into a good old
			// vector...
				udword curvertex = 0;		
			if(Mesh->mOffsetVectors)
			{
				const NakedPoint* v = Mesh->mOffsetVectors;
				udword i=0;
				NakedPoint p;								
							
			
				while(i<Sum)
				{
				 p.x=p.y=p.z=0.0f;
				 for(udword j=i;j<i+Mesh->mBonesNb[curvertex];j++)
				 {
				  	p.x += v[j].x * Mesh->mWeights[j];
				  	p.y += v[j].y * Mesh->mWeights[j];
				  	p.z += v[j].z * Mesh->mWeights[j];
				 }
				 i+=Mesh->mBonesNb[curvertex];
				 curvertex++;					
				 
				 uto_vector pp,res;
				 float matriz[4][4];

				 Quat_to_RotMatrix(-mesh.mPrs.mRotation.x,-mesh.mPrs.mRotation.y,
					 -mesh.mPrs.mRotation.z,mesh.mPrs.mRotation.w,matriz);
				
				 pp.x = p.x; pp.y = p.y; pp.z = p.z;
				 Mult_Vector_Matrix3(&pp,matriz,&res);

				 p.x = res.x + mesh.mPrs.mPosition.x;
				 p.y = res.y + mesh.mPrs.mPosition.y;
				 p.z = res.z + mesh.mPrs.mPosition.z;

				 // TEST

/*				 p.x += mesh.mPrs.mPosition.x;
				 p.y += mesh.mPrs.mPosition.y;
				 p.z += mesh.mPrs.mPosition.z;*/

				 mGeomObjects.Store(p.x);
			 	 mGeomObjects.Store(p.y);
			 	 mGeomObjects.Store(p.z);				
				}
			}			

			sprintf(EXPORTER_DEBUG_STRING,"skin done, %d vertices\n",curvertex);
			Debug(1);
		}
		
	}


	// Empezamos la juerga con los vértices

	if((Mesh->mVerts) && !(flags & U3D_SKINNED_OBJECT))
	{
		const NakedPoint* v = Mesh->mVerts;
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
	else
	{
	 sprintf(EXPORTER_DEBUG_STRING,"WARNING: no UVs\n");
	 Debug(1);
	}

	
	// Continuamos con los vertex colors
	if(Mesh->mCVerts)
	{
		const NakedPoint* v = Mesh->mCVerts;
		for(udword i=0;i<Mesh->mNbCVerts;i++)
		{
			mGeomObjects.Store(v[i].x);
			mGeomObjects.Store(v[i].y);
			mGeomObjects.Store(v[i].z);	
		}
	}


	// Parece que también hay vertex alpha, habrá que ver si se usan para algo en el futuro

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
		// Si hay vertex color, la asociación entre vértices y vertex color
		if(Mesh->mCVerts)
			for(udword i=0;i<Mesh->mNbFaces;i++)
			{
				mGeomObjects.Store(f[i].CRef[0]);
				mGeomObjects.Store(f[i].CRef[1]);
				mGeomObjects.Store(f[i].CRef[2]);

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


	// Finalmente exportamos la bounding sphere

	if(Mesh->mFlags&MESH_BOUNDINGSPHERE)	
	{
			mGeomObjects.Store(Mesh->mBSCenter.x);
			mGeomObjects.Store(Mesh->mBSCenter.y);
			mGeomObjects.Store(Mesh->mBSCenter.z);
			mGeomObjects.Store(Mesh->mBSRadius);
	}	
	else return false;	// Espero que lo tome como un error


	return true;
}

// De un camera target tan solo hay que exportar la info básica, que tiene todo lo necesario
// (posición, node id...)

bool UtoFormat::ExportCameraTarget(const MeshDescriptor &ctarget)
{
	ExportBasicInfo(&ctarget, &mCameraTargets, false);

	return true;
}

bool UtoFormat::ExportBone(const MeshDescriptor &bone)
{
	//ExportBasicInfo(&bone, &mBones, false);
		
	// Checkings
	//if(!bone) return false;

	// Export name
	mBones	.StoreASCII((const char*)bone.mName);
	mBones	.Store((unsigned char)0);
	// Export database management information
	mBones	.Store(bone.mObjectID);
	mBones	.Store(bone.mParentID);
	mBones	.Store(bone.mIsHidden);

	mBones	.Store(bone.mPrs.mPosition.x);	// Position
	mBones	.Store(bone.mPrs.mPosition.y);
	mBones	.Store(bone.mPrs.mPosition.z);

	mBones	.Store(bone.mPrs.mRotation.x);	// Rotation (quaternion)
	mBones	.Store(bone.mPrs.mRotation.y);
	mBones	.Store(bone.mPrs.mRotation.z);
	mBones	.Store(bone.mPrs.mRotation.w);

	mBones	.Store(bone.mPrs.mScale.x);	// Scale
	mBones	.Store(bone.mPrs.mScale.y);
	mBones	.Store(bone.mPrs.mScale.z);


	// Export pivot

	
	mBones	.Store(bone.mPivot.mWorldPivot.mPosition.x);
	mBones	.Store(bone.mPivot.mWorldPivot.mPosition.y);
	mBones	.Store(bone.mPivot.mWorldPivot.mPosition.z);	
	
	nBones++;

	return true;
}