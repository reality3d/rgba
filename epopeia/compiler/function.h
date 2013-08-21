#ifndef EPOPEIA_FUNCTION
#define EPOPEIA_FUNCTION

#include "../epopeia.h"
#include "type.h"
#include "type_list.h"
#include "arglist.h"

typedef struct
{
    char* Name;
    void* CallAddress;
    TTypeId ReturnType;
    TTypeList* Args;
} TFunction;
typedef TFunction* TFunctionId;

EPOPEIA_API void        Function_AddParameterType(TFunctionId FunctionId, TTypeId Type);
EPOPEIA_API TFunctionId Function_New(const char* FunctionName, void* CallAddress, TTypeId ReturnType);

int                     Function_CheckParams(TFunctionId Type, TArgList* ParamList);
void*                   Function_GetAddress(TFunctionId Id);
TTypeId                 Function_GetReturnType(TFunctionId Id);
void                    Function_Dump(TFunctionId func, FILE* fp);

int CompareFunctionPtr(void* f1, void* f2);


#endif //EPOPEIA_FUNCTION
