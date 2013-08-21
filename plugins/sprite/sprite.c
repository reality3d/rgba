#ifdef WIN32
 #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <glt/glt.h>
#include <glt/image.h>
#include <math.h>
#include <malloc.h>

#include "sprite.h"
#include "interp.h"


///////////////////////////////////////////////////////////////////////
// BEGIN - SINE DEFORM CODE
///////////////////////////////////////////////////////////////////////
#ifndef PI
#define PI 3.141592
#endif // PI



// DEFORM u,v
static void tex_paint_sprite_d1 (TSprite* self, float t,
                                 float dx, float dy, float x1, float y1)
{
    int i, j;
    float i_gridw    = 1.0 / self->grid_w;
    float i_gridh    = 1.0 / self->grid_h;
    float pi2_igridw = 2.0 * PI * self->grid_wu / self->grid_w;
    float pi2_igridh = 2.0 * PI * self->grid_wv / self->grid_h;

    float phase_x, phase_y;
    float def_x;
    float u1, v1;

    grid_t *pg1, *pg2;

    if (!self->grid)
        return;

    phase_x = t / self->grid_fx;
    phase_y = t / self->grid_fy;

    // Build grid
    u1  = self->sprite[self->actual_sprite].u1;
    v1  = self->sprite[self->actual_sprite].v1;
    pg1 = self->grid;
    for (j=0; j<self->grid_h+1; j++)
    {
        for (i=0; i<self->grid_w+1; i++)
        {
            // Init grid
            pg1->x = (GLfloat) dx * i * i_gridw + x1;
            pg1->y = (GLfloat) dy * j * i_gridh + y1;
            pg1->u = (GLfloat) i * i_gridw * u1;
            pg1->v = (GLfloat) j * i_gridh * v1;

            // Deform grid
            pg1->u = pg1->u + self->grid_au * sin ((t / self->grid_fu) + j * pi2_igridh);
            pg1->v = pg1->v + self->grid_av * cos ((t / self->grid_fv) + i * pi2_igridw);

            pg1 ++;
        }
    }


    // Paint grid
    pg1 = self->grid;
    pg2 = pg1 + self->grid_w+1;
    for (j=0; j<self->grid_h; j++)
    {
        glBegin (GL_QUAD_STRIP);
        for (i=0; i<self->grid_w+1; i++)
        {
            glTexCoord2fv (&pg1->u); glVertex2fv (&pg1->x);
            glTexCoord2fv (&pg2->u); glVertex2fv (&pg2->x);

            pg1 ++;
            pg2 ++;
        }
        glEnd ( );
    }
}

// DEFORM x,y
static void tex_paint_sprite_d2 (TSprite* self, float t,
                                 float dx, float dy, float x1, float y1)
{
    int i, j;
    float i_gridw    = 1.0 / self->grid_w;
    float i_gridh    = 1.0 / self->grid_h;
    float pi2_idx    = 2.0 * PI / dx;
    float pi2_igridh = 2.0 * PI * self->grid_wy / self->grid_h;

    float phase_x, phase_y;
    float def_x;

    float u1, v1;

    grid_t *pg1, *pg2;

    if (!self->grid)
        return;

    phase_x = t / self->grid_fx;
    phase_y = t / self->grid_fy;

    // Build grid
    u1  = self->sprite[self->actual_sprite].u1;
    v1  = self->sprite[self->actual_sprite].v1;
    pg1 = self->grid;
    for (j=0; j<self->grid_h+1; j++)
    {
        def_x = self->grid_ax * cos (phase_x + j * pi2_igridh);

        for (i=0; i<self->grid_w+1; i++)
        {
            // Init grid
            pg1->x = (GLfloat) dx * i * i_gridw + x1;
            pg1->y = (GLfloat) dy * j * i_gridh + y1;
            pg1->u = (GLfloat) i * i_gridw * u1;
            pg1->v = (GLfloat) j * i_gridh * v1;

            // Deform grid
            pg1->x = pg1->x + def_x;
            pg1->y = pg1->y + self->grid_ay * cos (phase_y + (pg1->x - x1) * pi2_idx); //2.0 * PI / dx);
            //pg1->u = pg1->u + 0.1 * sin ((t / self->grid_fx) + j * pi2_igridh);
            //pg1->v = pg1->v + 0.1 * cos ((t / self->grid_fy) + i * pi2_igridw);

            pg1 ++;
        }
    }


    // Paint grid
    pg1 = self->grid;
    pg2 = pg1 + self->grid_w+1;
    for (j=0; j<self->grid_h; j++)
    {
        glBegin (GL_QUAD_STRIP);
        for (i=0; i<self->grid_w+1; i++)
        {
            glTexCoord2fv (&pg1->u); glVertex2fv (&pg1->x);
            glTexCoord2fv (&pg2->u); glVertex2fv (&pg2->x);

            pg1 ++;
            pg2 ++;
        }
        glEnd ( );
    }
}
///////////////////////////////////////////////////////////////////////
// END - SINE DEFORM CODE
///////////////////////////////////////////////////////////////////////

//
// Convierte una imagen a tama¤o en potencia de 2
//
static void IMG_SetPower2 (TSpritePriv *self, image_t img)
{
    int new_w, new_h;
    int x, y, i;
    int pitch, depth;
    char *p;
    char *pdata;

    for (i=0; i<31; i++)
        if (img->width <= (1 << i))
        {
            new_w = (1 << i);
            break;
        }

    for (i=0; i<31; i++)
        if (img->height <= (1 << i))
        {
            new_h = (1 << i);
            break;
        }

    self->u1 = (float) img->width  / (float) new_w;
    self->v1 = (float) img->height / (float) new_h;

    if ((new_w == img->width) && (new_h == img->height))
        return;

    depth = IMG_GetBPP (img) / 8;

    p = (char *) malloc (new_w * new_h * depth);
    memset (p, 0, new_w * new_h * depth);

    pdata = (char *) img->data;
    for (y=0; y<img->height; y++)
    {
        memcpy (p + new_w * depth * y,
                pdata + img->width * depth * y,
                img->width * depth);
    }

    //printf ("NEW: w=%i, h=%i, u=%3.2f, v=%3.2f\n",
    //        new_w, new_h, self->u1, self->v1);

    free (img->data);
    img->data = (void *) p;
    img->width = new_w;
    img->height = new_h;
}



// Nota: Se debe ejecutar DESPUS de haber inicializado el modo gr fico!!!

TSprite* Sprite_New(char *filename)
{
    image_t textura;
    TSprite* self;
    int i;
    int real_w, real_h;
    char *token;

    self = malloc(sizeof(TSprite));

    // Get number of sprites
    i = 0;
    self->num_sprites = 0;
    while (filename[i] != '\0')
    {
        if (filename[i] == ',')
            self->num_sprites ++;
        i ++;
    }
    self->num_sprites ++;

    // Alloc sprites
    self->sprite = (TSpritePriv *) malloc (self->num_sprites * sizeof (TSpritePriv));
    if (!self->sprite)
        return 0;

    //printf ("NUM SPRITES: %i\n", self->num_sprites);

    // Leemos todos los sprites
    token = strtok (filename, ",");
    i = 0;
    while (token != NULL)
    {
        //printf ("SPRITE Loading: %s\n", token);

        textura = IMG_Read (token);
	if (!textura)
	{
	    return 0;
	}

        //IMG_Convert (textura, 32);

        real_w = IMG_GetWidth (textura);
        real_h = IMG_GetHeight (textura);

        IMG_SetPower2 (&self->sprite[i], textura);

	// Enlar: quito esto porque por ahora me falla en la red
	// Lo cambio por lo siguiente


        self->sprite[i].texture = TEX_New (textura->width,
                                           textura->height,
                                           textura->bits_per_pixel,
                                           textura->data,
                                           0);
	// Modificado por utopian para que inserte la textura en la caché.
        /*
	if(!CACHE_InsertTexture(token,self->sprite[i].texture))
	{
            printf("Fallo al insertar a la textura\n");
	    return 0;
	}
	*/
	// Siguiente:
//	self->sprite[i].texture = CACHE_LoadTexture(token, 0);

	if (!self->sprite[i].texture)
	{
            return 0;
	}
	/*
        glGenTextures (1, &self->sprite[i].spriteid);
        glBindTexture (GL_TEXTURE_2D, self->sprite[i].spriteid);
        glTexImage2D  (GL_TEXTURE_2D, 0, 4, textura->width, textura->height, 0,
                       GL_BGRA_EXT,
                       GL_UNSIGNED_BYTE, textura->data);
                       */
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Clamp texture
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        self->sprite[i].sizex   = real_w;
        self->sprite[i].sizex_2 = self->sprite[i].sizex/2;
        self->sprite[i].sizey   = real_h;
        self->sprite[i].sizey_2 = self->sprite[i].sizey/2;

        IMG_Delete (textura);

        token = strtok (NULL, ",");
        i ++;
    }

/*
    textura = IMG_Read (filename);
    if (!textura)
        ExitProcess (1);

    IMG_Convert (textura, 32);
    */

    self->actual_sprite = 0;
    self->posx = self->posy = 0.0f;
    self->scalex = self->scaley = 1.0f;
    self->alpha = 1.0f;
    self->ultimo_tick = 0;
    self->delta_posx=self->delta_posy=0.0f;
    self->delta_scalex=self->delta_scaley=0.0f;
    self->delta_alpha=0.0f;
    self->alphachange_timeout=0.0f;
    self->doblend=0; // No hace blend por defecto
    self->alpha_amplitud=0.0f;
    self->alpha_fase=0.0f;
    self->alpha_sinustime=0.0f;
    self->alpha_method=0;
    self->pos_method=0;
    self->rand_maxx=0.0f;
    self->rand_maxy=0.0f;
    self->rand_posx=0.0f;
    self->rand_posy=0.0f;
    self->randalpha_alpha=0.0f;
    self->randalpha_maxdesp=0.0f;
    self->alpha_pulse=0.0f;//r3d:Inicializo a 0 el alphapulse
    self->paridad=0;//r3d:Empieza en 0

    // Grid
    self->grid       = 0;
    self->deform     = 0;
    self->grid_w     = 4;
    self->grid_h     = 4;
    self->grid_ax    = 0.0; self->grid_fx = 1.0; self->grid_wx = 0.0;
    self->grid_ay    = 0.0; self->grid_fy = 1.0; self->grid_wy = 0.0;

    // 1 cambio de sprite por por segundo
    self->loop_mode    = 0; // No loop
    self->change_speed = 1000;
    self->base_time    = Epopeia_GetTimeMs();

    self->scale_fade = 0;

    // Init interp
    self->sclx_it = INTERP_New ( );
    if (!self->sclx_it)
    {
        free (self);
        return 0;
    }

    // Init interp
    self->scly_it = INTERP_New ( );
    if (!self->scly_it)
    {
        free (self);
        return 0;
    }



    return self;
}


void Sprite_Kill  (TSprite* self)
{
    int i;

    if (self->grid)
        free (self->grid);

// Modificado por utopian para que utilice la cache
/*    for (i=0; i<self->num_sprites; i++)
        TEX_Delete (self->sprite[i].texture);*/
      
      for (i=0; i<self->num_sprites; i++)
    	  CACHE_FreeTexture(self->sprite[i].texture);

        
        //glDeleteTextures(1, &self->sprite[i].spriteid);

    free (self->sprite);
    free (self);
}

void Sprite_Paint(TSprite* self, float x1,float y1,float x2,float y2,float x3,float y3,float x4, float y4,float transparencia,unsigned flags, float t)
{
    glPushAttrib (GL_ALL_ATTRIB_BITS);

    glColor4f(1.0f,1.0f,1.0f,transparencia);
    
    glEnable (GL_BLEND);

    if(flags == 1)    // Con este mezcla
        glBlendFunc (GL_SRC_ALPHA, GL_DST_ALPHA);
    else if(flags ==2) // Modo ADD (a£n en pruebas)
        glBlendFunc (GL_SRC_ALPHA,GL_ONE);
    else
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable (GL_DEPTH_TEST);
    glEnable (GL_TEXTURE_2D);

    //glBindTexture (GL_TEXTURE_2D, self->sprite[self->actual_sprite].spriteid);
    TEX_Bind (self->sprite[self->actual_sprite].texture);
    TEX_SetEnvironment(GL_MODULATE);

    glMatrixMode (GL_PROJECTION);
    glPushMatrix ( );
    glLoadIdentity ( );
    //glOrtho (0, self->xres, self->yres,0, -100, 100);
    //gluOrtho2D (0, self->xres, self->yres,0);
    gluOrtho2D (0, 100.0,100.0,0);

    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ( );
    glLoadIdentity ( );

    if (self->deform == 1)
    {
        tex_paint_sprite_d1 (self, t, x2-x1, y4-y1, x1, y1);
    }
    else if (self->deform == 2)
    {
        tex_paint_sprite_d2 (self, t, x2-x1, y4-y1, x1, y1);
    }
    else
    {
        glBegin(GL_QUADS);
         glTexCoord2f(0.0f,0.0f);
         glVertex3f(x1,y1,1.0f);
         glTexCoord2f(self->sprite[self->actual_sprite].u1,0.0f);
         glVertex3f(x2,y2,1.0f);
         glTexCoord2f(self->sprite[self->actual_sprite].u1,self->sprite[self->actual_sprite].v1);
         glVertex3f(x3,y3,1.0f);
         glTexCoord2f(0.0f,self->sprite[self->actual_sprite].v1);
         glVertex3f(x4,y4,1.0f);
        glEnd();
    }

    glMatrixMode (GL_PROJECTION);
    glPopMatrix ( );
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix ( );
    glEnable (GL_DEPTH_TEST);
    glDisable (GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    glPopAttrib ( );
}
