#include <epopeia/epopeia_system.h>
#include <malloc.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>


typedef struct
{
    float   r, g, b;
    float   to;
    float   time;
    int     xres, yres;

    long    init;
    long    mode;
}FADE;

//-----------------------------------------------------------------------------
static void* FFX_New( void )
{
    FADE *f;

    f = (FADE*)malloc( sizeof(FADE) );
    if( !f )
        return( 0 );

    f->r    = 1.0;
    f->g    = 1.0;
    f->b    = 1.0;
    f->xres = Epopeia_GetResX();
    f->yres = Epopeia_GetResY();
    f->to   = 0.0;
    f->time = 0.0;
    f->init = 0;
    f->mode = 0;

    return( f );
}
//-----------------------------------------------------------------------------
static void FFX_Delete( FADE * me)
{
    free( me );
}
//-----------------------------------------------------------------------------
static void FFX_DoFrame( FADE *me, TRenderContext* RC, TSoundContext* SC )
{
    float   s;

    if( !me->init ) return;


    //s = ((SC->Tick*60.0/1000.0) - me->to)/me->time;
    s = ((float)Epopeia_GetTimeMs()-me->to)/me->time;

    if( s<0.0 ) return;

    if( s>1.0 )
        me->init = 0;


    if( me->mode==1 )
        {
        if( s<0.5f )
            s *= 2.0;
        else
            s = 1.0 - (s-.5f)*2.0f;
        }

    //--------------

    // disable
    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_NORMALIZE );
    glDisable( GL_TEXTURE_2D );
    glShadeModel( GL_FLAT );


    // set 3d
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluOrtho2D( 0.0f, 1.0f, 0.0f, 1.0f );


    // draw quad

    glEnable( GL_BLEND );

    glBlendFunc( GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA );
    glColor4f( me->r, me->g, me->b, 1.0f-s );

    glBegin( GL_QUADS );
        glVertex2f( 0.0f, 1.0f );
        glVertex2f( 0.0f, 0.0f );
        glVertex2f( 1.0f, 0.0f );
        glVertex2f( 1.0f, 1.0f );
    glEnd();


    // dejarlo todo bien
    glDisable( GL_BLEND );
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
}
//-----------------------------------------------------------------------------
static void FFX_Start( FADE* me, int z)
{
}
//-----------------------------------------------------------------------------
static void FFX_Stop( FADE* me)
{
}
//-----------------------------------------------------------------------------
static void FFX_SetPesos( FADE *me, int mode, float r, float g, float b, float time )
{
    me->r = r;
    me->g = g;
    me->b = b;
    //me->to = (float)Epopeia_GetTimeMs()*(1.0f/1000000.0f);
    me->to = (float)Epopeia_GetTimeMs();
    me->time = time*1000.0;
    me->init = 1;
    me->mode = mode;
}

//-----------------------------------------------------------------------------
extern "C"
{

EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return( "Fade by iq" );
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;


    Id = Type_Register("Fade");
   
    pMethod = Method_New("New", (void*)FFX_New);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)FFX_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)FFX_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)FFX_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)FFX_Stop);
    Type_AddMethod(Id, pMethod);

    // --- params ---

    pMethod = Method_New("SetParams", (void*)FFX_SetPesos);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

}

}
