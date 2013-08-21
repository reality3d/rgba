#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <packer/packer.h>
#include <epopeia/epopeia.h>
#define DEBUG_VAR 0
#define DEBUG_MODULE "fixedss"
#include "../../epopeia/debug.h"

    int  Fixed_Init        (TSound* self, int ForceManualConfig);
    void Fixed_End         (TSound* self);
    int  Fixed_ModuleLoad  (TSound* self, char* ModName);
    void Fixed_ModuleUnload(TSound* self);
    void Fixed_ModuleStop  (TSound* self);
    void Fixed_ModulePlay  (TSound* self);
    void Fixed_SetVolume   (TSound* self, unsigned Percent);
    TSoundContext* Fixed_GetSoundContext(TSound* self);
    void Fixed_SetPos      (TSound* self, TPos* Pos);
    void Fixed_Delete      (TSound* self);
    EFileType Fixed_GetSongType(TSound* self);
    void Fixed_SetTimerDelay(TSound* self, int miliseconds);


    TPos silence_pos = { 0, -10000 };
    static int fixed_jump_ms = 0;
    static int new_frame = 0;

//-------------------------------------------------------------------
TSound* Fixed_New(void)
{
    TSound* self;

    self = malloc(sizeof(TSound));
    assert(self != NULL);

    self->Init = Fixed_Init;
    self->End  = Fixed_End;
    self->ModuleLoad   = Fixed_ModuleLoad;
    self->ModuleUnload = Fixed_ModuleUnload;
    self->ModuleStop   = Fixed_ModuleStop;
    self->ModulePlay   = Fixed_ModulePlay;
    self->SetVolume    = Fixed_SetVolume;
    self->GetSoundContext = Fixed_GetSoundContext;
    self->SetPos       = Fixed_SetPos;
    self->Delete       = Fixed_Delete;
    self->GetSongType  = Fixed_GetSongType;
    silence_pos.Order  = 0;
    silence_pos.Row    = -10000;
    new_frame = 0;
    DEBUG_msg("New Fixed_Sound created");
    return (TSound*)self;
}
//-------------------------------------------------------------------
int  Fixed_Init(TSound* self, int ForceManualConfig)
{
    return 0;
}

//-------------------------------------------------------------------
void Fixed_End(TSound* self)
{
    // Do nothing
}

//-------------------------------------------------------------------
int  Fixed_ModuleLoad(TSound* self, char* ModName)
{
    // Lets put something
    return 0;
}

//-------------------------------------------------------------------
void Fixed_ModuleUnload(TSound* self)
{
    // Do nothing
}

//-------------------------------------------------------------------
void Fixed_ModuleStop(TSound* self)
{
}

//-------------------------------------------------------------------
void Fixed_ModulePlay(TSound* self)
{
    fixed_jump_ms = 0;
}

//-------------------------------------------------------------------
void Fixed_SetVolume(TSound* self, unsigned Percent)
{
    // Do nothing
}

//-------------------------------------------------------------------
TSoundContext* Fixed_GetSoundContext(TSound* self)
{
    static int initialized = 0;
    static TSoundContext SoundContext;
    const int constant_time_increment = 1000/25;
    if(!initialized)
    {
        SoundContext.Tick = 0;
        SoundContext.Pos.Order = silence_pos.Order;
        SoundContext.Pos.Row   = silence_pos.Row;
        initialized = 1;
    }
    if(fixed_jump_ms != 0)
    {
        SoundContext.Tick = fixed_jump_ms;
        fixed_jump_ms     = 0;
    }
    SoundContext.SoundType = stFmod;
        // El tick lo ponemos usando temporizadores de alta resolucion
    // del sistema porque la granularidad del Fmod es muy alta/mala.
    if(new_frame)
	SoundContext.Tick     += constant_time_increment;
        // El row que se usa para la sincro se lo pedimos al Fmod
        // porque si usamos el del sistema el timing del fmod y el del
        // sistema se pueden desfasar y la cagamos :)
    SoundContext.Pos.Order = 0;
    SoundContext.Pos.Row   = SoundContext.Tick;
    new_frame = 0;
    return &SoundContext;
}

//-------------------------------------------------------------------
void Fixed_SetPos(TSound* self, TPos* Pos)
{
    silence_pos.Order = Pos->Order;
    silence_pos.Row   = Pos->Row;
}
//-------------------------------------------------------------------
void Fixed_Delete(TSound* self)
{
    free(self);
}
//-------------------------------------------------------------------
EFileType Fixed_GetSongType(TSound* self)
{
    DEBUG_msg_dec("SongType is: ", self->SongType);
    return stFmod;
}
//-------------------------------------------------------------------
void Fixed_SetTimerDelay(TSound* self, int miliseconds)
{

}
//-------------------------------------------------------------------
EPOPEIA_PLUGIN_API void Fixed_NextFrame(void)
{
    new_frame = 1;
}
