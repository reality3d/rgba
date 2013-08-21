#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int Height;
    int Width;
    int Bpp;
} TResolution;

typedef enum
{
    rtFrameBuffer,
    rtPgk,
    rtPtc,
    rtWgl,
    rtSdl
} EGraphicsType;


typedef struct TGraphicsContext_t
{
    EGraphicsType GraphicsType;
    unsigned Width;
    unsigned Height;
    unsigned Bpp;             // Bits utiles por pixel (32 bpp -> 24)
    unsigned BytesPerPixel;   // Cuantos bytes ocupa un pixel (32 bpp -> 4 bytes)
    unsigned Pitch;
    void* FrameBuffer;
    void* reserved;
} TGraphicsContext;

typedef struct TGraphics_t
{
    int  (*Init)(struct TGraphics_t* self, TResolution* RestrictedTo, int Windowed); // Inicializar
    void (*End )(struct TGraphics_t* self);                      // Desinicializar
    TGraphicsContext* (*FrameStart)(struct TGraphics_t* self);     // Llamado cada vez que se renderiza un frame
    void (*FrameEnd)(struct TGraphics_t* self);                  // Llamado cuando se ha acabado de renderizar el frame
    void (*Delete)(struct TGraphics_t* self);                    // Desinicializar y destruir
    void (*SetGlobalFade)(struct TGraphics_t* self, float r, float b, float g, float a);      // Control de "fade"
    void (*ToggleWindowed)(struct TGraphics_t* self);            // Cambia de ventana a fullscreen y al reves
    void (*SetZBuffer)(struct TGraphics_t* self, int bits);      // Establece los bits del Zbuffer, debe llamarse antes del Init
    void (*SetStencilBuffer)(struct TGraphics_t* self, int bits);// Establece los bits del Stencil buffer, debe llamarse antes del Init
} TGraphics;

typedef TGraphicsContext TRenderContext; // Source backwards compatibility
typedef TGraphics TRenderer;             // Source backwards compatibility

#ifdef __cplusplus
};
#endif

#endif
