#ifndef KEYBOARD_H
#define KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#define KEY_CTRL  'c'
//#define KEY_RIGHT 'r'
//#define KEY_LEFT  'l'

#define KEY_LEFT  37
#define KEY_RIGHT 39
#define KEY_SPACE ' '


typedef struct TKeyboard_t
{
    int  (*Init)(struct TKeyboard_t* self);
    void (*End) (struct TKeyboard_t* self);
    int  (*ExitPressed)(struct TKeyboard_t* self);
    int  (*KeyPressed) (struct TKeyboard_t* self); // <0 no, >0 keycode
    int  (*Pressed)(struct TKeyboard_t* self, int KeyCode);
    void (*Delete) (struct TKeyboard_t* self);

} TKeyboard;

#ifdef __cplusplus
};
#endif

#endif

