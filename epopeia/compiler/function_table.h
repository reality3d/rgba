#ifndef EPOPEIA_FUNCTION_TABLE
#define EPOPEIA_FUNCTION_TABLE

#include "../epopeia.h"
#include "function.h"

// Devuelve codigo de error, o 0 en caso de exito
EPOPEIA_API int FunctionTable_Register(TFunctionId Id);
void            FunctionTable_End(void);
// Devuelve Id de la funcion, o 0 en caso de error
TFunctionId     FunctionTable_GetId(const char* FunctionName);
// Devuelve codigo de error, o 0 en caso de exito
int             FunctionTable_Init(void);
// Vuelva informacion sobre las funciones registradas
void            FunctionTable_DumpInfo(FILE* fp);



#endif //EPOPEIA_FUNCTION_TABLE
