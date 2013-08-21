#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#include <packer/packer.h>
#include "epopeia.h"
#include "list.h"


typedef void (*RegisterFunc)(void);
typedef char*(*NameFunc)(void);

static TList* TmpEplList;
extern int console_output;

//---------------------------------------------------------
static int Epopeia_LoadPlugin(char* FileName)
{
    HINSTANCE hdll;
    RegisterFunc RegisterType;
    NameFunc GetPluginName;
    UINT Mode;

    Mode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

    hdll = LoadLibrary(FileName);
    if(!hdll)
    {
        char ctemp[400];
        sprintf(ctemp, "Error al cargar %s.\n", FileName);
        Epopeia_Print(ctemp);
        return -1;
    }

    RegisterType = (RegisterFunc)GetProcAddress(hdll, "Epopeia_RegisterType");
    if(!RegisterType)
    {
        Epopeia_Print("Error al intentar registrar el plugin.\n");
        return -2;
    }

    GetPluginName = (NameFunc)GetProcAddress(hdll, "Epopeia_PluginName");
    if(!GetPluginName)
    {
        Epopeia_Print("Error al intentar registrar el plugin.\n");
        return -2;
    }

    Epopeia_Print("Dando de alta plugin: ");
    Epopeia_Print(GetPluginName());
    Epopeia_Print("\n");

    SetErrorMode(Mode);

    RegisterType();

    return 0;
}

//---------------------------------------------------------
static int Epopeia_LoadPluginsFromPAK(void)
{
    int hay_ficheros = 0;

    TmpEplList = List_New(AlwaysLesser);
    hay_ficheros = pak_list_reset();

    while(hay_ficheros)
    {
        char* filename = pak_list_current_filename();
        int size = pak_list_current_size();
        char* tmpdir = getenv("TMP");
        char* tmpname, *tmpname2;
        FILE* fp;
        PFILE* pf;
        int count;
        void* buffer;


        if(strlen(filename) > 4 &&
           filename[strlen(filename)-1] == 'l' &&
           filename[strlen(filename)-2] == 'p' &&
           filename[strlen(filename)-3] == 'e' &&
           filename[strlen(filename)-4] == '.')
        {
            if(tmpdir == NULL)
                tmpdir = "c:\\";

            buffer = malloc(size);
            if(buffer == NULL)
            {
                return 1;
            }

            // Leer EPL
            pf = pak_open(filename, "rb");
            if(pf == NULL)
            {
                return 2;
            }

            count = pak_read(buffer, 1, size, pf);
            if(count != size)
            {
                pak_close(pf);
                free(buffer);
                return 3;
            }
            pak_close(pf);


            // Escribir EPL
            tmpname = _tempnam(tmpdir, "epl");
            tmpname2= (char*)malloc(strlen(tmpname)+4+1);
            strcpy(tmpname2, tmpname);
            strcat(tmpname2, ".epl");
            free(tmpname);
            tmpname=tmpname2;
            tmpname2=NULL;
            fp = fopen(tmpname, "wb");
            if(fp == NULL)
            {
                free(buffer);
                free(tmpname);
                return 4;
            }

            count = fwrite(buffer, 1, size, fp);
            if(count != size)
            {
                free(tmpname);
                fclose(fp);
                pak_close(pf);
                free(buffer);
                return 5;
            }
            fclose(fp);
            free(buffer);

            Epopeia_LoadPlugin(tmpname);
            //rename(tmpname, "epl99999.epl");
            //remove("epo99999.epl");

            // Alistar para librar y borrar en la descarga
            List_Insert(TmpEplList, tmpname);
//            unlink(tmpname);
//            free(tmpname);
        }
        hay_ficheros = pak_list_next();
    }
    return 0;
}
//---------------------------------------------------------
static int Epopeia_UnLoadPluginsFromPAK(void)
{
    List_Reset(TmpEplList);
    while(!List_EOF(TmpEplList))
    {
        char* filename;

        filename= List_Current(TmpEplList);
        FreeLibrary(GetModuleHandle(filename));
        remove(filename);
        free(filename);

        List_Next(TmpEplList);
    }
    List_Delete(TmpEplList);
    return 0;
}
//---------------------------------------------------------
static int Epopeia_LoadPluginsFromDirectory(void)
{
    WIN32_FIND_DATA FileData;
    HANDLE hSearch;
    char szDirPath[] = "plugin\\";
    char szNewPath[MAX_PATH];
 

    hSearch = FindFirstFile("plugin\\*.epl", &FileData);
    if (hSearch == INVALID_HANDLE_VALUE)
        return 0;
 
    for(;;)
    {
	int error;

        lstrcpy(szNewPath, szDirPath);
        lstrcat(szNewPath, FileData.cFileName);

        error = Epopeia_LoadPlugin(szNewPath);
        if(error)
            return error;

        if (!FindNextFile(hSearch, &FileData))
        {
            if (GetLastError() == ERROR_NO_MORE_FILES)
            {
                FindClose(hSearch);
                return 0;
            }
            else
            {
                FindClose(hSearch);
                return 1;
            }
        }
    }

}
//---------------------------------------------------------
EPOPEIA_API int Epopeia_LoadPlugins(void)
{
    int error = 0;
    error = Epopeia_LoadPluginsFromPAK();
    if(!error)
        error = Epopeia_LoadPluginsFromDirectory();
    return error;
}
//---------------------------------------------------------
EPOPEIA_API int Epopeia_UnLoadPlugins(void)
{
    int error;
    error = Epopeia_UnLoadPluginsFromPAK();
    return error;
}

#endif //WIN32
