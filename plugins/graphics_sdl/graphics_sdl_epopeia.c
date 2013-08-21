#include <epopeia/epopeia_system.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>

#include "graphics_sdl.h"


EPOPEIA_PLUGIN_API int Epopeia_PluginAPIVersion(void)
{
    return 140;
}


EPOPEIA_PLUGIN_API EPluginType Epopeia_PluginType(void)
{
    return ptGraphics;
}

EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return "SDL graphics subsystem by Enlar";
}

EPOPEIA_PLUGIN_API TGraphics *Epopeia_GetGraphicsSystem(void)
{
    TGraphics *r;

    r = (TGraphics*)Graphics_Sdl_New();
    return r;
}

