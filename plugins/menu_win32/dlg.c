///////////////////////////////////////////////////////////////////////
//
// navmo'02 by rgba  -  main dialog window
//
///////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <commctrl.h>
#include "res/resource.h"
#include "dlg.h"
#include "iface/button.h"
#include "iface/hlink.h"


// Dialog vars
static int g_ResX, g_ResY;
static int g_Windowed;
static const char* g_DemoName;
static char* g_GroupBoxTitle;


// Globals
static HINSTANCE g_hInstance;
static UINT dynamic_timer_id;

// Cursor
static HCURSOR hCursorStandard = NULL;
static HCURSOR hCursorMove     = NULL;

// Buttons
static TButton *button_ok;
static TButton *button_cancel;

// Hyper link
static THyperLink *hlink;


int HANDLE_OnInitDialog (HWND hWnd)
{
    HWND hCombo;

    // Set window title
    SetWindowText (hWnd, g_DemoName);

    // Set window icon
    SendMessage (hWnd, WM_SETICON,
                 (WPARAM) ICON_BIG,
                 (LPARAM) LoadIcon (g_hInstance, MAKEINTRESOURCE (IDI_RGBAICON)));

    // Load cursors
    hCursorStandard = LoadCursor (NULL, IDC_ARROW);
    hCursorMove     = LoadCursor (NULL, IDC_SIZEALL);

    // Fill combo box
    hCombo = GetDlgItem (hWnd, IDC_COMBO1);
    ComboBox_ResetContent (hCombo);
    ComboBox_InsertString (hCombo, 0, "640x480");
    ComboBox_InsertString (hCombo, 1, "800x600");
    ComboBox_InsertString (hCombo, 2, "1024x768");
    ComboBox_SetCurSel    (hCombo, 1);

    // Init buttons
    button_ok     = BUTTON_New (GetDlgItem (hWnd, IDOK));
    button_cancel = BUTTON_New (GetDlgItem (hWnd, IDCANCEL));

    // Hyper link
    hlink = HLINK_New (GetDlgItem (hWnd, IDC_RGBA_LINK),
                       "http://www.rgba.org",
                       RGB (0x00, 0x00, 0x00),
                       RGB (0xFF, 0x00, 0x00));
    HLINK_SetTooltip (hlink, "visit our website!");

    // Set demo name in groupbox
    //    GetDlgItem(hWnd, IDC_STATIC)
    g_GroupBoxTitle = malloc(strlen(g_DemoName)+100);
    sprintf(g_GroupBoxTitle, " [ %s ] ", g_DemoName);
    SetDlgItemText(hWnd, IDC_GROUPBOX, g_GroupBoxTitle);
    return 1;
}

static void Shutdown (void)
{
    BUTTON_Delete (button_ok);
    BUTTON_Delete (button_cancel);
    HLINK_Delete  (hlink);
}

//
// DLG Window Procedure Definition
//
static LRESULT CALLBACK DLG_WndProc (HWND hWnd,
                                     UINT message,
                                     WPARAM wParam,
                                     LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        if (!HANDLE_OnInitDialog (hWnd))
            EndDialog (hWnd, 0);
        return FALSE;
        break;

    case WM_LBUTTONDOWN:
        SetCursor (hCursorMove);
        SendMessage (hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        break;

    case WM_LBUTTONUP:
        SetCursor (hCursorStandard);
        break;

    case WM_COMMAND:
        switch (LOWORD (wParam))
        {
        case IDOK:
            KillTimer (NULL, dynamic_timer_id);

            // Get windowed flag
            g_Windowed = (Button_GetCheck (GetDlgItem (hWnd, IDC_WINDOWED)) == BST_CHECKED);

            // Get resolution
            switch (ComboBox_GetCurSel (GetDlgItem (hWnd, IDC_COMBO1)))
            {
            case 0: g_ResX =  640; g_ResY = 480; break;
            case 1: g_ResX =  800; g_ResY = 600; break;
            case 2: g_ResX = 1024; g_ResY = 768; break;
            }

            Shutdown ( );
            EndDialog (hWnd, 1);
            break;

        case IDCANCEL:
            Shutdown ( );
            EndDialog (hWnd, 0);
            break;
        }
        break;

    default:
        // We didn't process the message, so let windows process it...
        return FALSE;
        break;
    }

    // We processed the dialog's messages
    return TRUE;
}



///////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////
int DLG_Main (HINSTANCE hInstance, TDialogData *dd, const char *DemoName)
{
    BOOL drag_full_window;
    int  ret;

    g_DemoName = DemoName;
    g_hInstance = hInstance;
    InitCommonControls ( );

    // Get state
    SystemParametersInfo (SPI_GETDRAGFULLWINDOWS, 0, &drag_full_window, 0);

    // Set TRUE!
    SystemParametersInfo (SPI_SETDRAGFULLWINDOWS, TRUE, NULL, 0);

    ret = DialogBox (hInstance,
                     (LPCTSTR) MAKEINTRESOURCE (IDD_DIALOG1),
                     NULL,
                     (DLGPROC) DLG_WndProc);

    // Restore state
    SystemParametersInfo (SPI_SETDRAGFULLWINDOWS, drag_full_window, NULL, 0);

    if (ret && (g_ResX != 0) && (g_ResY != 0))
    {
        dd->res_x    = g_ResX;
        dd->res_y    = g_ResY;
        dd->windowed = g_Windowed;
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
int WinMain (HINSTANCE hInstance,
             HINSTANCE hPrevInstance,
             LPSTR     lpCmdLine,
             int       nCmdShow)
{
    TDialogData dd;

    DLG_Main (hInstance, &dd);
}
*/
