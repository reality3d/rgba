#include <math.h>   // floor()
#include <stdlib.h> // rand(), srand()

#define TABSIZE	256

#define INDEX_3D(ix,iy,iz)    (( (ix)+perm[((iy)+perm[(iz)&255])&255])&255)

static unsigned char perm[TABSIZE] = {
        225,155,210,108,175,199,221,144,203,116, 70,213, 69,158, 33,252,
          5, 82,173,133,222,139,174, 27,  9, 71, 90,246, 75,130, 91,191,
        169,138,  2,151,194,235, 81,  7, 25,113,228,159,205,253,134,142,
        248, 65,224,217, 22,121,229, 63, 89,103, 96,104,156, 17,201,129,
         36,  8,165,110,237,117,231, 56,132,211,152, 20,181,111,239,218,
        170,163, 51,172,157, 47, 80,212,176,250, 87, 49, 99,242,136,189,
        162,115, 44, 43,124, 94,150, 16,141,247, 32, 10,198,223,255, 72,
         53,131, 84, 57,220,197, 58, 50,208, 11,241, 28,  3,192, 62,202,
         18,215,153, 24, 76, 41, 15,179, 39, 46, 55,  6,128,167, 23,188,
        106, 34,187,140,164, 73,112,182,244,195,227, 13, 35, 77,196,185,
         26,200,226,119, 31,123,168,125,249, 68,183,230,177,135,160,180,
         12,  1,243,148,102,166, 38,238,251, 37,240,126, 64, 74,161, 40,
        184,149,171,178,101, 66, 29, 59,146, 61,254,107, 42, 86,154,  4,
        236,232,120, 21,233,209, 45, 98,193,114, 78, 19,206, 14,118,127,
         48, 79,147, 85, 30,207,219, 54, 88,234,190,122, 95, 67,143,109,
        137,214,145, 93, 92,100,245,  0,216,186, 60, 83,105, 97,204, 52
};

void kkTURB_Init( float *table, int sr )
{
    int     i;

    srand( sr );
    for( i=0; i<TABSIZE; i++ )
        table[i] = -1.0 + 2.0*(float)(rand()&255)/255.0;

}


static float kknoise3d( float *table, float x, float y, float z)
{
    signed int  ix,iy,iz;
    float       fx,fy,fz;
    float       a,b,c,d,e,f,g,h;
    float       l1,l2,l3,l4,l5,l6,l7;

    ix = (signed int)floor(x);
    iy = (signed int)floor(y); 
    iz = (signed int)floor(z);

    fx = x - ix;
    fy = y - iy;
    fz = z - iz;

    a = table[ INDEX_3D( ix,  iy,  iz   ) ];
    b = table[ INDEX_3D( ix+1,iy,  iz   ) ];
    c = table[ INDEX_3D( ix,  iy+1,iz   ) ];
    d = table[ INDEX_3D( ix+1,iy+1,iz   ) ];
    e = table[ INDEX_3D( ix,  iy,  iz+1 ) ];
    f = table[ INDEX_3D( ix+1,iy,  iz+1 ) ];
    g = table[ INDEX_3D( ix,  iy+1,iz+1 ) ];
    h = table[ INDEX_3D( ix+1,iy+1,iz+1 ) ];

    l1 = a + fx*(b-a);
    l2 = c + fx*(d-c);
    l3 = e + fx*(f-e);
    l4 = g + fx*(h-g);

    l5 = l1 + fy*(l2-l1);
    l6 = l3 + fy*(l4-l3);

    l7 = l5 + fz*(l6-l5);

    return( l7 );
}



float kkTURB_Turbulence3d( float *table, float x, float y, float z, long octaves )
{
    float   t, s;

    t = 0.0;
    s = 1.0;


    while( octaves-- )
        {
        s*=0.5;
        t += kknoise3d(table,x,y,z)*s;
        x*=2.0;
        y*=2.0;
        z*=2.0;
        }

    if( t> 1.0 ) t= 1.0;
    if( t<-1.0 ) t=-1.0;

    t = fabs(t);

    return( t );
}
