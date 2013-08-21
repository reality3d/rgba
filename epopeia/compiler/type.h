#ifndef TYPE_H
#define TYPE_H

#include <stdio.h>
#include "../list.h"
#include "../epopeia.h"


typedef struct TType_t
{
    char* Name;
    struct TType_t* Id;    // Lease TTypeId Id;
    TList* Methods;        // Lista de TMethods, Debe incluir New, Delete, Load, Unload, ...
    TList* Constants;      // Lista de constantes
} TType;

typedef TType* TTypeId;

void Type_Dump(TTypeId type, FILE* fp);

EPOPEIA_API void     Type_AddConstantInteger(TTypeId Type, char* ConstantName, int Value);
EPOPEIA_API void     Type_AddConstantReal   (TTypeId Type, char* ConstantName, double Value);
EPOPEIA_API void     Type_AddConstant       (TTypeId Type, char* ConstantName, TTypeId ConstantType, void* Value);
EPOPEIA_API TTypeId  Type_GetConstantType   (TTypeId Type, char* ConstantName);
EPOPEIA_API void*    Type_GetConstantValue  (TTypeId Type, char* ConstantName);
EPOPEIA_API int      Type_GetConstantValueInteger(TTypeId Type, char* ConstantName);
EPOPEIA_API double   Type_GetConstantValueReal   (TTypeId Type, char* ConstantName);
EPOPEIA_API int      Type_HasConstant       (TTypeId Type, char* ConstantName);


#endif
