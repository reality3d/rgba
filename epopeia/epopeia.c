#ifdef WIN32
#include <windows.h>
#include "dir.h"     // por ahora solo para windows
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "epopeia.h"
#include "epopeia_internal.h"
#include "demo.h"
#include "compiler/type_table.h"
#include "compiler/lex.h"
#include "compiler/grammar.h"
#include "compiler/st.h"
#include "compiler/errors.h"
#include "compiler/function_table.h"
#include "inter/inter.h"
#include "inter/command_list.h"
#include "inter/command_list_builder.h"
#include "graphics.h"
#include "sound.h"
#include "keyboard.h"
#include "packer/packer.h"
#if defined(LINUX)
  #include <sys/time.h>     // para el ftime()
#endif
#define DEBUG_VAR 1
#define DEBUG_MODULE "epopeia"
#include "debug.h"


#ifdef WIN32
static void Epopeia_ScriptCompileFromDirectory(const char *path);
#endif

struct Epopeia_t g_Epopeia = { NULL, NULL, NULL, 0, 1, NULL };
EPOPEIA_API_VAR const char* EpopeiaVersionString = "v1.4.2";

static int Windowed = 0;
static int Width  = 800;
static int Height = 600;
#ifdef WIN32
static int Bpp    = 32;
#else
static int Bpp    = 24;
#endif
static int ZBuffer = 24;
static int Stencil = 8;
static int SongNumber = 1;
static int ShowMenu = 1;


int console_output = 1;
//-----------------------------------------------------------------------------
EPOPEIA_API int Epopeia_Init(void)
{
    int Error;

    TResolution resolution;

    Epopeia_Print("\n");
    Epopeia_Print(DemoGetStartMessage());
    Epopeia_Print("\n\n");
    DEBUG_msg("Epopeia init");

    Epopeia_Print(". Initializing...\n");

    resolution.Width  = Width;
    resolution.Height = Height;
    resolution.Bpp    = Bpp;

    // Llamar al menu principal
    if(g_Epopeia.MainMenu != NULL && ShowMenu)
    {
        Error = g_Epopeia.MainMenu(DemoGetName(), &resolution.Width, &resolution.Height, &Windowed);
        if(Error)
        {
            Epopeia_Print("Error al salir del menu.\n");
            return -1;
        }
	Width  = resolution.Width;
	Height = resolution.Height;
    }

    // Inicializar subsistemas Grafico, Teclado, Sonido

    if(g_Epopeia.Graphics == NULL)
    {
        DEBUG_msg("There is no graphics subsystem");
        Epopeia_Print("There is no graphics subsystem");
        return -1;
    }

    if(g_Epopeia.Keyboard == NULL)
    {
        DEBUG_msg("There is no keyboard subsystem");
        Epopeia_Print("There is no keyboard subsystem");
        return -1;
    }

    if(g_Epopeia.Sound == NULL)
    {
        DEBUG_msg("There is no sound subsystem");
        Epopeia_Print("There is no sound subsystem");
        return -1;
    }


    DEBUG_msg("Sound system init...");
    Epopeia_Print(". Iniciando subsistema de sonido... \n");
    Error = g_Epopeia.Sound->Init(g_Epopeia.Sound, 0);
    if(Error)
    {
        Epopeia_Print("Error al inicializar subsistema de sonido.\n");
        return -1;
    }

    DEBUG_msg("Graphics system init...");
    Epopeia_Print(". Iniciando subsistema grafico...\n");
    // Esto debería hacerlo quiza el Graphics por omision FIXME
    g_Epopeia.Graphics->SetZBuffer(g_Epopeia.Graphics, ZBuffer);
    g_Epopeia.Graphics->SetStencilBuffer(g_Epopeia.Graphics, Stencil);
    // End Esto...

    Error = g_Epopeia.Graphics->Init(g_Epopeia.Graphics, &resolution, Windowed);
    if(Error)
    {
        Epopeia_Print("Error al inicializar subsistema grafico.\n");
        return -1;
    }

    DEBUG_msg("Keyboard system init...");
    // Obtener Keyboard
    Epopeia_Print(". Iniciando subsistema de teclado...\n");
    Error = g_Epopeia.Keyboard->Init(g_Epopeia.Keyboard);
    if(Error)
    {
        Epopeia_Print("Error al inicializar subsistema de teclado.\n");
        return -1;
    }

#ifdef _WIN32
    // Establecer resolucion del timer de resolucion a 1ms
    timeBeginPeriod(1);
#endif //_WIN32

    return 0;
}
//-----------------------------------------------------------------------------
EPOPEIA_API int Epopeia_ScriptInit(void)
{
    int Error;

    Epopeia_Print(">>>>>>>>>> Epopeia Demosystem by enlar/rgba - http://www.rgba.org <<<<<<<<<<\n");
    Epopeia_Print(EpopeiaVersionString);
    Epopeia_Print("\n");

    DEBUG_msg_dec("console_output:", console_output);
    DEBUG_msg("Registering types");
    Epopeia_Print(". Loading plugins...\n");
    Type_Init();
    Error = ST_Init();
    if(Error)
        return Error;

    Error = FunctionTable_Init();
    
    // Registrar tipos dinamicos
    Epopeia_RegisterBuilinTypes();
    DEBUG_msg("End ScriptInit");
    return 0;
}

//-----------------------------------------------------------------------------

EPOPEIA_API int Epopeia_ScriptCompile(char* ScriptFilename)
{
    int Error;

    Epopeia_Print(". Checking type consistency...\n");
    DEBUG_msg("Checking type consistency");
    // Comprobar que los tipos dados de alta tienen los metodos
    // obligatorios, comprobacion basica
    Error = Type_ConsistencyCheck();

    if(Error)
    {
        Epopeia_Print("Dinamic type consistency check failed.\n");
        return Error;
    }

    DEBUG_msg("Commandlist init");
    Epopeia_Print(". CommandList initializing ...\n");
    Error = CommandList_Init();
    if(Error)
    {
        Epopeia_Print("Script interpreter init failed\n");
        return Error;
    }
    DEBUG_msg("CommandListBuilder init");
    Error = CommandListBuilder_Init();
    if(Error)
    {
        Epopeia_Print("Command list builder init failed\n");
        return Error;
    }
    
    DEBUG_msg("Script player init");
    Epopeia_Print(". Initializing script player...\n");
    Error = Inter_Init();
    if(Error)
    {
        Epopeia_Print("Script interpreter init failed\n");
        return Error;
    }

    DEBUG_msg("Compiling script");
    Epopeia_Print(". Compiling script...");


	    Epopeia_Print("\t\t-> ");
	    Epopeia_Print(ScriptFilename);
            Epopeia_Print("\n");
	    Error = lex_init(ScriptFilename);
	    if(Error)
	    {
		char ctemp[200];

		sprintf(ctemp, "Lexer init error (%d).\n", Error);
		Epopeia_Print(ctemp);
		return Error;
	    }
	    lex_next_token();
	    DEBUG_msg("Calling DemoScript");
	    DemoScript();
	    DEBUG_msg("DemoScript call end");
	    lex_finish();

/*
    // LOOP START
    int files_left;
    files_left = pak_list_reset();
    while(files_left)
    {
	char *filename;
        int len;
	filename = pak_list_current_filename();
	len = strlen(filename);
	if(len > 3
	   && filename[len-4] == '.'
           && filename[len-3] == 'e'
           && filename[len-2] == 'p'
	   && filename[len-1] == 'o')
	{
	    Epopeia_Print("\t\t-> ");
	    Epopeia_Print(filename);
            Epopeia_Print("\n");
	    Error = lex_init(filename);
	    if(Error)
	    {
		char ctemp[200];

		sprintf(ctemp, "Lexer init error (%d).\n", Error);
		Epopeia_Print(ctemp);
		return Error;
	    }
	    lex_next_token();
	    DEBUG_msg("Calling DemoScript");
	    DemoScript();
	    DEBUG_msg("DemoScript call end");
	    lex_finish();
	}
	files_left = pak_list_next();
    }
*/

#ifdef WIN32
    Epopeia_ScriptCompileFromDirectory("./");
    Epopeia_ScriptCompileFromDirectory("data/");
#endif

    // LOOP END
    DEBUG_msg("ScriptCompile end");
    
    DEBUG_msg_dec("Error count ",error_get_count());
    
    return error_get_count();
}
//-----------------------------------------------------------------------------
#ifdef WIN32
static void Epopeia_ScriptCompileFromDirectory(const char *path)
{
    int Error;
    DIRECTORY *dir = DIRECTORY_Open(path);
    const char *filename;
    filename = DIRECTORY_GetNextFilename(dir);
    while(filename != NULL)
    {
	int len;
        char fullpath[1000];
	sprintf(fullpath, "%s%s", path, filename);
	len = strlen(fullpath);
	if(len > 3
	   && fullpath[len-4] == '.'
           && fullpath[len-3] == 'e'
           && fullpath[len-2] == 'p'
	   && fullpath[len-1] == 'o')
	{
	    Epopeia_Print("\t\t-> ");
	    Epopeia_Print(fullpath);
            Epopeia_Print("\n");
	    Error = lex_init(fullpath);
	    if(Error)
	    {
		char ctemp[1000];

                sprintf(ctemp, "Lexer init error (%d) for script:\n%s\n", Error, fullpath);
		Epopeia_Print(ctemp);
	    }
	    lex_next_token();
	    DEBUG_msg("Calling DemoScript");
	    DemoScript();
	    DEBUG_msg("DemoScript call end");
	    lex_finish();
	}
        filename = DIRECTORY_GetNextFilename(dir);
    }
    DIRECTORY_Close(dir);
}
#endif

//-----------------------------------------------------------------------------
EPOPEIA_API int Epopeia_Load(void)
{
    int Error;
    DEBUG_msg("Epopeia load");
    DEBUG_msg(DemoGetModFile());
//    DEBUG_msg_hex("Address now: ", g_Epopeia.Sound->ModuleLoad);
    Epopeia_Print(". Cargando musica... "); //fflush(stdout);
    assert(g_Epopeia.Sound != NULL);
    Error = g_Epopeia.Sound->ModuleLoad((TSound*)g_Epopeia.Sound, DemoGetModFile());
    DEBUG_msg("Returned from ModuleLoad");

    if(Error)
    {
        DEBUG_msg("Module load failed!");
        return Error;
    }
    Epopeia_Print("Correcto\n");

    // Repasar todos los objetos y construirlos con New
/*
    Epopeia_Print(". Construyendo objetos...\n");
    Error = ST_ConstructObjects();
    if(Error)
    {
        return Error;
    }
*/

    return 0;
}
//-----------------------------------------------------------------------------
EPOPEIA_API int Epopeia_Play(void)
{
    DEBUG_msg("playing demo!");
    Epopeia_Print(". Playing demo!\n");

    Inter_RunDemo();
    return 0;
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_Unload(void)
{
    Epopeia_Print(". Total object destruction...\n");
    DEBUG_msg("Destruyendo objetos...");
    ST_DestroyObjects();
    DEBUG_msg("Descargando musica...");
    g_Epopeia.Sound->ModuleUnload(g_Epopeia.Sound);
    DEBUG_msg("Descarga finalizada.");

}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_End(void)
{
    // Desactivar Graphics, Sound, Keyboard

    DEBUG_msg("Desactivando sonido...");
    g_Epopeia.Sound->End(g_Epopeia.Sound);
    g_Epopeia.Sound->Delete(g_Epopeia.Sound);
    DEBUG_msg("Desactivando sensores...");
    g_Epopeia.Keyboard->End(g_Epopeia.Keyboard);
    g_Epopeia.Keyboard->Delete(g_Epopeia.Keyboard);
    DEBUG_msg("Desactivando renderer...");
    g_Epopeia.Graphics->End(g_Epopeia.Graphics);
    g_Epopeia.Graphics->Delete(g_Epopeia.Graphics);
    DEBUG_msg("Desactivando interprete...");    
    Inter_End();

#if WIN32
    // Desactivar la resolucion del timer
    timeEndPeriod(1);
#endif

    Epopeia_Print("\n");
    Epopeia_Print(DemoGetEndMessage());
    Epopeia_Print("\n");
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_RegisterBuilinTypes(void)
{

}

//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_GetResolution(int* width, int* height, int* bpp, int* windowed)
{
    *width = Width;
    *height= Height;
    *bpp   = Bpp;
    *windowed = Windowed;
}
//-----------------------------------------------------------------------------
EPOPEIA_API unsigned int Epopeia_GetResX(void)
{
    return Width;
}
//-----------------------------------------------------------------------------
EPOPEIA_API unsigned int Epopeia_GetResY(void)
{
    return Height;
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_SetOutputFilename(const char *filename)
{
    error_set_output_file(filename);
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_SetWindowed(int w)
{
    Windowed = w;
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_SetResolution(int width, int height, int bpp)
{
    Width  = width;
    Height = height;
    Bpp    = bpp;
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_SetZBuffer(int bits)
{
    ZBuffer = bits;
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_SetStencilBuffer(int bits)
{
    Stencil = bits;
}

//-----------------------------------------------------------------------------
EPOPEIA_API unsigned long Epopeia_GetTimeMs(void)
{
#ifdef WIN32
    return timeGetTime();
#elif defined(LINUX)
    struct timeval tv;
    struct timezone tz;
    
    gettimeofday(&tv, &tz);
    return (tv.tv_sec*1000) + tv.tv_usec/1000;
#endif
}

//-----------------------------------------------------------------------------
EPOPEIA_API TSoundContext* Epopeia_GetSoundContext(void)
{
    static TSoundContext DefaultSC = { stInvalid, { -1, -1 }, 0};
    if(g_Epopeia.Sound == NULL)
        return &DefaultSC;
    else
        return g_Epopeia.Sound->GetSoundContext(g_Epopeia.Sound);
}

//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_DumpClassInfo(FILE* fp)
{
    if(fp == NULL)
        fp = stdout;
    TypeTable_DumpClassInfo(fp);
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_DumpFunctionInfo(FILE* fp)
{
    if(fp == NULL)
        fp = stdout;
    FunctionTable_DumpInfo(fp);
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_SetConsoleOutput(int boolean)
{
    console_output = boolean;
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_SetSongNumber(int n)
{
    if(n>0)
	SongNumber = n;
}
//-----------------------------------------------------------------------------
EPOPEIA_API int Epopeia_GetSongNumber(void)
{
    return SongNumber;
}

//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_DisableMenu()
{
    ShowMenu = 0;
}
//-----------------------------------------------------------------------------
EPOPEIA_API char *Epopeia_GetDemoName()
{
    return DemoGetName();
}
//-----------------------------------------------------------------------------
EPOPEIA_API void Epopeia_Print(const char *string)
{
    if(console_output)
        fputs(string, stderr);
    else
    {
        FILE *fp;
        fp = fopen("epopeia-output.txt", "at");
        if(fp != NULL)
        {
            fputs(string, fp);
            fclose(fp);
        }
    }
}

