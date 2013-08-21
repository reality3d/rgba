#ifndef TYPE_TABLE_H
#define TYPE_TABLE_H

#include "../list.h"
#include "arglist.h"
#include "type.h"
#include "method.h"

/*
#ifdef EPOPEIA_EXPORTS
#define EPOPEIA_API __declspec(dllexport)
#else
#define EPOPEIA_API __declspec(dllimport)
#endif
*/

// Inicializa el modulo y establece los tipos nativos
void Type_Init(void);
void Type_End (void);

// Funciones exportadas
EPOPEIA_API TTypeId  Type_Register (char* TypeName);
EPOPEIA_API void     Type_AddMethod(TTypeId Type, TMethod* Method);
EPOPEIA_API TTypeId  Type_GetId    (char* TypeName);
EPOPEIA_API TMethod* Type_GetMethod(TTypeId Type, char* MethodName);
EPOPEIA_API int      Type_HasMethod(TTypeId Type, char* MethodName);

// Funciones no exportadas
int      Type_ConsistencyCheck(void);
int      Type_CheckParams(TTypeId Type, char* MethodName, TArgList* ParamList);

void TypeTable_DumpClassInfo(FILE* fp);

extern EPOPEIA_API_VAR TTypeId Type_InvalidId;
extern EPOPEIA_API_VAR TTypeId Type_IntegerId;
extern EPOPEIA_API_VAR TTypeId Type_RealId;
extern EPOPEIA_API_VAR TTypeId Type_StringId;


#endif //TYPE_TABLE_H
