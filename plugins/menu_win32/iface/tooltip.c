//-------------------------------------------------------------------//
// ToolTip Control (win32 gui api)                                   //
//-------------------------------------------------------------------//
#include <windows.h>
#include <commctrl.h>
#include <assert.h>
#include <malloc.h>
#include "tooltip.h"


TToolTip *TOOLTIP_New (HWND hWnd, char *text)
{
    TToolTip *tt;
    RECT      rect;
    INITCOMMONCONTROLSEX iccex;
    unsigned int uid = 0;

    tt = (TToolTip *) malloc (sizeof (TToolTip));
    assert (tt != NULL);
    memset (tt, 0, sizeof (TToolTip));

    // Initialize common controls
    iccex.dwICC  = ICC_WIN95_CLASSES;
    iccex.dwSize = sizeof (INITCOMMONCONTROLSEX);
    InitCommonControlsEx (&iccex);

    tt->hWnd = CreateWindowEx (WS_EX_TOPMOST,
                               TOOLTIPS_CLASS,
                               NULL,
                               WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               hWnd,
                               NULL,
                               NULL, // hInstance
                               NULL);

    SetWindowPos (tt->hWnd,
                  HWND_TOPMOST,
                  0,
                  0,
                  0,
                  0,
                  SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    GetClientRect (hWnd, &rect);

    // Initialize members of the toolinfo structure
    tt->ti.cbSize   = sizeof (TOOLINFO);
    tt->ti.uFlags   = TTF_SUBCLASS;
    tt->ti.hwnd     = hWnd;
    tt->ti.hinst    = NULL; // ghThisInstance;
    tt->ti.uId      = uid;
    tt->ti.lpszText = text;

    // ToolTip control will cover the whole window
    tt->ti.rect.left   = rect.left;
    tt->ti.rect.top    = rect.top;
    tt->ti.rect.right  = rect.right;
    tt->ti.rect.bottom = rect.bottom;

    SendMessage (tt->hWnd, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &tt->ti);

    return tt;
}

void TOOLTIP_SetWidth (TToolTip *tt, int w)
{
    SendMessage (tt->hWnd, TTM_SETMAXTIPWIDTH, 0, w);
}

void TOOLTIP_Delete (TToolTip *tt)
{
    DestroyWindow (tt->hWnd);

    free (tt);
}
