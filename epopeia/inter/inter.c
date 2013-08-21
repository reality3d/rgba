#include <stdio.h>
#include <malloc.h>
#include "../assert.h"
#include "../epopeia.h"
#include "../epopeia_internal.h"
#include "../list.h"
#include "inter.h"
#include "../compiler/type_table.h"
#include "../compiler/st.h"
#include "../compiler/errors.h"
#include "../pos.h"
#include "command_list.h"
#include "permanent_command_list.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "inter"
#include "../debug.h"

#ifdef LINUX
#include <unistd.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

int debug=0;

typedef struct
{
    TTypeId Id;
    void* Object;
    void (*DoFrame)(void* SelfObject, TGraphicsContext* RC, TSoundContext* SC);
    unsigned StartTick;
    TPos StartPos;
    long Depth;
} TRender;

TList* g_RenderList;
TSoundContext* g_SC;


void Inter_ProcessKeys(TSoundContext** SC);
int  Inter_ExecCommands(TSoundContext* SC);
void Inter_Renderizar(TGraphicsContext* RC, TSoundContext* SC);
void Inter_RenderListRemoveObjects(void);

//-------------------------------------------------------------------
int Inter_RenderCompare(void* Data1, void* Data2)
{
    // La lista generica ordena de forma ascendente
    // Pero nosotros lo queremos de forma descendente,
    // asi que invertimos los signos
    TRender*r1, *r2;

    r1 = (TRender*)Data1;
    r2 = (TRender*)Data2;
    DEBUG_msg_dec("Compare depth 1: ", r1->Depth);
    DEBUG_msg_dec("Compare depth 2: ", r2->Depth);
    return -(r1->Depth - r2->Depth);
}
//-------------------------------------------------------------------
int Inter_Init(void)
{
    int Error;
    g_RenderList = List_New(Inter_RenderCompare);

    Error = PermanentCommandList_Init();
    if(Error)
    {
        printf("Permanent command list init failed\n");
        return Error;
    }

    return 0;
}
//-------------------------------------------------------------------
void Inter_End(void)
{
    PermanentCommandList_End();
    List_Delete(g_RenderList);
}
//-------------------------------------------------------------------
void Inter_RunDemo(void)
{
    TGraphicsContext* RC;
    int error;
    int exit_now = 0;
    int fade_out_mark = 0;

    // Perform all construction
    printf("Loading data...\n");
    error = CommandList_ExecuteLoad();
    if(error)
    {
	printf("Error loading data!");
        return;
    }
    // Play
    CommandList_Reset();
    DEBUG_msg("CommandList reset complete");

    error = 0;
    while(!exit_now)
    {
        g_SC = g_Epopeia.Sound->GetSoundContext(g_Epopeia.Sound);

        DEBUG_msg("Got sound context");
        DEBUG_msg_dec("Order:", g_SC->Pos.Order);
	DEBUG_msg_dec("mSecs:", g_SC->Pos.Row);
        printf("%d\n", g_SC->Pos.Row);
        ASSERT(g_SC != NULL);
        DEBUG_msg_hex("KeyPressed? - MoveKey is:", g_Epopeia.MoveKeys);
        if(g_Epopeia.MoveKeys)
	{
            if(g_Epopeia.Keyboard->KeyPressed(g_Epopeia.Keyboard))
            {
                DEBUG_msg("Processing keys");
                Inter_ProcessKeys(&g_SC);
            }
        }
        DEBUG_msg("KeyPressed? - over");

        error = Inter_ExecCommands(g_SC);
        if(error)
        {
            DEBUG_msg("La ejecucion de comandos ha fallado.");
            break;
        }

	DEBUG_msg("ExecCommands - over");

        DEBUG_msg("ExecPermanentCommands - begin");
        error = PermanentCommandList_Exec(g_SC);
        if(error)
        {
            DEBUG_msg("La ejecucion de comandos permanentes ha fallado.");
            break;
        }
        DEBUG_msg("ExecPermanentCommands - over");

        RC = g_Epopeia.Graphics->FrameStart(g_Epopeia.Graphics);
        DEBUG_msg("FrameStart - over");
        ASSERT(RC != NULL);
        Inter_Renderizar(RC, g_SC);
        DEBUG_msg("Renderizar - over");
	if(g_Epopeia.Keyboard->ExitPressed(g_Epopeia.Keyboard))
	{
            float percent;
#define FADE_LENGTH_MS 1500.0f
            DEBUG_msg("ExitPressed is true!");
	    if(fade_out_mark == 0)
		fade_out_mark = Epopeia_GetTimeMs();

	    percent = (Epopeia_GetTimeMs() - fade_out_mark)/FADE_LENGTH_MS;
	    g_Epopeia.Sound->SetVolume(g_Epopeia.Sound, (int)((1.0f-percent)*100));
	    g_Epopeia.Graphics->SetGlobalFade(g_Epopeia.Graphics, 0.0, 0.0, 0.0, percent);
	    if(percent >= 1.0 || g_Epopeia.Keyboard->ExitPressed(g_Epopeia.Keyboard)>=2)
                exit_now = 1;
	}
        g_Epopeia.Graphics->FrameEnd(g_Epopeia.Graphics);
	DEBUG_msg("FrameEnd - over");
    }

    if(error)
    {
        DEBUG_msg_dec("Exiting due to error ", error);
        printf("Error #%d. Saliendo.\n", error);
    }

    DEBUG_msg("Out of inter loop! - stop module");
    g_Epopeia.Sound->ModuleStop(g_Epopeia.Sound);
    DEBUG_msg("stop module complete");

}
//-------------------------------------------------------------------
void Inter_Renderizar(TGraphicsContext* RC, TSoundContext* SC)
{
    TSoundContext RelativeSC;
    int i;
    i = 1;
    DEBUG_msg("Renderizando");
    List_Reset(g_RenderList);
    while(!List_EOF(g_RenderList))
    {
        TRender* Render = List_Current(g_RenderList);
        ASSERT(Render != NULL);
        RelativeSC.Tick = SC->Tick - Render->StartTick;
        DEBUG_msg_dec("Tick: ", RelativeSC.Tick);
        RelativeSC.Pos  = Pos_Substract(&SC->Pos, &Render->StartPos);
        DEBUG_msg_dec("Llamando DoFrame: ", i);
        DEBUG_msg_str("Llamando DoFrame (tipo): ", Render->Id->Name);

        Render->DoFrame(Render->Object, RC, &RelativeSC);

        List_Next(g_RenderList);
        i++;
    }
}
//-------------------------------------------------------------------
void Inter_ProcessKeys(TSoundContext** SC)
{
#define PRESSED(x) g_Epopeia.Keyboard->Pressed(g_Epopeia.Keyboard, x)

    if(PRESSED(KEY_RIGHT))
    {
        DEBUG_msg("Processing key_right");
        switch(g_Epopeia.Sound->GetSongType(g_Epopeia.Sound))
        {
        case ftMod:
            (*SC)->Pos.Order ++;
            DEBUG_msg_dec("Order is", (*SC)->Pos.Order);
            Pos_Normalize(&(*SC)->Pos);
            DEBUG_msg_dec("Order now is", (*SC)->Pos.Order);
            g_Epopeia.Sound->SetPos(g_Epopeia.Sound, &(*SC)->Pos);
            break;
        case ftOgg:
            DEBUG_msg("Ogg type processing");
            (*SC)->Pos.Row += 10000;
            g_Epopeia.Sound->SetPos(g_Epopeia.Sound, &(*SC)->Pos);
            break;
        default:
            DEBUG_msg("SongType not recognized");
            break;
        }
        *SC = g_Epopeia.Sound->GetSoundContext(g_Epopeia.Sound);
    }
    else if(PRESSED(KEY_SPACE))
    {
	// FIXME: this crashes on windows
//        g_Epopeia.Graphics->ToggleWindowed(g_Epopeia.Graphics);
    }
    else if(PRESSED(KEY_LEFT))
    {
        DEBUG_msg("Processing key_left");
        switch(g_Epopeia.Sound->GetSongType(g_Epopeia.Sound))
        {
        case ftMod:
            (*SC)->Pos.Order --;
	    Pos_Normalize(&(*SC)->Pos);
            CommandList_Reset();
            g_Epopeia.Sound->SetPos(g_Epopeia.Sound, &(*SC)->Pos);
	    Inter_RenderListRemoveObjects();
            PermanentCommandList_RemoveCommandsAll();
            break;
        case ftOgg:
            (*SC)->Pos.Row -= 10000;
            CommandList_Reset();
            g_Epopeia.Sound->SetPos(g_Epopeia.Sound, &(*SC)->Pos);
	    Inter_RenderListRemoveObjects();
            PermanentCommandList_RemoveCommandsAll();
	    break;
        default:
            DEBUG_msg("SongType not recognized");
            break;
        }
        *SC = g_Epopeia.Sound->GetSoundContext(g_Epopeia.Sound);
    }

}
//-------------------------------------------------------------------
int Inter_ExecCommands(TSoundContext* SC)
{
    int Error = 0;

    while(!Error && 0>=Pos_Compare(CommandList_CurrentPos(), &(SC->Pos)))
    {
	TPos *Pos;

        Pos = CommandList_CurrentPos();
	DEBUG_msg_dec("CL pos->order : ", Pos->Order);
	DEBUG_msg_dec("CL pos->row : ", Pos->Row);
        Error = CommandList_CurrentExec();
        if(Error)
        {
            printf("Error al ejecutar un comando (error #%d)\n", Error);
            return 1;
        }
        Error = CommandList_Next();
	if(Error)
	{
            printf("No hay proximo Commando! (inter.c)\n");
	    return 2;
        }

    }
    return 0;
}
//-------------------------------------------------------------------
void Inter_AddObjectToRenderList(void* Identifier, long Depth)
{
    TRender* Render = malloc(sizeof(TRender));
    ASSERT(Render != NULL);
    Render->Object  = ST_Value(Identifier);
    Render->Id      = ST_Type(Identifier);
    Render->Depth   = Depth;

    ASSERT(Type_HasMethod(Render->Id, "DoFrame"));

    Render->DoFrame = Type_GetMethod(Render->Id, "DoFrame")->CallAddress;
    Render->StartPos = g_SC->Pos;
    Render->StartTick= g_SC->Tick;
    List_Insert(g_RenderList, Render);
    DEBUG_msg_hex("Added to render list identifier: ", (unsigned long)Identifier);
    DEBUG_msg_hex("Added to render list object address: ", (unsigned long)ST_Value(Identifier));
}
//-------------------------------------------------------------------
void Inter_RemoveObjectFromRenderList(void* Identifier)
{
    DEBUG_msg_str("We want to delete object indent: ", Identifier);
    DEBUG_msg_hex("We want to delete object address: ", (unsigned long)ST_Value(Identifier));
    
    List_Reset(g_RenderList);
    while(!List_EOF(g_RenderList))
    {
        TRender* Render = List_Current(g_RenderList);
        ASSERT(Render != NULL);
        if(Render->Object == ST_Value(Identifier))
        {
            DEBUG_msg("borrando objeto");
            List_Drop(g_RenderList, Render);
            DEBUG_msg("reseteando lista de render");
            List_Reset(g_RenderList);
            DEBUG_msg("saliendo de removeobject");
            return;
	}
        List_Next(g_RenderList);
    }
    if(List_EOF(g_RenderList))
    {
        char ctemp[200];
        sprintf(ctemp, "Se ha intentado hacer Stop del objeto %s, pero no se ha hecho ningun Start.", (char*)Identifier);
        warning_runtime(ctemp);
    }
    List_Reset(g_RenderList);
}
//-------------------------------------------------------------------
void Inter_RenderListRemoveObjects(void)
{
    List_Reset(g_RenderList);
    while(!List_EOF(g_RenderList))
    {
        TRender* Render = List_Current(g_RenderList);
	List_Drop(g_RenderList, Render);
	List_Reset(g_RenderList);
    }
}

//-------------------------------------------------------------------
void Inter_AddCommandsToPermanentList(TPos start, TPos end, TPos step, TList* commands)
{
    PermanentCommandList_InsertCommands(start, end, step, commands);
}
//-------------------------------------------------------------------
void Inter_RemoveCommandsFromPermanentList(TPos end, TPos step, TList* commands)
{
    PermanentCommandList_RemoveCommands(end, step, commands);
}

