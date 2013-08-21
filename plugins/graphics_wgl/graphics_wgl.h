#ifndef RENDERER_WGL_H
#define RENDERER_WGL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>
#include <epopeia/graphics.h>
#include <epopeia/keyboard.h>

//
// CONFIG
//

#define TDS_KEYS
#define TDS_MOUSE
//#define TDS_ICON   IDI_GLICON
#define TDS_TIMER         // Include TDS_InitTimer and TDS_GetTimer
#define TDS_TIMER_HRPC    // High-resolution performance counter
//#define TDS_TIMER_SYSTEM  // System timer
#define TDS_MAX_TIMERS 8  // How much timer_ids? (0..IDS_MAX_TIMERS-1)


typedef struct TGraphicsWgl_t
{
    int  (*Init)(struct TGraphicsWgl_t* self, TResolution* RestrictedTo, int Windowed); // Inicializar
    void (*End )(struct TGraphicsWgl_t* self);                      // Desinicializar
    TGraphicsContext* (*FrameStart)(struct TGraphicsWgl_t* self);     // Llamado cada vez que se renderiza un frame
    void (*FrameEnd)(struct TGraphicsWgl_t* self);                  // Llamado cuando se ha acabado de renderizar el frame
    void (*Delete)  (struct TGraphicsWgl_t* self);                 // Destruir objeto
    void (*SetGlobalFade)(struct TGraphicsWgl_t* self, float r, float b, float g, float a);      // Control de "fade"
    void (*ToggleWindowed)(struct TGraphicsWgl_t* self);            // Cambia de ventana a fullscreen y al reves
    void (*SetZBuffer)(struct TGraphicsWgl_t* self, int bits);      // Establece los bits del Zbuffer
    void (*SetStencilBuffer)(struct TGraphicsWgl_t* self, int bits);// Establece los bits del Stencil buffer
    TKeyboard* (*GetKeyboardSubsystem)(struct TGraphicsWgl_t* self);

    // Propio de la clase WGL
    TGraphicsContext RC;
    BOOL (*IsPressed)(struct TGraphicsWgl_t* self, int vkey);
    int  (*GetKey)   (struct TGraphicsWgl_t* self);
    int  (*ExitPressed)(struct TGraphicsWgl_t* self);
    float fade_r, fade_g, fade_b, fade_a;
    int width, height, bpp, flags;
    int zbuffer, stencil;
    HWND      hWnd;          // Handle to our window
    HINSTANCE hInstance;     // Instance
    HDC       hDC;           // Device Context
    HGLRC     hRC;           // Rendering Context (OPENGL)
    BOOL      active;
    BOOL      fullscreen;
    char      last_error[128];

#ifdef TDS_KEYS
    BOOL keys[256];
    int  last_key;
    int  exit_pressed;
#endif // TDS_KEYS

} TGraphicsWgl;

extern TGraphics* Wgl_New(void);

#ifdef __cplusplus
};
#endif

#endif
