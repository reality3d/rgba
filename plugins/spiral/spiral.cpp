
// solo fallos en las interpolaciones...

// changec1( col, time ) // time<=0.0f -> set
// changec2( col, time ) // time<=0.0f -> set
// changew1( cw1, time ) // time<=0.0f -> set
// changew2( cw2, time ) // time<=0.0f -> set
// changede( def, time ) // time<=0.0f -> set
             
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include <math.h>       // sinf(), ...
#include <malloc.h>		// malloc(), free()
#include <string.h>		// memset()
#include <math.h>		// sqr(), log(), fmod()
    
typedef struct
{
	int		state;
	float	to;
	float	td;
}PARAM_STATE;

#define PALETASIZE  1024
#define NUMDEFORMS	6
#define NUMPARAMS	5

typedef struct
{
	// --- actual state ---
    float           r0, g0, b0, a0;
    float           r1, g1, b1, a1;
    float           cw1;
    float           cw2;
	int				ta;
    float           speed;

	// --- state change request ---
	PARAM_STATE		pst[NUMPARAMS];
	float			ri0, gi0, bi0, ai0;
	float			rf0, gf0, bf0, af0;
	float			ri1, gi1, bi1, ai1;
	float			rf1, gf1, bf1, af1;
	float			cwf1, cwi1;
	float			cwf2, cwi2;

	int				tai, taf;
	
	// --- geometry --- 
    int             xres, yres;

    unsigned int    tid;
    float           *verts;
    float           *texts;
    int             *indices;
	int				nv, ni;

	// --- LUTs & textures ---

	float			*tablas[NUMDEFORMS];
    float           *tabla;
    unsigned char   *textura;

}SPIRAL;

//-------------------------------------------------
static float smoothstep( float x, float a, float b )
{
    float f;

    f = (x-a)/(b-a);

    if( f<0.0f ) f=0.0f;
    if( f>1.0f ) f=1.0f;

    f = f*f*(3.0f-2.0f*f);

    return( f );
}

static void	interpola_tablas( float *dst, float *ti, float *tf, int amount, float h )
{
	while( amount-- )
		{
		*dst++ = ti[0] + (tf[0]-ti[0])*h;
		ti++;
		tf++;
		}
}

float CRUMPLE_Do( float *ptr, int num, float sca, float x, float y )
{
    int     k;
    float   dx, dy, d, min;


    x = fmodf( x, 1.0f );
    y = fmodf( y, 1.0f );

    min = 1e20;
    for( k=0; k<num; k++ )
        {
        dx = x - *ptr++;
        dy = y - *ptr++;

        dx = fabsf(dx); if( dx>.5f ) dx -= 1.0f;
        dy = fabsf(dy); if( dy>.5f ) dy -= 1.0f;

        d = dx*dx + dy*dy;

        if( d<min ) min = d;
        }

    d = sca*sqrtf( min );

	if( d>1.0f ) d=1.0f;

    return( d );
}

static void hace_tabla( SPIRAL *me, int tipo )
{
	float   a,  r, x, y, f;
    int     i, j;
    float   *ta;

    ta = me->tablas[tipo];


    for( j=0; j<=me->yres; j++ )
    for( i=0; i<=me->xres; i++ )
        {
        x =  1.0f - 2.0f*(float)i/(float)me->xres;
        y = -1.0f + 2.0f*(float)j/(float)me->yres;
        r = sqrtf( x*x + y*y );
        a = atan2f( y, x );

     
        switch( tipo )
            {
            default:
            case 0:
                f = a*(5.0f/3.1416f);
                break;

            case 1:
                f = y + sinf(3.1416f*x) + sinf(3.1416f*y);
                break;

            case 2:
                f = r*2.0f + sinf(2.0f*r+3.0f*a);
                break;

            case 3:
                f = r + a;
                break;

            case 4:
                f = x + sinf(6.2831f*1.0f*r) + a*4.0f;
                break;

            case 5:
                f = sinf(6.2831f*1.0f*r) + sinf(2.0f*r+3.0f*a);
                break;
            }

        *ta++ = f;
        }

}

static void calc_texcoord( SPIRAL *me, float t )
{
    float   a,  r, x, y, f;
    int     i, j;
    float   *ta;
    float u[4];
    float *te = me->texts;
	int tipo = me->ta;

    float   tt = t*me->speed;

	if( tipo>=NUMDEFORMS )
		tipo=NUMDEFORMS;

    //ta = me->tablas[tipo];
	ta = me->tabla;

    for( j=0; j<me->yres; j++ )
	{
    for( i=0; i<me->xres; i++ )
        {
        u[0] = tt+ta[me->xres+1];    // 1   2
        u[1] = tt+ta[0];             //
        u[2] = tt+ta[1];             // 0   3
        u[3] = tt+ta[me->xres+2];
        ta++;

//----------
		if( (u[1]-u[0])>2.0f )
			u[0] += 1.0f + floor(u[1]-u[0]);

if( (u[0]-u[1])>2.0f )
	u[1] += 1.0f + floor(u[0]-u[1]);

		if( (u[3]-u[2])>2.0f ) 
			u[2] += 1.0f + floor(u[3]-u[2]);

if( (u[2]-u[3])>2.0f ) 
	u[3] += 1.0f + floor(u[2]-u[3]);
//----------


        te[ 0+4*(me->xres*j+i) ] = u[0];
        te[ 1+4*(me->xres*j+i) ] = u[1];
        te[ 2+4*(me->xres*j+i) ] = u[2];
        te[ 3+4*(me->xres*j+i) ] = u[3];
        }
	ta++;
	}


}




static void hacepaleta( SPIRAL *me )
{
    int     i;
    float   f, h;

    for( i=0; i<PALETASIZE; i++ )
        {
        f = (float)i*(1.0f/(float)PALETASIZE);

        h  = smoothstep( f, 0.5f-me->cw1-me->cw2, .5f-me->cw1+me->cw2 );
        h -= smoothstep( f, 0.5f+me->cw1-me->cw2, .5f+me->cw1+me->cw2 );

        me->textura[0+4*i] = (int)(me->r0 + (me->r1-me->r0)*h);
        me->textura[1+4*i] = (int)(me->g0 + (me->g1-me->g0)*h);
        me->textura[2+4*i] = (int)(me->b0 + (me->b1-me->b0)*h);
        me->textura[3+4*i] = (int)(me->a0 + (me->a1-me->a0)*h);
        }

    glBindTexture( GL_TEXTURE_1D, me->tid );
    glTexSubImage1D( GL_TEXTURE_1D, 0, 0, PALETASIZE, GL_RGBA, GL_UNSIGNED_BYTE, me->textura );
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT );

}

//----------------------------------------------------------------------------

void SPIRAL_Delete( void *vme )
{
    SPIRAL  *me = (SPIRAL*)vme;
	int		i;

	if( !me ) return;

    if( me->verts ) free( me->verts );
    if( me->texts ) free( me->texts );
    if( me->indices ) free( me->indices );
    if( me->textura ) free( me->textura );
    if( me->tabla ) free( me->tabla );
	for( i=0; i<NUMDEFORMS; i++ )
		if( me->tablas[i] )
			free( me->tablas[i] );
    if( me->tid ) glDeleteTextures( 1, &me->tid );


    free( me );
}

void *SPIRAL_New( void )
{
	SPIRAL *me;
    int     i, j, k;

	me = (SPIRAL*)malloc( sizeof(SPIRAL) );
	if( !me )
		return( 0 );

	memset( me, 0, sizeof(SPIRAL) );

    // default values
    me->xres = 64;
    me->yres = 48;

    me->r0 = 229.0f;
    me->g0 = 255.0f;
    me->b0 = 225.0f;
    me->a0 = 225.0f;

    me->r1 = 255.0f;
    me->g1 = 255.0f;
    me->b1 = 255.0f;
    me->a1 = 225.0f;

    me->cw1 = 0.20f;
    me->cw2 = 0.02f;

    me->speed = 1.0f;

	me->nv = 4*me->xres*me->yres;
	me->ni = 4*me->xres*me->yres;

    me->verts = (float*)malloc( me->nv*2*sizeof(float) );
    if( !me->verts )
        {
        SPIRAL_Delete( me );
        return( 0 );
        }

    me->texts = (float*)malloc( me->nv*1*sizeof(float) );
    if( !me->texts )
        {
        SPIRAL_Delete( me );
        return( 0 );
        }

    me->indices = (int*)malloc( me->ni*sizeof(int) );
    if( !me->indices )
        {
        SPIRAL_Delete( me );
        return( 0 );
        }

    me->textura = (unsigned char*)malloc( PALETASIZE*4 );
    if( !me->textura )
        {
        SPIRAL_Delete( me );
        return( 0 );
        }
        
    me->tabla = (float*)malloc( (me->xres+1)*(me->yres+1) *1*sizeof(float) );
    if( !me->tabla )
        {
        SPIRAL_Delete( me );
        return( 0 );
        }

    // crea indices
    k = 0;
    for( j=0; j<me->yres; j++ )
    for( i=0; i<me->xres; i++ )
        {
        me->indices[k++] = 0+4*(me->xres*j+i);
        me->indices[k++] = 1+4*(me->xres*j+i);
        me->indices[k++] = 2+4*(me->xres*j+i);
        me->indices[k++] = 3+4*(me->xres*j+i);
        }

    // crea vertices
    k = 0;
    for( j=0; j<me->yres; j++ )
	for( i=0; i<me->xres; i++ )
		{
        me->verts[k++] = (float)(i+0)/(float)me->xres;
        me->verts[k++] = (float)(j+1)/(float)me->yres;

        me->verts[k++] = (float)(i+0)/(float)me->xres;
        me->verts[k++] = (float)(j+0)/(float)me->yres;

        me->verts[k++] = (float)(i+1)/(float)me->xres;
        me->verts[k++] = (float)(j+0)/(float)me->yres;

        me->verts[k++] = (float)(i+1)/(float)me->xres;
        me->verts[k++] = (float)(j+1)/(float)me->yres;
		}


    // sube textura 1d
    glGenTextures( 1, &me->tid );
    if( !me->tid )
        {
        SPIRAL_Delete( me );
        return( 0 );
        }


    glBindTexture( GL_TEXTURE_1D, me->tid );
    glTexImage1D( GL_TEXTURE_1D, 0, GL_RGBA8,
                  PALETASIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, me->textura );
      
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT );

    // crea textura
    hacepaleta( me );

	for( i=0; i<NUMDEFORMS; i++ )
		{
		me->tablas[i] = (float*)malloc( (me->xres+1)*(me->yres+1) *1*sizeof(float) );
		if( !me->tablas[i] )
			{
			SPIRAL_Delete( me );
			return( 0 );
			}
		hace_tabla( me, i );
		}

	memcpy( me->tabla, me->tablas[me->ta], (me->xres+1)*(me->yres+1)*1*sizeof(float) );

	return( me );
}

static void change_state( SPIRAL *me, float t )
{
	int		i;
	float	h;

	//-----------------

	// for each param
	for( i=0; i<NUMPARAMS; i++ )
		{
		if( me->pst[i].state==1 )
			{
			me->pst[i].state = 2;
			me->pst[i].to  = t;
			}
		if( me->pst[i].state==2 )
			{
			if( (t-me->pst[i].to)>me->pst[i].td )
				{
				me->pst[i].state = 0;
				}
			}

		}
	
	//-----------------

	if( me->pst[0].state==2 )
		{
		h = (t - me->pst[0].to) / me->pst[0].td;
		me->r0 = me->ri0 + h*(me->rf0-me->ri0);
		me->g0 = me->gi0 + h*(me->gf0-me->gi0);
		me->b0 = me->bi0 + h*(me->bf0-me->bi0);
		me->a0 = me->ai0 + h*(me->af0-me->ai0);
		}

	if( me->pst[1].state==2 )
		{
		h = (t - me->pst[1].to) / me->pst[1].td;
		me->r1 = me->ri1 + h*(me->rf1-me->ri1);
		me->g1 = me->gi1 + h*(me->gf1-me->gi1);
		me->b1 = me->bi1 + h*(me->bf1-me->bi1);
		me->a1 = me->ai1 + h*(me->af1-me->ai1);
		}

	if( me->pst[2].state==2 )
		{
		h = (t - me->pst[2].to) / me->pst[2].td;
		me->cw1 = me->cwi1 + h*(me->cwf1-me->cwi1);
		}
	if( me->pst[3].state==2 )
		{
		h = (t - me->pst[3].to) / me->pst[3].td;
		me->cw2 = me->cwi2 + h*(me->cwf2-me->cwi2);
		}

	if( me->pst[4].state==2 )
		{
		h = (t - me->pst[4].to) / me->pst[4].td;
		interpola_tablas( me->tabla, 
						  me->tablas[me->tai], 
						  me->tablas[me->taf], 
						  (me->xres+1)*(me->yres+1), h );
		}

	if( me->pst[0].state==2 || me->pst[1].state==2 ||
		me->pst[2].state==2 || me->pst[3].state==2 )
		{	
		hacepaleta( me  );
		}
}

void  SPIRAL_DoFrame( void *vme, float t )
{
    int i;
    SPIRAL *me = (SPIRAL*)vme;

	if( !me ) return;

	change_state( me, t );

    calc_texcoord( me, t );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glOrtho( 0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f );

    glDisable( GL_LIGHTING );
    glDisable( GL_FOG );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_NORMALIZE );
    glDisable( GL_BLEND );
    glDisable( GL_TEXTURE_2D );
    glShadeModel( GL_SMOOTH );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glEnable( GL_TEXTURE_1D );
    glBindTexture( GL_TEXTURE_1D, me->tid );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

/*
glClearColor(0,0,1,0);
glClear( GL_COLOR_BUFFER_BIT );
glColor3f( 1,1,1);
glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
*/


    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer( 1, GL_FLOAT, 1*sizeof(float), me->texts );

    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 2, GL_FLOAT, 2*sizeof(float), me->verts );

	glDrawElements( GL_QUADS, me->ni, GL_UNSIGNED_INT, me->indices );

    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );
		
    glDisable( GL_TEXTURE_1D );
}


void SPIRAL_ChangeColor1( void *vme, float r, float g, float b, float a, float time )
{
    SPIRAL *me = (SPIRAL*)vme;

	if( !me ) return;

	if( time<=0.0f )
		{
		me->r0 = r;
		me->g0 = g;
		me->b0 = b;
		me->a0 = a;
		hacepaleta( me );
		}
	else
		{
		me->pst[0].state = 1;
		me->pst[0].td  = time;
		me->rf0 = r;
		me->gf0 = g;
		me->bf0 = b;
		me->af0 = a;
		me->ri0 = me->r0;
		me->gi0 = me->g0;
		me->bi0 = me->b0;
		me->ai0 = me->a0;
		}
}

void SPIRAL_ChangeColor2( void *vme, float r, float g, float b, float a, float time )
{
    SPIRAL *me = (SPIRAL*)vme;

	if( !me ) return;

	if( time<=0.0f )
		{
		me->r1 = r;
		me->g1 = g;
		me->b1 = b;
		me->a1 = a;
		hacepaleta( me );
		}
	else
		{
		me->pst[1].state = 1;
		me->pst[1].td  = time;
		me->rf1 = r;
		me->gf1 = g;
		me->bf1 = b;
		me->af1 = a;
		me->ri1 = me->r1;
		me->gi1 = me->g1;
		me->bi1 = me->b1;
		me->ai1 = me->a1;
		}
}

void SPIRAL_ChangeWidth1( void *vme, float w, float time )
{
    SPIRAL *me = (SPIRAL*)vme;

	if( !me ) return;

	if( time<=0.0f )
		{
		me->cw1 = w;
		hacepaleta( me );
		}
	else
		{
		me->pst[2].state = 1;
		me->pst[2].td  = time;
		me->cwf1 = w;
		me->cwi1 = me->cw1;
		}
}

void SPIRAL_ChangeWidth2( void *vme, float w, float time )
{
    SPIRAL *me = (SPIRAL*)vme;

	if( !me ) return;

	if( time<=0.0f )
		{
		me->cw2 = w;
		hacepaleta( me );
		}
	else
		{
		me->pst[3].state = 1;
		me->pst[3].td  = time;
		me->cwf2 = w;
		me->cwi2 = me->cw1;
		}
}

void SPIRAL_ChangeTable( void *vme, int ta, float time )
{
	SPIRAL *me = (SPIRAL*)vme;

	if( !me ) return;

	if( ta<0 ) ta=0;
	if( ta>=NUMDEFORMS ) ta=NUMDEFORMS-1;

	if( time<=0.0f )
		{
		me->ta = ta;
		memcpy( me->tabla, me->tablas[ta], (me->xres+1)*(me->yres+1)*1*sizeof(float) );
		}
	else
		{
		me->pst[4].state = 1;
		me->pst[4].td  = time;
		me->taf = ta;
		me->tai = me->ta;
		}
}

void SPIRAL_SetSpeed( void *vme, float speed )
{
	SPIRAL *me = (SPIRAL*)vme;

	if( !me ) return;

    me->speed = speed;
}

