///////////////////////////////////////////////////////////////////////
// Blur
///////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include <assert.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <glt/glt.h>
#include "blur.h"
#include "interp.h"

// Devuelve el menor numero que sea mayor o igual a "n" y que sea potencia de 2
static int BiggerPow2 (int n)
{
    int i;

    for (i=0; i<31; i++)
        if ((1 << i) >= n)
            break;

    return (1 << i);
}

static int BLUR_DynamicInit (TBlur *self, GLint *viewport)
{
    int width_p2, height_p2; // width and height (power of 2)
    int i;

    // Find width and height (power of 2)
    width_p2  = BiggerPow2 (viewport[2]);
    height_p2 = BiggerPow2 (viewport[3]);

    // Existe textura con dimensiones inferiores a las requeridas, o no existe textura => Crear una nueva
    if ((self->texture && ((self->texture->width < width_p2) || (self->texture->height < height_p2))) || (!self->texture))
    {
        char texname[64];

        // Delete texture (if exists)
        if (self->texture)
            CACHE_FreeTexture (self->texture);

        // Create the symbolic name of the texture
        // RT2_<width>x<height>x<format>
        sprintf (texname, "R2T_%ix%ix%s", width_p2, height_p2, "RGBA");

        // Try to load from cache
        self->texture = CACHE_LoadTexture (texname, TEX_BUILD_MIPMAPS);
        if (!self->texture)
        {
            // Create new one
            // NOTA: Aunque no usamos mipmaps, como se va a compartir la textura,
            //       mejor los creamos por si otro plugin los necesita.
            //self->texture = TEX_New (width_p2, height_p2, GL_RGBA, NULL, TEX_BUILD_MIPMAPS);
            self->texture = TEX_New (width_p2, height_p2, 32, NULL, TEX_BUILD_MIPMAPS);
            if (!self->texture)
                return 0;

            if (!CACHE_InsertTexture (texname, self->texture))
                return 0;
        }
    }

    // Set texture parameters
    TEX_Bind (self->texture);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    TEX_SetWrap (GL_CLAMP, GL_CLAMP);
    TEX_SetEnvironment (GL_MODULATE);

    // Init texture mapping coords
    self->u0 = (GLfloat) 0.0f;
    self->v0 = (GLfloat) 0.0f;
    //self->u1 = (GLfloat) viewport[2] / (GLfloat) width_p2;
    //self->v1 = (GLfloat) viewport[3] / (GLfloat) height_p2;
    self->u1 = (GLfloat) viewport[2] / (GLfloat) self->texture->width;
    self->v1 = (GLfloat) viewport[3] / (GLfloat) self->texture->height;

    // Init texture coords
    self->x0 = (GLfloat) viewport[0];
    self->y0 = (GLfloat) viewport[1];
    self->x1 = (GLfloat) (viewport[2] + self->x0);
    self->y1 = (GLfloat) (viewport[3] + self->y0);

    return 1;
}


TBlur *BLUR_New (void)
{
    TBlur *self;

    self = (TBlur *) malloc (sizeof (TBlur));
    if (!self)
        return 0;

    // Init viewport to unused state
    self->viewport[0] = -1;
    self->viewport[1] = -1;
    self->viewport[2] = -1;
    self->viewport[3] = -1;

    // No texture (La creamos dinamicamente)
    self->texture = NULL;

    self->linear = 1;
    self->it     = INTERP_New ( );

    return self;
}

void BLUR_Do (TBlur *self)
{
    GLint viewport[4];

    // Get viewport dimensions
    glGetIntegerv (GL_VIEWPORT, viewport);
    if ((viewport[0] != self->viewport[0]) || (viewport[1] != self->viewport[1]) ||
        (viewport[2] != self->viewport[2]) || (viewport[3] != self->viewport[3]))
    {
        // Update viewport
        self->viewport[0] = viewport[0];
        self->viewport[1] = viewport[1];
        self->viewport[2] = viewport[2];
        self->viewport[3] = viewport[3];

        if (!BLUR_DynamicInit (self, viewport))
        {
            printf ("BLUR: Dynamic init failed!\n");
            exit (1);
        }
    }

    glEnable (GL_TEXTURE_2D);

    // Update texture
    TEX_Bind (self->texture);
    glCopyTexSubImage2D (GL_TEXTURE_2D, 0,
                         0, 0,
                         self->x0,
                         self->y0,
                         self->x1 - self->x0,
                         self->y1 - self->y0);

    if (self->linear)
        TEX_SetFilter (GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    else
        TEX_SetFilter (GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST);

    // Set bias: 0..10
    //self->blur_level = 50.0f * cos (Epopeia_GetTimeMs ( ) / 1200.0f) + 50.0f;
    glTexEnvf (GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, self->blur_level / 10.0f);

    // Set Matrix mode
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ( );
    gluOrtho2D (0.0, 1.0, 0.0, 1.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ( );

    glDisable (GL_BLEND);
    glColor4f (1.0f, 1.0f, 1.0f, 1.0f);

    glBegin (GL_QUADS);
     glTexCoord2f (self->u0, self->v1); glVertex2f (0.0f, 1.0f);
     glTexCoord2f (self->u1, self->v1); glVertex2f (1.0f, 1.0f);
     glTexCoord2f (self->u1, self->v0); glVertex2f (1.0f, 0.0f);
     glTexCoord2f (self->u0, self->v0); glVertex2f (0.0f, 0.0f);
    glEnd ( );

    glTexEnvf (GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, 0.0f);
    glDisable (GL_TEXTURE_2D);
}

void BLUR_Delete (TBlur *self)
{
    if (self->texture)
        CACHE_FreeTexture (self->texture);

    free (self);
}
