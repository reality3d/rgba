//
// Interpolador
//

#ifdef WIN32
#include <windows.h>
#endif
#include <malloc.h>
#include "interp.h"

TInterp *INTERP_New (void)
{
    TInterp *self;

    self = (TInterp *) malloc (sizeof (TInterp));
    if (!self)
        return 0;

    memset (self, 0, sizeof (TInterp));

    return self;
}

void INTERP_Delete (TInterp *self)
{
    free (self);
}

void INTERP_Start (TInterp *self, float val_src, float val_dst, float t_interval)
{
    self->val_src    = val_src;
    self->val_dst    = val_dst;
    self->t_src      = (float) Epopeia_GetTimeMs ( );
    self->t_dst      = self->t_src + t_interval;
    self->interval   = t_interval;
    self->transition = 1;
}

// RET: 0 - FIN
//      1 - OK
#define PI 3.141592
int INTERP_Get (TInterp *self, float *dst)
{
    float dt;
    float t;
    float res;

    t = (float) Epopeia_GetTimeMs ( );

    if (t > self->t_dst)
    {
        *dst = self->val_dst;
        self->transition = 0;

        return 0;
    }
    else
    {
        dt = (t - self->t_src) / self->interval;
        //dt = sin (dt*PI - PI * 0.5) * 0.5 + 0.5;
        res = (1.0 - dt) * self->val_src + dt * self->val_dst;
        *dst = res;

        return 1;
    }
}

int INTERP_IsEnd (TInterp *self)
{
    return !self->transition;
}
