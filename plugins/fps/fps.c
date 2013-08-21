///////////////////////////////////////////////////////////////////////
//
// FPS - Frames Per Second
//
///////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#else
typedef unsigned int DWORD;
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <malloc.h>
#include <stdio.h>
#include <epopeia/epopeia_system.h>
//#include "fonts.h"
#include "fps.h"


#include <time.h>
void FPS_DoFrame (TFPS *self, TRenderContext *RC, TSoundContext *SC)
{
    static int        frames = 0;
    static DWORD      last;
    static GLboolean  first = GL_TRUE;
    static float      fps = 0.0f;
    DWORD             now;
    float             diff;
    char text[64];

    frames++;
    if(first == GL_TRUE)
    {
        last = Epopeia_GetTimeMs ( );
        first = GL_FALSE;
    }
    else
    {
        now = Epopeia_GetTimeMs ( );

        /* update no more than once per second */
        diff = (float) (now - last);
        if (diff > 1000)
        {
            fps = 1000.0 * (float) frames / diff;
            frames = 0;
            last = now;
        }
    }

    glPushAttrib (GL_ALL_ATTRIB_BITS);

    sprintf (text, "FPS: %3.2f  ", fps);
    glColor4f (1, 1, 1, 1);
    Fonts_Write (self->font, text, 8.0, 25.0, 0);

    glPopAttrib ( );

/*
    static int    first_time = 1;
    static int    last, now;
    //float         diff;
    int diff;
    float         fps;
    char   text[128];

    //glColor4f (1, 1, 1, 1);
    //Fonts_Write (self->font, "HOLA 0123", 100.0, 10.0, 0);

    if (first_time)
    {
        last         = Epopeia_GetTimeMs ( );
        self->frames = 0;
        first_time   = 0;
    }
    else
    {
        now  = Epopeia_GetTimeMs ( );
        diff = (last - now) / 1000;

        printf ("DIFF: %i\n", diff);

        if (diff > 0)
        {
            fps          = (float)self->frames / (float) diff;
            self->frames = 0;
            last         = now;

            //sprintf (text, "FPS: %3.2f  ", 1000.0 * self->frames / (float) (Epopeia_GetTimeMs ( ) - self->t0));
            sprintf (text, "FPS: %3.2f  ", fps);

            glColor4f (1, 1, 1, 1);
            Fonts_Write (self->font, text, 100.0, 10.0, 0);
        }
    }
    */
}

TFPS *FPS_New (void)
{
    TFPS     *self;

    self = (TFPS *) malloc (sizeof (TFPS));
    if (!self)
        return 0;

    self->font = Fonts_New (1, 8, 0, 0, 640, 480);
    if (!self->font)
        return 0;


    return self;
}

void FPS_Kill (TFPS *self)
{
    //glDeleteLists (self->dlist, 256);
    Fonts_Delete (self->font);
    free (self);
}

void FPS_Start (TFPS *self)
{
    printf ("FPS START!!!!!!!!!\n");
}

void FPS_Stop (TFPS *self)
{
}

EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return "FPS - Frames Per Second";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId  Id;
    TMethod *pMethod;

    Id = Type_Register ("FPS");

    pMethod = Method_New ("New", (void*)FPS_New);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Delete", (void*)FPS_Kill);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("DoFrame", (void*)FPS_DoFrame);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Start", (void*)FPS_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Stop", (void*)FPS_Stop);
    Type_AddMethod (Id, pMethod);
}
