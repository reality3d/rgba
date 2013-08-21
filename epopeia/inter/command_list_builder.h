#ifndef COMMAND_LIST_BUILDER_H
#define COMMAND_LIST_BUILDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../pos.h"
#include "../compiler/arglist.h"
#include "../compiler/expression.h"

int  CommandListBuilder_Init(void);
void CommandListBuilder_End (void);
TPos CommandListBuilder_GetPos(void);
void CommandListBuilder_SetPos(int order, int row);
void CommandListBuilder_InsertAssign(char* Identifier, TExpr* Expression);
void CommandListBuilder_InsertCall (char* Identifier, char* Method, TArgList* ParamList, int isDelete);
void CommandListBuilder_InsertMusicPlay(void);
void CommandListBuilder_InsertStart(void* Identifier, long Depth);
void CommandListBuilder_InsertStop (void* Identifier);
void CommandListBuilder_InsertNew  (char* Identifier);
void CommandListBuilder_InsertSetOrder(int Order, int Row);
void CommandListBuilder_InsertRepetitiveCommands(TPos pos_start, TPos pos_end, TPos frequency, TList* command_list);


#ifdef __cplusplus
};
#endif

#endif // COMMAND_LIST_BUILDER_H
