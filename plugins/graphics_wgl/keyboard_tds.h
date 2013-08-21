#ifndef KEYBOARD_TDS_H
#define KEYBOARD_TDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <epopeia/keyboard.h>
#include "graphics_wgl.h"

typedef struct TKeyboardTds_t
{
    int  (*Init)       (struct TKeyboardTds_t* self);
    void (*End)        (struct TKeyboardTds_t* self);
    int  (*ExitPressed)(struct TKeyboardTds_t* self);
    int  (*KeyPressed) (struct TKeyboardTds_t* self); // <0 no, >0 keycode
    int  (*Pressed)    (struct TKeyboardTds_t* self, int KeyCode);
    void (*Delete)     (struct TKeyboardTds_t* self);
    // variables propias
    TGraphicsWgl* Tds;

} TKeyboardTds;

TKeyboard* TdsKeyboard_New(TGraphicsWgl* Tds);

#ifdef __cplusplus
};
#endif

#endif
