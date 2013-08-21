// --------------------------------------------------------------------
// SCROLLFONT . epsilum & utopian. March 2003
// --------------------------------------------------------------------
#ifndef __SCROLLFONT_H__
#define __SCROLLFONT_H__

#include <glt/glt.h>
#include "fonts.h"

typedef struct
{
    TFont *font;
    int lineas_texto[3];
    char **texto[3];
    int max_size[3];
    int blend;
    int flags;
    float y;
    float avance;
    int current;
    int changing; // Define el estado de cambio; 0: normal; 1: fadeout para cambiar; 2: fade de vuelta
} TScroll;


TScroll *SCROLLFont_New       (char *text1,char *text2,char *text3,int interspace, int space, char *filename, char *filename_nfo);
void     SCROLLFont_Do        (TScroll *scroll,float elapsed);
void     SCROLLFont_Delete    (TScroll *scroll);
void SCROLLFont_SetScale (TScroll *self, float scalex,float scaley);

#endif // __SCROLLFONT_H__
