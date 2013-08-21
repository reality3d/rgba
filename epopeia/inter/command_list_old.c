#include <stdio.h>
#include "../assert.h"
#include "../epopeia.h"
#include "../compiler/type_table.h"
#include "../compiler/method.h"
#include "command.h"
#include "command_list.h"
#include "../pos.h"
#include "../compiler/expression.h"
#include "../compiler/st.h"
#define DEBUG_VAR 0
#define DEBUG_MODULE "command_list"
#include "../debug.h"

typedef struct
{
    TPos Pos;
    TList* Commands;
} TPosList;

void CommandList_InsertCall(char* Identifier, char* Method, TArgList* ParamList, int isDelete);
void CommandList_Flash (int r, int g, int b);
void CommandList_SetPos(int order, int row);
void CommandList_Add   (TPos* Pos, TCommand* Command);

TList* g_CommandList;
TPos g_Pos;
static TList* g_NewList;

//-------------------------------------------------------------------
int FIntCompare(void* Data1, void* Data2)
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
int CommandList_Init(void)
{
    g_CommandList = List_New(Pos_Compare);
    // Inneficient but AlwaysLesser so that objects build in script order
    g_NewList = List_New(AlwaysLesser);
    ASSERT(g_CommandList != NULL);
    ASSERT(g_NewList != NULL);
    g_Pos.Order = -1;
    g_Pos.Row   = 0;
    return 0;
}
//-------------------------------------------------------------------
void CommandList_End (void)
{
    List_Delete(g_CommandList);
}
//-------------------------------------------------------------------
TList* CommandList_SetCommandList(TList* NewCommandList)
{
    TList* old_list;
    old_list = g_CommandList;
    g_CommandList = NewCommandList;
    return old_list;
}
//-------------------------------------------------------------------
TPos CommandList_GetPos(void)
{
    return g_Pos;
}
//-------------------------------------------------------------------
void CommandList_SetPos(int Order, int Row)
{
    g_Pos.Order = Order;
    g_Pos.Row   = Row;
}
//-------------------------------------------------------------------
void CommandList_InsertAssign(char* Identifier, TExpr* Expression)
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
void CommandList_InsertCall(char* Identifier, char* Method, TArgList* ParamList, int isDelete)
{
    TCommand* Command;
    TCommandCallData* Data;
    TTypeId Id;
    TMethod* MethodData;
    //int ParameterIndex;

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
//    Data->ParameterCount  = List_Length(ParamList) + 1;
//    Data->ParameterTable  = (void**)malloc(sizeof(void*)*Data->ParameterCount);
    Data->ArgExprs = ParamList;
    Data->Identifier  = strdup(Identifier);
    Data->FunctionName= strdup(Method);
    //    ASSERT(Data->ParameterTable);
/*
    List_Reset(ParamList);

    // El primer parametro siempre es el puntero al objeto de la clase del metodo
    // Pero como no lo tenemos, pues estamos en tiempo de compilacion,
    // guardamos el nombre de variable para obteer luego el objeto
    Data->ParameterTable[0] = strdup(Identifier);
    ParameterIndex = 1;
    while(!List_EOF(ParamList))
    {
        Data->ParameterTable[ParameterIndex] = ((TTerm*)List_Current(ParamList))->Value;
        ParameterIndex++;
        List_Next(ParamList);
    }
*/
    CommandList_Add(&g_Pos, Command);
}
//-------------------------------------------------------------------
void CommandList_InsertMusicPlay(void)
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
void CommandList_InsertStart(void* Identifier, long Depth)
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

    //printf("Anyadiendo Start en %d,%d\n", g_Pos.Order, g_Pos.Row);
    CommandList_Add(&g_Pos, Command);
}
//-------------------------------------------------------------------
void CommandList_InsertStop(void* Identifier)
{
    TCommand* Command;

    Command = (TCommand*)malloc(sizeof(TCommand));
    ASSERT(Command != NULL);

    Command->Tick = 0;
    Command->CommandCode = Command_Stop;
    Command->Data = Identifier;

    //printf("Anyadiendo Stop en %d,%d\n", g_Pos.Order, g_Pos.Row);
    CommandList_Add(&g_Pos, Command);
}
//-------------------------------------------------------------------
void CommandList_InsertNew(char* Identifier)
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
void CommandList_InsertSetOrder(int Order, int Row)
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

void CommandList_InsertRepetitiveCommands(TPos pos_start, TPos pos_end, TPos frequency, TList* command_list)
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

//-------------------------------------------------------------------
void CommandList_Add   (TPos* Pos, TCommand* Command)
{
    TPosList* PosList;

    DEBUG_msg_hex("Adding command ", (unsigned)Command);
    DEBUG_msg_hex("to list ", (unsigned)g_CommandList);

    List_Reset  (g_CommandList);
    while(!List_EOF(g_CommandList)
          && 0!=Pos_Compare(&((TPosList*)List_Current(g_CommandList))->Pos, Pos))
        List_Next(g_CommandList);

    //List_GotoPos(g_CommandList, Pos);
    DEBUG_msg_dec("Is eof? ", List_EOF(g_CommandList));
    DEBUG_msg_dec("NewOrder ", Pos->Order);
    DEBUG_msg_dec("NewRow ", Pos->Row);
    if(!List_EOF(g_CommandList))
    {
        DEBUG_msg_dec("CurOrder ", ((TPosList*)List_Current(g_CommandList))->Pos.Order);
        DEBUG_msg_dec("CurRow ", ((TPosList*)List_Current(g_CommandList))->Pos.Row);
    }
    if(List_EOF(g_CommandList)
       || 0 != Pos_Compare(&((TPosList*)List_Current(g_CommandList))->Pos, Pos) )
        // No hay una lista para esta posicion; la creamos
    {
        DEBUG_msg("No existe Lista de comandos para ese Pos, creando nuevo.");
        PosList = (TPosList*)malloc(sizeof(TPosList));
	PosList->Pos = *Pos;
	// Meter los comandos al final de la lista para que se ejecuten en el orden
        // del script y no inverso
	PosList->Commands = List_New(AlwaysLesser);
        List_Insert(g_CommandList, PosList);
    }
    else
    {
        DEBUG_msg("Ya existe Lista de comandos para ese Pos, reutilizando.");
        PosList = List_Current(g_CommandList);
    }

    DEBUG_msg_dec("Order ", PosList->Pos.Order);
    DEBUG_msg_dec("Row ",   PosList->Pos.Row);
    DEBUG_msg_hex("PosList ",   (int)PosList->Commands);
    DEBUG_msg_dec("PosList commandlist len ",   List_Length(PosList->Commands));
    List_Insert(PosList->Commands, Command);
    DEBUG_msg_dec("Order ", PosList->Pos.Order);
    DEBUG_msg_dec("Row ",   PosList->Pos.Row);
    DEBUG_msg_hex("PosList ",   (int)PosList->Commands);
    DEBUG_msg_dec("PosList commandlist len ",   List_Length(PosList->Commands));
}
//-------------------------------------------------------------------
TPos* CommandList_CurrentPos(void)
{
    return &g_Pos;
}
//-------------------------------------------------------------------
int CommandList_CurrentExec(void)
{
    TPosList* PosList;    
    TList* CL;
    ASSERT(!List_EOF(g_CommandList));

//    printf("CommandList_CurrentExec 2\n");
    PosList = List_Current(g_CommandList);
//    printf("Ejecutando lista de comandos de pos: %d,%d\n", PosList->Pos.Order, PosList->Pos.Row);
    CL = PosList->Commands;
    List_Reset(CL);
//    printf("CommandList_CurrentExec 3\n");
    while(!List_EOF(CL))
    {
        int error;

        TCommand* Cmd = List_Current(CL);
        DEBUG_msg_hex("Executing command ", (unsigned)Cmd);
        error = Cmd->CommandCode(Cmd->Data);
        if(error)
        {
            DEBUG_msg_dec("Received error #", error);
            return error;
        }

        List_Next(CL);
    }
    return 0;
}
//-------------------------------------------------------------------
int CommandList_Next(void)
{
    TPosList* PosList;
    List_Next(g_CommandList);
    if(List_EOF(g_CommandList))
        return -1;
    PosList = List_Current(g_CommandList);
    g_Pos = PosList->Pos;
    return 0;
}
//-------------------------------------------------------------------
void CommandList_Reset(void)
{
    TPosList* PosList;
    List_Reset(g_CommandList);
    PosList = List_Current(g_CommandList);
    g_Pos = PosList->Pos;
}

//-------------------------------------------------------------------
int CommandList_ExecuteLoad(void)
{
    List_Reset(g_NewList);
    while(!List_EOF(g_NewList))
    {
        int error;

        TCommand* Cmd = List_Current(g_NewList);
        DEBUG_msg_hex("Executing command ", (unsigned)Cmd);
        error = Cmd->CommandCode(Cmd->Data);
        if(error)
        {
            DEBUG_msg_dec("Received error #", error);
            return error;
        }
        List_Next(g_NewList);
    }
    return 0;
}
