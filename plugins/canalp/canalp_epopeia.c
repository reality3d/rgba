///////////////////////////////////////////////////////////////////////
//
// CANAL PLUS CODER Epopeia interface
//
///////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <malloc.h>

#include <epopeia/epopeia_system.h>
#include "canalp.h"

///////////////////////////////////////////////////////////////////////
// TIMER CODE
///////////////////////////////////////////////////////////////////////
/*static double  _counter_frequency;
static __int64 _init_ticks;
static double  _timer_frequency;


static int TDS_InitTimer (int freq)
{

    __int64 cnt_freq;

    // Get high-resolution performance counter frequency
    if (!QueryPerformanceFrequency ((LARGE_INTEGER*) &cnt_freq))
        return 0;
    _counter_frequency = (double) cnt_freq;

    // Get init counter ticks
    if (!QueryPerformanceCounter ((LARGE_INTEGER*) &_init_ticks))
        return 0;

    _timer_frequency = (double) freq;

    return 1;
}

static double TDS_GetTimer (void)
{
    return unsigned long Epopeia_GetTimeMs(void);

    // get time
    __int64 ticks;
    QueryPerformanceCounter ((LARGE_INTEGER*) &ticks);

    // convert to seconds and return
    return (double) (ticks - _init_ticks) * _timer_frequency / (double) _counter_frequency;
}*/

static int TDS_InitTimer (int freq)
{
    return 1;
}

static double TDS_GetTimer (void)
{
    return (double)Epopeia_GetTimeMs()*0.001;

}

///////////////////////////////////////////////////////////////////////
// END TIMER CODE
///////////////////////////////////////////////////////////////////////


typedef struct
{
    int    val_src, val_dst;
    double t_src, t_dst;
    double interval;

    int transition;
} interp_t;

//
// INTERPOLADOR
//

static interp_t it, it2;

static int it_init (interp_t *it, int val_src, int val_dst, float t_interval)
{
    it->val_src    = val_src;
    it->val_dst    = val_dst;
    it->t_src      = TDS_GetTimer ( );
    it->t_dst      = it->t_src + t_interval;
    it->interval   = t_interval;
    it->transition = 1;

    return 1;
}

// RET: 0 - FIN
//      1 - OK
#define PI 3.141592
static int it_get (interp_t *it, int *val)
{
    double dt;
    double t = TDS_GetTimer ( );
    float  res;

    if (t > it->t_dst)
    {
        *val = it->val_dst;
        it->transition = 0;

        return 0;
    }
    else
    {
        dt = (t - it->t_src) / it->interval;
        //dt = sin (dt*PI - PI * 0.5) * 0.5 + 0.5;
        res = (1.0 - dt) * (float)it->val_src + dt * (float)it->val_dst;
        *val = (int) res;

        return 1;
    }
}


TCanalPlus *CANALP_InitFirst (void)
{
    TDS_InitTimer (1);
    it.transition = 0;
    it2.transition = 0;

    return CANALP_Init ( );
}

void CANALP_DoFrame (TCanalPlus *self, TRenderContext *RC, TSoundContext *SC)
{
    if (it.transition)
        it_get (&it, &self->num_lines);

    if (it2.transition)
        it_get (&it2, &self->max_despl);

    CANALP_Do (self);
}

void CANALP_Start (TCanalPlus *self, int z)
{
}

void CANALP_Stop (TCanalPlus *self)
{
}

void CANALP_SetMode (TCanalPlus *self, int max_despl, int num_lines)
{
    self->max_despl = max_despl;
    self->num_lines = num_lines;
}

void CANALP_SetNumLinesChange (TCanalPlus *self, int num_lines, float dt)
{
    it_init (&it, self->num_lines, num_lines, dt);
}

void CANALP_SetDesplChange (TCanalPlus *self, int despl, float dt)
{
    it_init (&it2, self->max_despl, despl, dt);
}

void CANALP_SetDeform (TCanalPlus *self, int deform)
{
    self->deform = deform;
    self->t0     = Epopeia_GetTimeMs ( );
}

void CANALP_SetSpeed (TCanalPlus *self, float speed)
{
    self->speed = speed;
}



EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return "Canal Plus Encoder by epsilum";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId  Id;
    TMethod *pMethod;

    Id = Type_Register ("CanalP");

    pMethod = Method_New ("New", (void*)CANALP_InitFirst);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Delete", (void*)CANALP_Kill);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("DoFrame", (void*)CANALP_DoFrame);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Start", (void*)CANALP_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Stop", (void*)CANALP_Stop);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetMode", (void*)CANALP_SetMode);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetNumLinesChange", (void*)CANALP_SetNumLinesChange);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetDesplChange", (void*)CANALP_SetDesplChange);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetDeform", (void*)CANALP_SetDeform);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetSpeed", (void*)CANALP_SetSpeed);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);
}
