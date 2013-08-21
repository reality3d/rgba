#ifdef WIN32
// Renderer para OpenGL en Win32
// by Enlar
// Directamente ripeado y adaptado del TDS de Epsilum 8)

#include <assert.h>
#include <stdlib.h>
#define DEBUG_MODULE "renderer_wgl"
#define DEBUG_VAR 1
#include <epopeia/debug.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>
#include <stdio.h>
#include "graphics_wgl.h"
#include "keyboard_tds.h"
#include "../glt/cache.h"

//
// API
//
void  TDS_GetMousePosition (int *x, int *y, int *button);


#ifdef TDS_TIMER
int    TDS_InitTimer (int timer_id, int freq);
double TDS_GetTimer  (int timer_id);
#endif // TDS_TIMER

int  Wgl_Init(TGraphicsWgl* self, TResolution* RestrictedTo, int Windowed);
void Wgl_End (TGraphicsWgl* self);
TGraphicsContext* Wgl_FrameStart(TGraphicsWgl* self);
void Wgl_FrameEnd(TGraphicsWgl* self);
void Wgl_Delete  (TGraphicsWgl* self);
int  Wgl_ExitPressed(TGraphicsWgl* self);
int  Wgl_IsPressed(TGraphicsWgl* self, int value);
int  Wgl_GetKey(TGraphicsWgl* self);
void Wgl_SetGlobalFade(TGraphicsWgl* self, float r, float g, float b, float a);
void Wgl_ToggleWindowed(TGraphicsWgl* self);
void Wgl_SetZBuffer(TGraphicsWgl* self, int bits);
void Wgl_SetStencilBuffer(TGraphicsWgl* self, int bits);
int   Wgl_UpdateWindow(TGraphicsWgl* self);
char *Wgl_GetLastError (TGraphicsWgl* self);
int Wgl_OpenWindow(TGraphicsWgl* self);
int Wgl_CloseWindow(TGraphicsWgl* self);
TKeyboard* Wgl_GetKeyboardSubsystem(TGraphicsWgl* self);

char *wgl_demo_name = "rgba production";

//-------------------------------------------------------------------
TGraphics* Wgl_New(void)
{
    TGraphicsWgl* self;

    self = malloc(sizeof(TGraphicsWgl));
    assert(self != NULL);
    self->Init = Wgl_Init;
    self->End  = Wgl_End;
    self->FrameStart = Wgl_FrameStart;
    self->FrameEnd   = Wgl_FrameEnd;
    self->Delete     = Wgl_Delete;
    self->IsPressed  = Wgl_IsPressed;
    self->GetKey     = Wgl_GetKey;
    self->ExitPressed= Wgl_ExitPressed;
    self->SetGlobalFade    = Wgl_SetGlobalFade;
    self->ToggleWindowed   = Wgl_ToggleWindowed;
    self->SetZBuffer       = Wgl_SetZBuffer;
    self->SetStencilBuffer = Wgl_SetStencilBuffer;
    self->GetKeyboardSubsystem = Wgl_GetKeyboardSubsystem;
    
    self->zbuffer = 32;
    self->stencil = 8;
    
    return (TGraphics*)self;
}

//-------------------------------------------------------------------
int Wgl_Init(TGraphicsWgl* self, TResolution* res, int Windowed)
{
    int ok;
//    int flags;

    assert(res != NULL);

    self->width      = res->Width;
    self->height     = res->Height;
    self->bpp        = res->Bpp;
    self->fullscreen = !Windowed;
    self->active     = TRUE;

    ok = Wgl_OpenWindow(self);
    if(!ok)
        return 0;

    memset (self->keys, 0, sizeof (self->keys));
    self->exit_pressed = 0;
    self->last_key = -1;

    self->RC.GraphicsType  = rtWgl;
    self->RC.Width         = res->Width;
    self->RC.Height        = res->Height;
    self->RC.Bpp           = res->Bpp;
    self->RC.Pitch         = res->Width*4;
    self->RC.BytesPerPixel = res->Bpp/8;
    self->RC.FrameBuffer   = NULL;
    self->fade_a = 0.0f;

//    DEBUG_msg("Inicializando CACHE de GLT");
//    CACHE_Init();
    DEBUG_msg_dec("Wgl init returns: ", !ok);
    return !ok;
}

//-------------------------------------------------------------------
void Wgl_End (TGraphicsWgl* self)
{
    DEBUG_msg("Parando CACHE de GLT");
    DEBUG_msg("Cerrando TDS");
    Wgl_CloseWindow(self);
}

//-------------------------------------------------------------------
TGraphicsContext* Wgl_FrameStart(TGraphicsWgl* self)
{
    DEBUG_msg("Frame start executing");
    return &self->RC;
}
//-------------------------------------------------------------------
void Wgl_FrameEnd(TGraphicsWgl* self)
{
    DEBUG_msg("Frame end executing");
    Wgl_UpdateWindow(self);
}

//-------------------------------------------------------------------
void Wgl_Delete  (TGraphicsWgl* self)
{
    free(self);
}
//-------------------------------------------------------------------
int Wgl_ExitPressed(TGraphicsWgl* self)
{
    DEBUG_msg_dec("Exit pressed: ", self->exit_pressed);
    return self->exit_pressed;
}
//-------------------------------------------------------------------
int Wgl_IsPressed(TGraphicsWgl* self, int vkey)
{
    MSG message;

    DEBUG_msg_dec("Comprobando si tecla esta pulsada:", vkey);
    DEBUG_msg_dec("Comprobando si tecla esta pulsada2:", vkey & 0xff);
    if (self->keys[vkey & 0xFF])
    {
        while (self->keys[vkey & 0xFF])
        {
            while (PeekMessage (&message, self->hWnd, 0, 0, PM_REMOVE))
            {
                // translate and dispatch
                TranslateMessage (&message);
                DispatchMessage  (&message);
            }
            Sleep (1);
        }
        DEBUG_msg("SI esta pulsada");
        return TRUE;
    }
    else
        return FALSE;
    //return _tds_keys[vkey & 0xFF];
}
//-------------------------------------------------------------------
int Wgl_GetKey(TGraphicsWgl* self)
{
    return self->last_key;
}
//-------------------------------------------------------------------
void Wgl_SetGlobalFade(TGraphicsWgl* self, float r, float g, float b, float a)
{
    if(r>1.0)
	self->fade_r = 1.0;
    else
	self->fade_r = r;
    if(g>1.0)
	self->fade_g = 1.0;
    else
	self->fade_g = g;
    if(b>1.0)
	self->fade_b = 1.0;
    else
	self->fade_b = b;
    if(a>1.0)
	self->fade_a = 1.0;
    else
	self->fade_a = a;

}
//-------------------------------------------------------------------
void Wgl_SetZBuffer(TGraphicsWgl* self, int bits)
{
    self->zbuffer = bits;
}
//-------------------------------------------------------------------
void Wgl_SetStencilBuffer(TGraphicsWgl* self, int bits)
{
    self->stencil = bits;
}
//-------------------------------------------------------------------
void Wgl_ToggleWindowed(TGraphicsWgl* self)
{
    Wgl_CloseWindow(self);
    // Cambiamos el estado del modo
    self->fullscreen = !self->fullscreen;
    Wgl_OpenWindow(self);

}
//-------------------------------------------------------------------

//
// MOUSE Handler
//
#ifdef TDS_MOUSE
static int _tds_mouse_x;
static int _tds_mouse_y;
static int _tds_mouse_button;
void TDS_GetMousePosition (int *x, int *y, int *button)
{
    if (x      != NULL) *x      = _tds_mouse_x;
    if (y      != NULL) *y      = _tds_mouse_y;
    if (button != NULL) *button = _tds_mouse_button;
}
#endif // TDS_MOUSE

//
// OUR WINDOW PROC
//

static LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    TGraphicsWgl* self;

    // Obtener self para que WndProc lo pueda obtener
    self = (TGraphicsWgl*)GetWindowLong(hWnd, GWL_USERDATA);

    switch (uMsg)
    {
    case WM_ACTIVATE:
        if (!HIWORD (wParam))
            self->active = TRUE;
        else
            self->active = FALSE;
        return 0;

        /*
         case WM_ACTIVATEAPP:
         if (fullscreen)
         {
         // update active flag
         active = (BOOL) wParam;
         }
         break;
         */
        /*
         case WM_SETCURSOR:
         SetCursor(0);
         break;
         */
#ifdef TDS_MOUSE
    case WM_MOUSEMOVE:
        _tds_mouse_x = (int) LOWORD (lParam);
        _tds_mouse_y = (int) HIWORD (lParam);
        _tds_mouse_button = (int) wParam;
        break;
#endif // TDS_MOVE

    case WM_KEYUP:
        self->last_key = -1;
        self->keys[wParam & 0xFF] = FALSE;
        return 0;

    case WM_KEYDOWN:
        self->last_key = (int) (wParam & 0xFF);
        self->keys[wParam & 0xFF] = TRUE;
        DEBUG_msg_dec("Tecla pulsada:", wParam & 0xFF);

/*
        if((wParam&0xFF) == 32) //espacio
        {
            //Cambia el modo pero no queda centrado en la ventana
             DEVMODE    dm;
             memset (&dm, 0, sizeof (DEVMODE));
             dm.dmSize        = sizeof (DEVMODE);
             dm.dmPelsWidth   = res->Width;
             dm.dmPelsHeight  = res->Height;
             dm.dmBitsPerPel  = res->Bpp;
             dm.dmFields      = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

             ChangeDisplaySettings (&dm, CDS_FULLSCREEN);

        }
*/
        // close on escape key
        if((wParam & 0xFF) == 27)
        {
            DEBUG_msg("ESC pulsado");
            self->exit_pressed += 1;
        }
        return 0;

    case WM_CLOSE:
        self->exit_pressed += 1;
        return 0;
    }

    return DefWindowProc (hWnd, uMsg, wParam, lParam);
}
//-------------------------------------------------------------------
int Wgl_OpenWindow(TGraphicsWgl* self)
{
    // Originally in TDS code by Epsilum
    WNDCLASS   wc;
    RECT       rect;
    DEVMODE    dm;
    DWORD      dwExStyle;
    DWORD      dwStyle;
    PIXELFORMATDESCRIPTOR pfd;
    int        pixel_format;

    self->hInstance = GetModuleHandle (0);
    memset(&wc, 0, sizeof(WNDCLASS));
    // Register window class
    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = (WNDPROC) WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = self->hInstance;
#ifdef TDS_ICON
    wc.hIcon         = LoadIcon (self->hInstance, MAKEINTRESOURCE (TDS_ICON));
#else
    wc.hIcon         = LoadIcon (NULL, IDI_WINLOGO);
#endif //TDS_ICON

wc.hIcon = NULL;
    wc.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = wgl_demo_name;

    if (!RegisterClass (&wc))
    {
        sprintf (self->last_error, "TDS Error:: RegisterClass failed %ld", GetLastError());
        DEBUG_msg(self->last_error);
//        return 0;
    }

    // Change to fullscreen mode
    if (self->fullscreen)
    {
        memset (&dm, 0, sizeof (DEVMODE));
        dm.dmSize        = sizeof (DEVMODE);
        dm.dmPelsWidth   = self->width;
        dm.dmPelsHeight  = self->height;
        dm.dmBitsPerPel  = self->bpp;
        dm.dmFields      = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        if (ChangeDisplaySettings (&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            sprintf (self->last_error, "TDS Error:: ChangeDisplaySettings failed");
            DEBUG_msg(self->last_error);
            return 0;
        }
    }
    // Adjust styles
    if (self->fullscreen)
    {
        dwStyle   = WS_POPUP;
        dwExStyle = WS_EX_APPWINDOW;
        ShowCursor (0);
    }
    else
    {
        dwStyle   = WS_OVERLAPPEDWINDOW & (~WS_MAXIMIZEBOX) & (~WS_THICKFRAME);
        //dwStyle   = WS_OVERLAPPEDWINDOW;
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    }

    // Adjust window size
    rect.left   = (long) 0;
    rect.right  = (long) self->width;
    rect.top    = (long) 0;
    rect.bottom = (long) self->height;
    AdjustWindowRectEx (&rect, dwStyle, FALSE, dwExStyle);

    // Create window
    self->hWnd = CreateWindowEx (dwExStyle, wgl_demo_name, wgl_demo_name,
                           dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                           CW_USEDEFAULT, CW_USEDEFAULT,
                           rect.right  - rect.left,
                           rect.bottom - rect.top,
                           NULL,
                           NULL,
                           self->hInstance,
                           NULL);
    if (!self->hWnd)
    {
        sprintf (self->last_error, "TDS Error:: CreateWindowEx failed");
        DEBUG_msg(self->last_error);
        return 0;
    }
    // Pixel format descriptor (pfd)
    memset (&pfd, 0, sizeof (PIXELFORMATDESCRIPTOR));
    pfd.nSize        = sizeof (PIXELFORMATDESCRIPTOR);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cColorBits   = self->bpp;
    pfd.cAlphaBits   = 8;
    pfd.cDepthBits   = self->zbuffer;
    pfd.cStencilBits = self->stencil;
    pfd.iLayerType   = PFD_MAIN_PLANE;
    self->hDC = GetDC (self->hWnd);
    if (!self->hDC)
    {
        sprintf (self->last_error, "TDS Error:: GetDC failed");
        DEBUG_msg(self->last_error);
        return 0;
    }
    pixel_format = ChoosePixelFormat (self->hDC, &pfd);
    if (!pixel_format)
    {
        sprintf (self->last_error, "TDS Error:: ChoosePixelFormat failed");
        DEBUG_msg(self->last_error);
        return 0;
    }

    if (!SetPixelFormat (self->hDC, pixel_format, &pfd))
    {
        sprintf (self->last_error, "TDS Error:: SetPixelFormat failed");
        DEBUG_msg(self->last_error);
        return 0;
    }
    self->hRC = wglCreateContext (self->hDC);
    if (!self->hRC)
    {
        sprintf (self->last_error, "TDS Error:: wglCreateContext failed");
        DEBUG_msg(self->last_error);
        return 0;
    }

    if (!wglMakeCurrent (self->hDC, self->hRC))
    {
        sprintf (self->last_error, "TDS Error:: wglMakeCurrent failed");
        DEBUG_msg(self->last_error);
        return 0;
    }

    // Guardar self para que WndProc lo pueda obtener
    SetWindowLong(self->hWnd, GWL_USERDATA, (long)self);
    // Disable screen saver
    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 0, 0, 0);

    ShowWindow (self->hWnd, SW_SHOW);
    SetForegroundWindow (self->hWnd);
    SetFocus (self->hWnd);

    return 1;
}
//-------------------------------------------------------------------
int Wgl_CloseWindow(TGraphicsWgl* self)
{
    if (self->fullscreen)
    {
        ChangeDisplaySettings (NULL, 0); // Back to desktop
        ShowCursor (1);
    }
    if(!UnregisterClass(wgl_demo_name, self->hInstance))
    {
        DEBUG_msg("Class unregister failed!");
    }
    if (self->hRC)
    {
        wglMakeCurrent (0, 0);
        wglDeleteContext (self->hRC);
        self->hRC = 0;
    }
    // Kill hDC
    if (self->hDC)
    {
        ReleaseDC (self->hWnd, self->hDC);
        self->hDC = 0;
    }
    if (self->hWnd)
    {
        DestroyWindow (self->hWnd);
        self->hWnd = 0;
    }
    // Enable screen saver
    SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, 1, 0, 0);
    return 1;
}
//-------------------------------------------------------------------
int Wgl_UpdateWindow(TGraphicsWgl* self)
{
    MSG message;

    if (self->fade_a != 0.0f)
    {
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity ( );
        gluOrtho2D (0.0, 1.0, 0.0, 1.0);
        glMatrixMode (GL_MODELVIEW);
        glLoadIdentity ( );

        glDisable (GL_TEXTURE_2D);
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f (self->fade_r, self->fade_g, self->fade_b, self->fade_a);
        glRecti (0, 0, 1, 1);

        glDisable (GL_BLEND);
    }

    while (PeekMessage (&message, self->hWnd, 0, 0, PM_REMOVE))
    {
        // translate and dispatch
        TranslateMessage (&message);
        DispatchMessage  (&message);
    }

    if (self->active) {
        SwapBuffers (self->hDC);
        //		UpdateWindow(self->hWnd);
    }
    else
        //Sleep (1);
        WaitMessage ( );
    return 1;
}

//-------------------------------------------------------------------
char *Wgl_GetLastError (TGraphicsWgl* self)
{
    return self->last_error;
}

//-------------------------------------------------------------------
TKeyboard* Wgl_GetKeyboardSubsystem(TGraphicsWgl* self)
{
    return TdsKeyboard_New(self);
}

//-------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//
// TIMER CODE
//
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef TDS_TIMER

///////////////////////////////////////////////////////////////////////
// HIGH-RESOLUTION PERFORMANCE COUNTER
///////////////////////////////////////////////////////////////////////
#ifdef TDS_TIMER_HRPC
static double  _counter_frequency;
static __int64 _init_ticks[TDS_MAX_TIMERS];
static double  _timer_frequency[TDS_MAX_TIMERS];

int TDS_InitTimer (int timer_id, int freq)
{
    __int64 cnt_freq;
    // Get high-resolution performance counter frequency
    if (!QueryPerformanceFrequency ((LARGE_INTEGER*) &cnt_freq))
        return 0;
    _counter_frequency = (double) cnt_freq;
    // Get init counter ticks
    if (!QueryPerformanceCounter ((LARGE_INTEGER*) &_init_ticks[timer_id]))
        return 0;

    _timer_frequency[timer_id] = (double) freq;

    return 1;
}

double TDS_GetTimer (int timer_id)
{
    // get time
    __int64 ticks;
    QueryPerformanceCounter ((LARGE_INTEGER*) &ticks);

    // convert to seconds and return
    return (double) (ticks - _init_ticks[timer_id]) * _timer_frequency[timer_id] / (double) _counter_frequency;
}
#endif // TDS_TIMER_HRPC

///////////////////////////////////////////////////////////////////////
// SYSTEM TIMER
///////////////////////////////////////////////////////////////////////
#ifdef TDS_TIMER_SYSTEM
static DWORD _timer_counter_org[TDS_MAX_TIMERS];
static int   _timer_frequency[TDS_MAX_TIMERS];

int TDS_InitTimer (int timer_id, int freq)
{
    if ((timer_id < 0) || (timer_id >= TDS_MAX_TIMERS))
        return 0;
    _timer_counter_org[timer_id] = timeGetTime ( );
    _timer_frequency[timer_id]   = freq;
    return 1;
}

double TDS_GetTimer (int timer_id)
{
    DWORD diff_time;

    diff_time = timeGetTime ( ) - _timer_counter_org[timer_id];
    return (double) (diff_time * _timer_frequency[timer_id]) / 1000.0;
}
#endif // TDS_TIMER_SYSTEM

#endif // TDS_TIMER


#endif //WIN32
