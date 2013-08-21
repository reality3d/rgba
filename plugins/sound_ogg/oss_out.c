#include <stdint.h>
#include <fcntl.h>  // open
#include <unistd.h> // close
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include "oss_out.h"

#define OSS_DEVICE_OUT "/dev/dsp"
#define OSS_DEVICE_MIXER "/dev/mixer"


static int dev_out, dmixer;

//------------------------------------------------------------------------------
int OSS_OUT_Init(void)
{
    dev_out = open(OSS_DEVICE_OUT, O_WRONLY);
    if(dev_out == -1)
	return -1;
    dmixer = open(OSS_DEVICE_MIXER, O_WRONLY);
    if(dmixer == -1)
	return -1;
    // Set standard WAV settings
    int val = AFMT_S16_LE;
    ioctl(dev_out, SNDCTL_DSP_SETFMT, &val);
    val = 44100;
    ioctl(dev_out, SNDCTL_DSP_SPEED, &val);
    val = 1;
    ioctl(dev_out, SNDCTL_DSP_STEREO, &val);

    // Set master and PCM volume to 90% (both stereo channels)
//    val = 90*256+90;
//    ioctl(dmixer, SOUND_MIXER_WRITE_VOLUME, &val);
//    ioctl(dmixer, SOUND_MIXER_WRITE_PCM, &val);

    // Close mixer device, no more used
    close(dmixer);
    dmixer = -1;
    return 0;
}

//------------------------------------------------------------------------------
// Devuelve: -1 si ha habido un error
//           0 o mas indican el numero de bytes escritos
int OSS_OUT_Write(void *buffer, size_t buffer_bytes)
{
    int count;
    count = write(dev_out, buffer, buffer_bytes);

    return count;
}

//------------------------------------------------------------------------------
void OSS_OUT_Deinit(void)
{
    close(dev_out);
}

//------------------------------------------------------------------------------
uint32_t OSS_OUT_GetStatus(void)
{
    int val = AFMT_QUERY;
    ioctl(dev_out, SNDCTL_DSP_SETFMT, &val);
    printf("Format: %x\n", val);

    ioctl(dev_out, SOUND_PCM_READ_RATE, &val);
    printf("Rate: %d\n", val);
    return val;
}

//------------------------------------------------------------------------------
int OSS_OUT_GetDelay(void)
{
    int val;

    ioctl(dev_out, SNDCTL_DSP_GETODELAY, &val);
//    printf("Delay: %d\n", val);
    return val;
}


