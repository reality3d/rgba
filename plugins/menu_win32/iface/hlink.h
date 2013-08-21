//-------------------------------------------------------------------//
// HyperLink win32 gui control                                       //
//-------------------------------------------------------------------//
#ifndef __HLINK_H__
#define __HLINK_H__

#include <windows.h>
#include "tooltip.h"

typedef struct
{
    HWND      hWnd;
    HCURSOR   hCursor;               // Hand cursor
    COLORREF  colNormal, colOver;    // Link colors
    HFONT     hFont, hUnderlineFont; // Fonts;
    BOOL      isMouseOver;

    TToolTip *tooltip;
    char     *url;

    // Subclassing
    LONG      DefWindowData;
    WNDPROC   DefWindowProc;

    // Subclassing parent
    LONG      DefParentWindowData;
    WNDPROC   DefParentWindowProc;
} THyperLink;

THyperLink *HLINK_New (HWND hWnd, char *link,
                       COLORREF colNormal, COLORREF colOver);
void HLINK_SetTooltip (THyperLink *hlink, char *text);
void HLINK_Delete (THyperLink *hlink);

#endif // __HLINK_H__
