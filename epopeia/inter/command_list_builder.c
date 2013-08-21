#include <stdio.h>
#include "../assert.h"
#include "../epopeia.h"
#include "../compiler/type_table.h"
#include "../compiler/method.h"
#include "command.h"
#include "command_list.h"
#include "command_list_builder.h"
#include "../compiler/expression.h"
#include "../compiler/st.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "clist_builder"
#include "../debug.h"

// Track current building position
static TPos g_Pos;

//-------------------------------------------------------------------
int CommandListBuilder_Init(void)
{
    DEBUG_msg("Command list builder init");
    g_Pos.Order = -1;
    g_Pos.Row   = 0;
    DEBUG_msg("Command list builder init end");

    return 0;
}
//-------------------------------------------------------------------
void CommandListBuilder_End (void)
{

}
//-------------------------------------------------------------------
TPos CommandListBuilder_GetPos(void)
{
    return g_Pos;
}
//-------------------------------------------------------------------
void CommandListBuilder_SetPos(int Order, int Row)
{
    g_Pos.Order = Order;
    g_Pos.Row   = Row;
}
//-------------------------------------------------------------------
void CommandListBuilder_InsertAssign(char* Identifier, TExpr* Expression)
{
    TCommand* Command;
    TCommandAssignData* Data;
    
    Command = (TCommand*)malloc(sizeof(TCommand));
    ASSERT(Command != NULL);
    Data = (TCommandAssignData*)malloc(sizeof(TCommandAssignData));
    ASSERT(Data != NULL);

    Data->Identifier = strdup(Identifier);
    Data->Expression = Expression;
    
    Command->Tick = 0;
    Command->CommandCode = Command_Assign;
    Command->Data = Data;

    CommandList_Add(&g_Pos, Command);
}
//-------------------------------------------------------------------
void CommandListBuilder_InsertCall(char* Identifier, char* Method, TArgList* ParamList, int isDelete)
{
    TCommand* Command;
    TCommandCallData* Data;
    TTypeId Id;
    TMethod* MethodData;

    Command = (TCommand*)malloc(sizeof(TCommand));
    ASSERT(Command != NULL);

    Data = (TCommandCallData*)malloc(sizeof(TCommandCallData));
    ASSERT(Data != NULL);

    Id = ST_Type(Identifier);
    ASSERT(Id != 0);
    MethodData = (TMethod*)Type_GetMethod(Id, Method);
    ASSERT(MethodData != NULL);

    Command->Tick = 0;
    if(isDelete)
        Command->CommandCode = Command_Delete;
    else
        Command->CommandCode = Command_PerformCall;
    Command->Data = Data;

    DEBUG_msg_hex("Comand to execute: ", (unsigned)Command);
    DEBUG_msg_hex("Comand code to execute: ", (unsigned)Command->CommandCode);
    DEBUG_msg_hex("Comand data: ", (unsigned)Command->Data);

    Data->FunctionAddress = MethodData->CallAddress;
    Data->ArgExprs = ParamList;
    Data->Identifier  = strdup(Identifier);
    Data->FunctionName= strdup(Method);
    CommandList_Add(&g_Pos, Command);
}
//-------------------------------------------------------------------
void CommandListBuilder_InsertMusicPlay(void)
{
    TCommand* Command;

    Command = (TCommand*)malloc(sizeof(TCommand));
    ASSERT(Command != NULL);
    Command->Tick = 0;
    Command->CommandCode = Command_MusicPlay;
    Command->Data = NULL;

    TPos pos;
    pos.Order = 0;
    pos.Row   =  0;
    CommandList_Add(&pos, Command);

}
//-------------------------------------------------------------------
void CommandListBuilder_InsertStart(void* Identifier, long Depth)
{
    TCommand* Command;
    TCommandStartData* Data;

    Command = (TCommand*)malloc(sizeof(TCommand));
    Data    = (TCommandStartData*)malloc(sizeof(TCommandStartData));
    ASSERT(Command != NULL);
    ASSERT(Data    != NULL);

    Data->Identifier = Identifier;
    Data->Depth      = Depth;
    Command->Tick = 0;
    Command->CommandCode = Command_Start;
    Command->Data = Data;

    CommandList_Add(&g_Pos, Command);
}
//-------------------------------------------------------------------
void CommandListBuilder_InsertStop(void* Identifier)
{
    TCommand* Command;

    Command = (TCommand*)malloc(sizeof(TCommand));
    ASSERT(Command != NULL);

    Command->Tick = 0;
    Command->CommandCode = Command_Stop;
    Command->Data = Identifier;

    CommandList_Add(&g_Pos, Command);
}
//-------------------------------------------------------------------
void CommandListBuilder_InsertNew(char* Identifier)
{
    TCommand* Command;

    Command = (TCommand*)malloc(sizeof(TCommand));
    ASSERT(Command != NULL);

    Command->Tick = 0;
    Command->CommandCode = Command_New;
    Command->Data = Identifier;

    CommandList_Add(&g_Pos, Command);
    // Seems this feature was for integration with spontz demoeditor at bcnparty'04
    //    List_Insert(g_NewList, Command);
}
//-------------------------------------------------------------------
void CommandListBuilder_InsertSetOrder(int Order, int Row)
{
    TCommand* Command;
    TCommandSetOrderData* Data;

    Command = (TCommand*)malloc(sizeof(TCommand));
    ASSERT(Command != NULL);
    Data = (TCommandSetOrderData*)malloc(sizeof(TCommandSetOrderData));
    ASSERT(Data != NULL);

    Data->Order = Order;
    Data->Row   = Row;
    Command->Tick = 0;
    Command->CommandCode = Command_SetOrder;
    Command->Data = Data;

    CommandList_Add(&g_Pos, Command);
}
//-------------------------------------------------------------------

void CommandListBuilder_InsertRepetitiveCommands(TPos pos_start, TPos pos_end, TPos frequency, TList* command_list)
{
    TCommand* Command1, *Command2;
    TCommandRepetitiveCommandsData* Data;
    TPosList* list;

    Command1 = (TCommand*)malloc(sizeof(TCommand));
    ASSERT(Command1 != NULL);
    Command2 = (TCommand*)malloc(sizeof(TCommand));
    ASSERT(Command2 != NULL);
    Data = (TCommandRepetitiveCommandsData*)malloc(sizeof(TCommandRepetitiveCommandsData));
    ASSERT(Data != NULL);

    List_Reset(command_list);
    if(List_EOF(command_list))
    {
        // No hay commandos a añadir
        // Salimos
        return;
    }
    
    while(!List_EOF(command_list))
    {
        TPosList* pl;
        pl = List_Current(command_list);
        DEBUG_msg_dec("Order: ", (unsigned)pl->Pos.Order);
        DEBUG_msg_dec("Row: ", (unsigned)pl->Pos.Row);
        DEBUG_msg_hex("Commands: ", (unsigned)pl->Commands);
        List_Next(command_list);
    }
    
    Data->start    = pos_start;
    Data->end      = pos_end;
    Data->step     = frequency;
    list = (TPosList*)List_First(command_list);
    DEBUG_msg_hex("TPosList is ", (unsigned)list);
    Data->commands = list->Commands;
    DEBUG_msg_hex("TPosList->commands is ", (unsigned)list->Commands);
    
    Command1->Tick = 0;
    Command1->CommandCode = Command_InsertRepetitiveCommands;
    Command1->Data = Data;

    CommandList_Add(&g_Pos, Command1);

    Command2->Tick = 0;
    Command2->CommandCode = Command_RemoveRepetitiveCommands;
    Command2->Data = Data;
    
    CommandList_Add(&pos_end, Command2);
}
