#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <packer/packer.h>
#include <epopeia/epopeia.h>
#include <bass/bass.h>
#include "sound_bass.h"
#define DEBUG_VAR 0
#define DEBUG_MODULE "bass"

#include <epopeia/debug.h>

    int  Bass_Init        (TSoundBass* self, int ForceManualConfig);
    void Bass_End         (TSoundBass* self);
    int  Bass_ModuleLoad  (TSoundBass* self, char* ModName);
    void Bass_ModuleUnload(TSoundBass* self);
    void Bass_ModuleStop  (TSoundBass* self);
    void Bass_ModulePlay  (TSoundBass* self);
    void Bass_SetVolume   (TSoundBass* self, unsigned Percent);
    TSoundContext* Bass_GetSoundContext(TSoundBass* self);
    void Bass_SetPos      (TSoundBass* self, TPos* Pos);
    void Bass_Delete      (TSoundBass* self);
    EFileType Bass_GetSongType(TSoundBass* self);
    void Bass_SetTimerDelay(TSoundBass* self, int miliseconds);


    TPos silence_pos = { 0, -10000 };

static void *LoadFileToMemory(char *filename, int *length)
{
  PFILE *p;
  unsigned char *file;
  int size;
  
  p=pak_open(filename,"rb");
  if(p==NULL) return NULL;
  pak_fseek(p,0,SEEK_END);
  size=pak_ftell(p);
  pak_fseek(p,0,SEEK_SET);
  
  file = (unsigned char*)malloc(size*sizeof(unsigned char));
  if(!file) 
  {
  	pak_close(p);	
  	return NULL;
  }
  
  pak_read(file,size,1,p);
  pak_close(p);
  *length=size;
  return(file); 	
}

//-------------------------------------------------------------------
TSound* Bass_New(void)
{
    TSoundBass* self;

    self = malloc(sizeof(TSoundBass));
    assert(self != NULL);

    self->Init = Bass_Init;
    self->End  = Bass_End;
    self->ModuleLoad   = Bass_ModuleLoad;
    self->ModuleUnload = Bass_ModuleUnload;
    self->ModuleStop   = Bass_ModuleStop;
    self->ModulePlay   = Bass_ModulePlay;
    self->SetVolume    = Bass_SetVolume;
    self->GetSoundContext = Bass_GetSoundContext;
    self->SetPos       = Bass_SetPos;
    self->Delete       = Bass_Delete;
    self->GetSongType  = Bass_GetSongType;
    DEBUG_msg("New Bass_Sound created");
    return (TSound*)self;
}
//-------------------------------------------------------------------
int  Bass_Init(TSoundBass* self, int ForceManualConfig)
{
    if (BASS_GetVersion() != MAKELONG(2,0))
    {
        printf("Error : You are using the wrong DLL version!  You should be using BASS 2.0\n");
        return 1;
    }

    if(!BASS_Init(1,44100,0,0,NULL))
    {
        printf("Error!\n");
        printf("%d\n", BASS_ErrorGetCode());
        return 1;    	
    }
    self->mod=0;
    self->stream=0;
	self->Playing = 0;
    DEBUG_msg("Bass_Sound initialized");
    return 0;
}

//-------------------------------------------------------------------
void Bass_End(TSoundBass* self)
{
    BASS_Stop();
}

//-------------------------------------------------------------------
int  Bass_ModuleLoad(TSoundBass* self, char* ModName)
{
  void *buffer;
	
    char* OggExt[] = { ".ogg", ".mp3", ".wav", ".raw", ".mp2", NULL };
    int extension_mismatch = 0;
    int i;

    DEBUG_msg("ModuleLoad...");
    i = 0;
    do {
        char* tmpptr = ModName + strlen(ModName) - 4;
        extension_mismatch = strcmp(tmpptr, OggExt[i]);
	i++;
        DEBUG_msg_dec("Checking ext...", i);

    } while(OggExt[i] != NULL && extension_mismatch);
    DEBUG_msg("Extension check end");
    self->SongType = (extension_mismatch ? ftMod : ftOgg);
    switch(self->SongType)
    {
    case ftMod:
	DEBUG_msg("Calling LoadSong...");
	self->buffer=LoadFileToMemory(ModName,&self->length);
	self->mod = BASS_MusicLoad(1,self->buffer,0,self->length,0,0);
	free(self->buffer);
	DEBUG_msg("Calling MusicLoad...returned");
	if(!self->mod)
	{
	    Epopeia_Print("Error al cargar modulo!\n");	    
	    return 1;
	}
	DEBUG_msg("ModuleLoaded");
	Epopeia_Print("Title: ");
	Epopeia_Print(BASS_MusicGetName(self->mod));
        Epopeia_Print("\n");
	break;
    case ftOgg:
	self->buffer=LoadFileToMemory(ModName,&self->length);	
	if(!self->buffer)
	{	
	    printf("Error al leer ogg!\n");
	    return 1;		
	}
	self->stream = BASS_StreamCreateFile(1,self->buffer,0,self->length,BASS_MP3_SETPOS);
	
	if(!self->stream)
	{
	    printf("Error al cargar ogg!\n");
	    return 1;
	}
	DEBUG_msg("WavfileLoaded");
	break;
    default:
        DEBUG_msg("SongType not recognized");
	break;
    }
    DEBUG_msg_dec("SongType is: ", self->SongType);
    return 0;
}

//-------------------------------------------------------------------
void Bass_ModuleUnload(TSoundBass* self)
{
    switch(self->SongType)
    {
    case ftMod:
	BASS_MusicFree(self->mod);
        break;
    case ftOgg:
        BASS_StreamFree(self->stream);
        free(self->buffer);
	break;
    default:
        DEBUG_msg("SongType not recognized");
	break;
    }
}

//-------------------------------------------------------------------
void Bass_ModuleStop(TSoundBass* self)
{
    switch(self->SongType)
    {
    case ftMod:
	BASS_ChannelStop(self->mod);
        break;
    case ftOgg:
        BASS_ChannelStop(self->stream);
	break;
    default:
        DEBUG_msg("SongType not recognized");
	break;
    }
    self->Playing = 0;
}

//-------------------------------------------------------------------
void Bass_ModulePlay(TSoundBass* self)
{
    BASS_SetVolume(100); // Set maximum volume by default
	
    switch(self->SongType)
    {
    case ftMod:
        BASS_MusicPreBuf(self->mod);
	BASS_MusicPlayEx(self->mod,0,BASS_MUSIC_LOOP,0);
	self->TickStart = Epopeia_GetTimeMs();
        break;
    case ftOgg:
        BASS_StreamPreBuf(self->stream);
	BASS_StreamPlay(self->stream,1,BASS_SAMPLE_LOOP);    
	self->TickStart = Epopeia_GetTimeMs();
	break;
    default:
        DEBUG_msg("SongType not recognized");
	break;
    }
    self->Playing = 1;
}

//-------------------------------------------------------------------
void Bass_SetVolume(TSoundBass* self, unsigned Percent)
{
    BASS_SetVolume(Percent);
}

//-------------------------------------------------------------------
TSoundContext* Bass_GetSoundContext(TSoundBass* self)
{
    static TSoundContext SoundContext;
    QWORD position;

    if(!self->Playing)
    {
        SoundContext.Tick = 0;
        SoundContext.Pos.Order = silence_pos.Order;
        SoundContext.Pos.Row   = silence_pos.Row;
        return &SoundContext;
    }

    SoundContext.SoundType = stFmod;
    if(self->mod)
    {
        // El tick lo ponemos usando temporizadores de alta resolucion
        // del sistema porque la granularidad del Fmod es muy alta/mala.
	SoundContext.Tick      = Epopeia_GetTimeMs() - self->TickStart;
        position = BASS_ChannelGetPosition(self->mod);
        SoundContext.Pos.Order = LOWORD(position);
        SoundContext.Pos.Row   = HIWORD(position);        
        
        // Parece que esto no da una buena resolucion
        //        SoundContext.Tick  = FMUSIC_GetTime(self->mod);
    }
    else if(self->stream)
    {
        // El tick lo ponemos usando temporizadores de alta resolucion
        // del sistema porque la granularidad del Fmod es muy alta/mala.
	SoundContext.Tick      = Epopeia_GetTimeMs() - self->TickStart;
        // El row que se usa para la sincro se lo pedimos al Fmod
        // porque si usamos el del sistema el timing del fmod y el del
        // sistema se pueden desfasar y la cagamos :)
        SoundContext.Pos.Order = 0;
        position = BASS_ChannelGetPosition(self->stream);
        SoundContext.Pos.Row   = (int)(BASS_ChannelBytes2Seconds(self->stream,position)*1000);
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
void Bass_SetPos(TSoundBass* self, TPos* Pos)
{
    if(!self->Playing)
    {
        silence_pos.Order = Pos->Order;
        silence_pos.Row   = Pos->Row;
        return;
    }

    switch(self->SongType)
    {
    case ftMod:
	BASS_ChannelSetPosition(self->mod,MAKELONG(Pos->Row,Pos->Order));
        break;
    case ftOgg:
        BASS_ChannelSetPosition(self->stream,BASS_ChannelSeconds2Bytes(self->stream,(float)(Pos->Row)/1000.0f));
	break;
    default:
        DEBUG_msg("SongType not recognized");
	break;
    }
    // No se puede cambiar el row
}
//-------------------------------------------------------------------
void Bass_Delete(TSoundBass* self)
{
    free(self);
}
//-------------------------------------------------------------------
EFileType Bass_GetSongType(TSoundBass* self)
{
    DEBUG_msg_dec("SongType is: ", self->SongType);
    return self->SongType;
}
//-------------------------------------------------------------------
//void Fmod_SetTimerDelay(TSoundFmod* self, int miliseconds)
//{
//    self->timer_delay = miliseconds;
//}



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
