///////////////////////////////////////////////////////////////////////
// SPLINE  -  epsilum
///////////////////////////////////////////////////////////////////////
#ifndef __SPLINE_H__
#define __SPLINE_H__


#define SPLINE_LOOP_CONSTANT    0
#define SPLINE_LOOP_CONTINUITY  1
#define SPLINE_LOOP_MOD         2
#define SPLINE_LOOP_PINGPONG    3

typedef struct
{
    float t;      // Initial time
    float c[4];   // Polynomial coefficients
} TKey;

typedef struct
{
    int    nkeys;  // Number of cubic polynomials
    TKey  *p;      // Array of polynomials keys

    float t0;      // Initial time
    float t1;      // Final time
    float tfactor; // time factor

    int   loop_mode;
} TSpline;


TSpline *SPLINE_New       (char *filename);
void     SPLINE_SetFactor (TSpline *spline, float tfactor);
float    SPLINE_Eval      (TSpline *spline, float t);
void     SPLINE_Delete    (TSpline *spline);


#endif // __SPLINE_H__
