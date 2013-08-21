#include <epopeia/epopeia_system.h>
#include <malloc.h>     // malloc()
#include <string.h>     // strcmp()
#include <math.h>       // fabs()

#include "../apifantasy2/3dworld.h"
#include "../apifantasy2/fantasy_api.h"
#include "turb.h"
#include "misc.h"


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

extern "C"
{
	
typedef struct
{
 object_type    **mesh;          // puntero al objeto
 FantasyAPI	*api;		// Fantasy API object
 int 		*objindex;	
 int		numobj;		//Num objetos afectados por la deformación
 float          **sverts;        // copia est tica del objeto
 float          *scale;          // tama¤o del objeto
 float          ttable[256];    // tabla para el turbulence

 // --- par metros ---

 int            is4d;                       //
 int            octaves;                    //
 float          frex, frey, frez, fret;     //
 float          pha[3*4];                   //
 float          ampx, ampy, ampz;           //

}DEFORM;

//-----------------------------------------------------------------------------
static void* Deform3D_New( int sem, int is4d, int octaves )
{
    int     i;
    DEFORM  *self;

    if( octaves<2 || octaves>16 )
        return( 0 );

    self = (DEFORM *)malloc( sizeof(DEFORM)*10 );
    self +=5;
    if( self )
        {
        self->mesh   = NULL;
        self->sverts = NULL;
        self->numobj = 0;        
	self->scale  = NULL;
	self->objindex = NULL;
    
        self->is4d    = is4d;
        self->octaves = octaves;
    
        self->frex = 1.000f;
        self->frey = 1.000f;
        self->frez = 1.000f;
        self->fret = 1.000f;
        for( i=0; i<12; i++ )
            self->pha[i] = (float)i;
        self->ampx = 0.100f;
        self->ampy = 0.100f;
        self->ampz = 0.100f;
    
        OTROTURB_New( self->ttable, sem );
        }
  
    return( self );
}
//-----------------------------------------------------------------------------
static void Deform3D_Delete( DEFORM * self) 
{  
int i;  
 

if( !self ) return; 

if( self->sverts ) 
{ 
	for(i=0;i<self->numobj;i++) 
		if( self->sverts[i] )  
			free( self->sverts[i] );
	free( self->sverts );  
} 
if( self->mesh ) free( self->mesh ); 
if( self->scale ) free( self->scale ); 
if( self->objindex ) free( self->objindex ); 

//free( self );

}

//-----------------------------------------------------------------------------
static void Deform3D_DoFrame( DEFORM *self, TRenderContext* RC, TSoundContext* SC)
{
    float   x, y, z, t;
    float   *sv, *dv;
    int     h,i;
    float   frex, frey, frez;
    float   ampx, ampy, ampz;


    t = self->fret * 0.05f * (SC->Tick*60.0f)*(1.0f/1000.0f);

	for(h=0;h<self->numobj;h++)
	{       
    	if( &(self->mesh[h]) && &(self->sverts[h]) )
	    {
        	frex = self->frex / self->scale[h];
        	frey = self->frey / self->scale[h];
        	frez = self->frez / self->scale[h];
        	ampx = self->ampx * self->scale[h];
        	ampy = self->ampy * self->scale[h];
        	ampz = self->ampz * self->scale[h];

        	sv = self->sverts[h];
        	dv = self->mesh[h]->VertexPointer;
        	
        	

        	for( i=0; i<self->mesh[h]->number_of_vertices; i++ )
	        {
	            x = sv[0]*frex;
            	y = sv[1]*frey;
            	z = sv[2]*frez;

            	if( self->is4d )
                {
                	dv[0] = sv[0] + ampx*(-1.0f+2.0f*OTROTURB_Turbulence4d( self->ttable, x+self->pha[0], y+self->pha[1], z+self->pha[ 2], t+self->pha[ 3], self->octaves ));
                	dv[1] = sv[1] + ampy*(-1.0f+2.0f*OTROTURB_Turbulence4d( self->ttable, x+self->pha[4], y+self->pha[5], z+self->pha[ 6], t+self->pha[ 7], self->octaves ));
                	dv[2] = sv[2] + ampz*(-1.0f+2.0f*OTROTURB_Turbulence4d( self->ttable, x+self->pha[8], y+self->pha[9], z+self->pha[10], t+self->pha[11], self->octaves ));
                }
            	else
                {
	                dv[0] = sv[0] + ampx*(-1.0f+2.0f*OTROTURB_Turbulence3d( self->ttable, x+self->pha[0], y+self->pha[1], z+t+self->pha[2], self->octaves ));
                	dv[1] = sv[1] + ampy*(-1.0f+2.0f*OTROTURB_Turbulence3d( self->ttable, x+self->pha[3], y+self->pha[4], z+t+self->pha[5], self->octaves ));
                	dv[2] = sv[2] + ampz*(-1.0f+2.0f*OTROTURB_Turbulence3d( self->ttable, x+self->pha[6], y+self->pha[7], z+t+self->pha[8], self->octaves ));
                }

            	dv+=3;
            	sv+=3;
            }
            
        	self->api->Normalize(self->objindex[h]);
        	self->api->UpdateObject(self->mesh[h],1);
        }
   }

}

//-----------------------------------------------------------------------------
static void Deform3D_Start(void* self, int z)
{
}
//-----------------------------------------------------------------------------
static void Deform3D_Stop(void* self)
{
}

//-----------------------------------------------------------------------------
static void Deform3D_SetMesh( DEFORM * self, int puntero, char *name )
{
    int             i,j;
    
    self->api = (FantasyAPI *)puntero;


    //// todo este tocho s¢lo se ejecuta una vez ////////

    if( !self->mesh )//Solo si no se ha hecho ya un setmesh entra
    {
        // --- localiza el objeto concreto ---
    
		for(i=0;i<self->api->world->number_of_objects;i++)
 		{
	  		if(strmatch(name,self->api->world->obj[i].nombre_objeto)) // Hemos encontrado el objeto
  			{
	  			
  				self->numobj++;

	  		}
  		
	 	}	
	 	if(self->numobj>0)
	 	{
//	 		printf("hay %d objetos:\n",self->numobj);
	 		self->mesh= (object_type **) malloc( self->numobj*sizeof(object_type) );// Alojo memoria
	 		self->objindex= (int *) malloc( self->numobj*sizeof(int) );
	 		self->sverts=(float **) malloc(self->numobj*sizeof(float *));	 		
	 		self->scale= (float *) malloc( self->numobj*sizeof(float) );
	 		if(!self->mesh || !self->objindex || !self->sverts)
	 			return;
	 		j=0;		
			for(i=0;i<self->api->world->number_of_objects;i++)	
	 		{ 	
	 			if(strmatch(name,self->api->world->obj[i].nombre_objeto)) // Hemos encontrado el objeto
  				{	
	 				self->objindex[j] = i;
	 				self->mesh[j]= &(self->api->world->obj[i]);	    
 					j++;
				}
			}
          
   			// --- reserva memoria para los v‚rtices est ticos ---
    		for(i=0;i<self->numobj;i++)
    		{
	   			self->sverts[i] = (float*) malloc( self->mesh[i]->number_of_vertices*3*sizeof(float) );
   				if( !self->sverts[i])
		            return;
   				// --- copia los v‚rtices est ticos ---
    
	   			memcpy( self->sverts[i], self->mesh[i]->VertexPointer, self->mesh[i]->number_of_vertices*3*sizeof(float) );
    
   				// --- se queda con el tama¤o del objeto ---
    
			   	for( j=0; j<(3*self->mesh[i]->number_of_vertices); j++ )
		   				if( fabs(self->mesh[i]->VertexPointer[j]) > self->scale[i] )
        		        		self->scale[i] = fabs(self->mesh[i]->VertexPointer[j]);
	       }
	   	}//self->numobj>o
	}
}

//-----------------------------------------------------------------------------

static void Deform3D_SetXParams( DEFORM * self, float am, float fr, float px, float py, float pz, float pt )
{
    self->ampx = am;
    self->frex = fr;
    self->pha[0] = px;
    self->pha[1] = py;
    self->pha[2] = pz;
    self->pha[3] = pt;
}
static void Deform3D_SetYParams( DEFORM * self, float am, float fr, float px, float py, float pz, float pt )
{
    self->ampy = am;
    self->frey = fr;
    self->pha[4] = px;
    self->pha[5] = py;
    self->pha[6] = pz;
    self->pha[7] = pt;
}
static void Deform3D_SetZParams( DEFORM * self, float am, float fr, float px, float py, float pz, float pt )
{
    self->ampz = am;
    self->frez = fr;
    self->pha[8] = px;
    self->pha[9] = py;
    self->pha[10] = pz;
    self->pha[11] = pt;
}
static void Deform3D_SetTParams( DEFORM * self, float fr )
{
    self->fret = fr;
}



//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return( "Deform3D (turbulence) by iq, basado en el \"SDK for Fantasy\" de Utopian" );
}

//-----------------------------------------------------------------------------


EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;


    Id = Type_Register("Deform3D");
   
    pMethod = Method_New("New", (void*)Deform3D_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)Deform3D_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Deform3D_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)Deform3D_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)Deform3D_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetMesh", (void*)Deform3D_SetMesh);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);

    // --- params ---

    pMethod = Method_New("SetXParams", (void*)Deform3D_SetXParams);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetYParams", (void*)Deform3D_SetYParams);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetZParams", (void*)Deform3D_SetZParams);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetTParams", (void*)Deform3D_SetTParams);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
}

}
