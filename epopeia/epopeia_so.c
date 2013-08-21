#ifdef LINUX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <packer/packer.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include "epopeia.h"
#include "list.h"
#define DEBUG_VAR 0
#define DEBUG_MODULE "epopeia_so"
#include <debug.h>


typedef void (*RegisterFunc)(void);
typedef char*(*NameFunc)(void);

static TList* TmpEplList;

//---------------------------------------------------------
static void *Epopeia_LoadPlugin(char* FileName)
{
    void* module;
    RegisterFunc RegisterType;
    NameFunc GetPluginName;

    module = dlopen(FileName, RTLD_LAZY);
    if(!module)
    {
        Epopeia_Print(dlerror());
        Epopeia_Print("Error al cargar ");
        Epopeia_Print(FileName);
        Epopeia_Print("\n");
        return NULL;
    }

    RegisterType = (RegisterFunc)dlsym(module, "Epopeia_RegisterType");
    if(!RegisterType)
    {
        Epopeia_Print("Error al intentar registrar el plugin.\n");
        return NULL;
    }

    GetPluginName = (NameFunc)dlsym(module, "Epopeia_PluginName");
    if(!GetPluginName)
    {
        Epopeia_Print("Error al intentar registrar el plugin.\n");
        return NULL;
    }

    Epopeia_Print("Dando de alta plugin: ");
    Epopeia_Print(GetPluginName());
    Epopeia_Print("\n");
    RegisterType();
    return module;

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
        void* module;

        DEBUG_msg_str("file: %s\n", filename);

        if(strlen(filename) > 4 &&
           filename[strlen(filename)-1] == 'x' &&
           filename[strlen(filename)-2] == 'p' &&
           filename[strlen(filename)-3] == 'e' &&
           filename[strlen(filename)-4] == '.')
        {
            if(tmpdir == NULL)
                tmpdir = "/tmp";
            DEBUG_msg_dec("Buffer size: ", size);
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
            tmpname = tempnam(tmpdir, "epl");
            tmpname2= (char*)malloc(strlen(tmpname)+4+1);
            strcpy(tmpname2, tmpname);
            strcat(tmpname2, ".so");
            free(tmpname);
            tmpname=tmpname2;
            tmpname2=NULL;
            DEBUG_msg_str("Tmp filename: %s\n", tmpname);
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

            module = Epopeia_LoadPlugin(tmpname);
            remove(tmpname);

            // Alistar para librar y borrar en la descarga
            List_Insert(TmpEplList, module);
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
        void* module;

        module = List_Current(TmpEplList);
        dlclose(module);
//        remove(filename);
//        free(filename);

        List_Next(TmpEplList);
    }
    List_Delete(TmpEplList);
    return 0;
}
//---------------------------------------------------------
TList* Epopeia_GetPluginListFromDirectory(const char* dirname)
{
    DIR *dir;
    struct dirent *FileData;
    char szNewPath[PATH_MAX];
    TList* file_list;

    file_list = List_New(CompareStringPtr);
    if(file_list == NULL)
        return NULL;
    
    dir = opendir(dirname);
    if (dir == NULL)
        return file_list;

    FileData = readdir(dir);
    if (FileData == NULL)
        return file_list;
    
    DEBUG_msg_str("Name: %s\n", FileData->d_name);
 
    for(;;)
    {
        char *filename = FileData->d_name;
        if(strlen(filename) > 4 &&
           filename[strlen(filename)-1] == 'x' &&
           filename[strlen(filename)-2] == 'p' &&
           filename[strlen(filename)-3] == 'e' &&
	   filename[strlen(filename)-4] == '.')
	{
	    strcpy(szNewPath, dirname);
	    strcat(szNewPath, FileData->d_name);
	    DEBUG_msg_str("Name: %s\n", FileData->d_name);

            List_Insert(file_list, strdup(szNewPath));
	}
        if (!(FileData = readdir(dir)))
        {
                closedir(dir);
                return file_list;
        }
     	DEBUG_msg_str("Name: %s\n", FileData->d_name);
    }
}

//---------------------------------------------------------
static int Epopeia_LoadPluginsFromDirectory(void)
{
    DIR *dir;
    struct dirent *FileData;
    char szDirPath[] = "plugin/";
    char szNewPath[PATH_MAX];
 
    dir = opendir("plugin/");
    if (dir == NULL)
        return 0;

    FileData = readdir(dir);
    if (FileData == NULL)
        return 0;
    DEBUG_msg_str("Name: %s\n", FileData->d_name);
 
    for(;;)
    {
	void *module;
        char *filename = FileData->d_name;
        if(strlen(filename) > 4 &&
           filename[strlen(filename)-1] == 'x' &&
           filename[strlen(filename)-2] == 'p' &&
           filename[strlen(filename)-3] == 'e' &&
	   filename[strlen(filename)-4] == '.')
	{
	    strcpy(szNewPath, szDirPath);
	    strcat(szNewPath, FileData->d_name);
	    DEBUG_msg_str("Name: %s\n", FileData->d_name);

	    module = Epopeia_LoadPlugin(szNewPath);
	    if(module == NULL)
		return -1;
	}
        if (!(FileData = readdir(dir)))
        {
                closedir(dir);
                return 0;
        }
     	DEBUG_msg_str("Name: %s\n", FileData->d_name);

    }
}
//---------------------------------------------------------
EPOPEIA_API int Epopeia_LoadPlugins(void)
{
    int error = 0;
    error = Epopeia_LoadPluginsFromPAK();
    if(!error)
    {
        error = Epopeia_LoadPluginsFromDirectory();
    }
    return error;
}
//---------------------------------------------------------
EPOPEIA_API int Epopeia_UnLoadPlugins(void)
{
    int error;
    error = Epopeia_UnLoadPluginsFromPAK();
    return error;
}

#endif //LINUX
