#include <epopeia/epopeia_system.h>
#include "spiral.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif

//-----------------------------------------------------------------------------


static void* Spiral_New( void )
{
	return( SPIRAL_New() );
}
//-----------------------------------------------------------------------------
static void Spiral_Delete( void * self)
{
	SPIRAL_Delete( self );
}

//-----------------------------------------------------------------------------


static void Spiral_DoFrame( void *self, TRenderContext* RC, TSoundContext* SC)
{
    float   t;

    //t = SC->Tick*(60.0f/1000.0f);
    t = Epopeia_GetTimeMs()*.001f;

	SPIRAL_DoFrame( self, t );
}

//-----------------------------------------------------------------------------
static void Spiral_Start( void* self, int z)
{
}
//-----------------------------------------------------------------------------
static void Spiral_Stop( void* self)
{
}

//-----------------------------------------------------------------------------

static void Spiral_ChangeColor1( void * self, float r, float g, float b, float a, float time )
{
	SPIRAL_ChangeColor1( self, r, g, b, a, time );
}

static void Spiral_ChangeColor2( void * self, float r, float g, float b, float a, float time )
{
	SPIRAL_ChangeColor2( self, r, g, b, a, time );
}

static void Spiral_ChangeWidth1( void * self, float w, float time )
{
	SPIRAL_ChangeWidth1( self, w, time );
}

static void Spiral_ChangeWidth2( void * self, float w, float time )
{
	SPIRAL_ChangeWidth2( self, w, time );
}

static void Spiral_ChangeTable( void * self, int ta, float time )
{
	SPIRAL_ChangeTable( self, ta, time );
}

static void Spiral_SetSpeed( void * self, float speed )
{
    SPIRAL_SetSpeed( self, speed );
}

//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return( "Retro-Fashion Spiral by iq" );
}

//-----------------------------------------------------------------------------


EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;


    Id = Type_Register("Spiral");
   
    pMethod = Method_New("New", (void*)Spiral_New);
    //Method_AddParameterType(pMethod, Type_IntegerId);
    //Method_AddParameterType(pMethod, Type_IntegerId);
    //Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)Spiral_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Spiral_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)Spiral_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)Spiral_Stop);
    Type_AddMethod(Id, pMethod);




	pMethod = Method_New("ChangeColor1", (void*)Spiral_ChangeColor1 );
	Method_AddParameterType(pMethod, Type_RealId);
	Method_AddParameterType(pMethod, Type_RealId);
	Method_AddParameterType(pMethod, Type_RealId);
	Method_AddParameterType(pMethod, Type_RealId);
	Method_AddParameterType(pMethod, Type_RealId);
	Type_AddMethod(Id, pMethod);

	pMethod = Method_New("ChangeColor2", (void*)Spiral_ChangeColor2 );
	Method_AddParameterType(pMethod, Type_RealId);
	Method_AddParameterType(pMethod, Type_RealId);
	Method_AddParameterType(pMethod, Type_RealId);
	Method_AddParameterType(pMethod, Type_RealId);
	Method_AddParameterType(pMethod, Type_RealId);
	Type_AddMethod(Id, pMethod);

	pMethod = Method_New("ChangeWidth1", (void*)Spiral_ChangeWidth1 );
	Method_AddParameterType(pMethod, Type_RealId);
	Method_AddParameterType(pMethod, Type_RealId);
	Type_AddMethod(Id, pMethod);

	pMethod = Method_New("ChangeWidth2", (void*)Spiral_ChangeWidth2 );
	Method_AddParameterType(pMethod, Type_RealId);
	Method_AddParameterType(pMethod, Type_RealId);
	Type_AddMethod(Id, pMethod);

	pMethod = Method_New( "ChangeTable", (void*)Spiral_ChangeTable );
	Method_AddParameterType(pMethod, Type_IntegerId);
	Method_AddParameterType(pMethod, Type_RealId);
	Type_AddMethod(Id, pMethod);

    pMethod = Method_New( "SetSpeed", (void*)Spiral_SetSpeed );
	Method_AddParameterType(pMethod, Type_RealId);
	Type_AddMethod(Id, pMethod);

}

#ifdef __cplusplus
}
#endif
