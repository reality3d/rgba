#include <stdio.h>
#include <malloc.h>
#include "../assert.h"
#include "method.h"
#define DEBUG_VAR 0
#define DEBUG_MODULE "method"
#include "../debug.h"

extern TTypeId Type_InvalidId;

//-------------------------------------------------------------------
// Los parametros hay que anyadirlos en orden. El primer parametro
// siempre es un TRender (el objeto mismo vamos), ese no hay que
// meterlo, lo hace el constructor (_New)
//-------------------------------------------------------------------
EPOPEIA_API void Method_AddParameterType(TMethod* Method, TTypeId Type)
{
    ASSERT(Method != NULL);
    ASSERT(Type != NULL);
    List_Insert(Method->Arguments, Type);
}
//-------------------------------------------------------------------
EPOPEIA_API TMethod* Method_New(char * Name, void* CallAddress)
{
    TMethod* Method;

    Method = malloc(sizeof(TMethod));
    ASSERT(Method != NULL);
    Method->Name = Name;
    Method->Arguments = List_New(AlwaysLesser);
    Method->CallAddress = CallAddress;
    Method->ReturnType  = Type_InvalidId; // Esto significa que no devuelve valor

    return Method;
}
//-------------------------------------------------------------------
EPOPEIA_API void     Method_SetReturnType(TMethod* Method, TTypeId Type)
{
    ASSERT(Method != NULL);
    Method->ReturnType = Type;
}
//-------------------------------------------------------------------
void Method_Dump(TMethod* Method, FILE* fp)
{
    fprintf(fp, "  %s(", Method->Name);
    List_Reset(Method->Arguments);
    while(!List_EOF(Method->Arguments))
    {
        TTypeId type;
        type = List_Current(Method->Arguments);
        List_Next(Method->Arguments);
        if(List_EOF(Method->Arguments))
            fprintf(fp, "%s", type->Name);
        else
            fprintf(fp, "%s, ", type->Name);
    }
    if(Method->ReturnType == Type_InvalidId)
        fprintf(fp, ")\n");
    else
        fprintf(fp, ") returns %s\n", Method->ReturnType->Name);
}
//-------------------------------------------------------------------
int CompareMethodPtr(void* m1, void* m2)
{
    DEBUG_msg_hex("Comparando m1: ", (int)m1);
    DEBUG_msg_hex("Comparando m2: ", (int)m2);
    DEBUG_msg_str("Comparando m1: ", ((TMethod*)m1)->Name);
    DEBUG_msg_str("Comparando m2: ", ((TMethod*)m2)->Name);
    return strcmp(((TMethod*)m1)->Name, ((TMethod*)m2)->Name);
}
