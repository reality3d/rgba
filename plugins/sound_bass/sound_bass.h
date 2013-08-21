#ifndef SOUND_BASS_H
#define SOUND_BASS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <epopeia/pos.h>
#include <epopeia/sound.h>
#include <bass/bass.h>
#include <packer/packer.h>

typedef struct TSoundBass_t
{
    int  (*Init)        (struct TSoundBass_t* self, int ForceManualConfig);
    void (*End)         (struct TSoundBass_t* self);
    int  (*ModuleLoad)  (struct TSoundBass_t* self, char* ModName);
    void (*ModuleUnload)(struct TSoundBass_t* self);
    void (*ModuleStop)  (struct TSoundBass_t* self);
    void (*ModulePlay)  (struct TSoundBass_t* self);
    void (*SetVolume)   (struct TSoundBass_t* self, unsigned Percent);
    TSoundContext* (*GetSoundContext)(struct TSoundBass_t* self);
    void (*SetPos)      (struct TSoundBass_t* self, TPos* Pos);
    void (*Delete)      (struct TSoundBass_t* self);
    EFileType (*GetSongType)(struct TSoundBass_t* self);

    // metodos y variables miembro de la implementacion
    EFileType SongType;
    HMUSIC mod;
    HSTREAM stream;
    int length;
    void *buffer;
    int       TickStart;
    int       Playing; // si o no?

} TSoundBass;
 
TSound* Bass_New(void);


#ifdef __cplusplus
};
#endif

#endif // SOUND_FMOD_H

