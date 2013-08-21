///////////////////////////////////////////////////////////////////////
// COLOR BUFFER DEFORM Epopeia interface
///////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <epopeia/epopeia_system.h>
#include "deform.h"
#include "wave.h"

void DEFORM_DoFrame (TDeform *self, TRenderContext *RC, TSoundContext *SC)
{
    float t = (float) (SC->Tick*60.0)/1000.0;

    if (!INTERP_IsEnd (self->i_amplitude))
        INTERP_Get (self->i_amplitude, &self->amplitude);

    if (!INTERP_IsEnd (self->i_freq1))
        INTERP_Get (self->i_freq1, &self->frequency1);

    if (!INTERP_IsEnd (self->i_freq2))
        INTERP_Get (self->i_freq2, &self->frequency2);

    if (!INTERP_IsEnd (self->i_nwaves))
        INTERP_Get (self->i_nwaves, &self->num_waves);

    printf("Llamando Wave_RunAll\n");

    // Do waves
    WAVE_RunAll (self, (float) Epopeia_GetTimeMs ( ) / 1000.0f);

    printf("Llamando WaveRunAll back\n");

    DEFORM_Do (self, t);
}


void DEFORM_Start (TDeform *self, int z)
{

}

void DEFORM_Stop (TDeform *self)
{
}

void DEFORM_SetMode (TDeform *self, int mode)
{
    self->mode = mode;
}

void DEFORM_Amplitude (TDeform *self, float a)
{
    self->i_amplitude->transition = 0;
    self->amplitude = a;
}

void DEFORM_Frequency1 (TDeform *self, float f)
{
    self->i_freq1->transition = 0;
    self->frequency1 = f;
}

void DEFORM_Frequency2 (TDeform *self, float f)
{
    self->i_freq2->transition = 0;
    self->frequency2 = f;
}

void DEFORM_NumWaves (TDeform *self, float waves)
{
    self->i_nwaves->transition = 0;
    self->num_waves = waves;
}

void DEFORM_AmplitudeChange (TDeform *self, float a, float t)
{
    INTERP_Start (self->i_amplitude, self->amplitude, a, t * 1000.0f);
}

void DEFORM_Freq1Change (TDeform *self, float f, float t)
{
    INTERP_Start (self->i_freq1, self->frequency1, f, t * 1000.0f);
}

void DEFORM_Freq2Change (TDeform *self, float f, float t)
{
    INTERP_Start (self->i_freq2, self->frequency2, f, t * 1000.0f);
}

void DEFORM_NumWavesChange (TDeform *self, float nw, float t)
{
    INTERP_Start (self->i_nwaves, self->num_waves, nw, t * 1000.0f);
}

void DEFORM_SetExprZ (TDeform *self, char *expression)
{
    VL_Init ( );
    VL_AddVar ("x");
    VL_AddVar ("y");
    VL_AddVar ("z");
    VL_AddVar ("d");
    VL_AddVar ("t");
    VL_AddVar ("ampl");
    VL_AddVar ("freq1");
    VL_AddVar ("freq2");
    VL_AddVar ("nwaves");
    VL_AddVar ("u");
    VL_AddVar ("v");

    self->expr_z = EXPR_New (expression);
}

void DEFORM_SetExprX (TDeform *self, char *expression)
{
    VL_Init ( );
    VL_AddVar ("x");
    VL_AddVar ("y");
    VL_AddVar ("z");
    VL_AddVar ("d");
    VL_AddVar ("t");
    VL_AddVar ("ampl");
    VL_AddVar ("freq1");
    VL_AddVar ("freq2");
    VL_AddVar ("nwaves");
    VL_AddVar ("u");
    VL_AddVar ("v");

    self->expr_x = EXPR_New (expression);
}

void DEFORM_SetExprY (TDeform *self, char *expression)
{
    VL_Init ( );
    VL_AddVar ("x");
    VL_AddVar ("y");
    VL_AddVar ("z");
    VL_AddVar ("d");
    VL_AddVar ("t");
    VL_AddVar ("ampl");
    VL_AddVar ("freq1");
    VL_AddVar ("freq2");
    VL_AddVar ("nwaves");
    VL_AddVar ("u");
    VL_AddVar ("v");

    self->expr_y = EXPR_New (expression);
}

void DEFORM_SetExprU (TDeform *self, char *expression)
{
    VL_Init ( );
    VL_AddVar ("x");
    VL_AddVar ("y");
    VL_AddVar ("z");
    VL_AddVar ("d");
    VL_AddVar ("t");
    VL_AddVar ("ampl");
    VL_AddVar ("freq1");
    VL_AddVar ("freq2");
    VL_AddVar ("nwaves");
    VL_AddVar ("u");
    VL_AddVar ("v");

    self->expr_u = EXPR_New (expression);
}

void DEFORM_SetExprV (TDeform *self, char *expression)
{
    VL_Init ( );
    VL_AddVar ("x");
    VL_AddVar ("y");
    VL_AddVar ("z");
    VL_AddVar ("d");
    VL_AddVar ("t");
    VL_AddVar ("ampl");
    VL_AddVar ("freq1");
    VL_AddVar ("freq2");
    VL_AddVar ("nwaves");
    VL_AddVar ("u");
    VL_AddVar ("v");

    self->expr_v = EXPR_New (expression);
}

void DEFORM_ShowGrid (TDeform *self, int show)
{
    self->draw_grid = show;
}

void DEFORM_SetWireWidth (TDeform *self, float w)
{
    self->wire_width = w;
}

void DEFORM_SetWireAlpha (TDeform *self, float alpha)
{
    self->wire_color[3] = alpha;
}

void DEFORM_SetWireColor (TDeform *self, float r, float g, float b)
{
    self->wire_color[0] = r;
    self->wire_color[1] = g;
    self->wire_color[2] = b;
}


void DEFORM_SetIlumMode (TDeform *self, int mode)
{
    self->ilumination = mode;
}

void DEFORM_SetLight (TDeform *self, char *str, float a, float b, float c, float d)
{
    if (!strcmp (str, "ambient"))
        Set4f (self->light_ambient, a, b, c, d)
    else if (!strcmp (str, "diffuse"))
        Set4f (self->light_diffuse, a, b, c, d)
    else if (!strcmp (str, "specular"))
        Set4f (self->light_specular, a, b, c, d)
    else if (!strcmp (str, "position"))
        Set4f (self->light_position, a, b, c, d)
    else
    {
        printf ("DEFORM_SetLight invalid parameter \"%s\"\n", str);
        exit (1);
    }
}

void DEFORM_SetMaterial (TDeform *self, char *str, float a, float b, float c, float d)
{
    if (!strcmp (str, "ambient"))
        Set4f (self->mat_ambient, a, b, c, d)
    else if (!strcmp (str, "diffuse"))
        Set4f (self->mat_diffuse, a, b, c, d)
    else if (!strcmp (str, "specular"))
        Set4f (self->mat_specular, a, b, c, d)
    if (!strcmp (str, "emission"))
        Set4f (self->mat_emission, a, b, c, d)
    else if (!strcmp (str, "shininess"))
        self->mat_shininess = a;
    else
    {
        printf ("DEFORM_SetMaterial invalid parameter \"%s\"\n", str);
        exit (1);
    }
}


EPOPEIA_PLUGIN_API char *Epopeia_PluginName(void)
{
    return "color buffer deform by epsilum";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId  Id;
    TMethod *pMethod;

    Id = Type_Register ("Deform");

    pMethod = Method_New ("New", (void*)DEFORM_New);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Delete", (void*)DEFORM_Delete);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("DoFrame", (void*)DEFORM_DoFrame);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Start", (void*)DEFORM_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Stop", (void*)DEFORM_Stop);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetMode", (void*)DEFORM_SetMode);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Amplitude", (void*)DEFORM_Amplitude);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Frequency1", (void*)DEFORM_Frequency1);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Frequency2", (void*)DEFORM_Frequency2);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("NumWaves", (void*)DEFORM_NumWaves);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("AmplitudeChange", (void*)DEFORM_AmplitudeChange);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Freq1Change", (void*)DEFORM_Freq1Change);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("Freq2Change", (void*)DEFORM_Freq2Change);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("NumWavesChange", (void*)DEFORM_NumWavesChange);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetExprZ", (void*)DEFORM_SetExprZ);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetExprX", (void*)DEFORM_SetExprX);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetExprY", (void*)DEFORM_SetExprY);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetExprU", (void*)DEFORM_SetExprU);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetExprV", (void*)DEFORM_SetExprV);
    Method_AddParameterType(pMethod, Type_StringId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("ShowGrid", (void*)DEFORM_ShowGrid);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetWireWidth", (void*)DEFORM_SetWireWidth);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetWireAlpha", (void*)DEFORM_SetWireAlpha);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetWireColor", (void*)DEFORM_SetWireColor);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetIlumMode", (void*)DEFORM_SetIlumMode);
    Method_AddParameterType(pMethod, Type_IntegerId);;
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetLight", (void*)DEFORM_SetLight);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("SetMaterial", (void*)DEFORM_SetMaterial);
    Method_AddParameterType(pMethod, Type_StringId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod (Id, pMethod);

    // ---- WAVES ----
    pMethod = Method_New ("WaveLoad", (void*)WAVE_Load);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Method_AddParameterType (pMethod, Type_StringId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("WaveStart", (void*)WAVE_Start);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("WaveStop", (void*)WAVE_Stop);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);

    pMethod = Method_New ("WaveDelete", (void*)WAVE_Delete);
    Method_AddParameterType (pMethod, Type_IntegerId);
    Type_AddMethod (Id, pMethod);
}
