#include <epopeia/epopeia_system.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>

#include "dlg.h"

EPOPEIA_PLUGIN_API int Epopeia_PluginAPIVersion(void)
{
    return 140;
}


EPOPEIA_PLUGIN_API EPluginType Epopeia_PluginType(void)
{
    return ptMenu;
}

EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return "GTK 1.2 menu by Epsilum add. code by Enlar";
}

EPOPEIA_PLUGIN_API int Epopeia_LaunchMenu(const char *demoname, int *width, int *height, int *windowed)
{
    TDialogData dd;

    dd.width   = *width;
    dd.height  = *height;
    dd.windowed= *windowed;
    DLG_Show (&dd, demoname);
    *width   = dd.width;
    *height  = dd.height;
    *windowed= dd.windowed;
    return 0;
}

