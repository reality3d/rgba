//
// Cubes!
//

#include <windows.h>
#include <tds.h>
#include "fx.h"

//
// RESOLUTION
//
#define RES_X 640
#define RES_Y 480


//
// MAIN
//
//int main (void)              // Console Mode
int WinMainCRTStartup (void)   // Window Mode
{
    // Init OpenGL Window
    //if (!TDS_Init (RES_X, RES_Y, "Cube!", TDS_FULLSCREEN))
    if (!TDS_Init (RES_X, RES_Y, "Cube!", TDS_WINDOWED))
    {
        MessageBox (0, TDS_GetLastError ( ), "Error", MB_OK);
        return 1;
    }

    // Init FX
    if (!fx_init (RES_X, RES_Y))
    {
        MessageBox (0, "FX Init Error", "Error", MB_OK);
        return 1;
    }

    // Init Timer
    TDS_InitTimer (0, 60);

    // Main loop
    while (1)
    {
        fx_do (TDS_GetTimer (0));
        TDS_Update ( );
    }

    TDS_Close ( );
    return 0;
}
