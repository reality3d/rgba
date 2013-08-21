#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <packer/packer.h>
#include <epopeia/epopeia.h>
#include "fmodex/fmod.h"
#include "fmodex/fmod_errors.h"	// optional
#include "sound_fmodex.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "fmodex"
#include "../../epopeia/debug.h"

    int  Fmod_Init        (TSoundFmod* self, int ForceManualConfig);
    void Fmod_End         (TSoundFmod* self);
    int  Fmod_ModuleLoad  (TSoundFmod* self, char* ModName);
    void Fmod_ModuleUnload(TSoundFmod* self);
    void Fmod_ModuleStop  (TSoundFmod* self);
    void Fmod_ModulePlay  (TSoundFmod* self);
    void Fmod_SetVolume   (TSoundFmod* self, unsigned Percent);
    TSoundContext* Fmod_GetSoundContext(TSoundFmod* self);
    void Fmod_SetPos      (TSoundFmod* self, TPos* Pos);
    void Fmod_Delete      (TSoundFmod* self);
    EFileType Fmod_GetSongType(TSoundFmod* self);
    void Fmod_SetTimerDelay(TSoundFmod* self, int miliseconds);

    static void         Fmod_FileClose(unsigned int handle);
    static unsigned int Fmod_FileOpen(const char *name);
    static int          Fmod_FileRead(void *buffer, int size, unsigned int handle);
    static int          Fmod_FileSeek(unsigned int handle, int pos, signed char mode);
    static int          Fmod_FileTell(unsigned int handle);


#ifdef WIN32
#define FMOD_CALLBACK_INTERFACE _cdecl
#else
#define FMOD_CALLBACK_INTERFACE
#endif

    /*
    static unsigned int FMOD_CALLBACK_INTERFACE pak_OpenCallback(const char *name);
    static void FMOD_CALLBACK_INTERFACE pak_CloseCallback(unsigned int handle);
    static int FMOD_CALLBACK_INTERFACE pak_ReadCallback(void *buffer, int size, unsigned int handle);
    static int FMOD_CALLBACK_INTERFACE pak_SeekCallback(unsigned int handle, int pos, signed char mode);
    static int FMOD_CALLBACK_INTERFACE pak_TellCallback(unsigned int handle);
    */
    static int fmod_start_volume = 250;

    TPos silence_pos = { 0, -10000 };


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}
//-------------------------------------------------------------------
TSound* Fmod_New(void)
{
    TSoundFmod* self;

    self = malloc(sizeof(TSoundFmod));
    assert(self != NULL);

    self->Init = Fmod_Init;
    self->End  = Fmod_End;
    self->ModuleLoad   = Fmod_ModuleLoad;
    DEBUG_msg_hex("Address: ", (int)&Fmod_ModuleLoad);
    self->ModuleUnload = Fmod_ModuleUnload;
    self->ModuleStop   = Fmod_ModuleStop;
    self->ModulePlay   = Fmod_ModulePlay;
    self->SetVolume    = Fmod_SetVolume;
    self->GetSoundContext = Fmod_GetSoundContext;
    self->SetPos       = Fmod_SetPos;
    self->Delete       = Fmod_Delete;
    self->GetSongType  = Fmod_GetSongType;
    self->SetTimerDelay= Fmod_SetTimerDelay;
    self->TickStart    = 0;
    self->SongType     = 0;
    self->Playing      = 0;
    silence_pos.Order  = 0;
    silence_pos.Row    = -10000;
    self->timer_delay  = 0;
    self->Channel      = 0;
    DEBUG_msg("New Fmod_Sound created");
    return (TSound*)self;
}
//-------------------------------------------------------------------
int  Fmod_Init(TSoundFmod* self, int ForceManualConfig)
{
    FMOD_RESULT       result;
    unsigned int      version;

    /*
        Create a System object and initialize.
    */
    result = FMOD_System_Create(&self->System);
    ERRCHECK(result);

    result = FMOD_System_GetVersion(self->System, &version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        return 0;
    }

    result = FMOD_System_SetPluginPath(self->System, "plugins/");


//    FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
//    FSOUND_SetOutput(FSOUND_OUTPUT_OSS);
//    FSOUND_SetBufferSize(150);
    // Try to find a working output
    FMOD_System_SetOutput(self->System, FMOD_OUTPUTTYPE_ALSA);
    result = FMOD_System_Init(self->System, 1, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);
/*    if (!FSOUND_Init(44100, 32, FSOUND_INIT_USEDEFAULTMIDISYNTH))
    {
	FMOD_System_SetOutput(self->System, FMOD_OUTPUTTYPE_ALSA);
	FSOUND_SetOutput(FSOUND_OUTPUT_OSS);
	if (!FSOUND_Init(44100, 32, FSOUND_INIT_USEDEFAULTMIDISYNTH))
	{
	    FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
	    if (!FSOUND_Init(44100, 32, FSOUND_INIT_USEDEFAULTMIDISYNTH))
	    {
		printf("FMOD does not like any of the following: ALSA, OSS, ESD! Shitty FMOD!\n");
		printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));
		return 1;
	    }
	}
    }
*/
/*    FSOUND_File_SetCallbacks((FSOUND_OPENCALLBACK)pak_OpenCallback,
                             (FSOUND_CLOSECALLBACK)pak_CloseCallback,
                             (FSOUND_READCALLBACK)pak_ReadCallback,
                             (FSOUND_SEEKCALLBACK)pak_SeekCallback,
                             (FSOUND_TELLCALLBACK)pak_TellCallback);
*/
/*    FSOUND_File_SetCallbacks((FSOUND_OPENCALLBACK)Fmod_FileOpen,
                             (FSOUND_CLOSECALLBACK)Fmod_FileClose,
                             (FSOUND_READCALLBACK)Fmod_FileRead,
                             (FSOUND_SEEKCALLBACK)Fmod_FileSeek,
                             (FSOUND_TELLCALLBACK)Fmod_FileTell);
*/
    DEBUG_msg("Fmod_Sound initialized");
    return 0;
}

//-------------------------------------------------------------------
void Fmod_End(TSoundFmod* self)
{
    FMOD_RESULT result;

    result = FMOD_System_Close(self->System);
    ERRCHECK(result);
    result = FMOD_System_Release(self->System);
    ERRCHECK(result);
}

//-------------------------------------------------------------------
int  Fmod_ModuleLoad(TSoundFmod* self, char* ModName)
{
#if WIN32
#include <string.h>
#define strcasecmp _stricmp
#endif
    char* OggExt[] = { ".ogg", ".mp3", ".wav", ".raw", ".mp2", NULL };
    int extension_mismatch = 0;
    int i;
    FMOD_RESULT result;

    DEBUG_msg("ModuleLoad...");
    i = 0;
    do {
        char* tmpptr = ModName + strlen(ModName) - 4;
        extension_mismatch = strcasecmp(tmpptr, OggExt[i]);
	i++;
        DEBUG_msg_dec("Checking ext...", i);

    } while(OggExt[i] != NULL && extension_mismatch);
    DEBUG_msg("Extension check end");
    self->SongType = (extension_mismatch ? ftMod : ftOgg);
        self->pf_ogg = pak_open(ModName, "rb");
	self->Extinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        self->Extinfo.length = self->pf_ogg->size;
	result = FMOD_System_CreateSound(self->System, self->pf_ogg->data, FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM | FMOD_OPENMEMORY, &self->Extinfo, &self->Sound);
	ERRCHECK(result);

    DEBUG_msg_dec("SongType is: ", self->SongType);
    return 0;
}

//-------------------------------------------------------------------
void Fmod_ModuleUnload(TSoundFmod* self)
{
    FMOD_RESULT result;
    result = FMOD_Sound_Release(self->Sound);
    ERRCHECK(result);
}

//-------------------------------------------------------------------
void Fmod_ModuleStop(TSoundFmod* self)
{
    FMOD_Channel_SetPaused(self->Channel, 1);
    self->Playing = 0;
}

//-------------------------------------------------------------------
void Fmod_ModulePlay(TSoundFmod* self)
{
    FMOD_RESULT result;
    result = FMOD_System_PlaySound(self->System, FMOD_CHANNEL_FREE, self->Sound, 0, &self->Channel);
    ERRCHECK(result);
//        FSOUND_SetVolume(FSOUND_ALL, fmod_start_volume);
//	FSOUND_SetSFXMasterVolume(fmod_start_volume);
	self->TickStart = Epopeia_GetTimeMs();
    self->Playing = 1;
}

//-------------------------------------------------------------------
void Fmod_SetVolume(TSoundFmod* self, unsigned Percent)
{
    switch(self->SongType)
    {
    case ftMod:
//	FMUSIC_SetMasterVolume(self->mod, (Percent*255)/100);
        break;
    case ftOgg:
//	FSOUND_SetVolume(FSOUND_ALL, (Percent*255)/100);
//	FSOUND_SetSFXMasterVolume((Percent*255)/100);
	break;
    default:
        DEBUG_msg("SongType not recognized");
	break;
    }
}

//-------------------------------------------------------------------
TSoundContext* Fmod_GetSoundContext(TSoundFmod* self)
{
    static TSoundContext SoundContext;

    if(!self->Playing)
    {
        SoundContext.Tick = 0;
        SoundContext.Pos.Order = silence_pos.Order;
        SoundContext.Pos.Row   = silence_pos.Row;
        return &SoundContext;
    }

    SoundContext.SoundType = stFmod;
    if(self->Channel)
    {
        // El tick lo ponemos usando temporizadores de alta resolucion
        // del sistema porque la granularidad del Fmod es muy alta/mala.
	SoundContext.Tick      = Epopeia_GetTimeMs() - self->TickStart;
        // El row que se usa para la sincro se lo pedimos al Fmod
        // porque si usamos el del sistema el timing del fmod y el del
        // sistema se pueden desfasar y la cagamos :)
	SoundContext.Pos.Order = 0;
        unsigned int ms;
        FMOD_RESULT result = FMOD_Channel_GetPosition(self->Channel, &ms, FMOD_TIMEUNIT_MS);
	SoundContext.Pos.Row   = ms - self->timer_delay; // ajuste "a mano" opcional
        // Esto de aqui abajo no da una buena granularidad para mover
        // los efectos con suavidad
        //SoundContext.Tick  = FSOUND_Stream_GetTime(self->stream);
    }
    else
    {
        SoundContext.Pos.Order = 0;
        SoundContext.Pos.Row   = 0;
        SoundContext.Tick      = 0;
    }
    return &SoundContext;
}

//-------------------------------------------------------------------
void Fmod_SetPos(TSoundFmod* self, TPos* Pos)
{
    if(!self->Playing)
    {
        silence_pos.Order = Pos->Order;
        silence_pos.Row   = Pos->Row;
        return;
    }

    FMOD_Channel_SetPosition(self->Channel, Pos->Row, FMOD_TIMEUNIT_MS);
}
//-------------------------------------------------------------------
void Fmod_Delete(TSoundFmod* self)
{
    free(self);
}
//-------------------------------------------------------------------
EFileType Fmod_GetSongType(TSoundFmod* self)
{
    DEBUG_msg_dec("SongType is: ", self->SongType);
    return self->SongType;
}
//-------------------------------------------------------------------
void Fmod_SetTimerDelay(TSoundFmod* self, int miliseconds)
{
    self->timer_delay = miliseconds;
}



/*
//-------------------------------------------------------------------
static unsigned int FMOD_CALLBACK_INTERFACE pak_OpenCallback(const char *name)
{
    int h;
    h = (unsigned int)pak_open(name, "rb");
    DEBUG_msg_hex("pak_open returns: ", h);
    return h;
}
//-------------------------------------------------------------------
static void FMOD_CALLBACK_INTERFACE pak_CloseCallback(unsigned int handle)
{
    DEBUG_msg_hex("pak_close param ", handle);
    pak_close((PFILE*)handle);
}
//-------------------------------------------------------------------
static int FMOD_CALLBACK_INTERFACE pak_ReadCallback(void *buffer, int size, unsigned int handle)
{
    DEBUG_msg_hex("pak_read handle ", handle);
    DEBUG_msg_dec("pak_read count  ", size);
    DEBUG_msg_hex("pak_read dest_buf ", (int)buffer);
    DEBUG_msg_dec("pak_read current pos ", ((PFILE*)handle)->position);
    DEBUG_msg_dec("pak_read total size ", ((PFILE*)handle)->size);
    return pak_read(buffer, 1, size, (PFILE*)handle);
}
//-------------------------------------------------------------------
static int FMOD_CALLBACK_INTERFACE pak_SeekCallback(unsigned int handle, int pos, signed char mode)
{
    DEBUG_msg_hex("pak_seek handle ", handle);
    return pak_fseek((PFILE*)handle, pos, mode);

}
//-------------------------------------------------------------------
static int FMOD_CALLBACK_INTERFACE pak_TellCallback(unsigned int handle)
{
    DEBUG_msg_hex("pak_tell handle ", handle);
    return pak_ftell((PFILE*)handle);
}
//-------------------------------------------------------------------

*/
//-------------------------------------------------------------------
static void         Fmod_FileClose(unsigned int handle)
{
    pak_close(handle);
}
//-------------------------------------------------------------------
static unsigned int Fmod_FileOpen(const char *name)
{
    return pak_open(name, "rb");
}
//-------------------------------------------------------------------
static int          Fmod_FileRead(void *buffer, int size, unsigned int handle)
{
    return pak_read(buffer, 1, size, handle);
}
//-------------------------------------------------------------------
static int          Fmod_FileSeek(unsigned int handle, int pos, signed char mode)
{
    return pak_fseek(handle, pos, mode);
}
//-------------------------------------------------------------------
static int          Fmod_FileTell(unsigned int handle)
{
    return pak_ftell(handle);
}
//-------------------------------------------------------------------
