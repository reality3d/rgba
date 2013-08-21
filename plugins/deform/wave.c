#ifdef WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include "deform.h"
#include "wave.h"
#include <packer/packer.h>

static TWaveRecord rec[MAX_WAVE_RECORDS];

void WAVE_RunAll (TDeform *d, float t)
{
    int i;
    printf("RunAll 1\n");
    for (i=0; i<MAX_WAVE_RECORDS; i++)
    {
        if (rec[i].is_using)
        {
            printf("RunAll 2\n");
            WAVE_Exec (d, &rec[i], t - rec[i].t0);
            printf("RunAll 2 back\n");
        }
    }
}

void WAVE_Exec (TDeform *d, TWaveRecord *r, float t)
{
    TData d0, d1;
    int   i;

    float viscosity = r->last_viscosity;
    float height    = r->last_height;
    float width     = r->last_width;
    float ripple    = r->last_ripple;

    printf("Exec: viscosity\n");
    // Found viscosity
    i = 1;
    while (i < r->nviscosity)
    {
        if (r->lviscosity[i].t > t)
        {
            i --;
            r->last_viscosity = viscosity = r->lviscosity[i].d[0];
            break;
        }

        i ++;
    }
    if (i == r->nviscosity)
    {
        viscosity = r->lviscosity[r->nviscosity-1].d[0];
        //viscosity = r->last_viscosity;
    }


    // Found height
    i = 1;
    while (i < r->nheight)
    {
        if (r->lheight[i].t > t)
        {
            i --;
            r->last_height = height = r->lheight[i].d[0];
            break;
        }

        i ++;
    }
    if (i == r->nheight)
    {
        height = r->lheight[r->nheight-1].d[0];
        //height = r->last_height;
    }


    // Found width
    i = 1;
    while (i < r->nwidth)
    {
        if (r->lwidth[i].t > t)
        {
            i --;
            r->last_width = width = r->lwidth[i].d[0];
            break;
        }

        i ++;
    }
    if (i == r->nwidth)
    {
        width = r->lwidth[r->nwidth-1].d[0];
        //width = r->last_width;
    }


    printf("Exec: ripple\n");
    // Found ripple mode
    i = 1;
    while (i < r->nripple)
    {
        if (r->lripple[i].t > t)
        {
            i --;
            r->last_ripple = ripple = r->lripple[i].d[0];
            break;
        }

        i ++;
    }
    if (i == r->nripple)
    {
        ripple = r->lripple[r->nripple-1].d[0];
        //ripple = r->last_ripple;
    }


    printf("Exec: found point: %d\n", r->npoint);
    //
    // Found point
    //
    if (t < r->lpoint[0].t)
        return;

    printf("Exec: fatal while is to begin\n");
    i = 0;
    while ((i < r->npoint) && (r->lpoint[i].t <= t))
        i ++;

    i --;
    /*
    if (i == r->npoint)
    {
        // End of record
        r->is_using = 0;
        return;
    }
    else
        i --;
    */

    printf("Exec: final if\n");
    
    if (r->point_t != r->lpoint[i].t)
    {
        r->point_t   = r->lpoint[i].t;
        d->viscosity = viscosity;

        if ((int)ripple == 0)
            WATER_CreateRipple (d, r->lpoint[i].d[0], r->lpoint[i].d[1],
                                (int)width, height);
        else
            WATER_CreateBox (d, r->lpoint[i].d[0], r->lpoint[i].d[1],
                             (int) width, height);
    }
}

void WAVE_Init (void)
{
    int i;

    for (i=0; i<MAX_WAVE_RECORDS; i++)
        rec[i].is_using = 0;
}

void WAVE_Load (TDeform *d, int index, char *filename)
{
    PFILE *fp;
    TData data;
    int   i;
    char  op[16];
    TWaveRecord *r;

    assert ((index >= 0) && (index < MAX_WAVE_RECORDS));

    r = &rec[index];

    fp = pak_open (filename, "rb");
    assert (fp != NULL);

    memset (r, 0, sizeof (TWaveRecord));

    // Read nodes
    pak_read (&r->npoint, 1, sizeof (int), fp);
    if (r->npoint > 0)
    {
        r->lpoint = (TData *) malloc (r->npoint * sizeof (TData));
        assert (r->lpoint != NULL);
        pak_read (r->lpoint, r->npoint, sizeof (TData), fp);
    }

    // Read viscosity
    pak_read (&r->nviscosity, 1, sizeof (int), fp);
    if (r->nviscosity > 0)
    {
        r->lviscosity = (TData *) malloc (r->nviscosity * sizeof (TData));
        assert (r->lviscosity != NULL);
        pak_read (r->lviscosity, r->nviscosity, sizeof (TData), fp);
    }

    // Read height
    pak_read (&r->nheight, 1, sizeof (int), fp);
    if (r->nheight > 0)
    {
        r->lheight = (TData *) malloc (r->nheight * sizeof (TData));
        assert (r->lheight != NULL);
        pak_read (r->lheight, r->nheight, sizeof (TData), fp);
    }

    // Read ripple mode
    pak_read (&r->nripple, 1, sizeof (int), fp);
    if (r->nripple > 0)
    {
        r->lripple = (TData *) malloc (r->nripple * sizeof (TData));
        assert (r->lripple != NULL);
        pak_read (r->lripple, r->nripple, sizeof (TData), fp);
    }

    // Read width
    pak_read (&r->nwidth, 1, sizeof (int), fp);
    if (r->nwidth > 0)
    {
        r->lwidth = (TData *) malloc (r->nwidth * sizeof (TData));
        assert (r->lwidth != NULL);
        pak_read (r->lwidth, r->nwidth, sizeof (TData), fp);
    }

    // Read wait
    pak_read (&r->nwait, 1, sizeof (int), fp);
    if (r->nwait > 0)
    {
        r->lwait = (TData *) malloc (r->nwait * sizeof (TData));
        assert (r->lwait != NULL);
        pak_read (r->lwait, r->nwait, sizeof (TData), fp);
    }

    pak_close (fp);
}

void WAVE_Start (TDeform *d, int index)
{
    assert ((index >= 0) && (index < MAX_WAVE_RECORDS));

    rec[index].is_using = 1;
    rec[index].t0       = (float) Epopeia_GetTimeMs ( ) / 1000.0f;

    rec[index].point_t        = 0.0f;
    rec[index].last_viscosity = 0.0f;
    rec[index].last_height    = 0.0f;
    rec[index].last_ripple    = 0.0f;
    rec[index].last_width     = 0.0f;
    rec[index].last_wait      = 0.0f;
}

void WAVE_Stop (TDeform *d, int index)
{
    assert ((index >= 0) && (index < MAX_WAVE_RECORDS));

    rec[index].is_using = 0;
}

void WAVE_Delete (TDeform *d, int index)
{
    TWaveRecord *r;

    assert ((index >= 0) && (index < MAX_WAVE_RECORDS));

    r = &rec[index];

    if (r->lpoint)
        free (r->lpoint);

    if (r->lviscosity)
        free (r->lviscosity);

    if (r->lheight)
        free (r->lheight);

    if (r->lripple)
        free (r->lripple);

    if (r->lwidth)
        free (r->lwidth);

    if (r->lwait)
        free (r->lwait);
}
