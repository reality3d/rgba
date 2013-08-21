#include <stdio.h>
#include "../assert.h"
#include "../epopeia.h"
//#include "../compiler/type_table.h"
//#include "../compiler/method.h"
#include "command.h"
#include "command_list.h"
#include "../pos.h"
//#include "../compiler/expression.h"
//#include "../compiler/st.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "command_list"
#include "../debug.h"

static TList* g_CommandList;
static TPos g_Pos;
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
    DEBUG_msg("Command list init");
    g_CommandList = List_New(Pos_Compare);
    // Inneficient but AlwaysLesser so that objects build in script order
    g_NewList = List_New(AlwaysLesser);
    ASSERT(g_CommandList != NULL);
    ASSERT(g_NewList != NULL);
    g_Pos.Order = -1;
    g_Pos.Row   = 0;
    DEBUG_msg("Command list init -end");
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
//    g_Pos.Order = 0;
//    g_Pos.Row   = 0;

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
