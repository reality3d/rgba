//-------------------------------------------------------------------//
// Button Control (win32 gui api)                                    //
//-------------------------------------------------------------------//
#ifndef __BUTTON_H__
#define __BUTTON_H__

#include <windows.h>

typedef struct
{
    HWND    hWnd;
    HCURSOR hCursor;
    int     hover;

    // Subclassing
    LONG      DefWindowData;
    WNDPROC   DefWindowProc;
} TButton;


TButton *BUTTON_New    (HWND hWnd);
void     BUTTON_Delete (TButton *button);

#endif // __BUTTON_H__
