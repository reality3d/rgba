#ifndef INTER_H
#define INTER_H

#ifdef __cplusplus
extern "C" {
#endif

int  Inter_Init(void);
void Inter_End(void);
void Inter_RunDemo(void);
void Inter_AddObjectToRenderList(void* Object, long Depth);
void Inter_RemoveObjectFromRenderList(void* Object);
void Inter_AddCommandsToPermanentList(TPos start, TPos end, TPos step, TList* commands);
void Inter_RemoveCommandsFromPermanentList(TPos end, TPos step, TList* commands);

#ifdef __cplusplus
};
#endif

#endif
