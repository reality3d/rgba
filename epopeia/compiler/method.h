#ifndef METHOD_H
#define METHOD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../epopeia.h"
#include "type.h"
#include "type_list.h"

/*
#ifdef EPOPEIA_EXPORTS
#define EPOPEIA_API __declspec(dllexport)
#else
#define EPOPEIA_API __declspec(dllimport)
#endif
*/

typedef struct
{
    char* Name;
    TTypeList* Arguments;
    void* CallAddress;
    TTypeId ReturnType;
} TMethod;

// Los parametros hay que anyadirlos en orden. El primer parametro
// siempre es un TRender (el objeto mismo vamos), ese no hay que
// meterlo, lo hace el constructor (_New)
EPOPEIA_API void     Method_AddParameterType(TMethod* Method, TTypeId Type);
EPOPEIA_API TMethod* Method_New(char* Name, void* CallAddress);
EPOPEIA_API void     Method_SetReturnType(TMethod* Method, TTypeId Type);

void Method_Dump(TMethod* Method, FILE* fp);

int CompareMethodPtr(void* m1, void* m2);


#ifdef __cplusplus
};
#endif

#endif

