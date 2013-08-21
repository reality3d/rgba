#ifndef COMMAND_H
#define COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../compiler/arglist.h"
#include "../pos.h"
#include "../compiler/expression.h"
    
typedef struct
{
    int   Tick;
    int  (*CommandCode)(void *Data);
    void* Data;
} TCommand;

typedef struct
{
    void* FunctionAddress;
//    int   ParameterCount;
//    void** ParameterTable;
    char* Identifier;
    char* FunctionName;
    TArgList* ArgExprs;
} TCommandCallData;

typedef struct
{
    char* Identifier;
    long  Depth;
} TCommandStartData;

typedef struct
{
    int   Order;
    int   Row;
} TCommandSetOrderData;

typedef struct
{
    TPos start;
    TPos end;
    TPos step;
    TList* commands;
} TCommandRepetitiveCommandsData;

typedef struct
{
    char* Identifier;
    TExpr* Expression;
} TCommandAssignData;

int Command_Assign(void* _Data);
int Command_Start(void* Data);
int Command_Stop(void* Data);
int Command_PerformCall(void* Data);
int Command_New(void* Data);
int Command_Delete(void* Data);
int Command_SetOrder(void* Data);
int Command_MusicPlay(void* _Data_not_used);
int Command_RemoveRepetitiveCommands(void* Data);
int Command_InsertRepetitiveCommands(void* Data);


#ifdef __cplusplus
};
#endif

#endif // COMMAND_H
