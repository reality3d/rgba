#include <stdio.h>
#include <malloc.h>
#include "../assert.h"
#include "../epopeia.h"
#include "../compiler/type_table.h"
#include "../compiler/method.h"
#include "command.h"
#include "permanent_command_list.h"
#include "../pos.h"
#include "../compiler/expression.h"
#include "../compiler/st.h"
#define DEBUG_VAR 0
#define DEBUG_MODULE "permanent_cl"
#include "../debug.h"

typedef struct
{
    TPos End;
    TPos Step;
    TPos LastExec;
    void* Command;
} TPosStepList;

void PermanentCommandList_Add   (TPos* Start, TPos* End, TPos* Step, TCommand* Command);
void PermanentCommandList_Remove   (TPos* End, TPos* Step, TCommand* Command);

static TList* g_PermanentCommandList;

//-------------------------------------------------------------------
int PCL_FIntCompare(void* Data1, void* Data2)
{
    int Pos1 = (int)Data1;
    int Pos2 = (int)Data2;

    if(Pos1 < Pos2)
        return -1;
    if(Pos1 > Pos2)
        return 1;
    return 0;
}
//-------------------------------------------------------------------
int PermanentCommandList_Init(void)
{
    g_PermanentCommandList = List_New(Pos_Compare);
    DEBUG_msg("Permanent command list initialized");
    ASSERT(g_PermanentCommandList != NULL);
    return 0;
}
//-------------------------------------------------------------------
void PermanentCommandList_End (void)
{
    List_Delete(g_PermanentCommandList);
}

//-------------------------------------------------------------------
void PermanentCommandList_InsertCommands(TPos pos_start, TPos pos_end, TPos frequency, TList* command_list)
{
    List_Reset(command_list);
    DEBUG_msg_hex("Received permanent command list to add: ", (int)command_list);
    while(!List_EOF(command_list))
    {
        DEBUG_msg_hex("Adding permanent command: ", (int)List_Current(command_list));
        PermanentCommandList_Add(&pos_start, &pos_end, &frequency, List_Current(command_list));
        List_Next(command_list);
    }
}

//-------------------------------------------------------------------
void PermanentCommandList_RemoveCommands(TPos pos_end, TPos frequency, TList* command_list)
{
    List_Reset(command_list);
    DEBUG_msg_hex("Received permanent command list to remove: ", (int)command_list);
    while(!List_EOF(command_list))
    {
        PermanentCommandList_Remove(&pos_end, &frequency, List_Current(command_list));
        List_Next(command_list);
    }
}
//-------------------------------------------------------------------
void PermanentCommandList_RemoveCommandsAll(void)
{
    List_Reset(g_PermanentCommandList);
    while(!List_EOF(g_PermanentCommandList))
    {
	free(List_First(g_PermanentCommandList));
	List_DropFirst(g_PermanentCommandList);
    }
}

//-------------------------------------------------------------------
void PermanentCommandList_Add   (TPos* Start, TPos* End, TPos* Step, TCommand* Command)
{
    TPosStepList* PosStepList;
    DEBUG_msg_hex("Insert Permanent Command #", (int)Command);
    PosStepList = (TPosStepList*)malloc(sizeof(TPosStepList));
    PosStepList->End  = *End;
    PosStepList->Step = *Step;
    PosStepList->LastExec = *Start;
    PosStepList->Command = Command;
    List_Insert(g_PermanentCommandList, PosStepList);
}

//-------------------------------------------------------------------
void PermanentCommandList_Remove   (TPos* End, TPos* Step, TCommand* Command)
{
    int removed = 0;
    
    DEBUG_msg_hex("Remove Permanent Command #", (int)Command);
    //    List_GotoPos(g_PermanentCommandList, End);

    List_Reset(g_PermanentCommandList);
    while(!List_EOF(g_PermanentCommandList) && !removed)
    {
        TPosStepList* item = List_Current(g_PermanentCommandList);
        if(item->Command == Command)
        {
            List_Drop(g_PermanentCommandList, item);
            removed = 1;
        }
        List_Next(g_PermanentCommandList);
        }

    if(!removed)
    {
        DEBUG_msg("Command was not removed from permanent command list");
    }
}

//-------------------------------------------------------------------
int PermanentCommandList_Exec(TSoundContext* SC)
{
    DEBUG_msg("Start function");
    List_Reset(g_PermanentCommandList);
    DEBUG_msg("Reset list over");
    DEBUG_msg_dec("Order: ", SC->Pos.Order);
    DEBUG_msg_dec("Row  : ", SC->Pos.Row);
    DEBUG_msg_dec("PermanentCommandList EOF? ", List_EOF(g_PermanentCommandList));
    while(!List_EOF(g_PermanentCommandList))
    {
        int error;
        TPosStepList *cmd = List_Current(g_PermanentCommandList);
        TPos pos_exec;
        DEBUG_msg_hex("PosStepList is ", (int)cmd);
        DEBUG_msg_dec("LastExec Row:", cmd->LastExec.Row);
        DEBUG_msg_dec("LastExec Order:", cmd->LastExec.Order);
        DEBUG_msg_dec("Step Row:", cmd->Step.Row);
        DEBUG_msg_dec("Step Order:", cmd->Step.Order);
        pos_exec = Pos_Add(&cmd->LastExec, &cmd->Step);
        DEBUG_msg_dec("pos_exec Row:", pos_exec.Row);
        DEBUG_msg_dec("pos_exec Order:", pos_exec.Order);
        DEBUG_msg_dec("CurrPos Row:", SC->Pos.Row);
        DEBUG_msg_dec("CurrPos Order:", SC->Pos.Order);
        if(Pos_Compare(&pos_exec, &SC->Pos) <= 0)
        {
            TCommand* Cmd = (TCommand*)cmd->Command;
            DEBUG_msg("Buenas NAS");
            DEBUG_msg_hex("Comand to execute: ", (int)Cmd);
            DEBUG_msg_hex("Comand code to execute: ", (int)Cmd->CommandCode);
            DEBUG_msg_hex("Comand data: ", (int)Cmd->Data);
            error = Cmd->CommandCode(Cmd->Data);
            if(error)
            {
                DEBUG_msg("ERROR executing a permanent command");
                return error;
            }
            cmd->LastExec = Pos_Add(&cmd->LastExec, &cmd->Step);
        }
        List_Next(g_PermanentCommandList);
    }
    DEBUG_msg("Func end");
    return 0;
}
