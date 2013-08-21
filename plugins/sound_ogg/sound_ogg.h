#ifndef SOUND_OGG_H
#define SOUND_OGG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <epopeia/pos.h>
#include <epopeia/sound.h>
#include <packer/packer.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <semaphore.h>

#define OGG_BUFFER_SIZE ((44100*2*2)/2)
#define OGG_STEP_SIZE ((44100*2*2)/10)

typedef struct TSoundOgg_t
{
    int  (*Init)        (struct TSoundOgg_t* self, int ForceManualConfig);
    void (*End)         (struct TSoundOgg_t* self);
    int  (*ModuleLoad)  (struct TSoundOgg_t* self, char* ModName);
    void (*ModuleUnload)(struct TSoundOgg_t* self);
    void (*ModuleStop)  (struct TSoundOgg_t* self);
    void (*ModulePlay)  (struct TSoundOgg_t* self);
    void (*SetVolume)   (struct TSoundOgg_t* self, unsigned Percent);
    TSoundContext* (*GetSoundContext)(struct TSoundOgg_t* self);
    void (*SetPos)      (struct TSoundOgg_t* self, TPos* Pos);
    void (*Delete)      (struct TSoundOgg_t* self);
    EFileType (*GetSongType)(struct TSoundOgg_t* self);

    // metodos y variables miembro de la implementacion
    void (*SetTimerDelay)      (struct TSoundOgg_t* self, int miliseconds);
    int       TickStart;
    int       Playing; // si o no?
    EFileType SongType;
    int       timer_delay;


    OggVorbis_File vf;
    volatile int   ReadPosition;
    volatile int   Eof;
    sem_t empty_steps;
    sem_t full_steps;
    char pcmout[OGG_BUFFER_SIZE+OGG_STEP_SIZE];
} TSoundOgg;
 
TSound* Ogg_New(void);


#ifdef __cplusplus
};
#endif

#endif // SOUND_OGG_H

