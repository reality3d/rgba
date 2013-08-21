#ifndef FLASH_H
#define FLASH_H

typedef struct TFlash_t
{
    long ultimo_tick;
    float r,g,b;
    float timeout;
    float alpha;
    float delta;

    int blend_mode;
}TFlash;


#endif
