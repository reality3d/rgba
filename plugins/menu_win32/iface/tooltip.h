//-------------------------------------------------------------------//
// ToolTip Control (win32 gui api)                                   //
//-------------------------------------------------------------------//
#ifndef __TOOLTIP_H__
#define __TOOLTIP_H__

#include <windows.h>
#include <commctrl.h>

typedef struct
{
    HWND     hWnd;
    TOOLINFO ti;
} TToolTip;

TToolTip *TOOLTIP_New    (HWND hWnd, char *text);
void      TOOLTIP_Delete (TToolTip *tt);

#endif // __TOOLTIP_H__
