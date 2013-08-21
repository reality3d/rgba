#include <math.h>       // floor()
#include <string.h>     // memset()


#define INDEX_1D(ix)          (  (ix)&255 )
#define INDEX_2D(ix,iy)       (( (ix)+perm[(iy)&255])&255)
#define INDEX_3D(ix,iy,iz)    (( (ix)+perm[((iy)+perm[(iz)&255])&255])&255)
#define INDEX_4D(ix,iy,iz,iw) (( (ix)+perm[((iy)+perm[((iz)+perm[(iw)&255])&255])&255])&255)

extern "C"
{

static unsigned char perm[256] = {
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

//----------------------------------------------

static long mirand = 0;

static unsigned short mrand( void )
{
    mirand = 1 + mirand*0x015a4e35;

    return( (mirand>>16)&32767 );
}

static void msrand( long semilla )
{
    mirand = semilla;
}


//----------------------------------------------

static float OTROnoise4d( float *table, float x, float y, float z, float w)
{
    signed int      ix,iy,iz,iw;
    float           fx,fy,fz,fw;
    float           a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p;
    float           l1,l2,l3,l4,l5,l6,l7;
    float           l1b,l2b,l3b,l4b,l5b,l6b,l7b;

    ix = (signed int)floor(x);
    iy = (signed int)floor(y); 
    iz = (signed int)floor(z);
    iw = (signed int)floor(w);

    fx = x - ix;
    fy = y - iy;
    fz = z - iz;
    fw = w - iw;

    fx = fx*fx*(3.0f-2.0f*fx);
    fy = fy*fy*(3.0f-2.0f*fy);
    fz = fz*fz*(3.0f-2.0f*fz);
    fw = fw*fw*(3.0f-2.0f*fw);

    a = table[ INDEX_4D( ix,  iy,  iz  ,iw   ) ];
    b = table[ INDEX_4D( ix+1,iy,  iz  ,iw   ) ];
    c = table[ INDEX_4D( ix,  iy+1,iz  ,iw   ) ];
    d = table[ INDEX_4D( ix+1,iy+1,iz  ,iw   ) ];
    e = table[ INDEX_4D( ix,  iy,  iz+1,iw   ) ];
    f = table[ INDEX_4D( ix+1,iy,  iz+1,iw   ) ];
    g = table[ INDEX_4D( ix,  iy+1,iz+1,iw   ) ];
    h = table[ INDEX_4D( ix+1,iy+1,iz+1,iw   ) ];

    i = table[ INDEX_4D( ix,  iy,  iz  ,iw+1 ) ];
    j = table[ INDEX_4D( ix+1,iy,  iz  ,iw+1 ) ];
    k = table[ INDEX_4D( ix,  iy+1,iz  ,iw+1 ) ];
    l = table[ INDEX_4D( ix+1,iy+1,iz  ,iw+1 ) ];
    m = table[ INDEX_4D( ix,  iy,  iz+1,iw+1 ) ];
    n = table[ INDEX_4D( ix+1,iy,  iz+1,iw+1 ) ];
    o = table[ INDEX_4D( ix,  iy+1,iz+1,iw+1 ) ];
    p = table[ INDEX_4D( ix+1,iy+1,iz+1,iw+1 ) ];

    l1 = a + fx*(b-a);
    l2 = c + fx*(d-c);
    l3 = e + fx*(f-e);
    l4 = g + fx*(h-g);
    l5 = l1 + fy*(l2-l1);
    l6 = l3 + fy*(l4-l3);
    l7 = l5 + fz*(l6-l5);

    l1b = i + fx*(j-i);
    l2b = k + fx*(l-k);
    l3b = m + fx*(n-m);
    l4b = o + fx*(p-o);
    l5b = l1b + fy*(l2b-l1b);
    l6b = l3b + fy*(l4b-l3b);
    l7b = l5b + fz*(l6b-l5b);

    return( l7 + fw*(l7b-l7) );
}

static float OTROnoise3d( float *table, float x, float y, float z)
{
    signed int      ix,iy,iz;
    float           fx,fy,fz;
    float           a,b,c,d,e,f,g,h;
    float           l1,l2,l3,l4,l5,l6,l7;

    ix = (signed int)floor(x);
    iy = (signed int)floor(y); 
    iz = (signed int)floor(z);

    fx = x - ix;
    fy = y - iy;
    fz = z - iz;

    fx = fx*fx*(3.0f-2.0f*fx);
    fy = fy*fy*(3.0f-2.0f*fy);
    fz = fz*fz*(3.0f-2.0f*fz);

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

//----------------------------- API ------------------------------------

void OTROTURB_New( float *table, int sr )
{
    int i;

    msrand( sr );
    for( i=0; i<256; i++ )
        table[i] = ((float)(mrand()&255))*(1.0f/255.0f);
}

float OTROTURB_Turbulence4d( float *table, float x, float y, float z, float w, long octaves )
{
    float   t, s, v;
    int     i;

    t = 0.0f;
    s = 0.5f;

    for( i=0; i<octaves; i++ )
        {
        t += s*OTROnoise4d( table,x,y,z,w );
        x*=2.0f;
        y*=2.0f;
        z*=2.0f;
        w*=2.0f;
        s*=0.5f;
        }

    if( t<0.0f ) t=0.0f;
    if( t>1.0f ) t=1.0f;

    return( t );
}

float OTROTURB_Turbulence3d( float *table, float x, float y, float z, long octaves )
{
    float   t, s, v;
    int     i;

    t = 0.0f;
    s = 0.5f;

    for( i=0; i<octaves; i++ )
        {
        t += s*OTROnoise3d( table,x,y,z );
        x*=2.0f;
        y*=2.0f;
        z*=2.0f;
        s*=0.5f;
        }

    if( t<0.0f ) t=0.0f;
    if( t>1.0f ) t=1.0f;

    return( t );
}


}
