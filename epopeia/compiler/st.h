#ifndef SIMBOL_TABLE_H
#define SIMBOL_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "type_table.h"

#define TYPE_INVALID ((unsigned long)Type_InvalidId)
#define TYPE_INT    ((unsigned long)Type_IntegerId)
#define TYPE_REAL   ((unsigned long)Type_RealId)
#define TYPE_STRING ((unsigned long)Type_StringId)

typedef struct
{
    char* Name;
    TTypeId Type;
    void* Value;
    TArgList* ConstructorParams;
} TObject;

 int ST_Init(void);
void ST_End (void);

void      ST_Register(char* Identifier, TTypeId Type, void* Value, TArgList* ConstructorParams);
int       ST_UnRegister(char* Identifier);
int       ST_Exists  (char* Identifier);
TTypeId   ST_Type    (char* Identifier);
void*     ST_Value   (char* Identifier);
TArgList* ST_ConstructorParams(char* Identifier);
int       ST_ConstructObject(char* Identifier);
int       ST_DestroyObjects(void);
void      ST_SetValue(char* Identifier, void* Value);


#ifdef __cplusplus
};
#endif


#endif /* SIMBOL_TABLE */
