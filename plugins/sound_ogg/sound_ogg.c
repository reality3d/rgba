#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <packer/packer.h>
#include <epopeia/epopeia.h>
#include <pthread.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include "sound_ogg.h"
#include "oss_out.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "sound_ogg"
#include "../../epopeia/debug.h"


    int  Ogg_Init        (TSoundOgg* self, int ForceManualConfig);
    void Ogg_End         (TSoundOgg* self);
    int  Ogg_ModuleLoad  (TSoundOgg* self, char* ModName);
    void Ogg_ModuleUnload(TSoundOgg* self);
    void Ogg_ModuleStop  (TSoundOgg* self);
    void Ogg_ModulePlay  (TSoundOgg* self);
    void Ogg_SetVolume   (TSoundOgg* self, unsigned Percent);
    TSoundContext* Ogg_GetSoundContext(TSoundOgg* self);
    void Ogg_SetPos      (TSoundOgg* self, TPos* Pos);
    void Ogg_Delete      (TSoundOgg* self);
    EFileType Ogg_GetSongType(TSoundOgg* self);
    void Ogg_SetTimerDelay(TSoundOgg* self, int miliseconds);

static void         Ogg_FileClose(void *handle);
static void *       Ogg_FileOpen(const char *name);
static int          Ogg_FileRead(void *buffer, size_t size, size_t nmemb, void *handle);
static int          Ogg_FileSeek(void *handle, ogg_int64_t pos, int mode);
static int          Ogg_FileTell(void *handle);


static     TPos silence_pos = { 0, -10000 };

//-------------------------------------------------------------------
TSound* Ogg_New(void)
{
    TSoundOgg* self;

    self = malloc(sizeof(TSoundOgg));
    assert(self != NULL);

    self->Init = Ogg_Init;
    self->End  = Ogg_End;
    self->ModuleLoad   = Ogg_ModuleLoad;
    self->ModuleUnload = Ogg_ModuleUnload;
    self->ModuleStop   = Ogg_ModuleStop;
    self->ModulePlay   = Ogg_ModulePlay;
    self->SetVolume    = Ogg_SetVolume;
    self->GetSoundContext = Ogg_GetSoundContext;
    self->SetPos       = Ogg_SetPos;
    self->Delete       = Ogg_Delete;
    self->GetSongType  = Ogg_GetSongType;
    self->SetTimerDelay= Ogg_SetTimerDelay;
    self->TickStart    = 0;
    self->SongType     = 0;
    self->Playing      = 0;
    silence_pos.Order  = 0;
    silence_pos.Row    = -10000;
    self->timer_delay  = 0;
    DEBUG_msg("New Ogg_Sound created");
    return (TSound*)self;
}
//-------------------------------------------------------------------
int  Ogg_Init(TSoundOgg* self, int ForceManualConfig)
{
    OSS_OUT_Init();
    DEBUG_msg("Ogg_Sound initialized");
    // Establecemos el delay que nos diga el sistema
    return 0;
}

//-------------------------------------------------------------------
void Ogg_End(TSoundOgg* self)
{
    OSS_OUT_Deinit();
}

//-------------------------------------------------------------------
int  Ogg_ModuleLoad(TSoundOgg* self, char* ModName)
{
#if WIN32
#include <string.h>
#define strcasecmp _stricmp
#endif

    DEBUG_msg("ModuleLoad...");
    self->SongType = ftOgg;

    ov_callbacks ovc;
    ovc.read_func = Ogg_FileRead;
    ovc.seek_func = Ogg_FileSeek;
    ovc.close_func= Ogg_FileClose;
    ovc.tell_func = Ogg_FileTell;

    if(ov_open_callbacks(Ogg_FileOpen(ModName), &self->vf, NULL, 0, ovc) < 0)
    {
	fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
	printf("Error al cargar ogg!\n");
	DEBUG_msg("Error al cargar ogg");
	return 1;
    }

    DEBUG_msg("WavfileLoaded");
    DEBUG_msg_dec("SongType is: ", self->SongType);
    return 0;
}

//-------------------------------------------------------------------
void Ogg_ModuleUnload(TSoundOgg* self)
{
    ov_clear(&self->vf);
}

//-------------------------------------------------------------------
void Ogg_ModuleStop(TSoundOgg* self)
{
    self->Playing = 0;
}

//-------------------------------------------------------------------
void *Ogg_ModuleDecodeLoop(void *p)
{
    TSoundOgg* self = (TSoundOgg*)p;
    int eof = 0;
    int current_section;
    int read_bytes = 0;
    int step_cur = 0;

    sem_wait(&self->empty_steps);
    sem_wait(&self->empty_steps);
    while(!eof)
    {
	if(step_cur < (read_bytes/OGG_STEP_SIZE))
	{
            step_cur++;
	    sem_wait(&self->empty_steps);
	    // Ajustar buffer circular si tocar rellenar primer step
            if(read_bytes % OGG_BUFFER_SIZE < OGG_STEP_SIZE)
		memcpy(self->pcmout, self->pcmout + OGG_BUFFER_SIZE, read_bytes % OGG_BUFFER_SIZE);
	}
	long ret=ov_read(&self->vf,self->pcmout + (read_bytes%OGG_BUFFER_SIZE),OGG_STEP_SIZE,0,2,1,&current_section);
	if (ret == 0) {
	/* EOF */
	    eof=1;
	} else if (ret < 0) {
	    /* error in the stream.  Not a problem, just reporting it in
	     case we (the app) cares.  In this case, we don't. */
	} else {
	    /* we don't bother dealing with sample rate changes, etc, but
	     you'll have to*/
            //printf("leido step offset %d, ret %ld\n", (read_bytes%OGG_BUFFER_SIZE), ret);
	    read_bytes += ret;
	    if(step_cur < (read_bytes/OGG_STEP_SIZE))
	    {
		sem_post(&self->full_steps);
	    }
	}
    }
    self->Eof = 1;
    return NULL;
}
//-------------------------------------------------------------------
void *Ogg_ModuleOutLoop(void *p)
{
    TSoundOgg* self = (TSoundOgg*)p;
    int eof = 0;
    int firstloop = 1;

    self->ReadPosition = 0;
    while(!eof)
    {
	int i;
	if(0 != sem_trywait(&self->full_steps))
	{
	    if(self->Eof)
	    {
		eof = 1;
		continue;
	    }
	    sem_wait(&self->full_steps);
	}
	if(firstloop)
	{
	    self->TickStart = Epopeia_GetTimeMs();
            firstloop = 0;
	}
	for(i=0; i<40*9; i++)
	{
	    OSS_OUT_Write(self->pcmout + (self->ReadPosition % OGG_BUFFER_SIZE), OGG_STEP_SIZE/(40*9));
	    self->ReadPosition += (OGG_STEP_SIZE/(40*9));
	}
	sem_post(&self->empty_steps);
    }
    return NULL;
}
//-------------------------------------------------------------------
void Ogg_ModulePlay(TSoundOgg* self)
{
    pthread_t t, t2;
    if(self->Playing)
	return;
    self->Eof = 0;
    sem_init (&self->empty_steps, 0, 5);
    sem_init (&self->full_steps, 0, 0);
    pthread_create(&t, NULL, Ogg_ModuleDecodeLoop, self);
    pthread_create(&t2, NULL, Ogg_ModuleOutLoop, self);
    self->Playing = 1;
}
//-------------------------------------------------------------------
void Ogg_SetVolume(TSoundOgg* self, unsigned Percent)
{
    // nada
}

//-------------------------------------------------------------------
TSoundContext* Ogg_GetSoundContext(TSoundOgg* self)
{
    static TSoundContext SoundContext;
    int ms;

    if(!self->Playing)
    {
        SoundContext.Tick = 0;
        SoundContext.Pos.Order = silence_pos.Order;
        SoundContext.Pos.Row   = silence_pos.Row;
        return &SoundContext;
    }

    SoundContext.SoundType = stInvalid;
    double factor = 1000.0/(44100*2*2);
    ms = (self->ReadPosition-OSS_OUT_GetDelay())*factor;
    if(ms < 0)
	ms = 0;
    SoundContext.Tick      = Epopeia_GetTimeMs() - self->TickStart;
//    SoundContext.Tick      = ms;// - self->TickStart;
    SoundContext.Pos.Order = 0;
    SoundContext.Pos.Row   = ms - self->timer_delay; // ajuste "a mano" opcional
    return &SoundContext;
}

//-------------------------------------------------------------------
void Ogg_SetPos(TSoundOgg* self, TPos* Pos)
{
    int error;
    if(!self->Playing)
    {
        silence_pos.Order = Pos->Order;
        silence_pos.Row   = Pos->Row;
        return;
    }

    printf("Seek to %f\n", Pos->Row/1000.0);
    error = ov_time_seek(&self->vf, Pos->Row/1000.0);
    if(error)
        printf("Ogg vorbis seek error %d\n", error);
    // No se puede cambiar el row
}
//-------------------------------------------------------------------
void Ogg_Delete(TSoundOgg* self)
{
    free(self);
}
//-------------------------------------------------------------------
EFileType Ogg_GetSongType(TSoundOgg* self)
{
    return ftOgg;
}
//-------------------------------------------------------------------
void Ogg_SetTimerDelay(TSoundOgg* self, int miliseconds)
{
    self->timer_delay = miliseconds;
}


//-------------------------------------------------------------------
static void         Ogg_FileClose(void *handle)
{
    pak_close(handle);
}
//-------------------------------------------------------------------
static void * Ogg_FileOpen(const char *name)
{
    return pak_open(name, "rb");
}
//-------------------------------------------------------------------
static int          Ogg_FileRead(void *buffer, size_t size, size_t nmemb, void *handle)
{
    return pak_read(buffer, size, nmemb, handle);
}
//-------------------------------------------------------------------
static int          Ogg_FileSeek(void *handle, ogg_int64_t pos, int mode)
{
    return pak_fseek(handle, pos, mode);
}
//-------------------------------------------------------------------
static int          Ogg_FileTell(void *handle)
{
    return pak_ftell(handle);
}
//-------------------------------------------------------------------
