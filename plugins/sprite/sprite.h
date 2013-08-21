#ifndef SPRITE_H
#define SPRITE_H

#include <glt/glt.h>
#include "interp.h"

typedef struct
{
    float x, y;
    float u, v;
} grid_t;

typedef struct
{
    float    u1, v1;
    //unsigned spriteid;
    TTexture *texture;
    unsigned sizex;
    unsigned sizey;
    unsigned sizex_2;  // Sizex / 2
    unsigned sizey_2;  // Sizey / 2
} TSpritePriv;


typedef struct TSprite_t
{
    int          num_sprites;
    TSpritePriv *sprite; // Lista de sprites

    int          actual_sprite;
    int          change_speed;
    int          base_time;
    int          loop_mode; // 0 - Static, 1 - Loop


    /*
    float    u1, v1;
    unsigned spriteid;
    unsigned sizex;
    unsigned sizey;
    unsigned sizex_2;  // Sizex / 2
    unsigned sizey_2;  // Sizey / 2
*/

    int doblend;
    
    long ultimo_tick;
    float posx,posy;
    float scalex,scaley;
    float alpha;
    float delta_posx,delta_posy;        // desplazamiento lineal
    float delta_scalex,delta_scaley;    // escalado lineal
    float delta_alpha;                  // variacion lineal del alpha
    float alphachange_timeout;
    float alpha_amplitud;               // variacion sinusoidal del alpha
    float alpha_fase;
    float alpha_sinustime;
    float rand_maxx;                    // desplazamiento aleatorio
    float rand_maxy;
    float rand_posx;
    float rand_posy;
    float randalpha_alpha;              // variacion aleatoria del alpha
    float randalpha_maxdesp;
    int pos_method;   // 0: movimiento normal; 1: movimiento aleatorio
   	int alpha_method; // 0: no alpha change; 1: linear change;
                      // 2: sinusoidal change; 3: random change; 4: pulse change
	float alpha_pulse;					//r3D: Cuando llega al tiempo de fase invierte el pulso y reinicia su valor                      
	int	paridad;						//r3D:0 par 1 impar. Usado en SetAlphaPulse
                      

    // DEFORM
    grid_t *grid;
    int    deform;                    // 0: No deform, 1: XY deform, 2: UV deform
    int    grid_w, grid_h;            // grid dimensions
    float  grid_ax, grid_fx, grid_wx; // amplitude, frequency, waves
    float  grid_ay, grid_fy, grid_wy; // amplitude, frequency, waves
    float  grid_au, grid_fu, grid_wu;
    float  grid_av, grid_fv, grid_wv;

    int scale_fade;
    TInterp *sclx_it;
    TInterp *scly_it;
} TSprite;


TSprite* Sprite_New(char *filename);
void Sprite_Kill  (TSprite* self);
void Sprite_Paint(TSprite* self, float x1,float y1,float x2,float y2,float x3,float y3,float x4, float y4,float transparencia,unsigned flags, float t);


#endif
