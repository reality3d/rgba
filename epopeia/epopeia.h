#ifndef EPOPEIA_H
#define EPOPEIA_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
  #ifdef EPOPEIA_EXPORTS
    #define EPOPEIA_API __declspec(dllexport)
    #define EPOPEIA_API_VAR __declspec(dllexport)
  #else
    #define EPOPEIA_API __declspec(dllimport)
    #define EPOPEIA_API_VAR __declspec(dllimport)
  #endif
  #define EPOPEIA_PLUGIN_API __declspec(dllexport)
  #define strcasecmp _stricmp
#elif defined(LINUX)
  #define EPOPEIA_API extern
  #define EPOPEIA_API_VAR
  #define EPOPEIA_PLUGIN_API
  #define stricmp strcasecmp
#else
  #error "Sistema no definido"
#endif


#include <string.h>
#include <stdio.h>
#include "graphics.h"
#include "sound.h"
#include "keyboard.h"

    EPOPEIA_API int  Epopeia_ScriptInit(void);
    EPOPEIA_API int  Epopeia_LoadPlugins(void);
    EPOPEIA_API int  Epopeia_UnLoadPlugins(void);
    EPOPEIA_API int  Epopeia_ScriptCompile(char* ScriptFilename);
    EPOPEIA_API int  Epopeia_Init(void);
    EPOPEIA_API int  Epopeia_Load(void);
    EPOPEIA_API void Epopeia_RegisterBuilinTypes(void);
    EPOPEIA_API int  Epopeia_Play(void);
    EPOPEIA_API void Epopeia_Unload(void);
    EPOPEIA_API void Epopeia_End(void);
    EPOPEIA_API void Epopeia_SetOutputFilename(const char *filename);
    EPOPEIA_API void Epopeia_SetWindowed(int w); // 0 / 1
    EPOPEIA_API void Epopeia_SetResolution(int width, int height, int bpp);
    EPOPEIA_API void Epopeia_GetResolution(int* width, int* height, int* bpp, int* windowed);
    EPOPEIA_API void Epopeia_SetZBuffer(int bits);
    EPOPEIA_API void Epopeia_SetStencilBuffer(int bits);
    EPOPEIA_API unsigned int Epopeia_GetResX(void);
    EPOPEIA_API unsigned int Epopeia_GetResY(void);
    EPOPEIA_API unsigned long Epopeia_GetTimeMs(void);
    EPOPEIA_API TSoundContext* Epopeia_GetSoundContext(void);
    EPOPEIA_API void Epopeia_DumpClassInfo(FILE* fp);
    EPOPEIA_API void Epopeia_DumpFunctionInfo(FILE* fp);
    EPOPEIA_API void Epopeia_SetConsoleOutput(int boolean);
    EPOPEIA_API void Epopeia_SetSongNumber(int n); // default is 1
    EPOPEIA_API int  Epopeia_GetSongNumber(void);
    EPOPEIA_API void Epopeia_DisableMenu(void);
    EPOPEIA_API char *Epopeia_GetDemoName(void);
    EPOPEIA_API void Epopeia_Print(const char *string);

    typedef int (*MainMenuFunc)(const char *demoname, int *width, int *height, int *windowed);
    typedef TGraphics *(*GraphicsSystemGetFunc)(void);
    typedef TSound    *(*SoundSystemGetFunc)(void);
    typedef TKeyboard *(*KeyboardSystemGetFunc)(TGraphics*);

    typedef enum { ptEffect = 1, ptMenu = 2, ptGraphics = 3, ptSound = 4 } EPluginType;

    EPOPEIA_API const char* EpopeiaVersionString;


#ifdef __cplusplus
};
#endif

#endif
