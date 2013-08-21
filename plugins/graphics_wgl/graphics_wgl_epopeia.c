#include <epopeia/epopeia_system.h>

#include <windows.h>
#include <stdio.h>

#include "graphics_wgl.h"


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
    return "WGL/TDS graphics subsystem by Epsilum&Enlar";
}

EPOPEIA_PLUGIN_API TGraphics *Epopeia_GetGraphicsSystem(void)
{
    TGraphics *r;

    r = (TGraphics*)Wgl_New();
    return r;
}

