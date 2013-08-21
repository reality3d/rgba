#include <GL/gl.h>
#include <GL/glu.h>
#include <glt/glt.h>
#include <glt/image.h>


typedef struct
{
    TTexture *texture;
} TSpritePriv;

typedef struct
{
  	TImage  imagen;
} TImaPriv;
 

typedef struct TSprite_t
{
    int          num_sprites;
    TSpritePriv *sprite; // Lista de sprites
    TImaPriv    *ima; // Donde guardamos las imagenes. en ima[0] colocaremos la imagen en curso.
    int          actual_sprite;
    float 		 anterior_alpha;//El alpha externo global del SetAlpha
    float 		 alpha;//El alpha externo global del SetAlpha
    int          change_speed;
    int          base_time;
    int          loop_mode; // 0 - Static, 1 - Loop
    long ultimo_tick;
    int spriteanterior;
    unsigned    *pixel_buffer;
    int          max_image_size;
                      
} TSprite;


TSprite* FantasySprite_New(char *filename,char *texid);
void FantasySprite_Kill  (TSprite* self);
