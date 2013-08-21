#ifndef EPOPEIA_INTERNAL_H
#define EPOPEIA_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "epopeia.h"

struct Epopeia_t
{
    TGraphics* Graphics;
    TSound*    Sound;
    TKeyboard* Keyboard;
    int        MoveKeys;
    int        WarningLevel; // 0 = no warnings, 1 = warnings activated, 2 = warnings are errors
    MainMenuFunc MainMenu;
};

extern struct Epopeia_t g_Epopeia;


#ifdef __cplusplus
};
#endif

#endif
