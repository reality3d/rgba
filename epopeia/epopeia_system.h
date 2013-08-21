#ifndef EPOPEIA_SYSTEM_H
#define EPOPEIA_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "epopeia.h"

//-----------------------------------------------
// Tipos relacionados con los TIPOS Y METODOS

// Definiciones opacas

typedef void* TTypeId;
typedef void TMethod;
typedef void*  TFunctionId;

EPOPEIA_API TTypeId  Type_Register (char * TypeName);
EPOPEIA_API void     Type_AddMethod(TTypeId Type, TMethod* Method);
EPOPEIA_API TMethod* Method_New(char* Name, void* CallAddress);
// Los parametros hay que anyadirlos en orden. El primer parametro
// siempre es el objeto mismo (self), ese no hay que meterlo
EPOPEIA_API void     Method_AddParameterType(TMethod* Method, TTypeId Type);
EPOPEIA_API void     Method_SetReturnType(TMethod* Method, TTypeId Type);
extern EPOPEIA_API_VAR TTypeId Type_IntegerId;
extern EPOPEIA_API_VAR TTypeId Type_RealId;
extern EPOPEIA_API_VAR TTypeId Type_StringId;

/* Alta de funciones intrinsecas */
EPOPEIA_API void        Function_AddParameterType(TFunctionId FunctionId, TTypeId Type);
EPOPEIA_API TFunctionId Function_New(const char* FunctionName, void* CallAddress, TTypeId ReturnType);


#ifdef __cplusplus
};
#endif

#endif

