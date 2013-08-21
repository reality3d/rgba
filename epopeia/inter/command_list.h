#ifndef COMMAND_LIST_H
#define COMMAND_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../pos.h"
#include "../list.h"
#include "../compiler/arglist.h"
#include "../compiler/expression.h"
#include "../compiler/type_table.h"
#include "command.h"

int  CommandList_Init(void);
void CommandList_End (void);
TList* CommandList_SetCommandList(TList* NewCommandList); // Devuelve el anterior

void CommandList_Add   (TPos* Pos, TCommand* Command);

TPos* CommandList_CurrentPos(void);
int   CommandList_CurrentExec(void);
int   CommandList_Next(void);
void  CommandList_Reset(void);

int CommandList_ExecuteLoad(void);

typedef struct
{
    TPos Pos;
    TList* Commands;
} TPosList;


#ifdef __cplusplus
};
#endif

#endif // COMMAND_LIST_H
