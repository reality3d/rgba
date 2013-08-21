#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "type_table.h"
#include "method.h"
#include "constant.h"
#include "../assert.h"
#include "../list.h"

void Type_Dump(TTypeId type, FILE* fp)
{
    fprintf(fp, "Class name: %s\n", type->Name);
    fprintf(fp, "Class methods:\n");
    List_Reset(type->Methods);
    while(!List_EOF(type->Methods))
    {
        Method_Dump(List_Current(type->Methods), fp);
        List_Next(type->Methods);
    }
}
//-------------------------------------------------------------------
EPOPEIA_API void     Type_AddConstant     (TTypeId Type, char* ConstantName, TTypeId ConstantType, void* Value)
{
    TConstant* c;

    c = malloc(sizeof(TConstant));
    ASSERT(c != NULL);
    c->Name  = strdup(ConstantName);
    c->Type  = ConstantType;
    c->ValueInteger = 0;
    c->ValueReal    = 0.0;
    c->ValueObject  = NULL;
    List_Insert(Type->Constants, c);
}
//-------------------------------------------------------------------
EPOPEIA_API void     Type_AddConstantInteger(TTypeId Type, char* ConstantName, int Value)
{
    TConstant* c;

    c = malloc(sizeof(TConstant));
    ASSERT(c != NULL);
    c->Name  = strdup(ConstantName);
    c->Type  = Type_IntegerId;
    c->ValueInteger = Value;
    c->ValueReal    = 0.0;
    c->ValueObject  = NULL;
    List_Insert(Type->Constants, c);
}
//-------------------------------------------------------------------
EPOPEIA_API void     Type_AddConstantReal   (TTypeId Type, char* ConstantName, double Value)
{
    TConstant* c;

    c = malloc(sizeof(TConstant));
    ASSERT(c != NULL);
    c->Name  = strdup(ConstantName);
    c->Type  = Type_RealId;
    c->ValueInteger = 0;
    c->ValueReal    = Value;
    c->ValueObject  = NULL;
    List_Insert(Type->Constants, c);
}
//-------------------------------------------------------------------
EPOPEIA_API TTypeId  Type_GetConstantType (TTypeId Type, char* ConstantName)
{
    TConstant c;
    TConstant* cr;
    // La funcion de comparacion solo tiene en cuenta el nombre
    c.Name = ConstantName;
    List_Goto(Type->Constants, &c);
    ASSERT(!List_EOF(Type->Constants));
    cr = (TConstant*)List_Current(Type->Constants);
    return cr->Type;
}
//-------------------------------------------------------------------
EPOPEIA_API void*    Type_GetConstantValue(TTypeId Type, char* ConstantName)
{
    TConstant c;
    TConstant* cr;
    // La funcion de comparacion solo tiene en cuenta el nombre
    c.Name = ConstantName;
    List_Goto(Type->Constants, &c);
    ASSERT(!List_EOF(Type->Constants));
    cr = (TConstant*)List_Current(Type->Constants);
    return cr->ValueObject;
}
//-------------------------------------------------------------------
EPOPEIA_API int      Type_GetConstantValueInteger(TTypeId Type, char* ConstantName)
{
    TConstant c;
    TConstant* cr;
    // La funcion de comparacion solo tiene en cuenta el nombre
    c.Name = ConstantName;
    List_Goto(Type->Constants, &c);
    ASSERT(!List_EOF(Type->Constants));
    cr = (TConstant*)List_Current(Type->Constants);
    return cr->ValueInteger;
}
//-------------------------------------------------------------------
EPOPEIA_API double   Type_GetConstantValueReal   (TTypeId Type, char* ConstantName)
{
    TConstant c;
    TConstant* cr;
    // La funcion de comparacion solo tiene en cuenta el nombre
    c.Name = ConstantName;
    List_Goto(Type->Constants, &c);
    ASSERT(!List_EOF(Type->Constants));
    cr = (TConstant*)List_Current(Type->Constants);
    return cr->ValueReal;
}
//-------------------------------------------------------------------
EPOPEIA_API int      Type_HasConstant     (TTypeId Type, char* ConstantName)
{
    TConstant c;

    // La funcion de comparacion solo tiene en cuenta el nombre
    c.Name = ConstantName;
    List_Goto(Type->Constants, &c);
    return List_EOF(Type->Constants) == 0;
}
//-------------------------------------------------------------------

