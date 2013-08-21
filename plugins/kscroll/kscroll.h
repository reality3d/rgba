#ifndef KSCROLL_H
#define KSCROLL_H

#include <glt/glt.h>

typedef struct TKScroll_t
{
    TTexture *texture;
    float posx,posy;
    float scalex,scaley;
} TKScroll;

TKScroll *KScroll_New(char *filename);

void KScroll_Kill  (TKScroll* self);

#endif