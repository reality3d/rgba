#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__


typedef struct
{
    int x_offset, y_offset;
    int width, height;

    // Adds
    float dx, dy;   // Desplazamientos
    long  last_tick;

    float fx_offset, fy_offset;
} TViewport;

#endif // __VIEWPORT_H__
