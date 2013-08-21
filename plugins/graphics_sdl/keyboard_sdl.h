#ifndef KEYBOARD_SDL_H
#define KEYBOARD_SDL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <epopeia/keyboard.h>

typedef struct TKeyboardSdl_t
{
    int  (*Init)       (struct TKeyboardSdl_t* self);
    void (*End)        (struct TKeyboardSdl_t* self);
    int  (*ExitPressed)(struct TKeyboardSdl_t* self);
    int  (*KeyPressed) (struct TKeyboardSdl_t* self); // <0 no, >0 keycode
    int  (*Pressed)    (struct TKeyboardSdl_t* self, int KeyCode);
    void (*Delete)     (struct TKeyboardSdl_t* self);
    // variables propias
    int exit_req;
    int key;

} TKeyboardSdl;

TKeyboard* Keyboard_Sdl_New(void);

#ifdef __cplusplus
};
#endif

#endif

