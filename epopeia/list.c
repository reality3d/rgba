//-------------------------------------------------------------------
// LIST - Lista generica sobre la que construir listas
//
// Enlar/RGBA 2001
//
//-------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "critical.h"
#include "list.h"

#define DEBUG_VAR 0
#define DEBUG_MODULE "list"
#include "debug.h"
//-------------------------------------------------------------------
static int CompareDefault(void* Data1, void* Data2)
{
    return -1;
}
//-------------------------------------------------------------------
int AlwaysLesser(void* Data1, void* Data2)
{
    return -1;
}
//-------------------------------------------------------------------
int AlwaysGreater(void* Data1, void* Data2)
{
    return 1;
}
//-------------------------------------------------------------------
int CompareStringPtr(void* Data1, void* Data2)
{
    DEBUG_msg("CompareStringStr");
    DEBUG_msg_str("1", Data1);
    DEBUG_msg_str("2", Data2);
    return strcmp((char*)Data1, (char*)Data2);
}

//-------------------------------------------------------------------
TList* List_New   (FCompare* ComparationFunc)
{
    TList* List;

    List = (TList*)malloc(sizeof(TList));
    if(List == NULL)
        return NULL;

    if(ComparationFunc == NULL)
        List->Compare = CompareDefault;
    else
        List->Compare = ComparationFunc;

    List->First   = NULL;
    List->Current = NULL;

    return List;
}
//-------------------------------------------------------------------
void List_Delete(TList* List)
{
    ASSERT(List != NULL);
    List_Reset(List);
    while(!List_EOF(List))
        List_Pop(List);
    free(List);
}
//-------------------------------------------------------------------
void List_Insert(TList* List, void* Data)
{
    TListNode* Node;
    TListNode* NewNode;

    DEBUG_msg("Dentro Insert");
    ASSERT(List != NULL);
    NewNode = (TListNode*)malloc(sizeof(TListNode));
    ASSERT(NewNode != NULL);
    NewNode->Data = Data;
    DEBUG_msg("Check first");
    if(List->First == NULL)
    {
        NewNode->Next     = NULL;
        NewNode->Previous = NULL;
        List->First   = NewNode;
        return;
    }
    if(List->Compare(List->First->Data, Data) > 0)
    // Hay que meterlo en primer lugar
    {
        DEBUG_msg("Second in");
        NewNode->Previous = NULL;
        NewNode->Next     = List->First;
        List->First->Previous = NewNode;
        List->First       = NewNode;
        return;
    }

    // Habra que meter el nuevo nodo despues del Nodo que tengamos
    // al salir del siguiente bucle. (antes de nodo->next)
    DEBUG_msg("Somewhere in middle or final");
    Node = List->First;
    while(Node->Next != NULL && List->Compare(Node->Next->Data, Data) <= 0)
        Node = Node->Next;

    // Metemos el nodo en ultimo lugar o en medio
    NewNode->Next     = Node->Next;
    NewNode->Previous = Node;
    Node->Next        = NewNode;
    if(NewNode->Next != NULL)
        NewNode->Next->Previous = NewNode;
}
//-------------------------------------------------------------------
void  List_Drop  (TList* List, void* Data)
{
    TListNode* Nodo;
    TListNode* TmpNode;
    ASSERT(List != NULL);
    ASSERT(List->First != NULL);

    if(List->First->Data == Data)
    {
        TmpNode = List->First;
        if(List->First->Next != NULL)
            List->First->Next->Previous = NULL;
        List->First = List->First->Next;
        free(TmpNode);        
        return;
    }

    Nodo = List->First;
    while(Nodo->Next != NULL && Nodo->Next->Data != Data)
        Nodo = Nodo->Next;

    ASSERT(Nodo->Next != NULL);
    TmpNode = Nodo->Next;
    Nodo->Next = TmpNode->Next;
    if(Nodo->Next != NULL)
        Nodo->Next->Previous = Nodo;

    free(TmpNode);

}
//-------------------------------------------------------------------
void*  List_First (TList* List)
{
    ASSERT(List != NULL);
    ASSERT(List->First != NULL);
    return List->First->Data;
}
//-------------------------------------------------------------------
void List_DropFirst(TList* List)
{
    ASSERT(List != NULL);
    if(List->First != NULL)
    {
        TListNode* TmpNode = List->First;
        List->First = List->First->Next;
        if(List->First != NULL)
            List->First->Previous = NULL;
        free(TmpNode);
    }
}
//-------------------------------------------------------------------
// Manejo a modo de pila sin orden
//-------------------------------------------------------------------
void* List_Pop   (TList* List)
{
    void* Data;
    Data = List_First(List);
    List_DropFirst(List);
    return Data;
}
//-------------------------------------------------------------------
void List_Push  (TList* List, void* Data)
{
    TListNode* Node;
    TListNode* NewNode;
    ASSERT(List != NULL);
    
    Node = List->First;
    NewNode = (TListNode*)malloc(sizeof(TListNode));
    ASSERT(NewNode != NULL);

    NewNode->Data = Data;
    NewNode->Previous = NULL;
    NewNode->Next = Node;
    List->First = NewNode;
    if(Node != NULL)
        Node->Previous = NewNode;
    
}
//-------------------------------------------------------------------
// Funciones para recorrer la lista
//-------------------------------------------------------------------
void   List_Reset   (TList* List)
{
    if(List != NULL)
    {
        List->Current = List->First;
        List->BOF = 0;
    }
}
//-------------------------------------------------------------------
void   List_Next    (TList* List)
{
    ASSERT(List != NULL);
    ASSERT(List->Current != NULL);
    List->Current = List->Current->Next;
    List->BOF = 0;
}
//-------------------------------------------------------------------
void   List_Previous(TList* List)
{
    ASSERT(List != NULL);
    ASSERT(List->Current != NULL);
    List->Current = List->Current->Previous;
    
    List->BOF = List->Current == NULL;
}
//-------------------------------------------------------------------
void*  List_Current (TList* List)
{
    ASSERT(List != NULL);
    return List->Current->Data;
}
//-------------------------------------------------------------------
void   List_ChangeCurrent(TList* List, void* Data)
{
    ASSERT(List != NULL);
    List->Current->Data = Data;
}
//-------------------------------------------------------------------
void   List_DropCurrent(TList* List)
{
    TListNode* NodeToDrop;
    ASSERT(List != NULL);
    ASSERT(List->Current != NULL);

    NodeToDrop = List->Current;
    List->Current = NodeToDrop->Next;
    // List->Current ahora es el siguiente elemento en la lista
    if(List->Current != NULL)
        List->Current->Previous = NodeToDrop->Previous;

    // Si el nodo que hemos quitado era el primero
    // arreglar el puntero al primero
    if(List->First == NodeToDrop)
	List->First = NodeToDrop->Next;
    else // sino el del anterior!
        NodeToDrop->Previous->Next = NodeToDrop->Next;
    
    free(NodeToDrop);
}
//-------------------------------------------------------------------
int    List_IsLast  (TList* List)
{
    ASSERT(List != NULL);
    ASSERT(List->Current != NULL);
    return List->Current->Next == NULL;
}
//-------------------------------------------------------------------
int    List_EOF     (TList* List)
{
    ASSERT(List != NULL);
    return List->First == NULL || List->Current == NULL;
}
//-------------------------------------------------------------------
int    List_IsFirst (TList* List)
{
    ASSERT(List != NULL);
    return List->Current == List->First;
}
//-------------------------------------------------------------------
int    List_BOF     (TList* List)
{
    ASSERT(List != NULL);
    return List->BOF;
}
//-------------------------------------------------------------------
// Va al nodo especificado por Data. No tiene por que ser un nodo real,
// basta con que sea un nodo "preparado" para el ComparationFunction
//-------------------------------------------------------------------
void   List_GotoPos(TList* List, void* Data)
{
    ASSERT(List != NULL);
    List_Reset(List);
    while(!List_EOF(List) && List->Compare(List_Current(List), Data) < 0)
        List_Next(List);
}
//-------------------------------------------------------------------
// Data *tiene* que ser un dato real
void   List_Goto   (TList* List, void* Data)
{
    ASSERT(List != NULL);
    List_Reset(List);
    while(!List_EOF(List) && 0 != List->Compare(List_Current(List), Data))
        List_Next(List);
}
//-------------------------------------------------------------------
int  List_Length(TList* List)
{
    int Length = 0;
    List_Reset(List);
    while(!List_EOF(List))
    {
        Length++;
        List_Next(List);
    }
    return Length;
}
//-------------------------------------------------------------------
