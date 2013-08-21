#ifndef __DLG_MAIN__H__
#define __DLG_MAIN__H__

#include <windows.h>

typedef struct
{
    int res_x, res_y;  // Window resolution
    int windowed;      // 1: windowed, 0: fullscreen
} TDialogData;


int DLG_Main (HINSTANCE hInstance, TDialogData *dlg_data, const char *DemoName);


#endif // __DLG_MAIN__H__
