#include <math.h>
#include <string.h>

#include "fantasy.h"
#include "carga.h"
#include "mates.h"
#include "misc.h"
#include "quatern.h"
#include "material.h"

//---------------------------------------------------------------------------------------------
// Parte 1: interpolación previa de splines (preparación de las tangentes de entrada y salida)
//---------------------------------------------------------------------------------------------

void interpola_spline (int numero_keys,
		       Vector **t_entrada,
		       Vector **t_salida,
		       info_pos_path *keys)
{
    int i,j;
    Vector g1,g2,g3;
    Vector *tangentes_salida;
    Vector *tangentes_entrada;
        
    *t_entrada = (Vector *) malloc(numero_keys*sizeof(Vector));
    *t_salida = (Vector *) malloc(numero_keys*sizeof(Vector));

    tangentes_entrada = *t_entrada;
    tangentes_salida  = *t_salida;

    // El proceso se calcula en dos fases:
    // 1. C lculo de los vectores tangentes de la spline en cada uno de los puntos
    // 2. Interpolaci¢n seg£n la f¢rmula de la spline de Hermite, con los puntos y los vectores

    
    // Si solo hay dos keys, las tangentes se calculan con una formula especial
    
    if(numero_keys == 2)
    {
     tangentes_entrada[0].x = tangentes_entrada[0].y = tangentes_entrada[0].z = 0.0f;
     tangentes_salida[1].x = tangentes_salida[1].y = tangentes_salida[0].z = 0.0f;

     tangentes_salida[0].x = (keys[1].wx - keys[0].wx) * (1-keys[0].spflags.tension);
     tangentes_salida[0].y = (keys[1].wy - keys[0].wy) * (1-keys[0].spflags.tension);
     tangentes_salida[0].z = (keys[1].wz - keys[0].wz) * (1-keys[0].spflags.tension);

     tangentes_entrada[1].x = (keys[1].wx - keys[0].wx) * (1-keys[1].spflags.tension);
     tangentes_entrada[1].y = (keys[1].wy - keys[0].wy) * (1-keys[1].spflags.tension);
     tangentes_entrada[1].z = (keys[1].wz - keys[0].wz) * (1-keys[1].spflags.tension);
    }
    else  // Calculamos las tangentes seg£n la f¢rmula general
    {
        for(i=1;i<numero_keys-1;i++)
        {
            g1.x = (keys[i].wx - keys[i-1].wx)*(1-keys[i].spflags.bias);
            g1.y = (keys[i].wy - keys[i-1].wy)*(1-keys[i].spflags.bias);
            g1.z = (keys[i].wz - keys[i-1].wz)*(1-keys[i].spflags.bias);
            g2.x = (keys[i+1].wx - keys[i].wx)*(1-keys[i].spflags.bias);
            g2.y = (keys[i+1].wy - keys[i].wy)*(1-keys[i].spflags.bias);
            g2.z = (keys[i+1].wz - keys[i].wz)*(1-keys[i].spflags.bias);
            g3.x = g2.x - g1.x;
            g3.y = g2.y - g1.y;
            g3.z = g2.z - g1.z;
            
            tangentes_entrada[i].x = (g1.x + g3.x*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_entrada[i].y = (g1.y + g3.y*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_entrada[i].z = (g1.z + g3.z*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i].x =  (g1.x + g3.x*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i].y =  (g1.y + g3.y*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i].z =  (g1.z + g3.z*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
        }
        // Primer punto

        tangentes_entrada[0].x = tangentes_entrada[0].y = tangentes_entrada[0].z = 0.0f;
        tangentes_salida[0].x = ((keys[1].wx - keys[0].wx)*1.5f - tangentes_entrada[1].x*0.5f) * (1-keys[0].spflags.tension);
        tangentes_salida[0].y = ((keys[1].wy - keys[0].wy)*1.5f - tangentes_entrada[1].y*0.5f) * (1-keys[0].spflags.tension);
        tangentes_salida[0].z = ((keys[1].wz - keys[0].wz)*1.5f - tangentes_entrada[1].z*0.5f) * (1-keys[0].spflags.tension);

        // éltimo punto
        tangentes_salida[numero_keys-1].x =
        tangentes_salida[numero_keys-1].y =
        tangentes_salida[numero_keys-1].z = 0.0f;

        tangentes_entrada[numero_keys-1].x =
            ((keys[numero_keys-1].wx - keys[numero_keys-2].wx)*1.5f - tangentes_salida[numero_keys-2].x*0.5f) * (1-keys[numero_keys-1].spflags.tension);
        tangentes_entrada[numero_keys-1].y =
            ((keys[numero_keys-1].wy - keys[numero_keys-2].wy)*1.5f - tangentes_salida[numero_keys-2].y*0.5f) * (1-keys[numero_keys-1].spflags.tension);
        tangentes_entrada[numero_keys-1].z =
            ((keys[numero_keys-1].wz - keys[numero_keys-2].wz)*1.5f - tangentes_salida[numero_keys-2].z*0.5f) * (1-keys[numero_keys-1].spflags.tension);
    }
}

void interpola_spline_scale (int numero_keys,
		       Vector **t_entrada,
		       Vector **t_salida,
		       info_scale_path *keys)
{
    int i,j;
    Vector g1,g2,g3;
    Vector *tangentes_salida;
    Vector *tangentes_entrada;
        
    *t_entrada = (Vector *) malloc(numero_keys*sizeof(Vector));
    *t_salida = (Vector *) malloc(numero_keys*sizeof(Vector));

    tangentes_entrada = *t_entrada;
    tangentes_salida  = *t_salida;

    // El proceso se calcula en dos fases:
    // 1. C lculo de los vectores tangentes de la spline en cada uno de los puntos
    // 2. Interpolaci¢n seg£n la f¢rmula de la spline de Hermite, con los puntos y los vectores

    
    // Si solo hay dos keys, las tangentes se calculan con una formula especial
    
    if(numero_keys == 2)
    {
     tangentes_entrada[0].x = tangentes_entrada[0].y = tangentes_entrada[0].z = 0.0f;
     tangentes_salida[1].x = tangentes_salida[1].y = tangentes_salida[0].z = 0.0f;

     tangentes_salida[0].x = (keys[1].scalex - keys[0].scalex) * (1-keys[0].spflags.tension);
     tangentes_salida[0].y = (keys[1].scaley - keys[0].scaley) * (1-keys[0].spflags.tension);
     tangentes_salida[0].z = (keys[1].scalez - keys[0].scalez) * (1-keys[0].spflags.tension);

     tangentes_entrada[1].x = (keys[1].scalex - keys[0].scalex) * (1-keys[1].spflags.tension);
     tangentes_entrada[1].y = (keys[1].scaley - keys[0].scaley) * (1-keys[1].spflags.tension);
     tangentes_entrada[1].z = (keys[1].scalez - keys[0].scalez) * (1-keys[1].spflags.tension);
    }
    else  // Calculamos las tangentes seg£n la f¢rmula general
    {
        for(i=1;i<numero_keys-1;i++)
        {
            g1.x = (keys[i].scalex - keys[i-1].scalex)*(1-keys[i].spflags.bias);
            g1.y = (keys[i].scaley - keys[i-1].scaley)*(1-keys[i].spflags.bias);
            g1.z = (keys[i].scalez - keys[i-1].scalez)*(1-keys[i].spflags.bias);
            g2.x = (keys[i+1].scalex - keys[i].scalex)*(1-keys[i].spflags.bias);
            g2.y = (keys[i+1].scaley - keys[i].scaley)*(1-keys[i].spflags.bias);
            g2.z = (keys[i+1].scalez - keys[i].scalez)*(1-keys[i].spflags.bias);
            g3.x = g2.x - g1.x;
            g3.y = g2.y - g1.y;
            g3.z = g2.z - g1.z;
            
            tangentes_entrada[i].x = (g1.x + g3.x*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_entrada[i].y = (g1.y + g3.y*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_entrada[i].z = (g1.z + g3.z*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i].x =  (g1.x + g3.x*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i].y =  (g1.y + g3.y*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i].z =  (g1.z + g3.z*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
        }
        // Primer punto

        tangentes_entrada[0].x = tangentes_entrada[0].y = tangentes_entrada[0].z = 0.0f;
        tangentes_salida[0].x = ((keys[1].scalex - keys[0].scalex)*1.5f - tangentes_entrada[1].x*0.5f) * (1-keys[0].spflags.tension);
        tangentes_salida[0].y = ((keys[1].scaley - keys[0].scaley)*1.5f - tangentes_entrada[1].y*0.5f) * (1-keys[0].spflags.tension);
        tangentes_salida[0].z = ((keys[1].scalez - keys[0].scalez)*1.5f - tangentes_entrada[1].z*0.5f) * (1-keys[0].spflags.tension);

        // éltimo punto
        tangentes_salida[numero_keys-1].x =
        tangentes_salida[numero_keys-1].y =
        tangentes_salida[numero_keys-1].z = 0.0f;

        tangentes_entrada[numero_keys-1].x =
            ((keys[numero_keys-1].scalex - keys[numero_keys-2].scalex)*1.5f - tangentes_salida[numero_keys-2].x*0.5f) * (1-keys[numero_keys-1].spflags.tension);
        tangentes_entrada[numero_keys-1].y =
            ((keys[numero_keys-1].scaley - keys[numero_keys-2].scaley)*1.5f - tangentes_salida[numero_keys-2].y*0.5f) * (1-keys[numero_keys-1].spflags.tension);
        tangentes_entrada[numero_keys-1].z =
            ((keys[numero_keys-1].scalez - keys[numero_keys-2].scalez)*1.5f - tangentes_salida[numero_keys-2].z*0.5f) * (1-keys[numero_keys-1].spflags.tension);
    }
}



void interpola_spline_roll (int numero_keys,
		       float **t_entrada,
		       float **t_salida,
		       info_roll_path *keys)
{
    int i,j,k;
    float g1,g2,g3;
    float *tangentes_salida;
    float *tangentes_entrada;

    *t_entrada = (float *) malloc(numero_keys*sizeof(float));
    *t_salida = (float *) malloc(numero_keys*sizeof(float));

    tangentes_entrada = *t_entrada;
    tangentes_salida  = *t_salida;

    // El proceso se calcula en dos fases:
    // 1. C lculo de los vectores tangentes de la spline en cada uno de los puntos
    // 2. Interpolaci¢n seg£n la f¢rmula de la spline de Hermite, con los puntos y los vectores

    
    // Si solo hay dos keys, las tangentes se calculan con una formula especial
    
    if(numero_keys == 2)
    {
     tangentes_entrada[0]= 0.0f;
     tangentes_salida[1] = 0.0f;

     tangentes_salida[0] = (keys[1].roll - keys[0].roll) * (1-keys[0].spflags.tension);
     tangentes_entrada[1] = (keys[1].roll - keys[0].roll) * (1-keys[1].spflags.tension);
    }
    else  // Calculamos las tangentes seg£n la f¢rmula general
    {
        for(i=1;i<numero_keys-1;i++)
        {
            g1 = (keys[i].roll - keys[i-1].roll)*(1-keys[i].spflags.bias);
            g2 = (keys[i+1].roll - keys[i].roll)*(1-keys[i].spflags.bias);
            g3 = g2 - g1;
            
            tangentes_entrada[i] = (g1 + g3*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i] =  (g1 + g3*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
        }

        // Primer punto

        tangentes_entrada[0] = 0.0f;
        tangentes_salida[0]  = ((keys[1].roll - keys[0].roll)*1.5f - tangentes_entrada[1]*0.5f) *(1-keys[0].spflags.tension);

        // éltimo punto
        tangentes_salida[numero_keys-1] = 0.0f;
        tangentes_entrada[numero_keys-1]=
            ((keys[numero_keys-1].roll - keys[numero_keys-2].roll)*1.5f - tangentes_salida[numero_keys-2]*0.5f) * (1-keys[numero_keys-1].spflags.tension);
    }

  
}

void interpola_spline_fov (int numero_keys,
		       float **t_entrada,
		       float **t_salida,
		       info_fov_path *keys)
{
    int i,j,k;
    float g1,g2,g3;
    float *tangentes_salida;
    float *tangentes_entrada;

    *t_entrada = (float *) malloc(numero_keys*sizeof(float));
    *t_salida = (float *) malloc(numero_keys*sizeof(float));

    tangentes_entrada = *t_entrada;
    tangentes_salida  = *t_salida;

    // El proceso se calcula en dos fases:
    // 1. C lculo de los vectores tangentes de la spline en cada uno de los puntos
    // 2. Interpolaci¢n seg£n la f¢rmula de la spline de Hermite, con los puntos y los vectores

    
    // Si solo hay dos keys, las tangentes se calculan con una formula especial
    
    if(numero_keys == 2)
    {
     tangentes_entrada[0]= 0.0f;
     tangentes_salida[1] = 0.0f;

     tangentes_salida[0] = (keys[1].fov - keys[0].fov) * (1-keys[0].spflags.tension);
     tangentes_entrada[1] = (keys[1].fov - keys[0].fov) * (1-keys[1].spflags.tension);
    }
    else  // Calculamos las tangentes seg£n la f¢rmula general
    {
        for(i=1;i<numero_keys-1;i++)
        {
            g1 = (keys[i].fov - keys[i-1].fov)*(1-keys[i].spflags.bias);
            g2 = (keys[i+1].fov - keys[i].fov)*(1-keys[i].spflags.bias);
            g3 = g2 - g1;
            
            tangentes_entrada[i] = (g1 + g3*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i] =  (g1 + g3*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
        }

        // Primer punto

        tangentes_entrada[0] = 0.0f;
        tangentes_salida[0]  = ((keys[1].fov - keys[0].fov)*1.5f - tangentes_entrada[1]*0.5f) *(1-keys[0].spflags.tension);

        // éltimo punto
        tangentes_salida[numero_keys-1] = 0.0f;
        tangentes_entrada[numero_keys-1]=
            ((keys[numero_keys-1].fov - keys[numero_keys-2].fov)*1.5f - tangentes_salida[numero_keys-2]*0.5f) * (1-keys[numero_keys-1].spflags.tension);
    }

  
}




void interpola_rotaciones(int numero_keys,
		       Quaternion **t_entrada,
		       Quaternion **t_salida,
		       info_rot_path *keys)
{
    int i,j;
    int inicio,fin;
    float t,delta_t;
    float temp;
    Quaternion g1,g2,g3;
    Quaternion *tangentes_salida;
    Quaternion *tangentes_entrada;
  
    *t_entrada = (Quaternion *) malloc(numero_keys*sizeof(Quaternion));
    *t_salida = (Quaternion *) malloc(numero_keys*sizeof(Quaternion));
    tangentes_entrada = *t_entrada;
    tangentes_salida  = *t_salida;

    // El proceso se calcula en dos fases:
    // 1. C lculo de los vectores tangentes de la spline en cada uno de los puntos
    // 2. Interpolaci¢n seg£n la f¢rmula, con los puntos y los vectores

    // Si solo hay dos keys, las tangentes se calculan con una formula especial
    
    if(numero_keys == 2)
    {
        temp=(1.0f-keys[0].spflags.tension) *
            (1.0f+(keys[0].spflags.continuity*keys[0].spflags.bias)) / 3.0f;
        Slerp(&(keys[0].quat),&(keys[1].quat),temp,&(tangentes_salida[0]));

        temp=(1.0f-keys[1].spflags.tension) *
            (1.0f-(keys[1].spflags.continuity*keys[1].spflags.bias)) / 3.0f;
        Slerp(&(keys[1].quat),&(keys[0].quat),temp,&(tangentes_entrada[1]));

        tangentes_entrada[0].x = tangentes_entrada[0].y =
            tangentes_entrada[0].z = tangentes_entrada[0].w = 0.0f;
        tangentes_salida[1].x = tangentes_salida[1].y =
            tangentes_salida[1].z = tangentes_salida[1].w = 0.0f;
    }
    else  // Calculamos las tangentes seg£n la f¢rmula general
    {
        for(i=1;i<numero_keys-1;i++)
        {
            // Calculamos g1
            Slerp(&(keys[i].quat),&(keys[i-1].quat),
                  -(1.0f+keys[i].spflags.bias)/3.0f,&g1);
            // Calculamos g2
            Slerp(&(keys[i].quat),&(keys[i+1].quat),
                  (1.0f-keys[i].spflags.bias)/3.0f,&g2);
            // Calculamos g3 para el caso entrante
            Slerp(&g1,&g2,0.5f+0.5f*keys[i].spflags.continuity,&g3);
            // Calculamos la tangente entrante
            Slerp(&(keys[i].quat),&g3,keys[i].spflags.tension-1.0f,
                  &(tangentes_entrada[i]));
            // Calculamos g3 para el caso saliente
            Slerp(&g1,&g2,0.5f-0.5f*keys[i].spflags.continuity,&g3);
            // Calculamos la tangente de salida
            Slerp(&(keys[i].quat),&g3,-(keys[i].spflags.tension-1.0f),
                  &(tangentes_salida[i]));
        }
        // Primer punto

        tangentes_entrada[0].x = tangentes_entrada[0].y =
            tangentes_entrada[0].z = tangentes_entrada[0].w = 0.0f;

        temp=(1.0f-keys[0].spflags.tension) *
            (1.0f+(keys[0].spflags.continuity*keys[0].spflags.bias)) / 3.0f;
        Slerp(&(keys[0].quat),&(keys[1].quat),temp,&(tangentes_salida[0]));

        // Ultimo punto
        tangentes_salida[numero_keys-1].x =
        tangentes_salida[numero_keys-1].y =
        tangentes_salida[numero_keys-1].z =
        tangentes_salida[numero_keys-1].w = 0.0f;

        temp=(1.0f-keys[numero_keys-1].spflags.tension) *
            (1.0f-(keys[numero_keys-1].spflags.continuity*
                   keys[numero_keys-1].spflags.bias)) / 3.0f;
        Slerp(&(keys[numero_keys-1].quat),&(keys[numero_keys-2].quat),temp,
              &(tangentes_entrada[numero_keys-1]));
    }
}



//---------------------------------------------------------------------------------------------
// Parte 2: interpolación de cada frame (preparación de coordenadas)
//---------------------------------------------------------------------------------------------

// Obtiene la posicion de un objeto en un frame determinado


void InterpolaPosicion(int numero_keys,
		       Vector *t_entrada,
		       Vector *t_salida,
		       info_pos_path *keys,
		       float frame,
		       float *x,float *y,float *z)
{
   int            i;
   
   // frame menor que el inicial
   if (frame < keys[0].num_frame)
   {
       *x = keys[0].wx;
       *y = keys[0].wy;
       *z = keys[0].wz;
       return;
   }

   // frame mayor que el final
   if (frame > keys[numero_keys-1].num_frame)
   {
       *x = keys[numero_keys-1].wx;
       *y = keys[numero_keys-1].wy;
       *z = keys[numero_keys-1].wz;
       return;
   }

   // Buscar intervalo de interpolacion
   i = 1;
   while (frame > (float)keys[i].num_frame)
       i ++;
   // Interpolamos en el intervalo [(i-1), ..., i]
   // 't' debe estar entre [0,1] en cada intervalo (para poder hacer las optimizaciones oportunas)
   float t = (frame - (float)keys[i-1].num_frame) / ((float)(keys[i].num_frame - keys[i-1].num_frame));
   float t2 = t * t;
   float t3 = t * t * t;

   // Calculamos la base
   float h[4];
   h[0] = 2.0*t3 - 3.0 * t2 + 1.0;
   h[1] = -2.0 * t3 + 3.0 * t2;
   h[2] = t3 - 2.0 * t2 + t;
   h[3] = t3 - t2;

   *x = keys[i-1].wx * h[0] + keys[i].wx * h[1] + t_salida[i-1].x * h[2] + t_entrada[i].x * h[3];
   *y = keys[i-1].wy * h[0] + keys[i].wy * h[1] + t_salida[i-1].y * h[2] + t_entrada[i].y * h[3];
   *z = keys[i-1].wz * h[0] + keys[i].wz * h[1] + t_salida[i-1].z * h[2] + t_entrada[i].z * h[3];
}

// Obtiene la escala de un objeto en un frame determinado

void InterpolaScale(int numero_keys,
		    Vector *t_entrada,
		    Vector *t_salida,
		    info_scale_path *keys,
		    float frame,
		    float *scalex,float *scaley,float *scalez)
{
   int            i;
   
   // frame menor que el inicial
   if (frame < keys[0].num_frame)
   {
       *scalex = keys[0].scalex;
       *scaley = keys[0].scaley;
       *scalez = keys[0].scalez;
       return;
   }

   // frame mayor que el final
   if (frame > keys[numero_keys-1].num_frame)
   {
       *scalex = keys[numero_keys-1].scalex;
       *scaley = keys[numero_keys-1].scaley;
       *scalez = keys[numero_keys-1].scalez;
       return;
   }

   // Buscar intervalo de interpolacion
   i = 1;
   while (frame > (float)keys[i].num_frame)
       i ++;
   // Interpolamos en el intervalo [(i-1), ..., i]
   // 't' debe estar entre [0,1] en cada intervalo (para poder hacer las optimizaciones oportunas)
   float t = (frame - (float)keys[i-1].num_frame) / ((float)(keys[i].num_frame - keys[i-1].num_frame));
   float t2 = t * t;
   float t3 = t * t * t;

   // Calculamos la base
   float h[4];
   h[0] = 2.0*t3 - 3.0 * t2 + 1.0;
   h[1] = -2.0 * t3 + 3.0 * t2;
   h[2] = t3 - 2.0 * t2 + t;
   h[3] = t3 - t2;

   *scalex = keys[i-1].scalex * h[0] + keys[i].scalex * h[1] + t_salida[i-1].x * h[2] + t_entrada[i].x * h[3];
   *scaley = keys[i-1].scaley * h[0] + keys[i].scaley * h[1] + t_salida[i-1].y * h[2] + t_entrada[i].y * h[3];
   *scalez = keys[i-1].scalez * h[0] + keys[i].scalez * h[1] + t_salida[i-1].z * h[2] + t_entrada[i].z * h[3];
}




// Obtiene el fov de un objeto en un frame determinado

void InterpolaFov(int numero_keys,
		       float *t_entrada,
		       float *t_salida,
		       info_fov_path *keys,
		       float frame,
		       float *fov)
{
   int            i;
   
   // frame menor que el inicial
   if (frame < keys[0].num_frame)
   {
       *fov = keys[0].fov;       
       return;
   }

   // frame mayor que el final
   if (frame > keys[numero_keys-1].num_frame)
   {
       *fov = keys[numero_keys-1].fov;
       return;
   }

   // Buscar intervalo de interpolacion
   i = 1;
   while (frame > (float)keys[i].num_frame)
       i ++;
   // Interpolamos en el intervalo [(i-1), ..., i]
   // 't' debe estar entre [0,1] en cada intervalo (para poder hacer las optimizaciones oportunas)
   float t = (frame - (float)keys[i-1].num_frame) / ((float)(keys[i].num_frame - keys[i-1].num_frame));
   float t2 = t * t;
   float t3 = t * t * t;

   // Calculamos la base
   float h[4];
   h[0] = 2.0*t3 - 3.0 * t2 + 1.0;
   h[1] = -2.0 * t3 + 3.0 * t2;
   h[2] = t3 - 2.0 * t2 + t;
   h[3] = t3 - t2;

   *fov = keys[i-1].fov * h[0] + keys[i].fov * h[1] + t_salida[i-1] * h[2] + t_entrada[i] * h[3];
}

// Obtiene el roll de un objeto en un frame determinado

void InterpolaRoll(int numero_keys,
		       float *t_entrada,
		       float *t_salida,
		       info_roll_path *keys,
		       float frame,
		       float *roll)
{
   int            i;
   
   // frame menor que el inicial
   if (frame < keys[0].num_frame)
   {
       *roll = keys[0].roll;       
       return;
   }

   // frame mayor que el final
   if (frame > keys[numero_keys-1].num_frame)
   {
       *roll = keys[numero_keys-1].roll;
       return;
   }

   // Buscar intervalo de interpolacion
   i = 1;
   while (frame > (float)keys[i].num_frame)
       i ++;
   // Interpolamos en el intervalo [(i-1), ..., i]
   // 't' debe estar entre [0,1] en cada intervalo (para poder hacer las optimizaciones oportunas)
   float t = (frame - (float)keys[i-1].num_frame) / ((float)(keys[i].num_frame - keys[i-1].num_frame));
   float t2 = t * t;
   float t3 = t * t * t;

   // Calculamos la base
   float h[4];
   h[0] = 2.0*t3 - 3.0 * t2 + 1.0;
   h[1] = -2.0 * t3 + 3.0 * t2;
   h[2] = t3 - 2.0 * t2 + t;
   h[3] = t3 - t2;

   *roll = keys[i-1].roll * h[0] + keys[i].roll * h[1] + t_salida[i-1] * h[2] + t_entrada[i] * h[3];
}


void InterpolaRotacion(int numero_keys,
		       Quaternion *tangentes_entrada,
		       Quaternion *tangentes_salida,
		       info_rot_path *keys,
		       float frame,
		       Quaternion *quat)
{
   int            i;
   Quaternion q0,q1,q2;   
   Quaternion g1,g2;
   
   // frame menor que el inicial. Suponemos rotación 0 (ojo, es distinto a todo lo anterior!)
   if (frame < keys[0].num_frame)
   {
       /*quat->x = 0.0f;       
       quat->y = 0.0f;              
       quat->z = 0.0f;       
       quat->w = 0.0f;  */            
       quat->x = keys[0].quat.x;
       quat->y = keys[0].quat.y;
       quat->z = keys[0].quat.z;              
       quat->w = keys[0].quat.w;
       return;
   }
   // frame mayor que el final
   if (frame > keys[numero_keys-1].num_frame)
   {
       quat->x = keys[numero_keys-1].quat.x;
       quat->y = keys[numero_keys-1].quat.y;
       quat->z = keys[numero_keys-1].quat.z;              
       quat->w = keys[numero_keys-1].quat.w;
       return;
   }

   // Buscar intervalo de interpolacion
   i = 1;
   while (frame > (float)keys[i].num_frame) i ++;
   
   float t = (frame - (float)keys[i-1].num_frame) / ((float)(keys[i].num_frame - keys[i-1].num_frame));

   Slerp(&(keys[i-1].quat),&(tangentes_salida[i-1]),t,&q0);
   Slerp(&(tangentes_salida[i-1]),&(tangentes_entrada[i]),t,&q1);
   Slerp(&(tangentes_entrada[i]),&(keys[i].quat),t,&q2);

   Slerp(&q0,&q1,t,&g1);
   Slerp(&q1,&q2,t,&g2);

   Slerp(&g1,&g2,t,quat); // Y ya deberíamos tener el resultado :)
}

//---------------------------------------------------------------------------------------------
// Parte 3: cuestión histórica: implementación de referencia de las splines
//---------------------------------------------------------------------------------------------



/*
// Solamente calculamos las tangentes para cada "key"


// Funci¢n de interpolaci¢n de paths por splines
// Basada en un documento de digisnap (thx!)
// Notas:
//  - Considera que los splines no hacen loop
//  - Hay alguna forma de no hacer tanto malloc/free para las tangentes ???

void Fantasy::interpola_spline(info_pos_path *keys, info_pos_path *path, int numero_keys,int numero_frames)
{
    int i,j;
    int inicio,fin;
    float t,t2,t3,delta_t;
    Vector g1,g2,g3;
    Vector *tangentes_salida;
    Vector *tangentes_entrada;

    if(numero_keys > numero_frames) numero_keys=numero_frames;
    
    tangentes_entrada = (Vector *) malloc(numero_keys*sizeof(Vector));
    tangentes_salida = (Vector *) malloc(numero_keys*sizeof(Vector));

    // El proceso se calcula en dos fases:
    // 1. C lculo de los vectores tangentes de la spline en cada uno de los puntos
    // 2. Interpolaci¢n seg£n la f¢rmula de la spline de Hermite, con los puntos y los vectores

    
    // Si solo hay dos keys, las tangentes se calculan con una formula especial
    
    if(numero_keys == 2)
    {
     tangentes_entrada[0].x = tangentes_entrada[0].y = tangentes_entrada[0].z = 0.0f;
     tangentes_salida[1].x = tangentes_salida[1].y = tangentes_salida[0].z = 0.0f;

     tangentes_salida[0].x = (keys[1].wx - keys[0].wx) * (1-keys[0].spflags.tension);
     tangentes_salida[0].y = (keys[1].wy - keys[0].wy) * (1-keys[0].spflags.tension);
     tangentes_salida[0].z = (keys[1].wz - keys[0].wz) * (1-keys[0].spflags.tension);

     tangentes_entrada[1].x = (keys[1].wx - keys[0].wx) * (1-keys[1].spflags.tension);
     tangentes_entrada[1].y = (keys[1].wy - keys[0].wy) * (1-keys[1].spflags.tension);
     tangentes_entrada[1].z = (keys[1].wz - keys[0].wz) * (1-keys[1].spflags.tension);
    }
    else  // Calculamos las tangentes seg£n la f¢rmula general
    {
        for(i=1;i<numero_keys-1;i++)
        {
            g1.x = (keys[i].wx - keys[i-1].wx)*(1-keys[i].spflags.bias);
            g1.y = (keys[i].wy - keys[i-1].wy)*(1-keys[i].spflags.bias);
            g1.z = (keys[i].wz - keys[i-1].wz)*(1-keys[i].spflags.bias);
            g2.x = (keys[i+1].wx - keys[i].wx)*(1-keys[i].spflags.bias);
            g2.y = (keys[i+1].wy - keys[i].wy)*(1-keys[i].spflags.bias);
            g2.z = (keys[i+1].wz - keys[i].wz)*(1-keys[i].spflags.bias);
            g3.x = g2.x - g1.x;
            g3.y = g2.y - g1.y;
            g3.z = g2.z - g1.z;
            
            tangentes_entrada[i].x = (g1.x + g3.x*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_entrada[i].y = (g1.y + g3.y*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_entrada[i].z = (g1.z + g3.z*(0.5f+0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i].x =  (g1.x + g3.x*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i].y =  (g1.y + g3.y*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
            tangentes_salida[i].z =  (g1.z + g3.z*(0.5f-0.5f*keys[i].spflags.continuity))*(1-keys[i].spflags.tension);
        }
        // Primer punto

        tangentes_entrada[0].x = tangentes_entrada[0].y = tangentes_entrada[0].z = 0.0f;
        tangentes_salida[0].x = ((keys[1].wx - keys[0].wx)*1.5f - tangentes_entrada[1].x*0.5f) * (1-keys[0].spflags.tension);
        tangentes_salida[0].y = ((keys[1].wy - keys[0].wy)*1.5f - tangentes_entrada[1].y*0.5f) * (1-keys[0].spflags.tension);
        tangentes_salida[0].z = ((keys[1].wz - keys[0].wz)*1.5f - tangentes_entrada[1].z*0.5f) * (1-keys[0].spflags.tension);

        // éltimo punto
        tangentes_salida[numero_keys-1].x =
        tangentes_salida[numero_keys-1].y =
        tangentes_salida[numero_keys-1].z = 0.0f;

        tangentes_entrada[numero_keys-1].x =
            ((keys[numero_keys-1].wx - keys[numero_keys-2].wx)*1.5f - tangentes_salida[numero_keys-2].x*0.5f) * (1-keys[numero_keys-1].spflags.tension);
        tangentes_entrada[numero_keys-1].y =
            ((keys[numero_keys-1].wy - keys[numero_keys-2].wy)*1.5f - tangentes_salida[numero_keys-2].y*0.5f) * (1-keys[numero_keys-1].spflags.tension);
        tangentes_entrada[numero_keys-1].z =
            ((keys[numero_keys-1].wz - keys[numero_keys-2].wz)*1.5f - tangentes_salida[numero_keys-2].z*0.5f) * (1-keys[numero_keys-1].spflags.tension);
    }

  // Al fin tenemos todas las tangentes. Ahora s¢lo (!) hay que interpolar los puntos

  for(i=1;i<numero_keys;i++)
  {
    inicio = keys[i-1].num_frame;
    fin = keys[i].num_frame;

    delta_t = 1.0f/((float)(fin-inicio));
    t = 0.0f;

    for(j=inicio;j<fin;j++)
    {
        t2 = t*t;  // tiempo al cuadrado
        t3 = t2*t; // tiempo al cubo

        path[j].wx = keys[i-1].wx * (2*t3-3*t2+1) +
                     tangentes_salida[i-1].x * (t3-2*t2+t) +
                     keys[i].wx * (-2*t3+3*t2) +
                     tangentes_entrada[i].x * (t3-t2);
        path[j].wy = keys[i-1].wy * (2*t3-3*t2+1) +
                     tangentes_salida[i-1].y * (t3-2*t2+t) +
                     keys[i].wy * (-2*t3+3*t2) +
                     tangentes_entrada[i].y * (t3-t2);
        path[j].wz = keys[i-1].wz * (2*t3-3*t2+1) +
                     tangentes_salida[i-1].z * (t3-2*t2+t) +
                     keys[i].wz * (-2*t3+3*t2) +
                     tangentes_entrada[i].z * (t3-t2);
      t+=delta_t;
    }
  }

// Ya est  hecho todo. Lo que pasa es que a veces el £ltimo key no est  en
// el £ltimo frame, as¡ que vamos a solucionarlo ahora mismito

 j = keys[numero_keys-1].num_frame; // Este es el £ltimo frame con key

 path[j].wx = keys[numero_keys-1].wx;
 path[j].wy = keys[numero_keys-1].wy;
 path[j].wz = keys[numero_keys-1].wz;

 for(i=j+1;i<numero_frames;i++)
 {
  path[i].wx = path[j].wx;
  path[i].wy = path[j].wy;
  path[i].wz = path[j].wz;
 }

  free (tangentes_entrada);
  free (tangentes_salida);
}
*/




/*


void Fantasy::interpola_rotaciones(info_rot_path *keys, info_rot_path *path, int numero_keys,int numero_frames)
{
    int i,j;
    int inicio,fin;
    float t,delta_t;
    float temp;
    Quaternion g1,g2,g3;
    Quaternion q0,q1,q2;
    Quaternion *tangentes_salida;
    Quaternion *tangentes_entrada;

    if(numero_keys > numero_frames) numero_keys=numero_frames;

   
    tangentes_entrada = (Quaternion *) malloc(numero_keys*sizeof(Quaternion));
    tangentes_salida = (Quaternion *) malloc(numero_keys*sizeof(Quaternion));

    // El proceso se calcula en dos fases:
    // 1. C lculo de los vectores tangentes de la spline en cada uno de los puntos
    // 2. Interpolaci¢n seg£n la f¢rmula, con los puntos y los vectores

    // Si solo hay dos keys, las tangentes se calculan con una formula especial
    
    if(numero_keys == 2)
    {
        temp=(1.0f-keys[0].spflags.tension) *
            (1.0f+(keys[0].spflags.continuity*keys[0].spflags.bias)) / 3.0f;
        Slerp(&(keys[0].quat),&(keys[1].quat),temp,&(tangentes_salida[0]));

        temp=(1.0f-keys[1].spflags.tension) *
            (1.0f-(keys[1].spflags.continuity*keys[1].spflags.bias)) / 3.0f;
        Slerp(&(keys[1].quat),&(keys[0].quat),temp,&(tangentes_entrada[1]));

        tangentes_entrada[0].x = tangentes_entrada[0].y =
            tangentes_entrada[0].z = tangentes_entrada[0].w = 0.0f;
        tangentes_salida[1].x = tangentes_salida[1].y =
            tangentes_salida[1].z = tangentes_salida[1].w = 0.0f;
    }
    else  // Calculamos las tangentes seg£n la f¢rmula general
    {
        for(i=1;i<numero_keys-1;i++)
        {
            // Calculamos g1
            Slerp(&(keys[i].quat),&(keys[i-1].quat),
                  -(1.0f+keys[i].spflags.bias)/3.0f,&g1);
            // Calculamos g2
            Slerp(&(keys[i].quat),&(keys[i+1].quat),
                  (1.0f-keys[i].spflags.bias)/3.0f,&g2);
            // Calculamos g3 para el caso entrante
            Slerp(&g1,&g2,0.5f+0.5f*keys[i].spflags.continuity,&g3);
            // Calculamos la tangente entrante
            Slerp(&(keys[i].quat),&g3,keys[i].spflags.tension-1.0f,
                  &(tangentes_entrada[i]));
            // Calculamos g3 para el caso saliente
            Slerp(&g1,&g2,0.5f-0.5f*keys[i].spflags.continuity,&g3);
            // Calculamos la tangente de salida
            Slerp(&(keys[i].quat),&g3,-(keys[i].spflags.tension-1.0f),
                  &(tangentes_salida[i]));
        }
        // Primer punto

        tangentes_entrada[0].x = tangentes_entrada[0].y =
            tangentes_entrada[0].z = tangentes_entrada[0].w = 0.0f;

        temp=(1.0f-keys[0].spflags.tension) *
            (1.0f+(keys[0].spflags.continuity*keys[0].spflags.bias)) / 3.0f;
        Slerp(&(keys[0].quat),&(keys[1].quat),temp,&(tangentes_salida[0]));

        // Ultimo punto
        tangentes_salida[numero_keys-1].x =
        tangentes_salida[numero_keys-1].y =
        tangentes_salida[numero_keys-1].z =
        tangentes_salida[numero_keys-1].w = 0.0f;

        temp=(1.0f-keys[numero_keys-1].spflags.tension) *
            (1.0f-(keys[numero_keys-1].spflags.continuity*
                   keys[numero_keys-1].spflags.bias)) / 3.0f;
        Slerp(&(keys[numero_keys-1].quat),&(keys[numero_keys-2].quat),temp,
              &(tangentes_entrada[numero_keys-1]));
    }

  // Al fin tenemos todas las tangentes. Ahora s¢lo (!) hay que interpolar los puntos

  for(i=1;i<numero_keys;i++)
  {
    inicio = keys[i-1].num_frame;
    fin = keys[i].num_frame;

    delta_t = 1.0f/((float)(fin-inicio));
    t = 0.0f;

    for(j=inicio;j<fin;j++)
    {
        Slerp(&(keys[i-1].quat),&(tangentes_salida[i-1]),t,&q0);
        Slerp(&(tangentes_salida[i-1]),&(tangentes_entrada[i]),t,&q1);
        Slerp(&(tangentes_entrada[i]),&(keys[i].quat),t,&q2);

        // Ahora vuelvo a usar g1 y g2, pero con distinto significado
        // al anterior (ojo!)

        Slerp(&q0,&q1,t,&g1);
        Slerp(&q1,&q2,t,&g2);

        Slerp(&g1,&g2,t,&(path[j].quat));

        t+=delta_t;
    }
  }

// Ya est  hecho todo. Lo que pasa es que a veces el £ltimo key no est  en
// el £ltimo frame, as¡ que vamos a solucionarlo ahora mismito

 j = keys[numero_keys-1].num_frame; // Este es el £ltimo frame con key

 path[j].quat.x = keys[numero_keys-1].quat.x;
 path[j].quat.y = keys[numero_keys-1].quat.y;
 path[j].quat.z = keys[numero_keys-1].quat.z;
 path[j].quat.w = keys[numero_keys-1].quat.w;

 for(i=j+1;i<numero_frames;i++)
 {
  path[i].quat.x = path[j].quat.x;
  path[i].quat.y = path[j].quat.y;
  path[i].quat.z = path[j].quat.z;
  path[i].quat.w = path[j].quat.w;
 }

 // Para cada frame anterior al primer key, se supone que no hay rotación


 j = keys[0].num_frame;	
 if(j>0)
 {
  for(i=0;i<j;i++)
  {
   path[i].quat.x = 0.0f;
   path[i].quat.y = 0.0f;
   path[i].quat.z = 0.0f;
   path[i].quat.w = 0.0f;   
  }
 }

  free (tangentes_entrada);
  free (tangentes_salida);
}

*/