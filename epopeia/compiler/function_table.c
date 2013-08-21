#define DEBUG_VAR 0
#define DEBUG_MODULE "FunctionTable"
#include "../assert.h"
#include "../epopeia.h"
#include "../debug.h"
#include "function.h"
#include "../list.h"
#include <math.h>
#include <stdlib.h>
#include "type_table.h"
#include "function_table.h"

static int g_Initialized = 0;
static TList* g_FunctionList = NULL;

#ifdef LINUX
struct {
    char* Nombre;
    void* CallAddress;
} g_IntrinsicFunctionDef1[] = {
    { "sin",    &sinf },
    { "cos",    &cosf },
    { "tan",    &tanf },
    { "asin",   &asinf },
    { "acos",   &acosf },
    { "atan",   &atanf },
    { "exp",    &expf },
    { "log",    &logf },
    { "sqrt",   &sqrtf },
    { "abs",    &fabsf },
    { NULL, NULL }
};


#elif WIN32
// MS sux

float mi_sinf (float f) { return (float)sin(f); }
float mi_cosf (float f) { return (float)cos(f); }
float mi_tanf (float f) { return (float)tan(f); }
float mi_asinf(float f) { return (float)asin(f); }
float mi_acosf(float f) { return (float)acos(f); }
float mi_atanf(float f) { return (float)atan(f); }
float mi_expf (float f) { return (float)exp(f); }
float mi_logf (float f) { return (float)log(f); }
float mi_sqrtf(float f) { return (float)sqrt(f); }
float mi_fabsf(float f) { return (float)fabs(f); }

    struct {
        char* Nombre;
        void* CallAddress;
    } g_IntrinsicFunctionDef1[] = {
        { "sin",    &mi_sinf },
        { "cos",    &mi_cosf },
        { "tan",    &mi_tanf },
        { "asin",   &mi_asinf },
        { "acos",   &mi_acosf },
        { "atan",   &mi_atanf },
        { "exp",    &mi_expf },
        { "log",    &mi_logf },
        { "sqrt",   &mi_sqrtf },
        { "abs",    &mi_fabsf },
        { NULL, NULL }
    };


#endif

float mi_fminf(float a, float b) {
    if(a > b)
        return b;
    return a;
}

float mi_fmaxf(float a, float b) {
    if(a > b)
        return a;
    return b;
}

int mi_min(int a, int b) {
    if(a > b)
        return b;
    return a;
}

int mi_max(int a, int b) {
    if(a > b)
        return a;
    return b;
}

#ifdef WIN32
float mi_atan2f(float f1, float f2) { return (float)atan2(f1, f2); }
float mi_fmodf(float f1, float f2) { return (float)fmod(f1, f2); }
float mi_powf(float f1, float f2) { return (float)pow(f1, f2); }
#endif

struct {
    char* Nombre;
    void* CallAddress;
} g_IntrinsicFunctionDef2f[] = {
#ifdef WIN32
    { "atan2",  &mi_atan2f },
    { "fmod",   &mi_fmodf },
    { "pow",    &mi_powf },
#else
    { "atan2",  &atan2f },
    { "fmod",   &fmodf },
    { "pow",    &powf },
#endif
    { "fmin",   &mi_fminf },
    { "fmax",   &mi_fmaxf },
    { NULL, NULL }
};

struct {
    char* Nombre;
    void* CallAddress;
} g_IntrinsicFunctionDef2i[] = {
    { "min",   &mi_min },
    { "max",   &mi_max },
    { NULL, NULL }
};

// Devuelve el "tiempo" de la musica
// - Segundos, o
// - ooorrr, donde ooo=Order y rrr=row con 0s
// Esta funcion solo devuelve algo util con streams
// almenos por ahora
float GetTime(void)
{
    //    return Epopeia_GetTimeMs()/1000.0;
    TSoundContext* SC;

    SC = Epopeia_GetSoundContext();
    return SC->Pos.Row/1000.0;
}

float Random(float min, float max)
{
    return (min + ((max-min)*rand())/(RAND_MAX+0.0));
}


//-------------------------------------------------------------------
void            FunctionTable_End(void)
{
    if(g_Initialized)
    {
        List_Delete(g_FunctionList);
        g_Initialized = 0;
    }
    else
    {
        DEBUG_msg("Trying to End FunctionTable without having Init it!");
        ASSERT("Trying to End FunctionTable without having Init it!" != NULL);
    }
}
//-------------------------------------------------------------------
TFunctionId     FunctionTable_GetId(const char* FunctionName)
{
    ASSERT(g_Initialized != 0);
    ASSERT(FunctionName != NULL);
    List_Reset(g_FunctionList);

    while(!List_EOF(g_FunctionList)
          && 0 != strcmp(((TFunction*)List_Current(g_FunctionList))->Name, FunctionName)
         )
        List_Next(g_FunctionList);

    if(List_EOF(g_FunctionList))
    {
        // No existe funcion!
        DEBUG_msg_str("No existe una funcion con el nombre: ", FunctionName);
        return NULL;
    }
    return (TFunctionId)List_Current(g_FunctionList);

}

//-------------------------------------------------------------------
int             FunctionTable_Init(void)
{
    int i;
    TFunctionId Id;
    
    if(!g_Initialized)
    {
        g_FunctionList = List_New(CompareFunctionPtr);
        g_Initialized = 1;
    }
    else
    {
        DEBUG_msg("Trying to Init FunctionTable having Init it!");
        ASSERT("Trying to Init FunctionTable having Init it!" != NULL);
    }

    for(i=0; g_IntrinsicFunctionDef1[i].Nombre != NULL; i++)
    {
        TFunctionId FuncId;

	DEBUG_msg_str("Añadiendo funcion ",g_IntrinsicFunctionDef1[i].Nombre);

        FuncId = Function_New(g_IntrinsicFunctionDef1[i].Nombre,
                              g_IntrinsicFunctionDef1[i].CallAddress,
                              Type_RealId);
        Function_AddParameterType(FuncId, Type_RealId);
        
        DEBUG_msg_dec("Numero de parametros ",List_Length(FuncId->Args));
        
        
        FunctionTable_Register(FuncId);
    }

    for(i=0; g_IntrinsicFunctionDef2i[i].Nombre != NULL; i++)
    {
        TFunctionId FuncId;
        FuncId = Function_New(g_IntrinsicFunctionDef2i[i].Nombre,
                              g_IntrinsicFunctionDef2i[i].CallAddress,
                              Type_IntegerId);
        Function_AddParameterType(FuncId, Type_IntegerId);
        Function_AddParameterType(FuncId, Type_IntegerId);
        FunctionTable_Register(FuncId);
    }

    for(i=0; g_IntrinsicFunctionDef2f[i].Nombre != NULL; i++)
    {
        TFunctionId FuncId;
        FuncId = Function_New(g_IntrinsicFunctionDef2f[i].Nombre,
                              g_IntrinsicFunctionDef2f[i].CallAddress,
                              Type_RealId);
        Function_AddParameterType(FuncId, Type_RealId);
        Function_AddParameterType(FuncId, Type_RealId);
        FunctionTable_Register(FuncId);
    }

    // Mas funciones
    Id = Function_New("GetTimeMs", &Epopeia_GetTimeMs, Type_IntegerId);
    FunctionTable_Register(Id);
    Id = Function_New("GetTime", &GetTime, Type_RealId);
    FunctionTable_Register(Id);
    Id = Function_New("GetResX", &Epopeia_GetResX, Type_IntegerId);
    FunctionTable_Register(Id);
    Id = Function_New("GetResY", &Epopeia_GetResY, Type_IntegerId);
    FunctionTable_Register(Id);
    Id = Function_New("Random", &Random, Type_RealId);
    Function_AddParameterType(Id, Type_RealId);
    Function_AddParameterType(Id, Type_RealId);
    FunctionTable_Register(Id);
    srand(Epopeia_GetTimeMs());
    return 0;
}
//-------------------------------------------------------------------
EPOPEIA_API int FunctionTable_Register(TFunctionId Id)
{
    ASSERT(g_Initialized);
    ASSERT(Id != NULL);
    List_Reset(g_FunctionList);
    List_Goto(g_FunctionList, Id);
    if(!List_EOF(g_FunctionList))
    {
        // Existe una funcion con el mismo nombre!
        DEBUG_msg("Ya existe una funcion con el mismo nombre!");
        return 1;
    }
    List_Insert(g_FunctionList, Id);
    return 0;
}
//-------------------------------------------------------------------
void FunctionTable_DumpInfo(FILE* fp)
{
    List_Reset(g_FunctionList);
    while(!List_EOF(g_FunctionList))
    {
        Function_Dump(List_Current(g_FunctionList), fp);
        List_Next(g_FunctionList);
    }
}
//-------------------------------------------------------------------
