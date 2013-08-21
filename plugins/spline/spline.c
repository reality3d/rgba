///////////////////////////////////////////////////////////////////////
// SPLINE  -  epsilum
///////////////////////////////////////////////////////////////////////
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include "spline.h"
#include <packer/packer.h>

static char magic_binpoly[] = "SPL2";
TSpline *SPLINE_New (char *filename)
{
    int      i;
    PFILE   *fp;
    TSpline *spl;
    char     magic[5];

    fp = pak_open (filename, "rb");
    assert (fp != NULL);

    // Check magic
    pak_read (magic, 1, 4, fp);
    magic[4] = '\0';
    assert (!strcmp (magic, magic_binpoly));

    spl = (TSpline *) malloc (sizeof (TSpline));
    assert (spl != NULL);
    spl->tfactor = 1.0f;

    // Number of nodes => number of polynomials = nodes + 1
    pak_read (&spl->nkeys, 4, 1, fp);
    assert (spl->nkeys != 0);

    spl->p = (TKey *) malloc ((spl->nkeys + 1) * sizeof (TKey));
    assert (spl->p != NULL);

    for (i=0; i<spl->nkeys; i++)
    {
        // Read time
        pak_read (&spl->p[i].t, 4, 1, fp);

        // Read polynomial coefficients
        pak_read (spl->p[i].c, 4, 4, fp);
    }

    // Last polynomial
    spl->p[i].c[0] = 0.0f;
    spl->p[i].c[1] = 0.0f;
    spl->p[i].c[2] = 0.0f;
    pak_read (&spl->p[i].t, 4, 1, fp);
    pak_read (&spl->p[i].c[3], 4, 1, fp);

    spl->nkeys ++;

    pak_close (fp);

    spl->t0 = spl->p[0].t;
    spl->t1 = spl->p[i].t;

    //spl->loop_mode = SPLINE_LOOP_CONSTANT;
    //spl->loop_mode = SPLINE_LOOP_CONTINUITY;
    spl->loop_mode = SPLINE_LOOP_MOD;

    return spl;
}

void SPLINE_Delete (TSpline *spline)
{
    free (spline->p);
    free (spline);
}

void SPLINE_SetFactor (TSpline *spl, float tfactor)
{
    spl->tfactor = tfactor;
}

static float SPLINE_GetLoopedTime (TSpline *spl, float t)
{
    float taux;
    int   n;

    switch (spl->loop_mode)
    {
    case SPLINE_LOOP_CONTINUITY:
        // Nothing to do
        break;

    case SPLINE_LOOP_MOD:
        t = fmod (t, spl->t1);
        break;

    case SPLINE_LOOP_PINGPONG:
        n = 0;
        taux = spl->t1;
        while (taux < t)
        {
            taux += spl->t1;
            n ++;
        }

        if (n&1)
            t = fmod (t, spl->t1);
        else
            t = spl->t1 - fmod (t, spl->t1);

        break;

    case SPLINE_LOOP_CONSTANT:
    default:
        if (t <= spl->t0)
             t = spl->p[0].t;
        else if (t >= spl->t1)
            t = spl->p[spl->nkeys-1].t;
        break;
    }

    return t;
}

float SPLINE_Eval (TSpline *spl, float t)
{
    float *c;      // Polynomial coefficients
    int    i;

    // Nos devuelve un t "loopeado" segun el modo en el que estemos
    t = SPLINE_GetLoopedTime (spl, t * spl->tfactor);

    // Search interval
    i = 1;
    while ((t > spl->p[i].t) && (i < spl->nkeys - 1))
        i ++;

    c = spl->p[i-1].c;

    // Base de potencias desplazadas en intervalo [0, 1]
    t = (t - spl->p[i-1].t) / (spl->p[i].t - spl->p[i-1].t);
    

//    printf("Tiempo %f. Devuelvo %f\n",t,(c[0] * t * t + c[1] * t + c[2]) * t + c[3]);

    // Eval polynomial
    return ((c[0] * t * t + c[1] * t + c[2]) * t + c[3]);
}
