#include <malloc.h>     // malloc(), free()
#include <stdlib.h>     // srand(), rand()  
#include <assert.h>     // assert()

#include "math3d.h"
#include "turb.h"
#include "psys.h"

#ifdef WIN32
 #include <windows.h>
#endif
#include <GL/gl.h>


#include "nieve3d.h"

static void copo_init( NIEVE3D *ni, VEC *pos )
{
    pos->x = ni->x1 + (ni->x2-ni->x1)*(float)(rand()&255)/255.0;
    pos->y = ni->y2;
    pos->z = ni->z1 + (ni->z2-ni->z1)*(float)(rand()&255)/255.0;
}

void NIEVE3D_Start( NIEVE3D *ni )
{
    int i;

    srand( 12 );
    for( i=0; i<ni->psys.n; i++ )
        {
        ni->psys.p[i].pos.x = ni->x1 + (ni->x2-ni->x1)*(float)(rand()&255)/255.0;
        ni->psys.p[i].pos.y = ni->y1 + (ni->y2-ni->y1)*(float)(rand()&255)/255.0;
        ni->psys.p[i].pos.z = ni->z1 + (ni->z2-ni->z1)*(float)(rand()&255)/255.0;
        }
}


NIEVE3D *NIEVE3D_Init( int num, float width, float x1, float y1, float z1,
                                             float x2, float y2, float z2 )
{
    int     i;
    NIEVE3D *n;


    n = (NIEVE3D*)malloc( sizeof(NIEVE3D) );
    assert(n != NULL);

    n->vx =  0.5;
    n->vy = -2.0;
    n->vz =  1.0;
    n->dt =  0.0;
    n->x1 = x1;
    n->y1 = y1;
    n->z1 = z1;
    n->x2 = x2;
    n->y2 = y2;
    n->z2 = z2;


    TURB_Init( n->vtable, 11315 );

    i = PSYS_Init( &n->psys, num, width );
    assert(i!=0);
    /*
    for( i=0; i<n->psys.n; i++ )
        {
        n->psys.p[i].col.x = 1.0;
        n->psys.p[i].col.y = 1.0;
        n->psys.p[i].col.z = 1.0;
        n->psys.p[i].al    = 1.0;
        }
    */
    NIEVE3D_Start( n );

    return( n );
}




void NIEVE3D_Draw( NIEVE3D *ni, float t )
{
    int     i;
    VEC     pos, inc;
    PSYS    ps;
    float   dt;

    dt = .001*(t-ni->dt);

    ps = ni->psys;

    for( i=0; i<ps.n; i++ )
        {
        pos = ps.p[i].pos;
        VecScale( &pos, 1.0 );
        
        inc.x = 100.0 * ps.width*(-1.0+2.0*TURB_Turbulence2d( ni->vtable, 1+i, t*.1, 4 ));
        inc.y = 100.0 * ps.width*(-1.0+2.0*TURB_Turbulence2d( ni->vtable, 2+i, t*.1, 4 ));
        inc.z = 100.0 * ps.width*(-1.0+2.0*TURB_Turbulence2d( ni->vtable, 3+i, t*.1, 4 ));

        inc.x += ni->vx;
        inc.y += ni->vy;
        inc.z += ni->vz;

        inc.x += 1.0*(-1.0+2.0*TURB_Turbulence2d( ni->vtable, 4+i, t*.02, 4 ));
        inc.z += 1.0*(-1.0+2.0*TURB_Turbulence2d( ni->vtable, 5+i, t*.02, 4 ));
        inc.y *= (0.3+0.7*TURB_Turbulence2d( ni->vtable, 5+i, t*.02, 4 ));

        VecScale( &inc, dt );

        VecInc( &ps.p[i].pos, &inc );

        if( ps.p[i].pos.x<ni->x1 || ps.p[i].pos.x>ni->x2 ||
            ps.p[i].pos.y<ni->y1 || ps.p[i].pos.y>ni->y2 ||
            ps.p[i].pos.z<ni->z1 || ps.p[i].pos.z>ni->z2 )
            {
            copo_init( ni, &ps.p[i].pos );
            }
        }

    ni->dt = t;

    PSYS_Draw( &ps );



}

void NIEVE3D_End( NIEVE3D *ni )
{
    PSYS_Free( &ni->psys );
    free( ni );
}

void NIEVE3D_SetWind( NIEVE3D *ni, float x, float y, float z )
{       
    ni->vx = x;
    ni->vy = y;
    ni->vz = z;
}
