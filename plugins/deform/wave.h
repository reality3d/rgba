#ifndef __WAVE_H__
#define __WAVE_H__

#include "deform.h"

#define MAX_WAVE_RECORDS 32

typedef struct
{
    float t;
    float d[2];
} TData;

typedef struct
{
    // ---- Data List's ----
    int    npoint;
    TData *lpoint;

    int    nviscosity;
    TData *lviscosity;

    int    nheight;
    TData *lheight;

    int    nripple;
    TData *lripple;

    int    nwidth;
    TData *lwidth;

    int    nwait;
    TData *lwait;

    // ---- Timers ----
    float t0;  // Base time
    float point_t;
    float last_viscosity;
    float last_height;
    float last_ripple;
    float last_width;
    float last_wait;

    // ---- Used state ----
    int is_using;
} TWaveRecord;

void WAVE_Init   (void);
void WAVE_RunAll (TDeform *d, float t);
void WAVE_Exec   (TDeform *d, TWaveRecord *r, float t);
void WAVE_Load   (TDeform *d, int index, char *filename);
void WAVE_Start  (TDeform *d, int index);
void WAVE_Stop   (TDeform *d, int index);
void WAVE_Delete (TDeform *d, int index);


#endif // __WAVE_H__
