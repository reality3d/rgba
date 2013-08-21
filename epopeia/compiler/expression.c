#include "expression.h"
#define DEBUG_MODULE "expression"
#define DEBUG_VAR 0
#include "../debug.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "errors.h"
#include "st.h"
#include "function.h"
#include "function_table.h"
#include "../inter/call.h"
#include "../assert.h"

#ifdef WIN32
#define __FUNCTION__ "VisualC sux"
#endif

static UParam  Expr_Call(void* FunctionAddress, TExprResult** Args, int ArgCount, TTypeId ReturnType);
void           Expr_Delete(TExpr* E);
static TExprResult* Expr_FunctionCall(const char* FunctionName, TArgList* Args);
TExprResult*   Expr_MethodCall(const char* ObjectName, const char* FunctionName, TArgList* Args);
static TExpr*  Expr_New(void);
TExpr*         Expr_NewAdd         (TExpr* E1, TExpr* E2);
TExpr*         Expr_NewCastInteger (TExpr* E1);
TExpr*         Expr_NewCastReal    (TExpr* E1);
TExpr*         Expr_NewDivide      (TExpr* E1, TExpr* E2);
TExpr*         Expr_NewFunctionCall(char* FunctionName, TArgList* Args);
TExpr*         Expr_NewInteger     (int i);
TExpr*         Expr_NewMethodCall  (char* ObjectName, char* MethodName, TArgList* Args);
TExpr*         Expr_NewModule      (TExpr* E1, TExpr* E2);
TExpr*         Expr_NewMutiply     (TExpr* E1, TExpr* E2);
TExpr*         Expr_NewNegative    (TExpr* E1);
TExpr*         Expr_NewReal        (double r);
TExpr*         Expr_NewSubstract   (TExpr* E1, TExpr* E2);
static void    Expr_ResultTableFree(TExprResult** ExprTable);
TExprResult*   Expr_Solve(TExpr* E);
static TExprResult** Expr_SolveArgList(TArgList* Args, int PreviousFreeArgs, unsigned int* ArgCount);


//-------------------------------------------------------------------
static UParam Expr_Call(void* FunctionAddress, TExprResult** Args, int ArgCount, TTypeId ReturnType)
{
    UParam* ParameterTable;
    int i;
    UParam Result;
    
    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    ParameterTable = malloc(sizeof(void*)*ArgCount);
    assert(ParameterTable != NULL);
    assert(Args != NULL);
    i = 0;
    while(i<ArgCount)
    {
        assert(Args[i] != NULL);
        if(Args[i]->Type == Type_IntegerId)
        {
//            printf("Param %d is integer with value %d\n", i, Args[i]->ValueInteger);
            ParameterTable[i].i = Args[i]->ValueInteger;
        }
        else if(Args[i]->Type == Type_RealId)
	{
            float f;
            f = (float)(Args[i]->ValueReal);
            ParameterTable[i].f = f;
//            printf("Param %d is real with value %f\n", i, Args[i]->ValueReal);
        }
        else
        {
//            printf("Param %d is object\n",i );
            ParameterTable[i].ptr = Args[i]->ValueObject;
        }
        i++;
    }

    // *****
    // AYEEEEEEEEEEEEEEEEEEEEEEEEEEE!!!
    // Mega hack warning banner
    // AYEEEEEEEEEEEEEEEEEEEEEEEEEEE!!!
    ///*****
    //
    // El problema al llamar a una funcion que devuelve un float
    // usando la funcion utilitaria Call es que la convencion de
    // devolucion de floats en PC es por registro en el copro
    // Esto jode horriblemente el invento de Call, que al estar
    // definido como int el llamante lee lo que hay en EAX
    // que sera cualquier tipo de entero o bien un puntero
    // Aunque se podria implementar una nueva funcion Callf,
    // que gestione correctamente la devolucion del float
    // desde el principio, eso seria duplicar!!! el numero de lineas
    // en ASM en este proyecto. Esto es INACEPTABLE 8)
    // Otra opcion seria modificar las rutinas a llamar de forma
    // que devuelvan el float como int, y por lo tanto por EAX
    // (usando magia de punteros). Esto es enormemente INCONVENIENTE,
    // ya que habria que preparar las rutinas especificamente para esto
    // no pudiendo usar funciones de la biblioteca C directamente.
    //
    // Asi que se opta por ENGAÑAR VILMENTE AL COMPILADOR
    // Le vamos a decir que Call realmente devuelve un float.
    // Aunque en teoria preserva EAX para devolver el valor original
    // de retorno de la funcion llamada, tambien es cierto que
    // PRESERVA los registros del copro, ya que no los toca para nada.
    // Asi que si el compilador asume que Call en realidad devuelve
    // un float, recuperara correctamente el valor de retorno float
    // que la funcion a la que llama Call ha dejado en el copro.
    //
    // Y asi nacio Callf, la funcion hermana bastarda de Call.

    DEBUG_msg("Vamos a hacer el Call...");
    if(ReturnType == Type_RealId)
    {
        float (*Callf)(void* Funcion, UParam* ParamVector, int ParamCount);

        Callf = (float (*)(void*, UParam*, int))Call;
        Result.f = Callf(FunctionAddress, ParameterTable, ArgCount);
    }
    else
    {
        Result.i = Call(FunctionAddress, ParameterTable, ArgCount);
    }
    DEBUG_msg("Hemos vuelto de hacer el call...");
    free(ParameterTable);

    return Result;
}

//-------------------------------------------------------------------
void           Expr_Delete(TExpr* E)
{
    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    assert(E != NULL);

    switch(E->OpType)
    {
    case EXPR_OP_ADD:
    case EXPR_OP_SUBSTRACT:
    case EXPR_OP_MULTIPLY:
    case EXPR_OP_DIVIDE:
        // Binary OP
        Expr_Delete(E->First);
        Expr_Delete(E->Second);
        E->First = E->Second = NULL;
        E->OpType = EXPR_OP_INVALID;
        free(E);
        break;
    case EXPR_OP_NEGATIVE:
    case EXPR_OP_CAST_REAL:
    case EXPR_OP_CAST_INTEGER:
        // Unary OP
        Expr_Delete(E->First);
        E->First = E->Second = NULL;
        E->OpType = EXPR_OP_INVALID;
        free(E);
        break;
    case EXPR_OP_INTEGER:
    case EXPR_OP_REAL:
        // Constant
        E->First = E->Second = NULL;
        E->OpType = EXPR_OP_INVALID;
        free(E);
        break;
    case EXPR_OP_METHOD_CALL:
        free(E->ObjectName);
    case EXPR_OP_FUNCTION_CALL:
        E->First = E->Second = NULL;
        E->OpType = EXPR_OP_INVALID;
        free(E->FunctionName);
        List_Delete(E->Args);
        E->ObjectName = E->FunctionName = NULL;
        E->Args = NULL;
        free(E);
        break;
    case EXPR_OP_INVALID:
        DEBUG_msg("La Expresion es invalida. No puedes liberarla!");
        assert("Expresion invalida en Expr_Delete" != NULL);
        break;
    default:
        DEBUG_msg_dec("El tipo de Expresion no se reconoce: ", E->OpType);
        assert("Tipo de Expresion desconocida en Expr_Delete" != NULL);
    }
}
//-------------------------------------------------------------------
static TExprResult* Expr_FunctionCall(const char* FunctionName, TArgList* ArgExprs)
{
    TExprResult* Result;
    void *FunctionAddress;
    unsigned int ArgCount;
    UParam ResultValue;
    TFunctionId FunctionId;
    TExprResult** Args;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    ASSERT(FunctionName != NULL);
    ASSERT(ArgExprs != NULL);
    DEBUG_msg_str("Vamos a llamar a la funcion ", FunctionName);
    Args   = Expr_SolveArgList(ArgExprs, 0, &ArgCount);
    
    FunctionId = FunctionTable_GetId(FunctionName);
    assert(FunctionId != NULL);
    FunctionAddress = Function_GetAddress(FunctionId);
    assert(FunctionAddress != NULL);
    
    ResultValue = Expr_Call(FunctionAddress, Args, ArgCount, Function_GetReturnType(FunctionId));
        
    Expr_ResultTableFree(Args);
    Args = NULL;

    // Crear extructura de retorno con valor y tipo
    // Creamos estructura
    Result = malloc(sizeof(TExprResult));
    assert(Result != NULL);

    Result->Type = Function_GetReturnType(FunctionId);
    Result->ValueObject = NULL;
    Result->ValueInteger = 0;
    Result->ValueReal    = 0.0;

    if(Result->Type == Type_IntegerId)
        Result->ValueInteger = ResultValue.i;
    else if(Result->Type == Type_RealId)
        Result->ValueReal = ResultValue.f;
    else
        Result->ValueObject = ResultValue.ptr;

    return Result;
}
//-------------------------------------------------------------------
TExprResult*   Expr_MethodCall(const char* ObjectName, const char* FunctionName, TArgList* ArgExprs)
{
    unsigned int ArgCount;
    TTypeId TypeId;
    TMethod* MethodData;
    TExprResult** Args;
    UParam ResultValue;
    TExprResult* Result;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    ASSERT(ObjectName != NULL);
    ASSERT(FunctionName != NULL);
    ASSERT(ArgExprs != NULL);
    DEBUG_msg_str("Vamos al metodo ", FunctionName);
    
    TypeId = ST_Type((char*)ObjectName);
    ASSERT(TypeId != 0);
    MethodData = (TMethod*)Type_GetMethod(TypeId, (char*)FunctionName);
    ASSERT(MethodData != NULL);

    Args    = Expr_SolveArgList(ArgExprs, 1, &ArgCount);

    Args[0] = malloc(sizeof(TExprResult));
    assert(Args[0] != NULL);
    Args[0]->Type = Type_IntegerId;
    Args[0]->ValueInteger = (int)ST_Value((char*)ObjectName);
    Args[0]->ValueObject = NULL;
    Args[0]->ValueReal    = 0.0;
    if(ArgCount > 1)
    {
        DEBUG_msg_dec("El 1er parametro de metodo es (entero): ", Args[1]->ValueInteger);
        DEBUG_msg_dec("El 1er parametro de metodo es (real): ", Args[1]->ValueReal);
    }
//    printf("Llamando a metodo %s del objeto %s\n", FunctionName, ObjectName);
    DEBUG_msg("Llamando a Expr_Call");
    DEBUG_msg_hex("Direccion metodo: ", (int)MethodData->CallAddress);
    DEBUG_msg_hex("Direccion args: ", (int)Args);
    DEBUG_msg_dec("Numero args: ", ArgCount);
    DEBUG_msg_str("Tipo de retorno: ", MethodData->ReturnType->Name);
    
    ResultValue  = Expr_Call(MethodData->CallAddress, Args, ArgCount, MethodData->ReturnType);
    DEBUG_msg("De vuelta desde Expr_Call");
    Expr_ResultTableFree(Args);
    Args = NULL;

    // Crear extructura de retorno con valor y tipo
    // Creamos estructura
    Result = malloc(sizeof(TExprResult));
    assert(Result != NULL);
    DEBUG_msg("Malloc para result realizado");

    Result->Type = MethodData->ReturnType;
    Result->ValueObject = NULL;
    Result->ValueInteger = 0;
    Result->ValueReal    = 0.0;

    DEBUG_msg_str("El tipo que devolvemos es ", MethodData->ReturnType->Name);
    if(Result->Type == Type_IntegerId)
        Result->ValueInteger = ResultValue.i;
    else if(Result->Type == Type_RealId)
        Result->ValueReal = ResultValue.f;
    else
        Result->ValueObject = ResultValue.ptr;

    return Result;
}

//-------------------------------------------------------------------
static TExpr*  Expr_New(void)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    E = malloc(sizeof(TExpr));
    assert(E != NULL);
    E->OpType = EXPR_OP_INVALID;
    E->Type   = Type_InvalidId;
    E->First  = NULL;
    E->Second = NULL;
    E->ObjectName   = NULL;
    E->FunctionName = NULL;
    E->Args   = NULL;
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewAdd         (TExpr* E1, TExpr* E2)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    ASSERT(E1 != NULL);
    ASSERT(E2 != NULL);

    if(E1->Type != E2->Type)
    {
//        printf("Error: imposible sumar un %s y un %s\n", E1->Type->Name, E2->Type->Name);
        return NULL;
    }
    
    E = Expr_New();
    E->OpType = EXPR_OP_ADD;
    E->First  = E1;
    E->Second = E2;
    E->Type   = E1->Type;
    return E;
}

//-------------------------------------------------------------------
TExpr*         Expr_NewCastInteger (TExpr* E1)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    E = Expr_New();
    E->OpType = EXPR_OP_CAST_INTEGER;
    E->First  = E1;
    E->Type   = Type_IntegerId;
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewCastReal    (TExpr* E1)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    E = Expr_New();
    E->OpType = EXPR_OP_CAST_REAL;
    E->First  = E1;
    E->Type   = Type_RealId;
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewDivide      (TExpr* E1, TExpr* E2)
{
    TExpr* E;
    
    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    ASSERT(E1 != NULL);
    ASSERT(E2 != NULL);

    if(E1->Type != E2->Type)
    {
//        printf("Error: imposible dividir un %s y un %s\n", E1->Type->Name, E2->Type->Name);
        return NULL;
    }

    E = Expr_New();
    E->OpType = EXPR_OP_DIVIDE;
    E->First  = E1;
    E->Second = E2;
    E->Type   = E1->Type;
    
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewFunctionCall(char* FunctionName, TArgList* Args)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    E = Expr_New();
    E->OpType = EXPR_OP_FUNCTION_CALL;
    E->FunctionName  = strdup(FunctionName);
    E->Args = Args;
    E->Type = Function_GetReturnType(FunctionTable_GetId(FunctionName));
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewInteger     (int i)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    E = Expr_New();
    E->OpType = EXPR_OP_INTEGER;
    E->ValueInteger = i;
    E->Type   = Type_IntegerId;
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewMethodCall  (char* ObjectName, char* MethodName, TArgList* Args)
{
    TExpr* E;
    TMethod* Method;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    E = Expr_New();
    E->OpType = EXPR_OP_METHOD_CALL;
    E->ObjectName   = strdup(ObjectName);
    E->FunctionName = strdup(MethodName);
    E->Args   = Args;
    Method = Type_GetMethod(ST_Type(ObjectName), MethodName);
    E->Type   = Method->ReturnType;
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewModule     (TExpr* E1, TExpr* E2)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    ASSERT(E1 != NULL);
    ASSERT(E2 != NULL);

    if(E1->Type != E2->Type)
    {
//        printf("Error: imposible multiplicar un %s y un %s\n", E1->Type->Name, E2->Type->Name);
        return NULL;
    }

    E = Expr_New();
    E->OpType = EXPR_OP_MODULE;
    E->First  = E1;
    E->Second = E2;
    E->Type   = E1->Type;

    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewMultiply    (TExpr* E1, TExpr* E2)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    ASSERT(E1 != NULL);
    ASSERT(E2 != NULL);

    if(E1->Type != E2->Type)
    {
//        printf("Error: imposible multiplicar un %s y un %s\n", E1->Type->Name, E2->Type->Name);
        return NULL;
    }

    E = Expr_New();
    E->OpType = EXPR_OP_MULTIPLY;
    E->First  = E1;
    E->Second = E2;
    E->Type   = E1->Type;

    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewNegative    (TExpr* E1)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    E = Expr_New();
    E->OpType = EXPR_OP_NEGATIVE;
    E->First  = E1;
    E->Type   = E1->Type;
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewReal        (double r)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    E = Expr_New();
    E->OpType    = EXPR_OP_REAL;
    E->ValueReal = r;
    E->Type      = Type_RealId;
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewString      (char* s)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    DEBUG_msg_str("String is ", s);
    E = Expr_New();
    E->OpType      = EXPR_OP_STRING;
    E->ValueObject = (void*)strdup(s);
    E->Type        = Type_StringId;
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewSubstract   (TExpr* E1, TExpr* E2)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    ASSERT(E1 != NULL);
    ASSERT(E2 != NULL);

    if(E1->Type != E2->Type)
    {
//        printf("Error: imposible restar un %s y un %s\n", E1->Type->Name, E2->Type->Name);
        return NULL;
    }

    E = Expr_New();
    E->OpType = EXPR_OP_SUBSTRACT;
    E->First  = E1;
    E->Second = E2;
    E->Type   = E1->Type;
    
    return E;
}
//-------------------------------------------------------------------
TExpr*         Expr_NewVariable    (char* var)
{
    TExpr* E;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    E = Expr_New();
    E->OpType      = EXPR_OP_VARIABLE;
    E->ObjectName  = strdup(var);
    E->Type        = ST_Type(var);
    DEBUG_msg_str("El nombre de la variable es ", var);
    return E;
}
//-------------------------------------------------------------------
static void    Expr_ResultTableFree(TExprResult** ExprTable)
{
    int i;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    assert(ExprTable != NULL);
    i = 0;
    DEBUG_msg_hex("Vamos a comenzar el bucle de liberacion", (unsigned)ExprTable);
    while(ExprTable[i] != NULL)
    {
        DEBUG_msg_hex("Liberando...", (unsigned)(ExprTable[i]));
        free(ExprTable[i]);
        i++;
    }
    DEBUG_msg("Bucle de libreracion finalizado");
    free(ExprTable);
    DEBUG_msg_str("Saliendo de ", __FUNCTION__);
}
//-------------------------------------------------------------------
TExprResult*   Expr_Solve(TExpr* E)
{
    TExprResult* Result;

    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    assert(E != NULL);
    
    Result = malloc(sizeof(TExprResult));
    assert(Result != NULL);
    DEBUG_msg_hex("Expresion de resultado sera...", (unsigned)Result);

    Result->Type = Type_InvalidId;
    Result->ValueObject = NULL;
    Result->ValueInteger = 0;
    Result->ValueReal    = 0.0;
    
    switch(E->OpType)
    {
        TExprResult* R1;
        TExprResult* R2;
//        TExprResult** Args;
    case EXPR_OP_ADD:
    case EXPR_OP_SUBSTRACT:
    case EXPR_OP_MULTIPLY:
    case EXPR_OP_DIVIDE:
    case EXPR_OP_MODULE:
        R1 = Expr_Solve(E->First);
        R2 = Expr_Solve(E->Second);
        if(R1->Type != R2->Type)
	{
            error_runtime("Expression: Tipos de datos no coinciden en operacion binaria");
            // Los errores de runtime finalizan la ejecucion
        }

        if(R1->Type == Type_StringId && E->OpType==EXPR_OP_ADD)
        {
            int count;
            count = strlen(R1->ValueObject) + strlen(R2->ValueObject) + 1;
            Result->ValueObject = malloc(count);
            strcpy(Result->ValueObject, R1->ValueObject);
            strcat(Result->ValueObject, R2->ValueObject);
            Result->Type = Type_StringId;
            free(R1);
            free(R2);
            break;
        }
        
        if(R1->Type != Type_IntegerId && R1->Type != Type_RealId)
        {
            error_runtime("Expression: Operacion no aplicable a operandos");
        }

        if(E->OpType==EXPR_OP_ADD)
        {
            Result->ValueInteger = R1->ValueInteger + R2->ValueInteger;
            Result->ValueReal    = R1->ValueReal    + R2->ValueReal;
//            printf("Sumando %d y %d que da %d", R1->ValueInteger, R2->ValueInteger, Result->ValueInteger);
        }
        else if(E->OpType==EXPR_OP_SUBSTRACT)
        {
            Result->ValueInteger = R1->ValueInteger - R2->ValueInteger;
            Result->ValueReal    = R1->ValueReal    - R2->ValueReal;
        }
        else if(E->OpType==EXPR_OP_MULTIPLY)
        {
            Result->ValueInteger = R1->ValueInteger * R2->ValueInteger;
            Result->ValueReal    = R1->ValueReal    * R2->ValueReal;
        }
        else if(E->OpType==EXPR_OP_DIVIDE)
        {
            DEBUG_msg("Habria que comprobar que no hacemos un divide by zero");
            if(R1->Type == Type_IntegerId)
                Result->ValueInteger = R1->ValueInteger / R2->ValueInteger;
            else
                Result->ValueReal    = R1->ValueReal    / R2->ValueReal;
        }
        else if(E->OpType==EXPR_OP_MODULE)
        {
            DEBUG_msg("Habria que comprobar que no hacemos un divide by zero");
            if(R1->Type == Type_IntegerId)
                Result->ValueInteger = R1->ValueInteger % R2->ValueInteger;
            else
                Result->ValueReal    = (float)fmod(R1->ValueReal, R2->ValueReal);
        }
        Result->Type = R1->Type;
        free(R1);
        free(R2);
        break;
    case EXPR_OP_NEGATIVE:
        R1 = Expr_Solve(E->First);
        if(R1->Type != Type_IntegerId && R1->Type != Type_RealId)
        {
            error_runtime("Expression: Operacion no aplicable a operando (Negativo)");
        }
        Result->ValueInteger = -R1->ValueInteger;
        Result->ValueReal    = -R1->ValueReal;
        Result->Type = R1->Type;
        free(R1);
        break;
    case EXPR_OP_CAST_REAL:
        R1 = Expr_Solve(E->First);
        if(R1->Type != Type_IntegerId && R1->Type != Type_RealId)
        {
            error_runtime("Expression: Operacion no aplicable a operando (Cast Real)");
        }

        if(R1->Type == Type_IntegerId)
            Result->ValueReal = R1->ValueInteger;
        else
            Result->ValueReal = R1->ValueReal;
        Result->Type = Type_RealId;
        free(R1);
        break;
    case EXPR_OP_CAST_INTEGER:
        R1 = Expr_Solve(E->First);
        if(R1->Type != Type_IntegerId && R1->Type != Type_RealId)
        {
            error_runtime("Expression: Operacion no aplicable a operando (Cast Integer)");
        }

        if(R1->Type == Type_IntegerId)
            Result->ValueInteger = R1->ValueInteger;
        else
            Result->ValueInteger = R1->ValueReal;
        Result->Type = Type_IntegerId;
        free(R1);
        break;
    case EXPR_OP_INTEGER:
        Result->Type = Type_IntegerId;
        Result->ValueInteger = E->ValueInteger;
        break;
    case EXPR_OP_REAL:
        Result->Type = Type_RealId;
        Result->ValueReal = E->ValueReal;
        break;
    case EXPR_OP_STRING:
        Result->Type = Type_StringId;
        Result->ValueObject = E->ValueObject;
        break;
    case EXPR_OP_METHOD_CALL:
        // No utilizamos en Result creado al principio de la funcion, asi que librarlo
        free(Result);
        Result = Expr_MethodCall(E->ObjectName, E->FunctionName, E->Args);
        break;
    case EXPR_OP_FUNCTION_CALL:
        // No utilizamos en Result creado al principio de la funcion, asi que librarlo
        free(Result);
        Result = Expr_FunctionCall(E->FunctionName, E->Args);
        break;
    case EXPR_OP_VARIABLE:
        DEBUG_msg_str("la expresion es la variable ", E->ObjectName);
        R1 = ST_Value(E->ObjectName);
        memcpy(Result, R1, sizeof(TExprResult));
        break;
    case EXPR_OP_INVALID:
        DEBUG_msg("La Expresion es invalida. Imposible resolverla!");
        assert("Expresion invalida en Expr_Solve" != NULL);
        break;
    default:
        DEBUG_msg_dec("El tipo de Expresion no se reconoce: ", E->OpType);
        assert("Tipo de Expresion desconocida en Expr_Delete" != NULL);
    }

    return Result;
}
//-------------------------------------------------------------------
// Devuelve un array de TExprResult*, y el numero de entradas del array en *ArgCount
static TExprResult** Expr_SolveArgList(TArgList* Args, int PreviousFreeArgs, unsigned int* ArgCount)
{
    TExprResult** ResultArray;
    int i;
    
    DEBUG_msg_str("Entrando a ", __FUNCTION__);
    *ArgCount = (List_Length(Args)+PreviousFreeArgs);
    ResultArray = (TExprResult**)malloc(sizeof(TExprResult*) * (*ArgCount+1));
    assert(ResultArray);

    List_Reset(Args);
    i = PreviousFreeArgs;
    DEBUG_msg("Vamos a comenzar el bucle de calculo de parametros");
    while(!List_EOF(Args))
    {
        ResultArray[i] = Expr_Solve((TExpr*)List_Current(Args));
//        printf("Integer value of result %d is %d (type is %s)\n", i, ResultArray[i]->ValueInteger, ResultArray[i]->Type->Name);
//        printf("Float   value of result %d is %f (type is %s)\n", i, ResultArray[i]->ValueReal, ResultArray[i]->Type->Name);
        i++;
        List_Next(Args);
    }
    DEBUG_msg("Bucle de calculo de parametros finalizado");
    ResultArray[i] = NULL;
    return ResultArray;
}
//-------------------------------------------------------------------

