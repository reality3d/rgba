//
// GL Font handler [Demo Version]
//

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <glt/glt.h>
#include "fonts.h"
#include <packer/packer.h>
#include "dst.h"
/*
#define fclose pak_close
#define fopen  pak_open
#define fwrite pak_write
#define fread  pak_read
#define ftell  pak_ftell
#define fseek  pak_fseek
#undef feof
#define feof   pak_feof
#define fgetc  pak_fgetc
#define FILE   PFILE
*/
//#define TRY_TO_FIX_PHOTOSHOP_BUG


//
// FONTS_INIT
//
// IN:
//  * interspace = Numero de pixels entre fuentes
//  * space      = Numero de pixels por espacio (o caracter no existente!)
//
// OUT:
//  * 1 = OK
//  * 0 = KO
//
TFont* Fonts_New (int interspace, int space, char *filename, char *filename_nfo,unsigned resx, unsigned resy)
{
    //image_t img;
    int     i, tmp;
//    unsigned int *p;
    int     width, height;
//unsigned int mask_color;
//    FILE *fp;
    TFont* self;

    self = malloc(sizeof(TFont));

    self->fonts_info = font_info;
        //calloc (256, sizeof (struct fonts_info));
    self->_interspace = interspace;
    self->x = 0.0;
    self->y = 0.0;
    self->flags = 0;
    self->texto = "not defined";

    self->alpha=1.0f;
    self->delta_alpha=0.0f;
    self->delta_posx=self->delta_posy=0.0f;
    self->alphachange_timeout=0.0f;
    self->ultimo_tick=0;
    self->xres=resx;
    self->yres=resy;
    self->scalex=self->scaley=1.0f;
    self->delta_scalex=self->delta_scaley=0.0f;

/*
    fp = fopen (filename_nfo, "rb");
    if (!fp)
    {
        printf ("Can't find %s\n", filename_nfo);
        return NULL;
    }

    fread (&tmp, 4, 1, fp);
    fread (&tmp, 4, 1, fp);
    for (i=0; i<256; i++)
    {
        fread (&self->fonts_info[i].x0, 2, 1, fp);
        fread (&self->fonts_info[i].y0, 2, 1, fp);
        fread (&self->fonts_info[i].x1, 2, 1, fp);
        fread (&self->fonts_info[i].y1, 2, 1, fp);
    }
    fclose (fp);
    */
    /*
    img = IMG_Read (filename);
    if (!img)
    {
        printf ("IMG Error:: %s\n", IMG_GetError ( ));
        return NULL;
    }

    IMG_Convert (img, 32);

#ifdef TRY_TO_FIX_PHOTOSHOP_BUG
    p = (unsigned int *) img->data;
    for (i=0; i<img->width*img->height; i++)
    {
        if (((p[i]>>24) != 0xFF) &&
            ((p[i+1]>>24)) == 0xFF)
        {
            mask_color = p[i+1] & 0x00FFFFFF;
            break;
        }
    }
    // Aplicamos la mascara
    mask_color |= 0xFF000000;
    for (i=0; i<img->width*img->height; i++)
    {
        if ((p[i]>>24) != 0xFF)
            p[i] = p[i] & mask_color;
    }
#endif //TRY_TO_FIX_PHOTOSHOP_BUG
*/
    glGenTextures   (1, &self->_fonts_texture_id);
    glBindTexture   (GL_TEXTURE_2D, self->_fonts_texture_id);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D    (GL_TEXTURE_2D, 0, 4, font_width, font_height,
                     0,
                     GL_BGRA_EXT,
                     //GL_RGBA,
                     GL_UNSIGNED_BYTE, font_data);

    // Creamos las listas
    self->_list_base = glGenLists (256);
    glBindTexture (GL_TEXTURE_2D, self->_fonts_texture_id);

    glLoadIdentity ( );

    for (i=0; i<256; i++)
    {
        width  = self->fonts_info[i].x1 - self->fonts_info[i].x0;
        height = self->fonts_info[i].y1 - self->fonts_info[i].y0;

        // Caracter que no tenemos en nuestras fuentes!!
        if (width == 0)
        {
            // Le asignamos pixels en blanco a partir del final
            width = space;
            self->fonts_info[i].x0 = 0;
        }

        // Generamos una lista
        glNewList (self->_list_base + i, GL_COMPILE);

        // Asignamos la nueva fuente
        glBegin (GL_QUADS);
         glTexCoord2f ((GLfloat)self->fonts_info[i].x0/(GLfloat)font_width, (GLfloat)self->fonts_info[i].y1/(GLfloat)font_height);
         glVertex2i   (0, height);

         glTexCoord2f ((GLfloat)self->fonts_info[i].x1/(GLfloat)font_width, (GLfloat)self->fonts_info[i].y1/(GLfloat)font_height);
         glVertex2i   (width, height);

         glTexCoord2f ((GLfloat)self->fonts_info[i].x1/(GLfloat)font_width, (GLfloat)self->fonts_info[i].y0/(GLfloat)font_height);
         glVertex2i   (width, 0);

         glTexCoord2f ((GLfloat)self->fonts_info[i].x0/(GLfloat)font_width, (GLfloat)self->fonts_info[i].y0/(GLfloat)font_height);
         glVertex2i   (0, 0);
        glEnd ( );

        // Desplazamos a la derecha
        glTranslatef ((GLfloat)(width + interspace), 0, 0);

        glEndList ( );
    }

    //IMG_Delete (img);
    return self;
}


int Fonts_GetWidth(TFont* self, char *text)
{
    int i;
    int width = 0;

    for (i=0; i<strlen (text); i++)
        width = width + self->fonts_info[text[i]].x1 - self->fonts_info[text[i]].x0 + self->_interspace;

    return width;
}

int Fonts_GetHeight(TFont*self, char *text)
{
    int i;
    int height=0;
    int newheight;

    for (i=0; i<strlen (text); i++)
    {
        newheight=self->fonts_info[text[i]].y1 - self->fonts_info[text[i]].y0;
        if(newheight>height) height=newheight;
    }
    
    return height;
}

void Fonts_Delete(TFont* self)
{
    glDeleteLists (self->_list_base, 256);
}

int Fonts_Write(TFont* self, char *text, float x, float y, int flags)
{
    glPushAttrib (GL_ALL_ATTRIB_BITS);

    glMatrixMode (GL_PROJECTION);
    glPushMatrix ( );
    glLoadIdentity ( );
    glOrtho (0, self->xres, self->yres,0, -100, 100);
    
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ( );
    glLoadIdentity ( );

    glDisable (GL_DEPTH_TEST);
    glEnable (GL_TEXTURE_2D);
    glBindTexture (GL_TEXTURE_2D, self->_fonts_texture_id);
    glEnable (GL_BLEND);

    if(flags && FLAG_BLEND)
    // Con este se deber¡a mezclar
        glBlendFunc (GL_SRC_ALPHA, GL_DST_ALPHA);
    else if(flags && FLAG_BLEND_MORE)
        glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    else
        // Con este blending se sobreimprime la font
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    //glTranslatef (pos_x - (float)FONTS_GetWidth (text)/2.0, pos_y, 0);

    //    glRotatef(15.0,0,0,1);


    //    glTranslatef (x, y, 0);
    //glTranslatef (x - (Fonts_GetWidth(self,text)*0.5f*self->scalex),
    //              y - (Fonts_GetHeight(self,text)*0.5f*self->scaley), 0);
    glTranslatef (x, self->yres - y, 0);

    glScalef(self->scalex,self->scaley,1.0);

    glListBase (self->_list_base);
    glCallLists (strlen(text), GL_BYTE, text);


    glMatrixMode (GL_PROJECTION);
    glPopMatrix ( );
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ( );

    glEnable (GL_DEPTH_TEST);
    glDisable (GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glPopAttrib ( );

    return 1;
}
