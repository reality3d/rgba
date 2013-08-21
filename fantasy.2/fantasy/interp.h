#ifndef INTERP_H
#define INTERP_H

#include "3dworld.h"

void interpola_spline (int numero_keys,Vector **t_entrada,Vector **t_salida,info_pos_path *keys);
void interpola_spline_scale (int numero_keys,Vector **t_entrada,Vector **t_salida,info_scale_path *keys);
void interpola_spline_fov (int numero_keys,float **t_entrada,float **t_salida,info_fov_path *keys);
void interpola_spline_roll (int numero_keys,float **t_entrada,float **t_salida,info_roll_path *keys);
void interpola_rotaciones(int numero_keys,Quaternion **t_entrada,Quaternion **t_salida,info_rot_path *keys);

void InterpolaPosicion(int numero_keys,
		       Vector *t_entrada,
		       Vector *t_salida,
		       info_pos_path *keys,
		       float frame,
		       float *x,float *y,float *z);
void InterpolaScale(int numero_keys,
		    Vector *t_entrada,
		    Vector *t_salida,
		    info_scale_path *keys,
		    float frame,
		    float *scalex,float *scaley,float *scalez);
void InterpolaFov(int numero_keys,
		       float *t_entrada,
		       float *t_salida,
		       info_fov_path *keys,
		       float frame,
		       float *fov);
void InterpolaRoll(int numero_keys,
		       float *t_entrada,
		       float *t_salida,
		       info_roll_path *keys,
		       float frame,
		       float *roll);
void InterpolaRotacion(int numero_keys,
		       Quaternion *tangentes_entrada,
		       Quaternion *tangentes_salida,
		       info_rot_path *keys,
		       float frame,
		       Quaternion *quat);		       
#endif