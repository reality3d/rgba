#ifdef WINDOZE
 #include <windows.h>
#endif

#include <stdio.h>
#include <string.h> // Necesario para linux, strlen es una macro
#include "3dworld.h"
#include "misc.h"

char FANTASY_DEBUG_STRING[1024];
int DEBUG_CURRENT_LEVEL=DEBUG_LEVEL_WARNING;
//int DEBUG_CURRENT_LEVEL=DEBUG_LEVEL_OVERDEBUG;

void Debug(int debuglevel)
{
 FILE *p;

 if(debuglevel <= DEBUG_CURRENT_LEVEL)
 {
    p=fopen("debug_fantasy.txt","a");
    if(!p) return;
    fputs(FANTASY_DEBUG_STRING,p);
    fclose(p);
 } 
}

//-------------------------------------------------------------
// Funciones para volcar la estructura cargada en memoria
// Util para encontrar bugs del exportador/cargador
//-------------------------------------------------------------

static void DumpPosPath(info_pos_path *path, int nkeys,FILE *p)
{
 int i;		
 if(!path) return;	
 
 for(i=0;i<nkeys;i++)
 {
  fprintf(p,"\t\t\tKey %d. Frame: %d. Pos: %f %f %f. Tension: %f. Cont: %f. Bias: %f\n",
  	 i,path[i].num_frame,path[i].wx,path[i].wy,path[i].wz,
  	 path[i].spflags.tension,path[i].spflags.continuity,path[i].spflags.bias);
 }
}

static void DumpRotPath(info_rot_path *path, int nkeys,FILE *p)
{
 int i;
 if(!path) return;	
 
 for(i=0;i<nkeys;i++)
 {
  fprintf(p,"\t\t\tKey %d. Frame: %d. Rot: %f %f %f %f. Tension: %f. Cont: %f. Bias: %f\n",
  	 i,path[i].num_frame,path[i].quat.x,path[i].quat.y,path[i].quat.z,path[i].quat.w,
  	 path[i].spflags.tension,path[i].spflags.continuity,path[i].spflags.bias);
 }
}

static void DumpScalePath(info_scale_path *path, int nkeys,FILE *p)
{
 int i;
 
 if(!path) return;	
 for(i=0;i<nkeys;i++)
 {
  fprintf(p,"\t\t\tKey %d. Frame: %d. Pos: %f %f %f. Tension: %f. Cont: %f. Bias: %f\n",
  	 i,path[i].num_frame,path[i].scalex,path[i].scaley,path[i].scalez,
  	 path[i].spflags.tension,path[i].spflags.continuity,path[i].spflags.bias);
 }
}


static void DumpRollPath(info_roll_path *path, int nkeys, FILE *p)
{
 int i;
 if(!path) return;	
 for(i=0;i<nkeys;i++)
 {
  fprintf(p,"\t\t\tKey %d. Frame: %d. Roll: %f. Tension: %f. Cont: %f. Bias: %f\n",
  	 i,path[i].num_frame,path[i].roll,path[i].spflags.tension,path[i].spflags.continuity,path[i].spflags.bias);
 }
}

static void DumpFovPath(info_fov_path *path, int nkeys, FILE *p)
{
 int i;
 if(!path) return;	
 for(i=0;i<nkeys;i++)
 {
  fprintf(p,"\t\t\tKey %d. Frame: %d. Fov: %f. Tension: %f. Cont: %f. Bias: %f\n",
  	 i,path[i].num_frame,path[i].fov,path[i].spflags.tension,path[i].spflags.continuity,path[i].spflags.bias);
 }
}



void DumpWorld(world_type *world)
{
 FILE *p;
 int i,j;
 
 p=fopen("world_debug.txt","a");
 if(!p) return;
 
 fprintf(p,"Numero de objetos: %d. Numero de fuentes: %d. Numero de camaras: %d. Numero de materiales: %d. Numero de bones: %d\n",
 	 world->number_of_objects,world->number_of_fuentes,world->number_of_camaras,world->number_of_materials,world->number_of_bones);
 fprintf(p,"Frame inicial: %d. Frame final: %d. Num frames: %d\n",
 	 world->frame_inicial,world->frame_final,world->num_frames);
 fprintf(p,"LUCES\n-----\n\n");
 
 for(i=0;i<world->number_of_fuentes;i++)
 {
  fprintf(p,"\tFuente: %s. Id: %d\n",world->fuentes[i].nombre_fuente,world->fuentes[i].id);
  fprintf(p,"\t\tPosicion: %f %f %f. Color: %f %f %f\n",world->fuentes[i].wx,world->fuentes[i].wy,world->fuentes[i].wz,
  	  world->fuentes[i].r,world->fuentes[i].g,world->fuentes[i].b);
  fprintf(p,"\t\tInicio rango: %f. Fin rango: %f. Multiplicador: %f. Atenuacion: %d\n",
          world->fuentes[i].inicio_rango,world->fuentes[i].fin_rango,world->fuentes[i].multiplicador,
          world->fuentes[i].atenuacion);
  fprintf(p,"\t\tKeys: %d\n",world->fuentes[i].numero_keys_path);
  DumpPosPath(world->fuentes[i].posiciones_keys,world->fuentes[i].numero_keys_path,p);
 }
 
 fprintf(p,"CAMARAS\n-------\n\n"); 
 
 for(i=0;i<world->number_of_camaras;i++) 
 {
  fprintf(p,"\tCamara: %s. Id: %d. Id target: %d\n",world->camaras[i].nombre_camara,world->camaras[i].id,world->camaras[i].target_id);
  fprintf(p,"\t\tPosicion: %f %f %f. Posicion del objetivo %f %f %f\n",world->camaras[i].wx,world->camaras[i].wy,world->camaras[i].wz,
  	  world->camaras[i].wxt,world->camaras[i].wyt,world->camaras[i].wzt);
  fprintf(p,"\t\tRoll: %f. Fov: %f. Near range: %f. Far range: %f\n",
 	  world->camaras[i].inclinacion,world->camaras[i].fov,world->camaras[i].near_range,world->camaras[i].far_range);
  fprintf(p,"\t\tKeys: %d\n",world->camaras[i].numero_keys_path);
  DumpPosPath(world->camaras[i].posiciones_keys,world->camaras[i].numero_keys_path,p);
  fprintf(p,"\t\tKeys target: %d\n",world->camaras[i].numero_keys_path_target);
  DumpPosPath(world->camaras[i].posiciones_keys_target,world->camaras[i].numero_keys_path_target,p);
  fprintf(p,"\t\tKeys roll: %d\n",world->camaras[i].numero_keys_rollpath);
  DumpRollPath(world->camaras[i].posiciones_rollkeys,world->camaras[i].numero_keys_rollpath,p);
  fprintf(p,"\t\tKeys fov: %d\n",world->camaras[i].numero_keys_fovpath);
  DumpFovPath(world->camaras[i].posiciones_fovkeys,world->camaras[i].numero_keys_fovpath,p);

 }
 fprintf(p,"OBJETOS\n-------\n\n"); 
 for(i=0;i<world->number_of_objects;i++)
 {
  fprintf(p,"\tObjeto: %s. Id: %d.\n",world->obj[i].nombre_objeto,world->obj[i].id);	
  fprintf(p,"\t\tPosicion: %f %f %f. Pivot: %f %f %f\n",world->obj[i].x,world->obj[i].y,world->obj[i].z,
          world->obj[i].pivotx,world->obj[i].pivoty,world->obj[i].pivotz);
  fprintf(p,"\t\tEscala: %f %f %f. Rotacion: %f %f %f %f\n",world->obj[i].xscale,world->obj[i].yscale,world->obj[i].zscale,
  	  world->obj[i].current_rotation.x,world->obj[i].current_rotation.y,world->obj[i].current_rotation.z,world->obj[i].current_rotation.w);
  fprintf(p,"\t\t# vertices: %d. # poligonos: %d. # coord. UV: %d\n",
  	  world->obj[i].number_of_vertices,world->obj[i].number_of_polygons,world->obj[i].number_of_uv);
  fprintf(p,"\t\tMaterial: %d. Centro bsphere: %f %f %f. Radio bsphere: %f\n",
  	  world->obj[i].num_material, world->obj[i].bspherecenter[0],world->obj[i].bspherecenter[1],
  	  world->obj[i].bspherecenter[2],world->obj[i].bsphereradius);
  fprintf(p,"\t\t,USpeed: %f. VSpeed: %f\n",world->obj[i].USpeed,world->obj[i].VSpeed);
  fprintf(p,"\t\t,Normalize: %d. Animate: %d. Visible: %d. Vtx shader: %u, Render mode: %d\n",
          world->obj[i].normalize,world->obj[i].animate,world->obj[i].visible,world->obj[i].vtx_shader_id,
          world->obj[i].render_mode);
  fprintf(p,"\t\tNumber of bones: %d\n",world->obj[i].numbones);
  fprintf(p,"\t\tKeys: %d\n",world->obj[i].numero_keys_path);
  DumpPosPath(world->obj[i].posiciones_keys,world->obj[i].numero_keys_path,p);
  fprintf(p,"\t\tKeys rotacion: %d\n",world->obj[i].numero_keys_path_rotacion);
  DumpRotPath(world->obj[i].path_rotacion,world->obj[i].numero_keys_path_rotacion,p);
  fprintf(p,"\t\tScale Keys: %d\n",world->obj[i].numero_keys_path_scale);
  DumpScalePath(world->obj[i].scale_keys,world->obj[i].numero_keys_path_scale,p);

  for(j=0;j<world->obj[i].number_of_vertices;j++)
  {
   fprintf(p,"%f %f %f\n",world->obj[i].VertexPointer[3*j],world->obj[i].VertexPointer[3*j+1],world->obj[i].VertexPointer[3*j+2]);	    	
  }


  if((world->obj[i].BoneIds) && (world->obj[i].BoneWeights))
  {
	fprintf(p,"Bones IDs por vertice y pesos asociados:\n");
	for(j=0;j<world->obj[i].number_of_vertices;j++) 
	{
		fprintf(p,"%d ",world->obj[i].BoneIds[4*j]);
		fprintf(p,"%d ",world->obj[i].BoneIds[4*j+1]);
		fprintf(p,"%d ",world->obj[i].BoneIds[4*j+2]);
		fprintf(p,"%d ",world->obj[i].BoneIds[4*j+3]);
		fprintf(p,"%f ",world->obj[i].BoneWeights[4*j]);
		fprintf(p,"%f ",world->obj[i].BoneWeights[4*j+1]);
		fprintf(p,"%f ",world->obj[i].BoneWeights[4*j+2]);
		fprintf(p,"%f ",world->obj[i].BoneWeights[4*j+3]);
		
		fprintf(p,"%f\n",world->obj[i].BoneWeights[4*j]+world->obj[i].BoneWeights[4*j+1]+world->obj[i].BoneWeights[4*j+2]+world->obj[i].BoneWeights[4*j+3]);

	}
  }
  fprintf(p,"\n");
  /*if(world->obj[i].BoneWeights)
  {
	fprintf(p,"Pesos de los bones por vertice: ");
	for(j=0;j<world->obj[i].number_of_vertices;j++) 
	{
		
		fprintf(p,"%f ",world->obj[i].BoneWeights[4*j]);
		fprintf(p,"%f ",world->obj[i].BoneWeights[4*j+1]);
		fprintf(p,"%f ",world->obj[i].BoneWeights[4*j+2]);
		fprintf(p,"%f ",world->obj[i].BoneWeights[4*j+3]);
		
		fprintf(p,"%f\n",world->obj[i].BoneWeights[4*j]+world->obj[i].BoneWeights[4*j+1]+world->obj[i].BoneWeights[4*j+2]+world->obj[i].BoneWeights[4*j+3]);
	}
  }
  fprintf(p,"\n");
*/
 }
 fprintf(p,"BONES\n-----\n\n"); 
 for(i=0;i<world->number_of_bones;i++)
 {
  fprintf(p,"\tBone: %s. Id: %d. Parentid %d\n",world->bones[i].nombre,world->bones[i].id,world->bones[i].parentid);	
 
  if(world->bones[i].hierarchy)
	  fprintf(p,"Primer hijo, id %d\n",world->bones[i].hierarchy->hijo->id);
  DumpPosPath(world->bones[i].posiciones_keys,world->bones[i].numero_keys_path,p);
  DumpRotPath(world->bones[i].path_rotacion,world->bones[i].numero_keys_path_rotacion,p);
 
 
 
 }
 fclose(p);	
}

int strmatch(char *pattern, char *cadena)
{
 int i,j,k;
 int length_pattern,length_string;
 
 length_string = strlen(cadena);	
 length_pattern = strlen(pattern);	
 
 for(i=0,j=0;i<length_pattern;i++)
 {
  switch(pattern[i])
  {

   case '*': if(i + 1 == length_pattern) return 1; // El * está al final, luego cualquier cosa vale
   	     for(k=j+1;k<length_string;k++)
   	     {
   	      if(strmatch(pattern+i+1,cadena+k)) return 1;
   	     } 
   	     return 0;
   	     break;
   case '?': j++; // Saltamos cualquier caracter
   	     if(j > length_string) return 0; // Nos hemos pasado del final 
   		break;
   default: if(j >= length_string) return 0;  // Estamos en el final
   	    if(pattern[i] != cadena[j]) return 0; // son diferentes
   	    j++;
   		break;	
  }
 }
  if(j == length_string) return 1;
  return 0;			
}