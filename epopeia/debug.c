#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "epopeia.h"

//#define TEXTIO_
   

EPOPEIA_API void
DEBUG_printMsg(char *debug_module, char *file, int line, char *MSG)
{
    FILE *DEBUG_fp=NULL;
    char* tempLine;

    tempLine = malloc(strlen(MSG) + strlen(debug_module) + strlen(file) + 100);
    if(tempLine==NULL)
        tempLine = "DEBUG routine malloc failure!";

    DEBUG_fp=fopen("debug.log", "at");
    sprintf(tempLine, "%5s (%-12s %4d): %s\n", debug_module, file, line, MSG);
    fprintf(DEBUG_fp, tempLine);
    fflush(DEBUG_fp);
    fclose(DEBUG_fp);
    free(tempLine);
}

EPOPEIA_API void
DEBUG_printMsgDec(char *debug_module, char *file, long line, char *MSG, long D)
{
    FILE *DEBUG_fp=NULL;
    char* tempLine;

    tempLine = malloc(strlen(MSG) + strlen(debug_module) + strlen(file) + 100);
    if(tempLine==NULL)
        tempLine = "DEBUG routine malloc failure!";

    DEBUG_fp=fopen("debug.log", "at");
    sprintf(tempLine, "%5s (%-12s %4ld): %s %ld\n", debug_module, file, line, MSG, D);
    fprintf(DEBUG_fp, tempLine);
    fflush(DEBUG_fp);
    fclose(DEBUG_fp);
    free(tempLine);
}

EPOPEIA_API void
DEBUG_printMsgHex(char *debug_module, char *file, long line, char *MSG, long H)
{
    FILE *DEBUG_fp=NULL;
    char* tempLine;

    tempLine = malloc(strlen(MSG) + strlen(debug_module) + strlen(file) + 100);
    if(tempLine==NULL)
        tempLine = "DEBUG routine malloc failure!";

    DEBUG_fp=fopen("debug.log", "at");
    sprintf(tempLine, "%5s (%-12s %4ld): %s %lxh\n", debug_module, file, line, MSG, H);
    fprintf(DEBUG_fp, tempLine);
    fflush(DEBUG_fp);
    fclose(DEBUG_fp);
    free(tempLine);
}

EPOPEIA_API void
DEBUG_printMsgReal(char *debug_module, char *file, long line, char *MSG, double f)
{
    FILE *DEBUG_fp=NULL;
    char* tempLine;

    tempLine = malloc(strlen(MSG) + strlen(debug_module) + strlen(file) + 100);
    if(tempLine==NULL)
        tempLine = "DEBUG routine malloc failure!";

    DEBUG_fp=fopen("debug.log", "at");
    sprintf(tempLine, "%5s (%-12s %4ld): %s %2.5f\n", debug_module, file, line, MSG, f);
    fprintf(DEBUG_fp, tempLine);
    fflush(DEBUG_fp);
    fclose(DEBUG_fp);
    free(tempLine);
}

EPOPEIA_API void
DEBUG_printMsgString(char *debug_module, char *file, long line, char *MSG, char* str)
{
    FILE *DEBUG_fp=NULL;
    char* tempLine;

    tempLine = malloc(strlen(str) + strlen(MSG) + strlen(debug_module) + strlen(file) + 100);
    if(tempLine==NULL)
        tempLine = "DEBUG routine malloc failure!";
    
    DEBUG_fp=fopen("debug.log", "at");
    sprintf(tempLine, "%5s (%-12s %4ld): %s %s\n", debug_module, file, line, MSG, str);
    fprintf(DEBUG_fp, tempLine);
    fflush(DEBUG_fp);
    fclose(DEBUG_fp);
    free(tempLine);
}


#endif
