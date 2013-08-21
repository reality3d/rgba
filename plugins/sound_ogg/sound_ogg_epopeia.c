#include <epopeia/epopeia_system.h>

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>

#include "sound_ogg.h"


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
    return "Ogg Vorbis sound subsystem v1.0 by Enlar";
}

EPOPEIA_PLUGIN_API TSound *Epopeia_GetSoundSystem(void)
{
    TSound *r;

    r = (TSound*)Ogg_New();
    return r;
}

