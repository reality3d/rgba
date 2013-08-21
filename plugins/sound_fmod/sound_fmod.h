#ifndef SOUND_FMOD_H
#define SOUND_FMOD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <epopeia/pos.h>
#include <epopeia/sound.h>
#ifdef FMODEX
#include "fmod4/fmod.h"
#else
#include <fmod/fmod.h>
#endif
#include <packer/packer.h>

typedef struct TSoundFmod_t
{
    int  (*Init)        (struct TSoundFmod_t* self, int ForceManualConfig);
    void (*End)         (struct TSoundFmod_t* self);
    int  (*ModuleLoad)  (struct TSoundFmod_t* self, char* ModName);
    void (*ModuleUnload)(struct TSoundFmod_t* self);
    void (*ModuleStop)  (struct TSoundFmod_t* self);
    void (*ModulePlay)  (struct TSoundFmod_t* self);
    void (*SetVolume)   (struct TSoundFmod_t* self, unsigned Percent);
    TSoundContext* (*GetSoundContext)(struct TSoundFmod_t* self);
    void (*SetPos)      (struct TSoundFmod_t* self, TPos* Pos);
    void (*Delete)      (struct TSoundFmod_t* self);
    EFileType (*GetSongType)(struct TSoundFmod_t* self);

    // metodos y variables miembro de la implementacion
    void (*SetTimerDelay)      (struct TSoundFmod_t* self, int miliseconds);
    FMUSIC_MODULE *mod;
    int       TickStart;
    EFileType SongTypeKK;
    FSOUND_STREAM* stream;
    int       Playing; // si o no?
    EFileType SongType;
    PFILE*    pf_ogg;
    int       timer_delay;
} TSoundFmod;
 
TSound* Fmod_New(void);


#ifdef __cplusplus
};
#endif

#endif // SOUND_FMOD_H

