#ifndef SOUND_H
#define SOUND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pos.h"
    
typedef enum { stMikmod, stFmod, stInvalid } ESoundType;
typedef enum { ftMod = 3, ftOgg = 4 } EFileType;

typedef struct
{
    ESoundType SoundType;
    TPos Pos;
    unsigned long Tick;   // milisegundos desde el comienzo de la cancion
} TSoundContext;

typedef struct TSound_t
{
    int  (*Init)(struct TSound_t* self, int ForceManualConfig);
    void (*End) (struct TSound_t* self);
    int  (*ModuleLoad)  (struct TSound_t* self, char* ModName);
    void (*ModuleUnload)(struct TSound_t* self);
    void (*ModuleStop)(struct TSound_t* self);
    void (*ModulePlay)(struct TSound_t* self);
    void (*SetVolume) (struct TSound_t* self, unsigned Percent);
    TSoundContext* (*GetSoundContext)(struct TSound_t* self);
    void (*SetPos)    (struct TSound_t* self, TPos* Pos);
    void (*Delete)    (struct TSound_t* self);
    EFileType (*GetSongType)(struct TSound_t* self);
    
    // metodos y variables miembro de la implementacion
    
} TSound;

#ifdef __cplusplus
};
#endif

#endif

