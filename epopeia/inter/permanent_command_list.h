#ifndef PERMANENT_COMMAND_LIST_H
#define PERMANENT_COMMAND_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../pos.h"
#include "../list.h"
#include "../compiler/arglist.h"
#include "../compiler/type_table.h"
#include "../sound.h"

int  PermanentCommandList_Init(void);
void PermanentCommandList_End (void);

void PermanentCommandList_InsertCommands(TPos pos_start, TPos pos_end, TPos frequency, TList* command_list);
void PermanentCommandList_RemoveCommands(TPos pos_end, TPos frequency, TList* command_list);
void PermanentCommandList_RemoveCommandsAll(void);
int  PermanentCommandList_Exec(TSoundContext* SC);


#ifdef __cplusplus
};
#endif

#endif // PERMANENT_COMMAND_H
