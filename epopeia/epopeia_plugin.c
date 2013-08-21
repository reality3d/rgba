#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <packer/packer.h>
#include "epopeia.h"
#include "epopeia_internal.h"
#include "list.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "epopeia_plugin"
#include "debug.h"

#ifdef LINUX
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
static const char* plugin_extension = ".epx";
static const int   plugin_ext_len   = 4;
static const char* default_tmp_dir  = "/tmp";
#elif WIN32
#include <windows.h>
static const char* plugin_extension = ".epl";
static const int   plugin_ext_len   = 4;
static const char* default_tmp_dir  = "c:\\";
#endif

typedef void (*RegisterFunc)(void);
typedef char*(*NameFunc)(void);
typedef int  (*PluginAPIVersionFunc)(void);
typedef EPluginType (*PluginTypeFunc)(void);



static TList* loaded_plugin_list_dir = NULL;
static TList* loaded_plugin_list_pak = NULL;

extern int console_output;

#ifdef LINUX
	#define DIR_SLASH '/'
#elif WIN32
	#define DIR_SLASH '\\'    
#endif


#ifdef LINUX
//---------------------------------------------------------
static void* Epopeia_LoadLibrary(const char *path)
{
    // Activamos el flag RTLD_GLOBAL para que los plugins
    // puedan resolver sus dependencias externas con los
    // simbolos de otros plugins
    // Para eso es necesario NO linkar un plugin con otro
    return dlopen(path, RTLD_LAZY|RTLD_GLOBAL);
}

static void* Epopeia_GetSymbolAddress(void *module, const char *symbol)
{
    return dlsym(module, symbol);
}

static const char* Epopeia_GetDynamicError(void)
{
    return dlerror();
}

#elif WIN32
//---------------------------------------------------------
static void* Epopeia_LoadLibrary(const char *path)
{
    HMODULE hdll;
    hdll = LoadLibrary(path);
    return hdll;
    // antes se hacia esto:
    //     return strdup(FileName);
    // pero HMODULE deberia valer
}

static void* Epopeia_GetSymbolAddress(void *handle, const char *symbol)
{
    return GetProcAddress(handle, symbol);
}

static const char* Epopeia_GetDynamicError(void)
{
    // Un poco capuza, pero bueno para lo que se usa suficiente
    static char ctemp[20];
    sprintf(ctemp, "%ld\n", GetLastError());

    return ctemp;
}
#endif
//---------------------------------------------------------
static void* Epopeia_LoadPlugin(char* FileName)
{
    void* handle;
    RegisterFunc RegisterType;
    NameFunc GetPluginName;
    PluginAPIVersionFunc PluginAPIVersion;
    void *func;
    char ctemp[400];

    DEBUG_msg_str("Cargando plugin: ", FileName);
    handle = Epopeia_LoadLibrary(FileName);
    if(!handle)
    {
	DEBUG_msg_str("Error cargando plugin: ", Epopeia_GetDynamicError());
	DEBUG_msg_str("Error al cargar  ", FileName);
        Epopeia_Print(Epopeia_GetDynamicError());
        Epopeia_Print("\nError al cargar ");
        Epopeia_Print(FileName);
        Epopeia_Print("\n");
        return NULL;
    }


    func = Epopeia_GetSymbolAddress(handle, "Epopeia_PluginAPIVersion");
    if(!func) // Legacy plugin
    {
        RegisterType = (RegisterFunc)Epopeia_GetSymbolAddress(handle, "Epopeia_RegisterType");
        if(!RegisterType)
        {
            Epopeia_Print("Error al intentar registrar el plugin.\n");
            return NULL;
        }

        GetPluginName = (NameFunc)Epopeia_GetSymbolAddress(handle, "Epopeia_PluginName");
        if(!GetPluginName)
        {
            Epopeia_Print("Error al intentar registrar el plugin.\n");
            return NULL;
        }
        sprintf(ctemp, "Plugin: | Legacy   |  Effect  | %s\n", GetPluginName());
        Epopeia_Print(ctemp);

        RegisterType();
        return handle;
    }

    PluginAPIVersion = (PluginAPIVersionFunc)func;
    if(PluginAPIVersion() == 140)
    {
        PluginTypeFunc PluginType;

        PluginType = (PluginTypeFunc)Epopeia_GetSymbolAddress(handle, "Epopeia_PluginType");
        if(!PluginType)
        {
            Epopeia_Print("Error al intentar obtener Epopeia_PluginType.\n");
            return NULL;
        }

        GetPluginName = (NameFunc)Epopeia_GetSymbolAddress(handle, "Epopeia_PluginName");
        if(!GetPluginName)
        {
            Epopeia_Print("Error al intentar obtener Epopeia_PluginName.\n");
            return NULL;
        }

        switch(PluginType())
        {
            GraphicsSystemGetFunc GraphicsSystemGet;
            SoundSystemGetFunc    SoundSystemGet;

        case ptEffect:
            RegisterType = (RegisterFunc)Epopeia_GetSymbolAddress(handle, "Epopeia_RegisterType");
            if(!RegisterType)
            {
                Epopeia_Print("Error al intentar registrar el plugin.\n");
                return NULL;
            }

            sprintf(ctemp, "Plugin: | v1.4.0   |  Effect  | %s\n", GetPluginName());
            Epopeia_Print(ctemp);
            RegisterType();
            return handle;

        case ptMenu:
            sprintf(ctemp, "Plugin: | v1.4.0   |  Menu    | %s\n", GetPluginName());
            Epopeia_Print(ctemp);
            g_Epopeia.MainMenu = (MainMenuFunc)Epopeia_GetSymbolAddress(handle, "Epopeia_LaunchMenu");
            return handle;

        case ptGraphics:
            sprintf(ctemp, "Plugin: | v1.4.0   | Graphics | %s\n", GetPluginName());
            Epopeia_Print(ctemp);
            GraphicsSystemGet = (GraphicsSystemGetFunc)Epopeia_GetSymbolAddress(handle, "Epopeia_GetGraphicsSystem");
            g_Epopeia.Graphics= GraphicsSystemGet();
            g_Epopeia.Keyboard= g_Epopeia.Graphics->GetKeyboardSubsystem(g_Epopeia.Graphics);
            return handle;
            
        case ptSound:
            sprintf(ctemp, "Plugin: | v1.4.0   |  Sound   | %s\n", GetPluginName());
            Epopeia_Print(ctemp);
            SoundSystemGet = (SoundSystemGetFunc)Epopeia_GetSymbolAddress(handle, "Epopeia_GetSoundSystem");
            g_Epopeia.Sound= SoundSystemGet();
            return handle;

        default:
            sprintf(ctemp, "Plugin type not recognized: %d; plugin name: %s\n", PluginType(), GetPluginName());
            Epopeia_Print(ctemp);
            return NULL;
        }

    }
    else
    {
        sprintf(ctemp, "Version de plugin desconocida: %d\n", PluginAPIVersion());
        Epopeia_Print(ctemp);
        return NULL;
    }

}

//---------------------------------------------------------
static void* Epopeia_LoadPluginFromPAK(void* list_element)
{
    char* filename = list_element;
    const char* tmpdir = getenv("TMP");
    char* tmpname, *tmpname2, *tmpname3;
    FILE* fp;
    PFILE* pf;
    int size;
    int count;
    void* buffer;
    void* element;
    int i;

    
#ifdef LINUX

    filename = strdup(list_element);
    for(i=0; i<strlen(filename); i++)
	if(filename[i] == '\\')
        filename[i] = '/';

#endif

    
    DEBUG_msg_str("file: ", filename);
    if(tmpdir == NULL)
        tmpdir = default_tmp_dir;

    pf = pak_open(filename, "rb");
    if(pf == NULL)
    {
        return NULL;
    }
    size = pf->size;
    
    DEBUG_msg_dec("Buffer size: ", size);
    buffer = malloc(size);
    if(buffer == NULL)
    {
        return NULL;
    }

    // Leer EPL
    count = pak_read(buffer, 1, size, pf);
    if(count != size)
    {
        pak_close(pf);
        free(buffer);
        return NULL;
    }
    pak_close(pf);

    // --- Fin del trabajo con el PACKER
    
    // Escribir EPL
    
#ifdef LINUX
    tmpname = tempnam(tmpdir, "epl");
#elif WIN32
    tmpname = _tempnam(tmpdir, "epl");
#endif
/*    tmpname2= (char*)malloc(strlen(tmpname)+4+1);
    strcpy(tmpname2, tmpname);
    strcat(tmpname2, ".so");
    free(tmpname);
    tmpname=tmpname2;
    tmpname2=NULL;
*/
    for(i=strlen(filename)-1; i>0 && filename[i] != DIR_SLASH; i--)
    	;
    	
    if(filename[i] == DIR_SLASH)
    	i++;
    	
    tmpname2 = strdup(&filename[i]);
    for(i=strlen(tmpname)-1; i>0 && tmpname[i] != DIR_SLASH; i--)
    	;
    if(tmpname[i] == DIR_SLASH)
    	i++;
    tmpname3	= malloc(strlen(tmpdir)+1+strlen(tmpname2)+1);    
    sprintf(tmpname3, "%s%c%s", tmpdir, DIR_SLASH, tmpname2);
    free(tmpname);
    free(tmpname2);
    tmpname = tmpname3;
    tmpname3 = NULL;
    
    DEBUG_msg_str("Tmp filename: ", tmpname);
    fp = fopen(tmpname, "wb");
    if(fp == NULL)
    {
        free(buffer);
        free(tmpname);
        return NULL;
    }

    count = fwrite(buffer, 1, size, fp);
    if(count != size)
    {
        free(tmpname);
        free(buffer);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    free(buffer);

    element = Epopeia_LoadPlugin(tmpname);
#ifdef LINUX
    remove(tmpname);
#endif
    free(tmpname);
    return element;
}

//---------------------------------------------------------
static void* Epopeia_LoadPluginFromDirectory(void* list_element)
{
    char* filename = list_element;
    return Epopeia_LoadPlugin(filename);
}
#ifdef LINUX
//---------------------------------------------------------
static int Epopeia_UnloadPluginFromPAK(void* list_element)
{
    void* module = list_element;

    dlclose(module);
    return 0;
}

#elif WIN32
//---------------------------------------------------------
static int Epopeia_UnloadPluginFromPAK(void* list_element)
{
    char* filename = list_element;

    FreeLibrary(GetModuleHandle(filename));
    remove(filename);
    free(filename);
    return 0;
}
#endif

#ifdef LINUX
//---------------------------------------------------------
static int Epopeia_UnloadPluginFromDirectory(void* list_element)
{
    void* module = list_element;

    dlclose(module);
    return 0;
}

#elif WIN32
//---------------------------------------------------------
static int Epopeia_UnloadPluginFromDirectory(void* list_element)
{
    char* filename = list_element;

    FreeLibrary(GetModuleHandle(filename));
    // No hay que borrarlo, no es un fichero temporal :)
//    remove(filename);
    free(filename);
    return 0;
}
#endif

//---------------------------------------------------------
static int Epopeia_UnloadPluginsGeneric(TList* loaded_plugin_list,
                                        int (*UnloadPlugin)(void* list_element))
{
    List_Reset(loaded_plugin_list);
    while(!List_EOF(loaded_plugin_list))
    {
        UnloadPlugin(List_Current(loaded_plugin_list));
        List_DropCurrent(loaded_plugin_list);
    }
    List_Delete(loaded_plugin_list);
    return 0;
}

//---------------------------------------------------------
static TList* Epopeia_GetPluginListFromPAK(const char* dirname)
{
    int hay_ficheros = 0;
    TList* plugin_list;

    plugin_list = List_New(CompareStringPtr);
    if(plugin_list == NULL)
        return NULL;

    hay_ficheros = pak_list_reset();
    while(hay_ficheros)
    {
        char* filename = pak_list_current_filename();
        DEBUG_msg_str("file: ", filename);

        if(strlen(filename) > 4 &&
           0 == strncmp(filename+strlen(filename)-plugin_ext_len, plugin_extension, plugin_ext_len))
        {
            List_Insert(plugin_list, strdup(filename));
        }
        hay_ficheros = pak_list_next();
    }

    return plugin_list;
}
//---------------------------------------------------------
#ifdef LINUX
static TList* Epopeia_GetPluginListFromDirectory(const char* dirname)
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
    
    DEBUG_msg_str("Name: ", FileData->d_name);
 
    for(;;)
    {
        char *filename = FileData->d_name;
        if(strlen(filename) > plugin_ext_len &&
           0 == strncmp(filename+strlen(filename)-plugin_ext_len, plugin_extension, plugin_ext_len))
	{
	    strcpy(szNewPath, dirname);
	    strcat(szNewPath, FileData->d_name);
	    DEBUG_msg_str("Name: ", FileData->d_name);

            List_Insert(file_list, strdup(szNewPath));
            DEBUG_msg("Back from List insert");
	}
        if (!(FileData = readdir(dir)))
        {
                closedir(dir);
                return file_list;
        }
     	DEBUG_msg_str("Name: ", FileData->d_name);
    }
}
#elif WIN32
//---------------------------------------------------------
static TList* Epopeia_GetPluginListFromDirectory(const char* dirname)
{
    WIN32_FIND_DATA FileData;
    HANDLE hSearch;
    const char* szDirPath = dirname;
    char szNewPath[2000];
    char* search_pattern;
    TList* plugin_list;

    DEBUG_msg("No casques!");
    plugin_list = List_New(CompareStringPtr);
    if(plugin_list == NULL)
        return NULL;

    search_pattern = malloc(strlen(dirname)+plugin_ext_len+2);
    sprintf(search_pattern, "%s*%s", dirname, plugin_extension);
    // por ejemplo: "plugin\\*.epl"

	DEBUG_msg_str("Buscando plugins en ", search_pattern);
    hSearch = FindFirstFile(search_pattern, &FileData);
    if (hSearch == INVALID_HANDLE_VALUE)
    {
        free(search_pattern);
        List_Delete(plugin_list);
        DEBUG_msg("FindFirst ha devuelto INVALID_HANDLE");
        return NULL;
    }
    DEBUG_msg("-2");
    for(;;)
    {
        lstrcpy(szNewPath, szDirPath);
        lstrcat(szNewPath, FileData.cFileName);
        List_Insert(plugin_list, strdup(szNewPath));
        if (!FindNextFile(hSearch, &FileData))
        {
            if (GetLastError() == ERROR_NO_MORE_FILES)
            {
                DEBUG_msg("Saliendo");
                FindClose(hSearch);
                free(search_pattern);
                return plugin_list;
            }
            else
            {
                DEBUG_msg("Error al hacer FindNextFile!");
                FindClose(hSearch);
                free(search_pattern);
                return plugin_list;
            }
        }
    }
}
#endif
//---------------------------------------------------------
static int Epopeia_LoadPluginsGeneric(const char* path_with_slash, TList* loaded_plugin_list,
                                      TList* (*GetPluginList)(const char* path),
                                      void* (*LoadPlugin)(void* list_element))
{
    TList* plugin_list;
    int count;

    plugin_list = GetPluginList(path_with_slash);
    if(plugin_list == NULL)
        return 0;
    DEBUG_msg("1");
    do {
        count = 0;
        
        List_Reset(plugin_list);
        while(!List_EOF(plugin_list))
        {
            void *module;
            char *filename = List_Current(plugin_list);
            module = LoadPlugin(filename);
            if(module != NULL)
            {
                DEBUG_msg_str("Plugin cargado: ", filename);
		free(filename);
		List_DropCurrent(plugin_list);
                count++;
                List_Insert(loaded_plugin_list, module);
	    }
	    else
	    {
		List_Next(plugin_list);
	    }

        }
    } while(count > 0);
    DEBUG_msg("2");
    count = List_Length(plugin_list);
    if(count != 0)
    {
        char ctemp[400];

    	DEBUG_msg("Error cargando algun plugin");
        sprintf(ctemp, "Error found loading the following %d plugins:\n", count);
        Epopeia_Print(ctemp);
        List_Reset(plugin_list);
        while(!List_EOF(plugin_list))
        {
	    char *filename = List_Current(plugin_list);
            sprintf(ctemp, "\t%s\n", filename);
            Epopeia_Print(ctemp);
            free(filename);
            List_DropCurrent(plugin_list);
        }
    }
    List_Delete(plugin_list);
    return count;
}
//---------------------------------------------------------
EPOPEIA_API int Epopeia_LoadPlugins(void)
{
    int error = 0;
#ifdef LINUX
    const char* plugin_dir = "plugin/";
#elif WIN32
    const char* plugin_dir = "plugin\\";
#endif

/* Esto no parece funcionar bien... ?
 Cambiado a resolucion global RTLD_GLOBAL
 ver mas arriba
#ifdef LINUX
    char *new_ld_path=NULL, *old_ld_path;
    const char *tmpdir;
    old_ld_path = getenv("LD_LIBRARY_PATH");
    tmpdir  = (getenv("TMP") ? getenv("TMP") : default_tmp_dir);
    if(old_ld_path != NULL)
	asprintf(&new_ld_path, "%s:%s", tmpdir, old_ld_path);
    else
	asprintf(&new_ld_path, "%s", tmpdir);
    setenv("LD_LIBRARY_PATH", new_ld_path, 1);
#endif
*/

    DEBUG_msg("Dentro de LoadPlugins");
    loaded_plugin_list_dir = List_New(AlwaysGreater);
    loaded_plugin_list_pak = List_New(AlwaysGreater);
    if(loaded_plugin_list_dir == NULL ||
       loaded_plugin_list_pak == NULL)
        return -1;

    error = Epopeia_LoadPluginsGeneric(plugin_dir, loaded_plugin_list_pak,
                                       Epopeia_GetPluginListFromPAK,
                                       Epopeia_LoadPluginFromPAK);
/*
#ifdef LINUX
    free(new_ld_path);
    if(old_ld_path != NULL)
	setenv("LD_LIBRARY_PATH", old_ld_path, 1);
    else
	unsetenv("LD_LIBRARY_PATH");
#endif
*/
    DEBUG_msg("LoadPluginsGeneric2");
    if(!error)
    {
        error = Epopeia_LoadPluginsGeneric(plugin_dir, loaded_plugin_list_dir,
                                           Epopeia_GetPluginListFromDirectory,
                                           Epopeia_LoadPluginFromDirectory);
        if(error)
        {
            char ctemp[200];
            sprintf(ctemp, "Error al cargar plugins desde DIR (código/count: %d)\n", error);
            Epopeia_Print(ctemp);
        }
    }
    else
    	Epopeia_Print("Error al cargar plugins desde PAK\n");
    DEBUG_msg_dec("LoadPluginsGeneric3, devolvemos ", error);
    return error;
}
//---------------------------------------------------------
EPOPEIA_API int Epopeia_UnLoadPlugins(void)
{
    int error;
    error = Epopeia_UnloadPluginsGeneric(loaded_plugin_list_pak, Epopeia_UnloadPluginFromPAK);
    error = error || Epopeia_UnloadPluginsGeneric(loaded_plugin_list_dir, Epopeia_UnloadPluginFromDirectory);
    return error;
}

