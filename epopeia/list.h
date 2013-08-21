//-------------------------------------------------------------------
// LIST - Lista generica sobre la que construir listas
//
// Enlar/RGBA 2001
//
//-------------------------------------------------------------------
#ifndef LIST_H
#define LIST_H

// Si Orden(Data1) < Orden(Data2) devuelve <0
// Si Orden(Data1) = Orden(Data2) devuelve =0
// Sino, devuelve >0
typedef int FCompare(void* Data1, void* Data2);

FCompare AlwaysLesser;
FCompare AlwaysGreater;
FCompare CompareStringPtr;
typedef struct TListNode_t
{
    void* Data;
    struct TListNode_t* Next;
    struct TListNode_t* Previous;
} TListNode;

typedef struct
{
    TListNode* First;
    TListNode* Current;
    FCompare*  Compare;
    int        BOF;
} TList;

TList* List_New   (FCompare* ComparationFunc);
void   List_Delete(TList* List);
void   List_Insert(TList* List, void* Data);
void   List_Drop  (TList* List, void* Data);
void*  List_First (TList* List);
void   List_DropFirst(TList* List);
int    List_Length(TList* List);

// Manejo a modo de pila sin orden
void*  List_Pop   (TList* List);
void   List_Push  (TList* List, void* Data);

// Funciones para recorrer la lista
void   List_Reset   (TList* List);
void   List_Next    (TList* List);
void   List_Previous(TList* List);
void*  List_Current (TList* List);
void   List_ChangeCurrent(TList* List, void* Data);
// Quita el elemento actual y cambia al siguiente
void   List_DropCurrent(TList* List);
int    List_IsLast  (TList* List);
int    List_EOF     (TList* List);
int    List_IsFirst (TList* List);
int    List_BOF     (TList* List);
// Va al nodo especificado por Data. No tiene por que ser un nodo real,
// basta con que sea un nodo "preparado" para el ComparationFunction
void   List_GotoPos(TList* List, void* Data);
// Data tiene que ser un dato que este en la lista
// Si Data no esta en la lista, activa EOF
void   List_Goto   (TList* List, void* Data);

#endif
