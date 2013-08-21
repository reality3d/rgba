#ifndef POS_H
#define POS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int Order;
    int Row;
} TPos;

TPos Pos_Add(TPos* Pos1, TPos* Pos2);
TPos Pos_Substract(TPos* Pos1, TPos* Pos2);
void Pos_SetOrderSize(int OrderSize);
void Pos_Normalize(TPos* Pos);
int  Pos_Compare(void* Pos1, void* Pos2);
#ifdef __cplusplus
};
#endif

#endif
