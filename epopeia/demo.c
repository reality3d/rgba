#include <string.h>

static struct
{
    char *Name;
    char *StartMessage;
    char *EndMessage;
    char *ModFilename;
    
} Demo;

//-------------------------------------------------------------------
void DemoSetName(char *Name)
{
    Demo.Name = strdup(Name);
}

//-------------------------------------------------------------------
char* DemoGetName(void)
{
    return strdup(Demo.Name);
}

//-------------------------------------------------------------------
void DemoSetStartMessage(char *Msg)
{
    Demo.StartMessage = strdup(Msg);
}
//-------------------------------------------------------------------
char*DemoGetStartMessage(void)
{
    return Demo.StartMessage;
}

//-------------------------------------------------------------------
void DemoSetEndMessage(char *Msg)
{
    Demo.EndMessage = strdup(Msg);
}
//-------------------------------------------------------------------
char*DemoGetEndMessage(void)
{
    return Demo.EndMessage;
}

//-------------------------------------------------------------------
void DemoSetModFile(char *Filename)
{
    Demo.ModFilename = strdup(Filename);
}

//-------------------------------------------------------------------
char *DemoGetModFile(void)
{
    return Demo.ModFilename;
}

