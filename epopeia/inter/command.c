#include <stdio.h>
#include <stdlib.h>
#include "../assert.h"
#include "../epopeia.h"
#include "../epopeia_internal.h"
#include "command.h"
#include "call.h"
#include "../compiler/st.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "command"
#include "../debug.h"
#include "../compiler/expression.h"
#include "inter.h"

extern int console_output;

//-------------------------------------------------------------------
int Command_Assign(void* _Data)
{
    TCommandAssignData* data = (TCommandAssignData*)_Data;
    TExprResult* r;
    void *current_value;
    
    DEBUG_msg_str("Ejecutando asignacion a la variable ", data->Identifier);
    r = Expr_Solve(data->Expression);
    current_value = ST_Value(data->Identifier);
    if(current_value != NULL)
        free(current_value);
    DEBUG_msg("Llamando a ST_SetValue");
    ST_SetValue(data->Identifier, r);
    return 0;
}
//-------------------------------------------------------------------
int Command_Start(void* _Data)
{
    TCommandStartData* Data = (TCommandStartData*)_Data;
    DEBUG_msg_dec("Start command! Depth: ", Data->Depth);
    Inter_AddObjectToRenderList(Data->Identifier, Data->Depth);
    return 0;
}
//-------------------------------------------------------------------
int Command_Stop(void* Identifier)
{
    DEBUG_msg_hex("Stop command! Object ", (unsigned)Identifier);
    Inter_RemoveObjectFromRenderList((char*)Identifier);
    return 0;
}
//-------------------------------------------------------------------
int Command_PerformCall(void* _Data)
{
    TCommandCallData* Data = (TCommandCallData*)_Data;
    TExprResult* Result;
    DEBUG_msg("Command_PerformCall in progress");
    ASSERT(Data != NULL);
    //    DEBUG_msg_dec("Call command parameter count: ", Data->ParameterCount);
    ASSERT(Data->Identifier != NULL);
    ASSERT(Data->FunctionName != NULL);
    ASSERT(Data->ArgExprs != NULL);

    Result = Expr_MethodCall(Data->Identifier, Data->FunctionName, Data->ArgExprs);
    free(Result);
    
//    Call(Data->FunctionAddress, (int*)Data->ParameterTable, Data->ParameterCount);
    return 0;
}
//-------------------------------------------------------------------
int Command_MusicPlay(void* _Data_not_used)
{
    if(console_output) printf("Reproduciendo modulo de sonido...\n");
    g_Epopeia.Sound->ModulePlay(g_Epopeia.Sound);
    return 0;
}
//-------------------------------------------------------------------
int  Command_New(void* _Data)
{
    char* Identifier = (char*)_Data;
    int error;

    DEBUG_msg_str("New command: ", Identifier);
    error = ST_ConstructObject(Identifier);
    DEBUG_msg_hex("failure in creation?: ", error);
    return error;
}
//-------------------------------------------------------------------
int Command_Delete(void* _Data)
{
    TCommandCallData* Data = (TCommandCallData*)_Data;
    TExprResult* Result;

    DEBUG_msg("Command_Delete in progress");
    ASSERT(Data != NULL);
//    DEBUG_msg_dec("Delete command param count ", Data->ParameterCount);
    ASSERT(Data->Identifier != NULL);
    ASSERT(Data->FunctionName != NULL);
    ASSERT(Data->ArgExprs != NULL);

    Result = Expr_MethodCall(Data->Identifier, Data->FunctionName, Data->ArgExprs);
    free(Result);
    
//    Data->ParameterTable[0] = ST_Value(Data->ParameterTable[0]);
//    Call(Data->FunctionAddress, (int*)Data->ParameterTable, Data->ParameterCount);

    ST_UnRegister(Data->Identifier);
    return 0;
}
//-------------------------------------------------------------------
int Command_SetOrder(void* _Data)
{
    TCommandSetOrderData* Data = (TCommandSetOrderData*)_Data;
    TPos pos;

    ASSERT(Data != NULL);
    DEBUG_msg_dec("SetOrder command order: %d" , Data->Order);
    DEBUG_msg_dec("SetOrder command row: %d", Data->Row);

    pos.Order = Data->Order;
    pos.Row   = Data->Row;
    g_Epopeia.Sound->SetPos(g_Epopeia.Sound, &pos);

    return 0;
}

//-------------------------------------------------------------------
int Command_RemoveRepetitiveCommands(void* _Data)
{
    TCommandRepetitiveCommandsData* Data = (TCommandRepetitiveCommandsData*)_Data;
    DEBUG_msg("Repetitive command remove!");
    
    Inter_RemoveCommandsFromPermanentList(Data->end, Data->step, Data->commands);
    return 0;
}
//-------------------------------------------------------------------
int Command_InsertRepetitiveCommands(void* _Data)
{
    TCommandRepetitiveCommandsData* Data = (TCommandRepetitiveCommandsData*)_Data;
    DEBUG_msg("Repetitive command insert!");
    
    Inter_AddCommandsToPermanentList(Data->start, Data->end, Data->step, Data->commands);
    return 0;
}

