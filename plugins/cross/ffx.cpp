#include <epopeia/epopeia_system.h>

#include <malloc.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include "glt/glt.h"


#define FROM_FILE   0
#define FROM_SAME   1
#define FROM_CROSS0 2
#define FROM_CROSS1 3
#define FROM_CROSS2 4
#define FROM_CROSS3 5
#define FROM_CROSS4 6
#define FROM_CROSS5 7

typedef struct
{
    long            running;
    long            cual;
    float           to;
    float           tf;
    long            xres, yres;
    unsigned char   *buffer;
    long            direccion;
}CROSS;

//-----------------------------------------------------------------------------
static void hace_tabla( unsigned char *buffer, long xres, long yres, long cual )
{
    long    i, j;
    float   x, y, a, d;
    float   ixres, iyres;

    ixres = 1.0f/(float)xres;
    iyres = 1.0f/(float)yres;

    for( j=0; j<yres; j++ )
    for( i=0; i<xres; i++ )
        {
        switch( cual )
            {
            case FROM_CROSS0:
                x = -.5f + (float)i*ixres;
                y = -.5f + (float)j*iyres;
                d = sqrt( x*x+y*y );
                a = atan2(x,y);
                x = cos( 6.2831f*(4.0f*d+a*(2.0f/3.1416f)) );
                d = .5f + .5f*d*x;
                if( d<0.0f ) d=0.0f; else if( d>1.0f ) d=1.0f;
                break;

            case FROM_CROSS1:
                x = -.5f + (float)i*ixres;
                y = -.5f + (float)j*iyres;
                a = atan2(x,y);
                d = .5f - (0.5f/3.1415927f)*a;
                break;

            case FROM_CROSS2:
                d = (float)i*ixres;
                break;

            case FROM_CROSS3:
                d = (float)j*iyres;
                break;

            case FROM_CROSS4:
                d = 0.5f + 0.25f*cos( 4.0f*6.2831f*i*ixres ) +
                           0.25f*cos( 3.0f*6.2831f*(j+i)/(float)(xres+yres) ) ;
                break;

            case FROM_CROSS5:
                x = -.5f + (float)i*ixres;
                y = -.5f + (float)j*iyres;
                d = sqrt( x*x+y*y )*(1.0f/0.707f);
                break;
            }

        *buffer++ = 255.0f*d;
        }
}

static void rellena( unsigned char *dst, long dx, long dy,
                     unsigned char *ori, long ox, long oy )
{
    long    i, j, u, v;
    long    c;

    long    fx, fy;
    float   x, y;
    long    c1, c2, c3, c4;

    for( j=0; j<dy; j++ )
    for( i=0; i<dx; i++ )
        {
        #if 0   // nearest....

            v = oy*j/(dy-1);
            u = ox*i/(dx-1);
            c = ori[ox*v+u];
        
        #else   // filtrado bilineal !!!!!!!!!!!!!!!!!!!!

            x = (float)ox*(float)i/(float)dx;
            y = (float)oy*(float)j/(float)dy;

            u = floor( x );
            v = floor( y );
            fx = 255.0*(x - (float)u);
            fy = 255.0*(y - (float)v);

            c1 = ori[ox*(v+0)+(u+0)];   // c1 -- c2
            c2 = ori[ox*(v+0)+(u+1)];   //  |    |
            c3 = ori[ox*(v+1)+(u+0)];   // c3 -- c4
            c4 = ori[ox*(v+1)+(u+1)];

            c1 =  (c1<<8) + (c2-c1)*fx;
            c3 =  (c3<<8) + (c4-c3)*fx;
            c  = ((c1<<8) + (c3-c1)*fy)>>16;
        #endif
        *dst++ = c;
        }
}

static long carga_fichero( unsigned char *buffer, long xres, long yres, char *name )
{
    TImage  bmp;

    bmp = IMG_Read( name );
    if( !bmp )
        return( 0 );

    if( bmp->bits_per_pixel!=8 )
        return( 0 );

    if( xres==bmp->width && yres==bmp->height )
        memcpy( buffer, bmp->data, xres*yres );
    else
        rellena( buffer, xres, yres, (unsigned char*)bmp->data, bmp->width, bmp->height );

    IMG_Delete( bmp );

    return( 1 );
}


static long obtiene_cual( char *str )
{
    long    cual = FROM_FILE;

         if( !strcmp( str, "same"   ) ) cual = FROM_SAME;
    else if( !strcmp( str, "cross01") ) cual = FROM_CROSS0;
    else if( !strcmp( str, "cross02") ) cual = FROM_CROSS1;
    else if( !strcmp( str, "cross03") ) cual = FROM_CROSS2;
    else if( !strcmp( str, "cross04") ) cual = FROM_CROSS3;
    else if( !strcmp( str, "cross05") ) cual = FROM_CROSS4;
    else if( !strcmp( str, "cross06") ) cual = FROM_CROSS5;

    return( cual );
}

static void* FFX_New( char *name )
{
    CROSS   *me;

    me = (CROSS*)malloc( sizeof(CROSS) );
    if( !me )
        return( 0 );

    me->running = 0;
    me->to      = 0.0f;
    me->xres    = Epopeia_GetResX();
    me->yres    = Epopeia_GetResY();
    me->cual    = obtiene_cual( name );
    me->buffer  = (unsigned char *)malloc( me->xres*me->yres );
    if( !me->buffer )
        return( 0 );

    // ------------------------------

    if( me->cual==FROM_SAME )
        return( 0 );

    if( me->cual==FROM_FILE )
        {
        if( !carga_fichero( me->buffer, me->xres, me->yres, name ) )
            return( 0 );
        }
    else
        {
        hace_tabla( me->buffer, me->xres, me->yres, me->cual );
        }

    return( me );
}
//-----------------------------------------------------------------------------
static void FFX_Delete( CROSS *me )
{
    free( me->buffer );
    free( me );
}
//-----------------------------------------------------------------------------
static void FFX_DoFrame( CROSS *me, TRenderContext* RC, TSoundContext* SC)
{
    long    th;
    float   t;

    t = (float)SC->Tick;

    if( me->running==1 )
        {
        me->to = t;
        me->running=2;
        }

    if( me->running==2 )
        {
        th = 256.0f * (t-me->to) / me->tf;
        if( th>255 )
            {
            me->running=0;
            glDisable( GL_STENCIL_TEST );
            }
        else
            {
            if( me->direccion )
                th = 255-th;
// !!!!!!!
glDrawPixels( me->xres, me->yres, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, me->buffer );
            glEnable( GL_STENCIL_TEST );
if( me->direccion )
            glStencilFunc( GL_GEQUAL, th, 0xff );
else
            glStencilFunc( GL_LEQUAL, th, 0xff );

            glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
            }
        }
}
//-----------------------------------------------------------------------------
static void FFX_Start( CROSS *me, int z )
{
}

static void FFX_Hace( CROSS *me, float tf, long direccion, char *cual )
{
    long    i;

    glDrawPixels( me->xres, me->yres, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, me->buffer );
    me->running   = 1;
    me->tf        = tf*1000.0f;
    me->direccion = direccion;

    i = obtiene_cual( cual );
    if( i==FROM_SAME || i==me->cual  )
        {
        }
    else
        {
        me->cual = i;
        if( i == FROM_FILE )
            {
            if( !carga_fichero( me->buffer, me->xres, me->yres, cual ) )
                {
                }
            }
        else
            {
            hace_tabla( me->buffer, me->xres, me->yres, me->cual );
            }
        }

    glDrawPixels( me->xres, me->yres, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, me->buffer );
}
//-----------------------------------------------------------------------------
static void FFX_Stop( CROSS* self)
{
}
//-----------------------------------------------------------------------------
extern "C"
{

EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return( "Cross by iq" );
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

    Id = Type_Register("Cross");
   
    pMethod = Method_New("New", (void*)FFX_New);
    Method_AddParameterType(pMethod, Type_StringId);
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

    pMethod = Method_New("Hace", (void*)FFX_Hace);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod(Id, pMethod);

}

}
