#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "type.h"
#include "type_table.h"
#include "constant.h"
#include "../critical.h"
#include "../list.h"
#include "errors.h"
#include "expression.h"
#define DEBUG_VAR 0
#define DEBUG_MODULE "type_table"
#include "../debug.h"

EPOPEIA_API_VAR TTypeId Type_InvalidId;
EPOPEIA_API_VAR TTypeId Type_IntegerId;
EPOPEIA_API_VAR TTypeId Type_RealId;
EPOPEIA_API_VAR TTypeId Type_StringId;

static TList* g_TypeList;
#define NATIVE_TYPE_COUNT 4
typedef enum { ntInvalid = 0, ntInteger, ntReal, ntString } ENativeType;
static char* g_NativeType[NATIVE_TYPE_COUNT] = { "Invalid type", "Integer", "Real", "String" };
#define MANDATORY_METHOD_COUNT 5
static char* g_MandatoryMethodName[MANDATORY_METHOD_COUNT] =
{
    "New",
    "Delete",
    "DoFrame",
    "Start",
    "Stop"
};

TTypeId  Type_Register (char* TypeName);
void     Type_AddMethod(TTypeId Type, TMethod* Method);
TTypeId  Type_GetId(char* TypeName);
TMethod* Type_GetMethod(TTypeId Type, char* MethodName);
int      Type_HasMethod(TTypeId Type, char* MethodName);
int      Type_ConsistencyCheck(void);
int      Type_CheckTypeConsistency(TType* Type);
int      Type_CheckParams(TTypeId Type, char* MethodName, TArgList* ParamList);


void TypeList_Goto(TList* List, char* TypeName)
{
    ASSERT(List != NULL);
    List_Reset(List);
    while(!List_EOF(List) && 0 != strcmp(((TType*)List_Current(List))->Name, TypeName))
    {
//        printf("Tipo: %s != %s\n", TypeName, ((TType*)List_Current(List))->Name);
        List_Next(List);
    }
}

int CompareTypePtr(void* t1, void* t2)
{
    return strcmp(((TTypeId)t1)->Name, ((TTypeId)t2)->Name);
}

//-------------------------------------------------------------------
// Inicializa el modulo y establece los tipos nativos
//-------------------------------------------------------------------
void Type_Init(void)
{
    g_TypeList = List_New(CompareTypePtr);
    Type_InvalidId = Type_Register(g_NativeType[ntInvalid]);
    Type_IntegerId = Type_Register(g_NativeType[ntInteger]);
    Type_RealId    = Type_Register(g_NativeType[ntReal]);
    Type_StringId  = Type_Register(g_NativeType[ntString]);

}
//-------------------------------------------------------------------
void Type_End (void)
{
    List_Delete(g_TypeList);
}
//-------------------------------------------------------------------
EPOPEIA_API TTypeId Type_Register (char* TypeName)
{
    TType* Type;
    Type = malloc(sizeof(TType));
    ASSERT(Type != NULL);

    Type->Name = TypeName;
    Type->Id   = Type;
    Type->Methods   = List_New(CompareMethodPtr);
    Type->Constants = List_New(CompareConstantPtr);
    List_Insert(g_TypeList, Type);
    
    return Type->Id;
}
//-------------------------------------------------------------------
EPOPEIA_API void Type_AddMethod(TTypeId Type, TMethod* Method)
{
    ASSERT(Type != NULL);
    ASSERT(Method != NULL);
    ASSERT(!Type_HasMethod(Type, Method->Name));
    List_Insert(Type->Methods, Method);
    DEBUG_msg_str("Metodo ", Method->Name);
    DEBUG_msg_hex("Direccion ", (int)Method->CallAddress);
}
//-------------------------------------------------------------------
EPOPEIA_API TTypeId Type_GetId(char* TypeName)
{
    List_Reset(g_TypeList);
    TypeList_Goto (g_TypeList, TypeName);
    if(List_EOF(g_TypeList))
        return NULL;
    return List_Current(g_TypeList);
}
//-------------------------------------------------------------------
EPOPEIA_API TMethod* Type_GetMethod(TTypeId Type, char* MethodName)
{
    ASSERT(Type != NULL);

    DEBUG_msg_str("Nombre tipo: ", Type->Name);
    DEBUG_msg_str("Nombre metodo: ", MethodName);

    List_Reset(Type->Methods);
    DEBUG_msg("Hagamos el goto");
    //List_Goto (Type->Methods, MethodName);
    while(!List_EOF(Type->Methods) &&
          0 != strcmp(((TMethod*)List_Current(Type->Methods))->Name, MethodName) )
        List_Next(Type->Methods);

    DEBUG_msg("Hagamos los asserts");
    ASSERT(!List_EOF(Type->Methods));
    ASSERT(0 == strcmp(((TMethod*)List_Current(Type->Methods))->Name, MethodName));
    return List_Current(Type->Methods);
}
//-------------------------------------------------------------------
EPOPEIA_API int Type_HasMethod(TTypeId Type, char* MethodName)
{
    ASSERT(Type != NULL);
    
    List_Reset(Type->Methods);
    while(!List_EOF(Type->Methods) &&
          0 != strcmp(((TMethod*)List_Current(Type->Methods))->Name, MethodName) )
        List_Next(Type->Methods);
    
    return !List_EOF(Type->Methods);
}
//-------------------------------------------------------------------
int Type_ConsistencyCheck(void)
{
    int Error = 0;
    List_Reset(g_TypeList);
    while(!List_EOF(g_TypeList) && !Error)
    {
        Error = Type_CheckTypeConsistency(List_Current(g_TypeList));
        List_Next(g_TypeList);
    }
    return Error;
}
//-------------------------------------------------------------------
int Type_CheckTypeConsistency(TType* Type)
{
    int i;
    int Error;
    // Los tipos nativos no tienen metodos
    for(i=0; i<NATIVE_TYPE_COUNT; i++)
    {
        if(0 == strcmp(g_NativeType[i], Type->Name))
           return 0;
    }
    Error = 0;
    for(i=0; i<MANDATORY_METHOD_COUNT; i++)
    {
	int e;

        e =  !Type_HasMethod(Type->Id, g_MandatoryMethodName[i]);
	Error |= e;
        if(e)
	    printf("El tipo '%s' no tiene el metodo '%s'\n", Type->Name, g_MandatoryMethodName[i]);
    }

    return Error;
}
//-------------------------------------------------------------------
int Type_CheckParams(TTypeId Type, char* MethodName, TArgList* ParamList)
{
    TMethod* Method;
    int CurrentParam;
    Method = Type_GetMethod(Type, MethodName);
    ASSERT(Method != NULL);

    List_Reset(Method->Arguments);
    List_Reset(ParamList);
    CurrentParam = 1;
    while(!List_EOF(Method->Arguments) && !List_EOF(ParamList))
    {
        TExpr*  Param;
        TTypeId TypeId;
        
        TypeId = List_Current(Method->Arguments);
        Param  = List_Current(ParamList);
        if(TypeId != Param->Type)
	{
            char ctemp[200];
	    if(TypeId == Type_IntegerId && Param->Type == Type_RealId)
	    {
		List_ChangeCurrent(ParamList, Expr_NewCastInteger(Param));
                sprintf(ctemp, "El parametro numero %d no es un entero.", CurrentParam);
                warning_semantic(&lookahead, ctemp);
	    }
	    else if(Param->Type == Type_IntegerId && TypeId == Type_RealId)
	    {
		List_ChangeCurrent(ParamList, Expr_NewCastReal(Param));
                sprintf(ctemp, "El parametro numero %d no es un numero real.", CurrentParam);
                warning_semantic(&lookahead, ctemp);
	    }
	    else
	    {
		printf("Error en el parametro %d: %s debia ser %s\n", CurrentParam,
		       TypeId->Name, Param->Type->Name);
		return CurrentParam;
	    }
        }
        List_Next(Method->Arguments);
        List_Next(ParamList);
        CurrentParam++;
    }
    if(!List_EOF(Method->Arguments))
        return -1;
    if(!List_EOF(ParamList))
        return -2;

    return 0;
}
//-------------------------------------------------------------------
void TypeTable_DumpClassInfo(FILE* fp)
{
    ASSERT(fp != NULL);
    List_Reset(g_TypeList);
    while(!List_EOF(g_TypeList))
    {
        Type_Dump(List_Current(g_TypeList), fp);
        List_Next(g_TypeList);
    }
}
//-------------------------------------------------------------------




