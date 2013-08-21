#include <epopeia/epopeia_system.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <GL/gl.h>

struct test { float valor; };

//-----------------------------------------------------------------------------
static void* Test_New(void)
{
    void* self;
    self = (void *)malloc(sizeof(struct test));
    printf("Test_New self is %lx\n", self);
    return self;
}
//-----------------------------------------------------------------------------
static void Test_Delete(void* self)
{
    printf("Test_Delete self is %lx\n", self);
    free(self);
}
//-----------------------------------------------------------------------------
static void Test_DoFrame(void* self, TRenderContext* RC, TSoundContext* SC)
{
    // Do nothing
}
//-----------------------------------------------------------------------------
static void Test_Start(void* self, int z)
{
}
//-----------------------------------------------------------------------------
static void Test_Stop(void* self)
{
}

static float  Test_GetValue(void* _self)
{
    struct test *self = _self;
    printf("Test_GetValue %f\n", self->valor);
    return self->valor;
}

static void Test_SetValue(void* _self, float valor)
{
    struct test *self = _self;
    self->valor = valor;
    printf("Test_SetValue %f\n", valor);
}

//-----------------------------------------------------------------------------
EPOPEIA_PLUGIN_API char*Epopeia_PluginName(void)
{
    return "State-of-the-art Test by Enlar";
}

EPOPEIA_PLUGIN_API void Epopeia_RegisterType(void)
{
    TTypeId Id;
    TMethod* pMethod;

//    printf("Registrando Test. "); //fflush(stdout);

    // Registramos el tipo del objeto/efecto, el nombre que
    // damos es el que se usar  en el script como tipo/clase.
    Id = Type_Register("Test");
//    printf("Cod. de tipo %x. ", Id); fflush(stdout);
   
    // A¤adimos los metodos obligatorios
    // Son: New, Delete, Start, Stop, DoFrame
    // New devuelve un puntero al objeto recien creado,
    // de forma que multiples llamadas a new devuelven
    // diferente puntero, pudiendo tener el mismo
    // efecto varias veces incluso concurrentes
    // El resto de las funciones, obligatorias y no obligatorias,
    // el primer parametro que reciben es ese puntero al objeto.

    pMethod = Method_New("New", (void*)Test_New);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Delete", (void*)Test_Delete);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("DoFrame", (void*)Test_DoFrame);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Start", (void*)Test_Start);
    Method_AddParameterType(pMethod, Type_IntegerId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("Stop", (void*)Test_Stop);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("SetValue", (void*)Test_SetValue);
    Method_AddParameterType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);

    pMethod = Method_New("GetValue", (void*)Test_GetValue);
    Method_SetReturnType(pMethod, Type_RealId);
    Type_AddMethod(Id, pMethod);
}

