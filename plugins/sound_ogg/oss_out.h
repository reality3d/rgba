#ifndef OSS_OUT_H
#define OSS_OUT_H

#include <stdint.h>
#include <stdio.h>

int  OSS_OUT_Init(void);
int  OSS_OUT_Write(void *buffer, size_t buffer_bytes);
void OSS_OUT_Deinit(void);
int OSS_OUT_GetDelay(void);

#endif
