#ifndef __INTERP_H__
#define __INTERP_H__

typedef struct
{
    float  val_src, val_dst;
    double t_src, t_dst;
    double interval;
    int    transition;
} TInterp;


TInterp *INTERP_New    (void);
void     INTERP_Delete (TInterp *self);
void     INTERP_Start  (TInterp *self, float val_src, float val_dst, float t_interval);
int      INTERP_Get    (TInterp *self, float *dst);
int      INTERP_IsEnd  (TInterp *self);

#endif // __INTERP_H__
