//
// MOTION BLUR
//
//
//  Un solo render a textura (con varias texturas que cubran el area
// del viewport)
//   1. Dibujar las texturas con los frames antiguos (con el alpha-blend elegido)
//   2. Renderizar el nuevo frame a las texturas
//   3. Loop
//
#ifdef WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <glt/glt.h>
#include "mblur.h"


static void DivideQuads (TMBlur *self,
                         int x0, int y0,
                         int x1, int y1,
                         const int max_size)
{
    int new_w, new_h;
    int w, h, i;
    TQuad *quadlist_tmp;
    TQuad *quad;

    if ((x0 == x1) || (y0 == y1))
        return;

    w = x1 - x0;
    h = y1 - y0;

    for (i=31; i>=0; i--)
        if (w & (1 << i))
        {
            new_w = 1 << i;
            break;
        }

    for (i=31; i>=0; i--)
        if (h & (1 << i))
        {
            new_h = 1 << i;
            break;
        }

    if (new_w > max_size) new_w = max_size;
    if (new_h > max_size) new_h = max_size;

    // Add new quad
    quadlist_tmp = self->quadlist;
    self->quadlist = (TQuad *) calloc (self->num_quads + 1, sizeof (TQuad));
    assert (self->quadlist != NULL);
    memcpy (self->quadlist, quadlist_tmp, self->num_quads * sizeof (TQuad));
    free (quadlist_tmp);

    // Actual quad
    quad = &self->quadlist[self->num_quads];
    self->num_quads ++;

    quad->x0 = (GLint) x0;
    quad->y0 = (GLint) y0;
    quad->x1 = (GLint) (x0 + new_w);
    quad->y1 = (GLint) (y0 + new_h);

    DivideQuads (self,
                 x0 + new_w,
                 y0 + new_h,
                 x1,
                 y1,
                 max_size);

    DivideQuads (self,
                 x0 + new_w,
                 y0,
                 x1,
                 y0 + new_h,
                 max_size);

    DivideQuads (self,
                 x0,
                 y0 + new_h,
                 x0 + new_w,
                 y1,
                 max_size);
}


static void DYNAMIC_Init (TMBlur *self, GLint *viewport)
{
    int i, max_texture_size;
    TQuad *quad;

    self->vp_x0 = viewport[0];
    self->vp_y0 = viewport[1];
    self->vp_x1 = viewport[2] + self->vp_x0;
    self->vp_y1 = viewport[3] + self->vp_y0;

    // Get maximum texture size
    glGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_texture_size);

    // Delete quads & textures
    for (i=0; i<self->num_quads; i++)
        TEX_Delete (self->quadlist[i].texture);

    if (self->quadlist)
        free (self->quadlist);

    self->num_quads = 0;
    self->quadlist  = NULL;
    DivideQuads (self,
                 self->vp_x0, self->vp_y0,
                 self->vp_x1, self->vp_y1,
                 max_texture_size);

    for (i=0; i<self->num_quads; i++)
    {
        quad = &self->quadlist[i];

        // Texture
        quad->texture = TEX_New (quad->x1 - quad->x0,
                                 quad->y1 - quad->y0,
                                 32,
                                 NULL,
                                 0);
        TEX_SetFilter (GL_LINEAR, GL_LINEAR);
        TEX_SetWrap (GL_CLAMP, GL_CLAMP);
        TEX_SetEnvironment (GL_MODULATE);
    }
}


TMBlur *MBLUR_Init (void)
{
    TMBlur *self;
    int viewport[4];

    self = (TMBlur *) malloc (sizeof (TMBlur));
    if (!self)
        return 0;

    // Get viewport dimensions
    glGetIntegerv (GL_VIEWPORT, viewport);
    self->width  = viewport[2];
    self->height = viewport[3];

    self->num_quads = 0;
    self->quadlist  = NULL;
    DYNAMIC_Init (self, viewport);

    // Configure default parameters
    self->blend_src_factor = GL_SRC_ALPHA;
    self->blend_dst_factor = GL_ONE;
    self->alpha            = 0.9f;
    self->clear_old_frames = 1;

    return self;
}

void MBLUR_Clear (TMBlur *self)
{
    self->clear_old_frames = 1;
}

void MBLUR_Do (TMBlur *self)
{
    TQuad *quad;
    int    i, w, h;
    int    viewport[4];

    // Get viewport dimensions
        glGetIntegerv (GL_VIEWPORT, viewport);

    if ((viewport[0] != self->vp_x0) ||
        (viewport[1] != self->vp_y0) ||
        (viewport[2] != (self->vp_x1 - self->vp_x0)) ||
        (viewport[3] != (self->vp_y1 - self->vp_y0)))
    {
        DYNAMIC_Init (self, viewport);
    }

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ( );
    gluOrtho2D (self->vp_x0, self->vp_x1, self->vp_y0, self->vp_y1);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ( );

    glEnable (GL_TEXTURE_2D);
    glEnable (GL_BLEND);

    // Paint OLD textures
    if (!self->clear_old_frames)
    {

        glBlendFunc (self->blend_src_factor, self->blend_dst_factor);
        glColor4f (1.0f, 1.0f, 1.0f, self->alpha);

        for (i=0; i<self->num_quads; i++)
        {
            quad = &self->quadlist[i];

            TEX_Bind (quad->texture);
            TEX_SetEnvironment (GL_MODULATE);

            glBegin (GL_QUADS);
             glTexCoord2f (0, 1); glVertex2f (quad->x0, quad->y1);
             glTexCoord2f (1, 1); glVertex2f (quad->x1, quad->y1);
             glTexCoord2f (1, 0); glVertex2f (quad->x1, quad->y0);
             glTexCoord2f (0, 0); glVertex2f (quad->x0, quad->y0);
            glEnd ( );
        }
    }

    // Update textures
    for (i=0; i<self->num_quads; i++)
    {

        quad = &self->quadlist[i];
    
        TEX_Bind (quad->texture);

        glCopyTexSubImage2D (GL_TEXTURE_2D, 0,
                         0, 0,
                         quad->x0,
                         quad->y0,
                         quad->texture->width,
                         quad->texture->height);

        glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    glDisable (GL_TEXTURE_2D);
    glDisable (GL_BLEND);

    if (self->clear_old_frames)
        self->clear_old_frames = 0;
}

void MBLUR_Kill (TMBlur *self)
{
    int i;

    for (i=0; i<self->num_quads; i++)
        TEX_Delete (self->quadlist[i].texture);

    free (self->quadlist);
    free (self);
}
