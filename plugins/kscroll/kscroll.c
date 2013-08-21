#ifdef WIN32
 #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <glt/glt.h>
#include <math.h>
#include <malloc.h>

#include "kscroll.h"


TKScroll* KScroll_New(char *filename)
{
    TKScroll* self;

    self = malloc(sizeof(TKScroll));
    self->texture=CACHE_LoadTexture (filename,0);  // Nada de mipmaps!
    TEX_SetFilter ( GL_LINEAR,GL_LINEAR);
    TEX_SetWrap (GL_CLAMP, GL_CLAMP);

    self->posx = 0.5f;
    self->posy = 0.5f;
    self->scalex = self->scaley = 1.0f;

    return self;
}


void KScroll_Kill  (TKScroll* self)
{
	int i;
	
	CACHE_FreeTexture (self->texture);
}
