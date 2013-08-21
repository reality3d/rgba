#include <epopeia/epopeia_system.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>

#include "sound_fmodex.h"


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
    return "Fmodex sound subsystem by Enlar";
}

EPOPEIA_PLUGIN_API TSound *Epopeia_GetSoundSystem(void)
{
    TSound *r;

    r = (TSound*)Fmod_New();
    return r;
}

