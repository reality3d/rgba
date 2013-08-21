#ifdef WIN32

#include <assert.h>
#include <malloc.h>
#include "keyboard_tds.h"
#define DEBUG_MODULE "keyboard_tds"
#define DEBUG_VAR 0
#include <epopeia/debug.h>

int  TdsKeyboard_Init       (struct TKeyboardTds_t* self);
void TdsKeyboard_End        (struct TKeyboardTds_t* self);
int  TdsKeyboard_ExitPressed(struct TKeyboardTds_t* self);
int  TdsKeyboard_KeyPressed (struct TKeyboardTds_t* self); // <0 no, >0 keycode
int  TdsKeyboard_Pressed    (struct TKeyboardTds_t* self, int KeyCode);
void TdsKeyboard_Delete     (struct TKeyboardTds_t* self);


//-------------------------------------------------------------------
TKeyboard* TdsKeyboard_New(TGraphicsWgl* Tds)
{
    TKeyboardTds* self;

    self = malloc(sizeof(TKeyboardTds));
    assert(self != NULL);
    memset(self, 0, sizeof(TKeyboardTds));
    
    self->Init = TdsKeyboard_Init;
    self->End  = TdsKeyboard_End;
    self->ExitPressed = TdsKeyboard_ExitPressed;
    self->KeyPressed  = TdsKeyboard_KeyPressed;
    self->Pressed     = TdsKeyboard_Pressed;
    self->Delete      = TdsKeyboard_Delete;
    self->Tds         = Tds;

    return (TKeyboard*)self;
}

//-------------------------------------------------------------------
int  TdsKeyboard_Init(struct TKeyboardTds_t* self)
{
    return 0;
}

//-------------------------------------------------------------------
void TdsKeyboard_End        (struct TKeyboardTds_t* self)
{
    // nothing
}

//-------------------------------------------------------------------
int  TdsKeyboard_ExitPressed(struct TKeyboardTds_t* self)
{
    DEBUG_msg("Here TDS keyb exit_pressed");
    return self->Tds->ExitPressed(self->Tds);
}

//-------------------------------------------------------------------
int  TdsKeyboard_KeyPressed (struct TKeyboardTds_t* self)  // <0 no, >0 keycode
{
    return self->Tds->GetKey(self->Tds);
}
//-------------------------------------------------------------------
int  TdsKeyboard_Pressed (struct TKeyboardTds_t* self, int KeyCode)
{
    return self->Tds->IsPressed(self->Tds, KeyCode);
}
//-------------------------------------------------------------------
void TdsKeyboard_Delete    (struct TKeyboardTds_t* self)
{
	free(self);
}
//-------------------------------------------------------------------

#endif //WIN32
