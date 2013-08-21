#ifndef GRAPHICS_SDL_H
#define GRAPHICS_SDL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL/SDL.h>
#include <epopeia/graphics.h>

typedef struct TGraphicsSdl_t
{
    int  (*Init)(struct TGraphicsSdl_t* self, TResolution* RestrictedTo, int Windowed); // Inicializar
    void (*End )(struct TGraphicsSdl_t* self);                      // Desinicializar
    TGraphicsContext* (*FrameStart)(struct TGraphicsSdl_t* self);     // Llamado cada vez que se renderiza un frame
    void (*FrameEnd)(struct TGraphicsSdl_t* self);                  // Llamado cuando se ha acabado de renderizar el frame
    void (*Delete)(struct TGraphicsSdl_t* self);                    // Desinicializar y destruir
    void (*SetGlobalFade)(struct TGraphicsSdl_t* self, float r, float b, float g, float a);      // Control de "fade"
    void (*ToggleWindowed)(struct TGraphicsSdl_t* self);            // Cambia de ventana a fullscreen y al reves
    void (*SetZBuffer)(struct TGraphicsSdl_t* self, int bits);      // Establece los bits del Zbuffer
    void (*SetStencilBuffer)(struct TGraphicsSdl_t* self, int bits);// Establece los bits del Stencil buffer
    TKeyboard* (*GetKeyboardSubsystem)(struct TGraphicsSdl_t* self);

    TGraphicsContext RC;
    // Propio del Graphics SDL
    float fade_r, fade_g, fade_b, fade_a;
    SDL_Surface* surface;
    int zbuffer, stencil;

} TGraphicsSdl;

TGraphicsSdl* Graphics_Sdl_New(void);

#ifdef __cplusplus
};
#endif

#endif
