#ifndef _PELO_H_
#define _PELO_H_

#include "../apifantasy/fantasy_api.h"
#include <glt/glt.h>



typedef struct
{
    float   *norm;
    float   *vert;
    float   cen[3];
    float   ra;
}MESH;

typedef struct
{
    int         num;
    float       lon;
    float       tscale;

    float       v, f;
    long        lighting;

    //-------------------
    object_type *obj;
    MESH        *mesh;
    TTexture        *texture;
    FantasyAPI  *api;

    long    lockarrays;
//    float   *sincos;

    float   to;
    float   po;

}PELO;
       
PELO *PELO_Init( int num, float longitud, char *textname, float tscale );
void  PELO_End( PELO *me );
void  PELO_DoFrame( PELO *me, float t );
void  PELO_SetPointer( PELO *me, int pointer, char *name );
void  PELO_SetMovim( PELO *me, float v, float f );
void  PELO_SetLigthing( PELO *me, long val );

#endif
