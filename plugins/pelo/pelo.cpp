//#define DEBUG

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>    // sprintf()
#include <malloc.h>   // malloc(), free()
#include <math.h>     // exp(), sin(), cos()
#include "glt/glt.h"

#include "../apifantasy/3dworld.h"
#include "../apifantasy/fantasy_api.h"

#include "pelo.h"



static void tamanio( MESH *m, long num )
{
    int     i;
    float   xmin, xmax, ymin, ymax, zmin, zmax, x, y, z, r;
    float   cen[3];
    float   *v = m->norm;

    ymin =  1e20;
    ymax = -1e20;
    xmin =  1e20;
    xmax = -1e20;
    zmin =  1e20;
    zmax = -1e20;


    for( i=0; i<num; i++ )
        {
        x = *v++;
        y = *v++;
        z = *v++;

        if( x<xmin ) xmin=x;
        if( y<ymin ) ymin=y;
        if( z<zmin ) zmin=z;
        if( x>xmax ) xmax=x;
        if( y>ymax ) ymax=y;
        if( z>zmax ) zmax=z;
        }

    cen[0] = .5*(xmin+xmax);
    cen[1] = .5*(ymin+ymax);
    cen[2] = .5*(zmin+zmax);

    r = fabs( xmax-cen[0] );
    if( fabs( xmin-cen[0] ) ) r = fabs( xmin-cen[0] );
    if( fabs( ymax-cen[1] ) ) r = fabs( ymax-cen[1] );
    if( fabs( ymin-cen[1] ) ) r = fabs( ymin-cen[1] );
    if( fabs( zmax-cen[2] ) ) r = fabs( zmax-cen[2] );
    if( fabs( zmin-cen[2] ) ) r = fabs( zmin-cen[2] );

    m->cen[0] = cen[0];
    m->cen[1] = cen[1];
    m->cen[2] = cen[2];
    m->ra = r;
}


PELO *PELO_Init( int num, float longitud, char *textname, float tscale )
{
    PELO    *f;
    int     i;
    TImage  bmp;

    if( num<=1 ) num=32;

    f = (PELO *)malloc( sizeof(PELO) );
    if( !f )
        return( 0 );

    f->api = 0;
    f->obj = 0;
    f->num = num;
    f->lon = longitud;
    f->tscale = tscale;
    f->lighting = 0;
    f->v = 1.0;
    f->f = 1.0;
    f->to = -455709.0f;
    f->po = 0.0f;


    f->mesh = (MESH*)malloc( num*sizeof(MESH) );
    if( !f->mesh )
        return( 0 );

    f->texture = CACHE_LoadTexture( textname, 0 );
    if( !f->texture )
        return( 0 );

/*
    f->sincos = (float*)malloc( (1024+256)*sizeof(float) );
    if( !f->sincos )
        return( 0 );

    for( i=0; i<(1024+256); i++ )
        f->sincos[i] = sin( 6.2831*i/1024.0 );
*/
    return( f );
}

void PELO_End( PELO *fan )
{
    int i;
    for( i=0; i<fan->num; i++ )
        {
        free( fan->mesh[i].vert );
        free( fan->mesh[i].norm );
        }


    CACHE_FreeTexture( fan->texture );

//    free( fan->sincos );
    free( fan->mesh );
    free( fan );
}


static void texture_scale( object_type *src, float s )
{
    float   *te;
    int     i;

    te = src->TexCoordPointer;

    for( i=0; i<src->number_of_vertices; i++ )
        {
        *te++ *= s;
        *te++ *= s;
        }
}


/*
__inline static void irota( float *dst, float *ori, long ian1, long ian2, long ian3, float *sincos  )
{
    float   xa, ya, za;

    float   co1 = sincos[ian1+256];
    float   si1 = sincos[ian1    ];
    float   co2 = sincos[ian2+256];
    float   si2 = sincos[ian2    ];
    float   co3 = sincos[ian3+256];
    float   si3 = sincos[ian3    ];

    xa = co1*ori[0] - si1*ori[2];
    ya = co2*ori[1] - si2*xa;
    za = si1*ori[0] + co1*ori[2];

    dst[0] = co2*xa + si2*ori[1];
    dst[1] = si3*za + co3*ya;
    dst[2] = co3*za - si3*ya;
}
*/

__inline static void rota( float *dst, float *ori, float an1, float an2, float an3 )
{
    float   co1 = cos(an1);
    float   si1 = sin(an1);
    float   co2 = cos(an2);
    float   si2 = sin(an2);
    float   co3 = cos(an3);
    float   si3 = sin(an3);
    float   xa, ya, za;

    xa = co1*ori[0] - si1*ori[2];
    ya = co2*ori[1] - si2*xa;
    za = si1*ori[0] + co1*ori[2];
    
    dst[0] = co2*xa + si2*ori[1];
    dst[1] = si3*za + co3*ya;
    dst[2] = co3*za - si3*ya;
}


//static void deforma( MESH *m, long num, float t, float *sincos )
static void deforma( MESH *m, long num, float t )
{
    int     i;
    float   *opos, *dpos;
    float   a, b, c;
    float   *p;
    float   s1,s2,s3;
    long    ia, ib, ic;

    dpos = m->vert;
    opos = m->norm;

    #if 1
        s1 = sin(0.05*1.41*t) / m->ra;
        s2 = sin(0.05*1.31*t) / m->ra;
        s3 = sin(0.05*1.00*t) / m->ra;
    #else
        s1 = (1024.0/6.2831)*sin(0.05*1.41*t) / m->ra;
        s2 = (1024.0/6.2831)*sin(0.05*1.31*t) / m->ra;
        s3 = (1024.0/6.2831)*sin(0.05*1.00*t) / m->ra;
    #endif

    for( i=0; i<num; i++ )
        {
        p = opos;

        #if 1
            a = s1 * *opos++;
            b = s2 * *opos++;
            c = s3 * *opos++;
            rota( dpos, p, a,b,c );
        #else
            ia = s1 * *opos++; ia &= 1023;
            ib = s2 * *opos++; ib &= 1023;
            ic = s3 * *opos++; ic &= 1023;
            irota( dpos, p, ia,ib,ic, sincos );
        #endif


        dpos[0] += m->cen[0];
        dpos[1] += m->cen[1];
        dpos[2] += m->cen[2];

        dpos += 3;
        }
}

static int copia_scale( MESH *dst, object_type *src, float s )
{
    float   *vo, *vd, *no, *nd;
    int     i;

    dst->vert = (float*)malloc( src->number_of_vertices*3*sizeof(float) );
    if( !dst->vert )
        return( 0 );

    dst->norm = (float*)malloc( src->number_of_vertices*3*sizeof(float) );
    if( !dst->norm )
        return( 0 );

    vo = src->VertexPointer;
    no = src->NormalPointer;
    vd = dst->vert;
    nd = dst->norm;

    for( i=0; i<src->number_of_vertices; i++ )
        {
        *nd++ =(*vo++) + s* *no++;
        *nd++ =(*vo++) + s* *no++;
        *nd++ =(*vo++) + s* *no++;
        }


    tamanio( dst, src->number_of_vertices );

    nd = dst->norm;
    for( i=0; i<src->number_of_vertices; i++ )
        {
        *nd++ = *nd - dst->cen[0];
        *nd++ = *nd - dst->cen[1];
        *nd++ = *nd - dst->cen[2];
        }

    return( 1 );
}




void PELO_SetPointer( PELO *fan, int pointer, char *name )
{
    int             i;
    float           s;

    if( fan->obj ) return;

    fan->api = (FantasyAPI *)pointer;

    i = fan->api->FindObject( name );

    if( i==-1 )
        {
        fan->obj = 0;
        #ifndef DEBUG
        printf( "PELO plugin:: objeto \"%s\" no encontrado!\n", name );
        #endif
        return;
        }


    fan->obj = fan->api->world->obj+i;

    texture_scale( fan->obj, fan->tscale );

    fan->api->Normalize( i );

    for( i=0; i<fan->num; i++ )
        {
        s = fan->lon*(float)(i+1)/(float)fan->num;

        copia_scale( fan->mesh+i, fan->obj, s );
        }

    fan->obj->visible = 0;
}


static void draw( MESH *m, object_type *o, long lighting )
{
    int             i, j, k, l;
    float           *ve, *no, *te;
    unsigned int    *in;

    in = o->IndexPointer;
    te = o->TexCoordPointer;
    ve = m->vert;
    no = o->NormalPointer;

    if( lighting )
        {
        glBegin( GL_TRIANGLES );
        for( i=0; i<(o->number_of_polygons*3); i++ )
            {
            k = *in++;
            l = k<<1;
            k *= 3;
    
            glTexCoord2fv( te+l );
            glNormal3fv(   no+k );
            glVertex3fv(   ve+k );
            }
        glEnd();
        }
    else
        {
        glBegin( GL_TRIANGLES );
        for( i=0; i<(o->number_of_polygons*3); i++ )
            {
            k = *in++;
            l = k<<1;
            k *= 3;
    
            glTexCoord2fv( te+l );
            glVertex3fv(   ve+k );
            }
        glEnd();
        }

}


void PELO_DoFrame( PELO *fan, float t )
{
    int     i;
    float   f;

    if( !fan->obj ) return;


    if( fan->to<0.0f  )
        {
        fan->to = t;
        }


    fan->po += fan->v*(t-fan->to);
    fan->to = t;


    glPushMatrix();

    glScalef( fan->obj->xscale, fan->obj->yscale, fan->obj->zscale);    
    glTranslatef(-fan->obj->pivotx,-fan->obj->pivoty,-fan->obj->pivotz);
    glMultMatrixf(&(fan->obj->mesh_matrix[0][0])); 

    if( fan->lighting )
        {
        glEnable( GL_LIGHTING );
        //glEnable( GL_NORMALIZE );
        }
    else
        {
        glDisable( GL_LIGHTING );
        //glDisable( GL_NORMALIZE );
        }
    glDisable( GL_NORMALIZE );

    glDisable( GL_FOG );
    glEnable(  GL_DEPTH_TEST );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glEnable( GL_TEXTURE_2D );
    TEX_Bind( fan->texture );
    TEX_SetFilter( GL_LINEAR, GL_LINEAR );
    TEX_SetWrap( GL_REPEAT, GL_REPEAT );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    for( i=0; i<fan->num; i++ )
        {
        glColor4f( 1.0f,1.0f,1.0f,1.0f-pow((float)i/(float)fan->num,.6f) );

        //f  = fan->po + fan->v*(t-fan->to) - fan->f*(float)(i+1)/(float)fan->num;
        f  = fan->po - fan->f*(float)(i+1)/(float)fan->num;
        deforma( fan->mesh+i, fan->obj->number_of_vertices, f );

        draw( fan->mesh+i, fan->obj, fan->lighting );
        }

    glPopMatrix();


    glDisable( GL_LIGHTING );
    glDisable( GL_NORMALIZE );
    glDisable( GL_TEXTURE_2D );
    glDisable( GL_BLEND );
    glColor4f( 1.0f,1.0f,1.0f,1.0f );
}

void PELO_SetMovim( PELO *me, float v, float f )
{
    me->v = v;
    me->f = f;
//    me->to = -455709.0f;
}

void PELO_SetLigthing( PELO *me, long val )
{
    me->lighting = val;   
}
