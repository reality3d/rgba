//
// Canal +
//

#ifdef WIN32
#include <windows.h>
#endif
#include <assert.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <glt/glt.h>
#include "canalp.h"


static void DivideQuads (TCanalPlus *self,
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


// mode: 0 - textured
//       1 - no textured
void DrawQuad (TQuad *quad)
{
    GLboolean textured;

    glGetBooleanv (GL_TEXTURE_2D, &textured);

    if (textured)
    {
        glBegin (GL_QUADS);
         glTexCoord2f (0, 1); glVertex2f (quad->x0, quad->y1);
         glTexCoord2f (1, 1); glVertex2f (quad->x1, quad->y1);
         glTexCoord2f (1, 0); glVertex2f (quad->x1, quad->y0);
         glTexCoord2f (0, 0); glVertex2f (quad->x0, quad->y0);
        glEnd ( );
    }
    else
    {
        glBegin (GL_QUADS);
         glVertex2f (quad->x0, quad->y1);
         glVertex2f (quad->x1, quad->y1);
         glVertex2f (quad->x1, quad->y0);
         glVertex2f (quad->x0, quad->y0);
        glEnd ( );
    }
}

TCanalPlus *CANALP_Init (void)
{
    TCanalPlus *self;
    TQuad *quad;
    int max_texture_size;
    int viewport[4];
    int i;

    self = (TCanalPlus *) malloc (sizeof (TCanalPlus));
    if (!self)
        return 0;

    // Get viewport dimensions
    glGetIntegerv (GL_VIEWPORT, viewport);

    // Get maximum texture size
    glGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_texture_size);
    //max_texture_size = 256;

    self->num_quads = 0;
    self->quadlist  = NULL;
    //DivideQuads (self, 0, 0, self->width, self->height, max_texture_size);

    self->vp_x0  = viewport[0];
    self->vp_y0  = viewport[1];
    self->vp_x1  = viewport[2] + self->vp_x0;
    self->vp_y1  = viewport[3] + self->vp_y0;
    self->width  = viewport[2];
    self->height = viewport[3];
    DivideQuads (self,
                 self->vp_x0, self->vp_y0,
                 self->vp_x1, self->vp_y1,
                 max_texture_size);

    // Init textures
    for (i=0; i<self->num_quads; i++)
    {
        quad = &self->quadlist[i];

        quad->texture = TEX_New (quad->x1 - quad->x0,
                                 quad->y1 - quad->y0,
                                 32,
                                 NULL,
                                 0);
        TEX_SetFilter (GL_LINEAR, GL_LINEAR);
        TEX_SetWrap (GL_CLAMP, GL_CLAMP);
        TEX_SetEnvironment (GL_MODULATE);
    }

    // Init deform array
    self->pos_y = (GLint *) malloc (self->height * sizeof (GLint));
    if (!self->pos_y)
        return 0;

    self->max_despl = 10;
    self->num_lines = 20; // 20 percent
    self->deform    = 0;  // Canal+ Deform
    self->speed     = 1.0;

    self->t0 = Epopeia_GetTimeMs();

    return self;
}


static float frand (void)
{
    return (float) (rand ( ) % 100) / 100.0f;
}


void CANALP_Do (TCanalPlus *self)
{
    TQuad *quad;
    int    w, h;
    int    i, j, k, tmp;
    int    viewport[4];
    int    max_texture_size;
    int    num_lines;
    float  ty, ty2;
    float  t;

    // Si no hay desplazamientos, no hacemos nada :)
    if ((self->deform == 0) && ((self->max_despl == 0) || (self->num_lines == 0)))
        return;

    // Get viewport dimensions
    glGetIntegerv (GL_VIEWPORT, viewport);
    if ((viewport[0] != self->vp_x0) ||
        (viewport[1] != self->vp_y0) ||
        (viewport[2] != (self->vp_x1 - self->vp_x0)) ||
        (viewport[3] != (self->vp_y1 - self->vp_y0)))
    {
        self->vp_x0 = viewport[0];
        self->vp_y0 = viewport[1];
        self->vp_x1 = viewport[2] + self->vp_x0;
        self->vp_y1 = viewport[3] + self->vp_y0;
        self->width  = viewport[2];
        self->height = viewport[3];

        // Get maximum texture size
        glGetIntegerv (GL_MAX_TEXTURE_SIZE, &max_texture_size);

        // Delete textures
        for (i=0; i<self->num_quads; i++)
            TEX_Delete (self->quadlist[i].texture);

        self->num_quads = 0;
        free (self->quadlist);
        self->quadlist = NULL;
        DivideQuads (self,
                     self->vp_x0, self->vp_y0,
                     self->vp_x1, self->vp_y1,
                     max_texture_size);

        for (i=0; i<self->num_quads; i++)
        {
            quad = &self->quadlist[i];

            quad->texture = TEX_New (quad->x1 - quad->x0,
                                     quad->y1 - quad->y0,
                                     32,
                                     NULL,
                                     0);
            TEX_SetFilter (GL_NEAREST, GL_NEAREST);
            TEX_SetWrap (GL_CLAMP, GL_CLAMP);
            TEX_SetEnvironment (GL_MODULATE);
        }

        free (self->pos_y);

        // Init deform array
        //self->pos_y = (GLint *) malloc (self->height * sizeof (GLint));
        self->pos_y = (GLint *) malloc (self->vp_y1 * sizeof (GLint));
        if (!self->pos_y)
            exit (1);
    }

    // Update deform
    if (self->deform == 1)
    {
        // Deformacion de desplazamiento vertical
        k = (int) ((float)(Epopeia_GetTimeMs() - self->t0) * self->speed);
        for (i=0; i<self->height; i++)
        {
            j = i + k;
            if (j < 0)
                j = (self->height - 1) - ((-j) % self->height);
            else
                j = j % self->height;

            self->pos_y[self->vp_y0 + i] = j;
        }
    }
    else
    {
        // Canal +

        // Desplazamientos originales (sin codificar)
        for (i=0; i<self->height; i++)
            self->pos_y[self->vp_y0 + i] = i;

        // Codificamos
        for (i=0; i<self->num_lines * viewport[3] / 100; i++)
        {
            j = self->vp_y0 + (rand ( ) % self->height);
            k = j + ((rand ( ) % (2 * self->max_despl)) - self->max_despl);
            //if (k < 0) k = -k;
            //k = k % self->height;
            if (k < self->vp_y0)  k = self->vp_y0;
            if (k >= self->vp_y1) k = self->vp_y1 - 1;

            // Swap lines
            tmp            = self->pos_y[j];
            self->pos_y[j] = self->pos_y[k];
            self->pos_y[k] = tmp;
        }
    }

    glEnable (GL_TEXTURE_2D);

    // Update textures
    for (i=0; i<self->num_quads; i++)
    {
        quad = &self->quadlist[i];

        TEX_Bind (quad->texture);
        glCopyTexImage2D (GL_TEXTURE_2D, 0,
                          GL_RGBA,
                          quad->x0,
                          quad->y0,
                          quad->texture->width,
                          quad->texture->height,
                          0);
        glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }


    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ( );
    gluOrtho2D (self->vp_x0, self->vp_x1,
                self->vp_y0, self->vp_y1);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ( );

    glEnable (GL_BLEND);

    //glClearColor (0.0, 0.0, 0.0, 1.0);
    //glClear (GL_COLOR_BUFFER_BIT);

    glColor4f (1, 1, 1, 1.0);
    //glColor4f (0.7, 0.0, 0.3, 1);
    glBlendFunc (GL_ONE, GL_ZERO);

    // Paint textures
    for (i=0; i<self->num_quads; i++)
    {
        quad = &self->quadlist[i];

        TEX_Bind (quad->texture);

        num_lines = quad->y1 - quad->y0;
        for (j=quad->y0; j<quad->y1; j++)
        {
            ty  = (GLfloat) (j - quad->y0) / (GLfloat) num_lines;
            ty2 = (GLfloat) ((j+1) - quad->y0) / (GLfloat) num_lines;

            //glLoadIdentity ( );
            //glTranslatef (0, self->pos_y[j], 0);

            glBegin (GL_QUADS);
             glTexCoord2f (0, ty2); glVertex2f (quad->x0, self->pos_y[j] + 1);
             glTexCoord2f (1, ty2); glVertex2f (quad->x1, self->pos_y[j] + 1);
             glTexCoord2f (1, ty);  glVertex2f (quad->x1, self->pos_y[j]);
             glTexCoord2f (0, ty);  glVertex2f (quad->x0, self->pos_y[j]);
            glEnd ( );
        }
    }

    glDisable (GL_TEXTURE_2D);
    glDisable (GL_BLEND);
}

void CANALP_Kill (TCanalPlus *self)
{
    int i;

    for (i=0; i<self->num_quads; i++)
        TEX_Delete (self->quadlist[i].texture);

    free (self->pos_y);
    free (self->quadlist);
    free (self);
}
