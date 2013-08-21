#include "pos.h"

static int g_Pos_OrderSize = 64;

//-------------------------------------------------------------------
TPos Pos_Add(TPos* Pos1, TPos* Pos2)
{
    TPos Temp;

    Temp.Row = Pos1->Row + Pos2->Row;
    Temp.Order = Pos1->Order + Pos2->Order;
    return Temp;
}

//-------------------------------------------------------------------
TPos Pos_Substract(TPos* Pos1, TPos* Pos2)
{
    TPos Temp;

    Temp.Row = Pos1->Row - Pos2->Row;
    Temp.Order = Pos1->Order - Pos2->Order;
    while(Temp.Row < 0)
    {
        Temp.Row += g_Pos_OrderSize;
        Temp.Order --;
    }

    return Temp;
}
//-------------------------------------------------------------------
void Pos_SetOrderSize(int OrderSize)
{
    g_Pos_OrderSize = OrderSize;
}
//-------------------------------------------------------------------
void Pos_Normalize(TPos* Temp)
{
    while(Temp->Row < 0)
    {
        Temp->Row += g_Pos_OrderSize;
        Temp->Order --;
    }
}
//-------------------------------------------------------------------
int Pos_Compare(void* Data1, void* Data2)
{
    TPos* Pos1 = (TPos*)Data1;
    TPos* Pos2 = (TPos*)Data2;

    if(Pos1->Order < Pos2->Order)
        return -1;
    if(Pos1->Order > Pos2->Order)
        return 1;
    if(Pos1->Row < Pos2->Row)
        return -1;
    if(Pos1->Row > Pos2->Row)
        return 1;

    return 0;
}

