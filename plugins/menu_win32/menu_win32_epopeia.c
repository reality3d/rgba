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
    return "Win32 menu by Epsilum add. code by Enlar";
}

EPOPEIA_PLUGIN_API int Epopeia_LaunchMenu(const char *demoname, int *width, int *height, int *windowed)
{
    TDialogData dd;
    int ok;

    dd.res_x   = *width;
    dd.res_y   = *height;
    dd.windowed= *windowed;

    ok = DLG_Main (GetModuleHandle("menu_win32.epl"), &dd, demoname);
    if(!ok)
        return 1;

    *width   = dd.res_x;
    *height  = dd.res_y;
    *windowed= dd.windowed;
    return 0;
}

