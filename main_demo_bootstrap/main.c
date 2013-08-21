#if _WIN32
#include <windows.h>
#include <fcntl.h>
#endif

#include <stdio.h>
#include "../epopeia/epopeia.h"
#include <packer/packer.h>
#include <glt/image.h>
#include <glt/cache.h>

#define NO_CONSOLE_OUTPUT 1

#ifdef NO_CONSOLE_OUTPUT
#define print(x)
#else
#define print(x) printf(x)
#endif

#if defined(WIN32) || defined(MINGW32)
//static HINSTANCE g_hInstance;

extern INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
//    g_hInstance = hInst;
	return main();
}
#endif


int
main(int argc, char *argv[])
{
    int Error, i;
    int volcado_plugins = 0;
    int volcado_funciones = 0;
    
#ifndef NO_CONSOLE_OUTPUT
    printf("<*> Epopeia Scripting Demo System %s\nPowering demos since 2001 by enlar/rgba\n", EpopeiaVersionString);
#endif

    // Opciones de ejecucion
    for(i=1; i<argc; i++)
    {
        if(0 == strcasecmp(argv[i], "-h"))
        {
            printf("Available options:\n\n");
            printf("\t-h                      Show this help\n");
            printf("\t-df --dump-functions    Show defined function list\n");
            printf("\t-dp --dump-plugin-api   Show defined class' information\n");
            printf("\t-e --save-errors        Save script errors to script.err\n");
	    printf("\t-s#                     Choose what song to use as SoundTrack\n");
	    printf("\t-w [0|1]                Run 0=fullscreen 1=windowed\n");
	    printf("\t-x <width> -y <height>  Set demo resolution in pixels\n");
            printf("\t-bpp <bits>             Set demo bits per pixel (16, 24, 32)\n");
            printf("\t--disable-menu          Do not stop with a start-menu. Just play the demo\n");
            exit(0);
        }
        else if(0 == strcmp(argv[i], "-bpp"))
	{
	    if(i+1 < argc)
            {
                int x, y, bpp, w;
                Epopeia_GetResolution(&x, &y, &bpp, &w);
		Epopeia_SetResolution(x, y, atoi(argv[i+1]));
                i++;
	    }
	    else
		printf("No bits per pixel specified.\n");
        }
        else if(0 == strcasecmp(argv[i], "-dp")
                || 0 == strcasecmp(argv[i], "--dump-plugin-api"))
        {
            volcado_plugins = 1;
        }
        else if(0 == strcasecmp(argv[i], "-df")
                || 0 == strcasecmp(argv[i], "--dump-functions"))
        {
            volcado_funciones = 1;
        }
        else if(0 == strcasecmp(argv[i], "-e")
                || 0 == strcasecmp(argv[i], "--save-errors"))
        {
            Epopeia_SetOutputFilename("script.err");
        }
        else if(0 == strncmp(argv[i], "-s", 2))
	{
	    int n;
            n = argv[i][2] - '0';
            if(n>0 && n<10)
		Epopeia_SetSongNumber(n);
	    else
                printf("Song number must be between 1-9\n");
        }
        else if(0 == strcmp(argv[i], "-w"))
	{
	    if(i+1 < argc)
	    {
		Epopeia_SetWindowed(atoi(argv[i+1]) > 0);
                i++;
	    }
	    else
		printf("No window mode specified.\n");
        }
        else if(0 == strcmp(argv[i], "-x"))
	{
	    if(i+1 < argc)
            {
                int x, y, bpp, w;
                Epopeia_GetResolution(&x, &y, &bpp, &w);
		Epopeia_SetResolution(atoi(argv[i+1]), y, bpp);
                i++;
	    }
	    else
		printf("No width specified.\n");
        }
        else if(0 == strcmp(argv[i], "-y"))
	{
	    if(i+1 < argc)
            {
                int x, y, bpp, w;
                Epopeia_GetResolution(&x, &y, &bpp, &w);
		Epopeia_SetResolution(x, atoi(argv[i+1]), bpp);
                i++;
	    }
	    else
		printf("No height specified.\n");
        }
	else if(0 == strcmp(argv[i], "--disable-menu"))
	{
            Epopeia_DisableMenu();
            printf("Start menu disabled.\n");
	}
        else
        {
            printf("Option '%s' not known.\n", argv[i]);
            exit(1);
        }
    }

    print("--- Inicializando packer ---\n");
    pak_init("data.pak");
// GLT: MOVE THIS
    IMG_SetIO (pak_open, pak_read, pak_close);

/*
#ifdef WIN32
    // Main dialog

    if (!DLG_Main (g_hInstance, &dd))
    {
        pak_kill ( );
        return 1;
    }

    Epopeia_SetResolution (dd.res_x, dd.res_y, 32);
    Epopeia_SetWindowed  (dd.windowed);
#elif LINUX
    DLG_Show (&dd, " Euskal Encounter 12 invitation by rgba ");

    //printf ("Resolution: %i x %i\n", dd.width, dd.height);
    //printf ("Windowed  : %i\n", dd.windowed);

    Epopeia_SetResolution (dd.width, dd.height, 24);
    Epopeia_SetWindowed  (dd.windowed);
#endif //WIN32
*/
//    Epopeia_SetConsoleOutput(0);
    print("--- Arrancando Epopeia ---\n");
    Error = Epopeia_ScriptInit();
    if(Error)
    {
        pak_kill();
        return Error;
    }
// GLT: MOVE THIS
    print("--- Inicializando GLT: cache---\n");
    CACHE_Init();
    print("--- Loading plugins ---\n");
    // Insert effect-types here!!!

    //Epopeia_RegisterType();

    Error = Epopeia_LoadPlugins();
    if(Error)
    {
        print("La carga de plugins fallo! Al infierno!!\n");
        pak_kill();
        return -1;
    }
    if(volcado_plugins)
    {
        Epopeia_DumpClassInfo(stderr);
    }
    if(volcado_funciones)
    {
        print("\nEpopeia defined functions:\n");
        Epopeia_DumpFunctionInfo(stderr);
    }
    
    print("--- Compilando script ---\n");
    Error = Epopeia_ScriptCompile("script.epo");
    if(Error)
    {
        pak_kill();
        return Error;
    }
    print(" OK!\n");
    print("--- Inicializando sistemas ---\n");
    Error = Epopeia_Init();
    if(Error)
    {
        pak_kill();
        return Error;
    }

    print("--- Cargando datos y creando objetos ---\n");
    // Crear objetos y cargar musica
    Error = Epopeia_Load();
    if(Error)
    {
        Epopeia_End();
        pak_kill();
        return Error;
    }

    print("--- Reproduciendo demo ---\n");
    Error = Epopeia_Play();
    if(Error)
    {
        printf("Se produjo error #%d reproduciendo demo.\n", Error);
        //DEBUG_msg_dec("Se produjo error reproduciendo demo: #", Error);
        // Seguimos para descargar y cerrar todo.
    }

    print("--- Descargando datos y liberando memoria ---\n");
    Epopeia_Unload();
    print("--- Desinicializando todos los sistemas ---\n");
    Epopeia_End();
    Epopeia_UnLoadPlugins();
    pak_kill();
    // Si hubo algun error, lo devolvemos
    return Error;
}
