#include <stdio.h>
#include "../assert.h"
#include "../epopeia.h"
#include "type.h"
#include "type_table.h"
#include "type_list.h"
#include "arglist.h"
#include "expression.h"
#include "errors.h"
#define DEBUG_VAR 0
#define DEBUG_MODULE "function"
#include "../debug.h"
#include "function.h"

//-------------------------------------------------------------------
EPOPEIA_API void        Function_AddParameterType(TFunctionId FunctionId, TTypeId Type)
{
    ASSERT(FunctionId != NULL);
    ASSERT(Type != NULL);
    List_Insert(FunctionId->Args, Type);
}
//-------------------------------------------------------------------
int                     Function_CheckParams(TFunctionId FunctionId, TArgList* ArgList)
{
    int CurrentParam;
    TTypeList* FuncArgTypes;
    
    FuncArgTypes = FunctionId->Args;
    List_Reset(FuncArgTypes);
    List_Reset(ArgList);
    CurrentParam = 1;

	DEBUG_msg_dec("Numero de parametros de la funcion ",List_Length(FuncArgTypes));
	DEBUG_msg_dec("Numero de parametros de la llamada ",List_Length(ArgList));
    
    List_Reset(FuncArgTypes);
    List_Reset(ArgList);
    
    while(!List_EOF(FuncArgTypes) && !List_EOF(ArgList))
    {
        TTypeId TypeId;
        TExpr* Param;
        
        TypeId = List_Current(FuncArgTypes);
        Param  = List_Current(ArgList);
        if(TypeId != Param->Type)
        {
            char ctemp[200];
	    // cast automatico de integer <-> float
	    if(TypeId == Type_IntegerId && Param->Type == Type_RealId)
	    {
		List_ChangeCurrent(ArgList, Expr_NewCastInteger(Param));
                sprintf(ctemp, "El parametro numero %d no es un entero.", CurrentParam);
                warning_semantic(&lookahead, ctemp);
	    }
	    else if(Param->Type == Type_IntegerId && TypeId == Type_RealId)
	    {
		List_ChangeCurrent(ArgList, Expr_NewCastReal(Param));
                sprintf(ctemp, "El parametro numero %d no es un numero real.", CurrentParam);
                warning_semantic(&lookahead, ctemp);
	    }
            else
            {
                char msg[200];
                sprintf(msg, "Error en el parametro %d: %s debia ser %s\n", CurrentParam,
                        Param->Type->Name, TypeId->Name);
                error_semantic_full(msg, -1, -1);
                return CurrentParam;
            }
        }
        List_Next(FuncArgTypes);
        List_Next(ArgList);
        CurrentParam++;
    }
    if(!List_EOF(FuncArgTypes))
        return -1;
    if(!List_EOF(ArgList))
        return -2;

    return 0;
}
//-------------------------------------------------------------------
void*                   Function_GetAddress(TFunctionId Id)
{
    ASSERT(Id!=NULL);
    return Id->CallAddress;
}
//-------------------------------------------------------------------
TTypeId                 Function_GetReturnType(TFunctionId Id)
{
    ASSERT(Id!=NULL);
    DEBUG_msg_hex("Pedido tipo de retorno de funcion ", (unsigned)Id);
    DEBUG_msg_hex("Tipo de retorno de la funcion es ", (unsigned)Id->ReturnType);
    return Id->ReturnType;
}
//-------------------------------------------------------------------
EPOPEIA_API TFunctionId Function_New(const char* FunctionName, void* CallAddress, TTypeId ReturnType)
{
    TFunction* Func;

    ASSERT(CallAddress != NULL);
    ASSERT(ReturnType != NULL);
    
    Func = (TFunction*)malloc(sizeof(TFunction));
    if(Func == NULL)
        return NULL;

    Func->Name = strdup(FunctionName);
    Func->CallAddress = CallAddress;
    Func->ReturnType  = ReturnType;
    Func->Args        = List_New(AlwaysLesser);

    return Func;
}
//-------------------------------------------------------------------
void Function_Dump(TFunctionId func, FILE* fp)
{
    fprintf(fp, "  %s(", func->Name);
    List_Reset(func->Args);
    while(!List_EOF(func->Args))
    {
        TTypeId type;
        type = List_Current(func->Args);
        List_Next(func->Args);
        if(List_EOF(func->Args))
            fprintf(fp, "%s", type->Name);
        else
            fprintf(fp, "%s, ", type->Name);
    }
    if(func->ReturnType == Type_InvalidId)
        fprintf(fp, ")\n");
    else
        fprintf(fp, ") returns %s\n", func->ReturnType->Name);
}
//-------------------------------------------------------------------
int CompareFunctionPtr(void* f1, void* f2)
{
    return strcmp(((TFunction*)f1)->Name, ((TFunction*)f2)->Name);
}




