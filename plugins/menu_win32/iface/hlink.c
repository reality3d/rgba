//-------------------------------------------------------------------//
// HyperLink win32 gui control                                       //
//-------------------------------------------------------------------//
#include <windows.h>
#include <windowsx.h>
#include <assert.h>
#include <malloc.h>
#include "hlink.h"


//
// Progress Bar - Window Procedure
//
static LRESULT CALLBACK HLINK_WindowProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static THyperLink *hlink;
    static DWORD position;
    static RECT  rect;
    static POINT point;

    // Get window data (our hyperlink structure)
    hlink = (THyperLink *) GetWindowLong (hWnd, GWL_USERDATA);

    switch (message)
    {
    case WM_MOUSEMOVE:
        if (GetCapture ( ) != hWnd)
        {
            hlink->isMouseOver = TRUE;
            SendMessage (hWnd, WM_SETFONT, (WPARAM) hlink->hUnderlineFont, FALSE);
            InvalidateRect (hWnd, NULL, FALSE);
            SetCapture (hWnd);
        }
        else
        {
            GetWindowRect (hWnd, &rect);
            point.x = LOWORD (lParam);
            point.y = HIWORD (lParam);
            ClientToScreen (hWnd, &point);

            if (!PtInRect(&rect, point))
            {
                hlink->isMouseOver = FALSE;
                SendMessage(hWnd, WM_SETFONT, (WPARAM) hlink->hFont, FALSE);
                InvalidateRect (hWnd, NULL, FALSE);
                ReleaseCapture ( );
            }
        }
        break;

        // ...
    case WM_CAPTURECHANGED:
        if (hlink->isMouseOver)
        {
            hlink->isMouseOver = FALSE;
            SendMessage(hWnd, WM_SETFONT, (WPARAM) hlink->hFont, FALSE);
            InvalidateRect (hWnd, NULL, FALSE);
        }
        break;

    case WM_LBUTTONDOWN:
        if (hlink->url)
            ShellExecute (hWnd, "open", hlink->url, NULL, NULL, SW_SHOWNORMAL);
        break;

    case WM_SETCURSOR:
        SetCursor (hlink->hCursor);
        return TRUE;
    }

    return CallWindowProc (hlink->DefWindowProc, hWnd, message, wParam, lParam);
}

static LRESULT CALLBACK HLINK_ParentWindowProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static THyperLink *hlink;

    // Get window data (our hyperlink structure)
    hlink = (THyperLink *) GetWindowLong (hWnd, GWL_USERDATA);

    switch (message)
    {
    case WM_CTLCOLORSTATIC:
        if (hlink->hWnd == (HWND) lParam)
        {
            LRESULT lr = CallWindowProc (hlink->DefParentWindowProc, hWnd, message, wParam, lParam);

            if (hlink->isMouseOver)
                SetTextColor ((HDC) wParam, hlink->colOver);
            else
                SetTextColor ((HDC) wParam, hlink->colNormal);

            return lr;
        }
        break;
    }

    return CallWindowProc (hlink->DefParentWindowProc, hWnd, message, wParam, lParam);
}



THyperLink *HLINK_New (HWND hWnd, char *linkurl, COLORREF colNormal, COLORREF colOver)
{
    THyperLink *hlink = (THyperLink *) malloc (sizeof (THyperLink));
    LOGFONT     lf;

    assert (hlink != NULL);
    memset (hlink, 0, sizeof (THyperLink));

    hlink->hWnd = hWnd;

    // Load hand cursor
    hlink->hCursor = LoadCursor (NULL, IDC_HAND);
    if (!hlink->hCursor)
        return NULL;

    hlink->url = _strdup (linkurl);

    // Load default font
    hlink->hFont = (HFONT) SendMessage (hWnd, WM_GETFONT, 0, 0);
    GetObject (hlink->hFont, sizeof (lf), &lf);
    lf.lfUnderline = (BYTE) TRUE;
    //lf.lfWeight    = FW_BOLD;
    //lf.lfQuality   = ANTIALIASED_QUALITY;
    hlink->hUnderlineFont = CreateFontIndirect (&lf);

    // Colors
    hlink->colNormal = colNormal;
    hlink->colOver   = colOver;

    // Window sub classing
    // Set our window proc and data (and save old ones)
    hlink->DefWindowProc = (WNDPROC) SetWindowLong (hWnd, GWL_WNDPROC,  (LONG) HLINK_WindowProc);
    hlink->DefWindowData = (LONG)    SetWindowLong (hWnd, GWL_USERDATA, (LONG) hlink);

    // Window sub classing parent window
    hlink->DefParentWindowProc = (WNDPROC) SetWindowLong (GetParent (hWnd), GWL_WNDPROC,  (LONG) HLINK_ParentWindowProc);
    hlink->DefParentWindowData = (LONG)    SetWindowLong (GetParent (hWnd), GWL_USERDATA, (LONG) hlink);

    return hlink;
}


void HLINK_SetTooltip (THyperLink *hlink, char *text)
{
    hlink->tooltip = TOOLTIP_New (hlink->hWnd, text);
}


void HLINK_Delete (THyperLink *hlink)
{
    if (hlink->tooltip)
        TOOLTIP_Delete (hlink->tooltip);

    free (hlink);
}
