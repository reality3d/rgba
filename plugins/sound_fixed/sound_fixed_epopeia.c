#include <epopeia/epopeia_system.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>

extern TSound *Fixed_New();

EPOPEIA_PLUGIN_API int Epopeia_PluginAPIVersion(void)
{
    return 140;
}


EPOPEIA_PLUGIN_API EPluginType Epopeia_PluginType(void)
{
    return ptSound;
}

EPOPEIA_PLUGIN_API char* Epopeia_PluginName(void)
{
    return "Fixed framerate sound subsystem by Enlar";
}

EPOPEIA_PLUGIN_API TSound *Epopeia_GetSoundSystem(void)
{
    TSound *r;

    r = (TSound*)Fixed_New();
    return r;
}

