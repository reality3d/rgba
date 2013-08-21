//-------------------------------------------------------------------//
// Button Control (win32 gui api)                                    //
//-------------------------------------------------------------------//
#include <windows.h>
#include <windowsx.h>
#include <assert.h>
#include <commctrl.h>
#include <string.h>
#include <malloc.h>
#include "button.h"

void BTN_DrawText (HDC hDC, char *caption, COLORREF text_color, LPRECT rect)
{
    COLORREF prevColor;
    prevColor = SetTextColor (hDC, text_color);
    SetBkMode (hDC, TRANSPARENT);
    DrawText (hDC, caption, strlen (caption), rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    SetTextColor (hDC, prevColor);
}

void DrawLine (HDC hDC, long left, long top, long right, long bottom, COLORREF color)
{
    HPEN newPen, oldPen;
    newPen = CreatePen (PS_SOLID, 1, color);
    oldPen = (HPEN) SelectObject (hDC, (HGDIOBJ)newPen);

    MoveToEx (hDC, left, top, NULL);
    LineTo (hDC, right, bottom);
    SelectObject (hDC, (HGDIOBJ)oldPen);
    DeleteObject (newPen);
}

void BTN_DrawFrame (HDC hDC, LPRECT lpRect)
{
    // Borde de fuera
    DrawLine (hDC, lpRect->left-1,  lpRect->top-1,  lpRect->right,  lpRect->top-1, 0);
    DrawLine (hDC, lpRect->left-1,  lpRect->bottom, lpRect->right,  lpRect->bottom, 0);
    DrawLine (hDC, lpRect->left-1,  lpRect->top-1,  lpRect->left-1, lpRect->bottom, 0);
    DrawLine (hDC, lpRect->right,   lpRect->top-1,  lpRect->right,  lpRect->bottom+1, 0);

    // Borde de dentro (borra 1 pixel del cuadro original)
    DrawLine (hDC, lpRect->left,    lpRect->top,      lpRect->right-1, lpRect->top,      0xFFFFFF);
    DrawLine (hDC, lpRect->left,    lpRect->bottom-1, lpRect->right-1, lpRect->bottom-1, 0xFFFFFF);
    DrawLine (hDC, lpRect->left,    lpRect->top,      lpRect->left,    lpRect->bottom-1, 0xFFFFFF);
    DrawLine (hDC, lpRect->right-1, lpRect->top,      lpRect->right-1, lpRect->bottom, 0xFFFFFF);

}

static void BUTTON_OnPaint (TButton *b)
{
    HDC              hDC;
    LPDRAWITEMSTRUCT lpDis;
    UINT             state;
    RECT             focusRect, btnRect;
    char             btn_caption[512];
    COLORREF         prevColor;
    HWND             btn_hWnd;

    btn_hWnd = b->hWnd;

    GetClientRect (b->hWnd, &btnRect);
    GetClientRect (b->hWnd, &focusRect);

    // Set the focus rectangle to just past the border decoration
    focusRect.left   += 1;
    focusRect.right  -= 1;
    focusRect.top    += 1;
    focusRect.bottom -= 1;

    // Retrieve the button's caption
    GetWindowText (btn_hWnd, btn_caption, 512);


    // Draw and label the button using draw methods
    hDC = GetDC (btn_hWnd);

    if (b->hover)
        //FillRect (hDC, &btnRect, CreateSolidBrush (RGB (245, 90, 90)));
        //FillRect (hDC, &btnRect, CreateSolidBrush (RGB (237, 90, 92)));
        FillRect (hDC, &btnRect, GetSysColorBrush (COLOR_BTNHIGHLIGHT));
    else
        FillRect (hDC, &btnRect, GetSysColorBrush (COLOR_BTNFACE));


    {
        HFONT hOldFont, hFont;

        //hFont = GetStockObject (DEFAULT_GUI_FONT);
                hFont = CreateFont (14, //-MulDiv(12, GetDeviceCaps(hDC, LOGPIXELSY), 72),
                            0,
                            0,
                            0,
                            FW_BOLD,
                            0, //TRUE,  // Italic
                            0,          // Underline
                            0,
                            0,
                            0,
                            0,
                            ANTIALIASED_QUALITY,
                            //CLEARTYPE_QUALITY,
                            0,
                            "Verdana");
                            //"Times New Roman");
        hOldFont = SelectObject (hDC, hFont);
        BTN_DrawText (hDC, btn_caption, 0x0000000, &btnRect);
        SelectObject (hDC, hOldFont);
        DeleteObject (hFont);
    }

    // Dibujamos el borde
    BTN_DrawFrame (hDC, &btnRect);

    ValidateRect (b->hWnd, &btnRect);
}

//
// Button - Window Procedure
//
static LRESULT CALLBACK BUTTON_WindowProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TButton *b;
    DWORD position;
    POINT point;
    RECT  rect;

    // Get window data (our button structure)
    b = (TButton *) GetWindowLong (hWnd, GWL_USERDATA);

    switch (message)
    {
    case WM_MOUSEMOVE:
        if ((!b->hover) && (wParam == 0))
        {
            b->hover = 1;
            SetTimer (b->hWnd, 1, 1, NULL);
            InvalidateRect (hWnd, NULL, TRUE);
        }
        else if (b->hover && (wParam != 0))
        {
            b->hover = 0;
            InvalidateRect (hWnd, NULL, TRUE);
        }
        break;

    case WM_TIMER:
        position = GetMessagePos ( );
        point.x = GET_X_LPARAM (position);
        point.y = GET_Y_LPARAM (position);

        GetWindowRect (hWnd, &rect);
        if (!PtInRect (&rect, point))
        {
            b->hover = 0;
            KillTimer (b->hWnd, 1);
            InvalidateRect (b->hWnd, NULL, TRUE);
        }
        break;

    case WM_SETCURSOR:
        SetCursor (b->hCursor);
        return TRUE;

    case WM_PAINT:
        BUTTON_OnPaint (b);
        return TRUE;
    }

    return CallWindowProc (b->DefWindowProc, hWnd, message, wParam, lParam);
}


TButton *BUTTON_New (HWND hWnd)
{
    TButton *button = (TButton *) malloc (sizeof (TButton));
    assert (button != NULL);
    memset (button, 0, sizeof (TButton));

    button->hWnd    = hWnd;
    button->hCursor = LoadCursor (NULL, IDC_HAND);

    // Window sub classing
    // Set our window proc and data (and save old ones)
    button->DefWindowProc = (WNDPROC) SetWindowLong (hWnd, GWL_WNDPROC,  (LONG) BUTTON_WindowProc);
    button->DefWindowData = (LONG)    SetWindowLong (hWnd, GWL_USERDATA, (LONG) button);

    return button;
}

void BUTTON_Delete (TButton *button)
{
    free (button);
}
