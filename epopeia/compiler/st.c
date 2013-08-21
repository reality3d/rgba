#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "../assert.h"
#include "st.h"
#include "../list.h"
#include "../inter/call.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "st"
#include "../debug.h"
#include "expression.h"

typedef struct
{
    char* Name;
    TTypeId Type;
    void* Value;
    TList* ConstructorParams;
    int IsConstructed;
} TIdentifier;

TList* g_IdentifierList;

#define DELETE_METHOD_NAME "Delete"
#define NEW_METHOD_NAME    "New"

int CompareIdentifierPtr(void* i1, void* i2)
{
    return strcmp(((TIdentifier*)i1)->Name, ((TIdentifier*)i2)->Name);
}
//-------------------------------------------------------------------
int ST_Init(void)
{
    g_IdentifierList = List_New(CompareIdentifierPtr);
    return g_IdentifierList == NULL;
}
//-------------------------------------------------------------------
void ST_End(void)
{
    List_Delete(g_IdentifierList);
}
//-------------------------------------------------------------------
// ConstructorParams puede ser NULL. (Por ejemplo variables simples que no son objetos)
void ST_Register(char* Identifier, TTypeId Type, void* Value, TArgList* ConstructorParams)
{
    TIdentifier* Ident;
    ASSERT(Identifier != NULL);
    ASSERT(Type != NULL);
    ASSERT(!ST_Exists(Identifier));

    Ident = malloc(sizeof(TIdentifier));
    ASSERT(Ident != NULL);

    Ident->Name = Identifier;
    Ident->Type = Type;
    Ident->Value = Value;
    Ident->ConstructorParams = ConstructorParams;
    Ident->IsConstructed = 0;
    List_Insert(g_IdentifierList, Ident);
    return;
}
//-------------------------------------------------------------------
int ST_UnRegister(char* Identifier)
{
    ASSERT(Identifier != NULL);
    ASSERT(ST_Exists(Identifier));

    while(!List_EOF(g_IdentifierList)
	  && strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier))
	List_Next(g_IdentifierList);

    ASSERT(!List_EOF(g_IdentifierList));
    ASSERT(0 == strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier));

    List_Drop(g_IdentifierList, List_Current(g_IdentifierList));

    return 0;
}
//-------------------------------------------------------------------
int ST_Exists(char* Identifier)
{
    ASSERT(Identifier != NULL);
    
    List_Reset(g_IdentifierList);

    while(!List_EOF(g_IdentifierList)
	  && strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier))
	List_Next(g_IdentifierList);

    if(!List_EOF(g_IdentifierList) &&
       0 == strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier))
        return 1;
    return 0;
}
//-------------------------------------------------------------------
TTypeId ST_Type(char* Identifier)
{
    ASSERT(Identifier != NULL);
    
    List_Reset(g_IdentifierList);
    while(!List_EOF(g_IdentifierList)
	  && strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier))
	List_Next(g_IdentifierList);
    ASSERT(!List_EOF(g_IdentifierList));
    ASSERT(0 == strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier));
    return ((TIdentifier*)List_Current(g_IdentifierList))->Type;
}
//-------------------------------------------------------------------
void* ST_Value(char* Identifier)
{
    ASSERT(Identifier != NULL);
    
    List_Reset(g_IdentifierList);
    while(!List_EOF(g_IdentifierList)
	  && strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier))
	List_Next(g_IdentifierList);

    ASSERT(!List_EOF(g_IdentifierList));
    ASSERT(0 == strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier));
    return ((TIdentifier*)List_Current(g_IdentifierList))->Value;
}
//-------------------------------------------------------------------
void      ST_SetValue(char* Identifier, void* Value)
{
    TIdentifier* ident;
    ASSERT(Identifier != NULL);
    
    List_Reset(g_IdentifierList);
    while(!List_EOF(g_IdentifierList)
	  && strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier))
	List_Next(g_IdentifierList);
    ASSERT(!List_EOF(g_IdentifierList));
    ASSERT(0 == strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier));
    ident = ((TIdentifier*)List_Current(g_IdentifierList));
    DEBUG_msg_str("Cambiando el valor de la variable ", ident->Name);
    DEBUG_msg_hex("al valor ", (unsigned)Value);
    ident->Value = Value;
}
//-------------------------------------------------------------------
TArgList* ST_ConstructorParams(char* Identifier)
{
    ASSERT(Identifier != NULL);
    
    List_Reset(g_IdentifierList);
    while(!List_EOF(g_IdentifierList)
	  && strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier))
	List_Next(g_IdentifierList);
    ASSERT(!List_EOF(g_IdentifierList));
    ASSERT(0 == strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier));
    return ((TIdentifier*)List_Current(g_IdentifierList))->ConstructorParams;
}
//-------------------------------------------------------------------
int ST_ConstructObject(char *Identifier)
{
    TIdentifier* Ident;
    TMethod* New;
    UParam* ParamVector;
    int ParamCount;
    int i;

    DEBUG_msg("Construct object to build a object");
    List_Reset(g_IdentifierList);
    while(!List_EOF(g_IdentifierList)
	  && strcmp(((TIdentifier*)List_Current(g_IdentifierList))->Name, Identifier))
	List_Next(g_IdentifierList);

    ASSERT(!List_EOF(g_IdentifierList));
    Ident = (TIdentifier*)List_Current(g_IdentifierList);
    ASSERT(0 == strcmp(Ident->Name, Identifier));
    ASSERT(Type_HasMethod(Ident->Type, NEW_METHOD_NAME));

    if(Ident->IsConstructed)
        return 0;

    New = Type_GetMethod(Ident->Type, NEW_METHOD_NAME);
    ParamCount = List_Length(Ident->ConstructorParams);
    ParamVector = malloc(sizeof(UParam)*ParamCount);

    ASSERT(ParamVector != NULL);

    List_Reset(Ident->ConstructorParams);
    for(i = 0; !List_EOF(Ident->ConstructorParams); i++)
    {
        TExpr* E = List_Current(Ident->ConstructorParams);
        if(E->OpType == EXPR_OP_VARIABLE)
            ParamVector[i].ptr = ST_Value((char*)E->ValueObject);
        else if(E->OpType == EXPR_OP_INTEGER)
            ParamVector[i].i = E->ValueInteger;
        else if(E->OpType == EXPR_OP_REAL)
        {
            float f = E->ValueReal;
            ParamVector[i].f = f;
        }
        else
            ParamVector[i].ptr = E->ValueObject;
        List_Next(Ident->ConstructorParams);
    }
    DEBUG_msg("Llamando al constructor (metodo new)");
    Ident->Value = (void*)Call(New->CallAddress, ParamVector, ParamCount);
    DEBUG_msg("El constructor ha vuelto");

    DEBUG_msg_str("Ident: ", Ident->Name);
    DEBUG_msg_hex("value: ", (int)Ident->Value);
    free(ParamVector);
    List_Delete(Ident->ConstructorParams);
    Ident->ConstructorParams = NULL;
    if(Ident->Value == NULL)
    {
        DEBUG_msg_str("Fallo al construir objeto ", Ident->Name);
        DEBUG_msg_str("\tde tipo ", Ident->Type->Name);
        printf("Fallo al construir objeto '%s' de tipo '%s'\n", Ident->Name, Ident->Type->Name);
        return -1;
    }
    Ident->IsConstructed = 1;
    return 0;
}
//-------------------------------------------------------------------
int ST_DestroyObjects(void)
{
    DEBUG_msg("DestroyObjects ha comenzado");
    List_Reset(g_IdentifierList);
    DEBUG_msg("DestroyObjects entra en bucle");
    while(!List_EOF(g_IdentifierList))
    {
        TIdentifier* Ident = List_Current(g_IdentifierList);
        TMethod* New;
        DEBUG_msg_str("Borrando: ", Ident->Name);
        DEBUG_msg_hex("Valor: ", (int)Ident->Value);
        //        ASSERT(Type_HasMethod(Ident->Type, DELETE_METHOD_NAME));
        // Si no tiene el metodo delete es que es un tipo plano y no una clase
        // Entonces hay que librar el valor pero no hay que llamar a Delete

        if(Ident->Value != NULL)
        {
            if(Type_HasMethod(Ident->Type, DELETE_METHOD_NAME))
            {
                DEBUG_msg("Llamamos al destructor...");
                New = Type_GetMethod(Ident->Type, DELETE_METHOD_NAME);

                Call(New->CallAddress, (UParam*)&Ident->Value, 1);
                Ident->Value = NULL;
            }
            else
            {
                DEBUG_msg("Libramos TExprResult o lo que sea...");
                // Libramos el TExprResult o lo que sea
                free(Ident->Value);
                Ident->Value = NULL;
	    }
            Ident->IsConstructed = 0;
        }
        DEBUG_msg("Siguiente...");
        List_Next(g_IdentifierList);
    }
    DEBUG_msg("DestroyObjects sale");
    return 0;
}
//-------------------------------------------------------------------
