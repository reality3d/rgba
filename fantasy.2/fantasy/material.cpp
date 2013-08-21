#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define MEGADEBUG

#include "material.h"
#include "textura.h"
#include <packer/packer.h>
#include "misc.h"
#include "extgl.h"
#include "mates.h"
#include "glsl.h"

extern CacheTexturas *ConjuntoTexturas;

static GLuint TextureUnits[32]={GL_TEXTURE0_ARB,GL_TEXTURE1_ARB,GL_TEXTURE2_ARB,GL_TEXTURE3_ARB,GL_TEXTURE4_ARB,GL_TEXTURE5_ARB,GL_TEXTURE6_ARB,GL_TEXTURE7_ARB,
GL_TEXTURE8_ARB,GL_TEXTURE9_ARB,GL_TEXTURE10_ARB,GL_TEXTURE11_ARB,GL_TEXTURE12_ARB,GL_TEXTURE13_ARB,GL_TEXTURE14_ARB,GL_TEXTURE15_ARB,
GL_TEXTURE16_ARB,GL_TEXTURE17_ARB,GL_TEXTURE18_ARB,GL_TEXTURE19_ARB,GL_TEXTURE20_ARB,GL_TEXTURE21_ARB,GL_TEXTURE22_ARB,GL_TEXTURE23_ARB,
GL_TEXTURE24_ARB,GL_TEXTURE25_ARB,GL_TEXTURE26_ARB,GL_TEXTURE27_ARB,GL_TEXTURE28_ARB,GL_TEXTURE29_ARB,GL_TEXTURE30_ARB,GL_TEXTURE31_ARB
};


static void token2string(char *token, char *string)
{
 int i;

 i=0;
 while((*token != ' ') && (*token != '\t') && (*token !='\0') && (*token != '\n'))
 {
  string[i] = *token++;
  string[i] = tolower(string[i]);
  i++;
 }
 string[i]='\0';
}


//
// Función principal de carga de materiales
//


int Material::MatReader(char *filename)
{
 material *matpointer=NULL;
 PFILE *p;
 char cadena[256];
 char cadena2[256];
 char texname[256];
 char *token;
 char separadores [] = " \t";
 int i,j;
 int nmats=0; 			// Number of materials
 int curmat=0;			// Material currently being loaded
 int ntextures;


 p = pak_open(filename,"r");
 if(!p) return -1;


 while(pak_fgets(cadena,256,p))
 {

  if((cadena[0] == '\n') || (cadena[0] == '#')) continue; // Saltamos comentarios y líneas en blanco
  token = strtok(cadena,separadores);
  token2string(token,cadena2);

   if(!strcmp(cadena2,"nmaterials"))  // Leemos el número de materiales y seguimos
   {
    token = strtok(NULL,separadores);
    token2string(token,cadena2);
    nmats = atoi(cadena2);
    matpointer = (material *) malloc(nmats * sizeof(material));    
    if(!matpointer) return -1;
    
    continue;
   }
   else	  // En este caso hemos cogido el nombre de un material
   {
    if(!nmats)
    {
     sprintf (FANTASY_DEBUG_STRING, "Fallo al leer el fichero %s\n",filename);
     Debug(DEBUG_LEVEL_ERROR);
     return -1;
    }

    if(curmat >= nmats)
    {     
       sprintf (FANTASY_DEBUG_STRING, "Error en el archivo %s. Tenemos más materiales de los definidos en nmats\n",filename);
       Debug(DEBUG_LEVEL_ERROR);
       return -1;
    }
    // Inicializamos algunos valores por defecto
    matpointer[curmat].specular[0] = matpointer[curmat].specular[1] = matpointer[curmat].specular[2] = 0.0f;
    matpointer[curmat].emission[0] = matpointer[curmat].emission[1] = matpointer[curmat].emission[2] = 0.0f;
    matpointer[curmat].specular[3] = 1.0f;
    matpointer[curmat].emission[3] = 1.0f;
    matpointer[curmat].shininess = 0.0f;
    // Empezamos con la juerga!     
    
    strcpy(matpointer[curmat].matname,cadena2);
    token = strtok(NULL,separadores);	 // Ahora leemos si es estático o dinámico
    token2string(token,cadena2);

    if(!strcmp(cadena2,"static")) 	// Material estático
    {
     matpointer[curmat].shader_based = 0;
    }
    else if(!strcmp(cadena2,"shader")) // Shader based material 
    {
        matpointer[curmat].shader_based = 1;
    }
    else
    {     
       sprintf (FANTASY_DEBUG_STRING, "Material %s no valido. No es static ni shader\n",matpointer[curmat].matname);
       Debug(DEBUG_LEVEL_ERROR);
       return -1;
    }

    // En caso de ser un shader, lo siguiente son los ficheros de los shader
    if(matpointer[curmat].shader_based == 1 )
    {
      token = strtok(NULL,separadores);
      token2string(token,cadena2);
      
      if(strcmp(cadena2,"standard")) // If shader_name == "standard", we are using the opengl pipeline
      	GLSL_LoadShader(&(matpointer[curmat].shader.vertexShader),cadena2, GLSL_VERTEX_SHADER);
      else
        matpointer[curmat].shader.vertexShader = 0;
       
      token = strtok(NULL,separadores);
      token2string(token,cadena2);
      
      // There must always be a fragment shader      
      GLSL_LoadShader(&(matpointer[curmat].shader.fragmentShader),cadena2, GLSL_FRAGMENT_SHADER);
      
      // CompileandLinkShader...	      
      GLSL_CompileAndLinkShader(&(matpointer[curmat].shader.programObj),
      				matpointer[curmat].shader.vertexShader,
      				matpointer[curmat].shader.fragmentShader);      
      // GetBasicUniforms
      GLSL_GetUniforms(&(matpointer[curmat]));
      
      matpointer[curmat].shader.params.used=0; // no custom parameters to the shader

    } 
    else // No shader, so let's fill the variables...
    {
    	matpointer[curmat].shader.vertexShader = 0;
    	matpointer[curmat].shader.fragmentShader = 0;
    	matpointer[curmat].shader.programObj = 0;
    }
     token = strtok(NULL,separadores);	 // Ahora leemos si es opaco o transparente
     token2string(token,cadena2);
     if(!strcmp(cadena2,"opaque"))	// Material opaco
     {
      matpointer[curmat].opaque = 1;
      matpointer[curmat].blend_type=-1;

      token = strtok(NULL,separadores);	 // Ahora leemos si es opaco o transparente
      token2string(token,cadena2);
      if(!strcmp(cadena2,"2sided"))	// Material con 2 caras
      {
       matpointer[curmat].twosided = 1;
      }
      else if(!strcmp(cadena2,"1sided"))	// Material con 2 caras
      {
       matpointer[curmat].twosided = 0;
      }
      else 
      {     
       sprintf (FANTASY_DEBUG_STRING, "Material %s no valido. No tiene 1 ni 2 caras\n",matpointer[curmat].matname);
       Debug(DEBUG_LEVEL_ERROR);
       return -1;
      }
     }
     else if(!strcmp(cadena2,"transp")) // Material traslúcido
     {
      matpointer[curmat].opaque=0;

      token = strtok(NULL,separadores);	 // Ahora leemos si es opaco o transparente
      token2string(token,cadena2);
      if(!strcmp(cadena2,"2sided"))	// Material con 2 caras
      {
       matpointer[curmat].twosided = 1;
      }
      else if(!strcmp(cadena2,"1sided"))	// Material con 2 caras
      {
       matpointer[curmat].twosided = 0;
      }
      else
      {     
       sprintf (FANTASY_DEBUG_STRING, "Material %s no valido. No tiene 1 ni 2 caras\n",matpointer[curmat].matname);
       Debug(DEBUG_LEVEL_ERROR);
       return -1;
      }

      token = strtok(NULL,separadores);	 // Ahora leemos el tipo de blend
      token2string(token,cadena2);
      if(!strcmp(cadena2,"add"))  	// Blend aditivo
      {
       matpointer[curmat].blend_type=BLEND_ADD;
      }
      else if(!strcmp(cadena2,"blend"))  // Blend normal
      {
       matpointer[curmat].blend_type=BLEND_BLEND;
      }
      else if(!strcmp(cadena2,"multiply"))  // Blend multiplicativo
      {
       matpointer[curmat].blend_type=BLEND_MULTIPLY;
      }
      else if(!strcmp(cadena2,"sub"))  	// Blend sustractivo
      {
       matpointer[curmat].blend_type=BLEND_SUB;      	
      }
      else 
      {     
       sprintf (FANTASY_DEBUG_STRING, "Material %s no valido. El blend no es valido\n",matpointer[curmat].matname);
       Debug(DEBUG_LEVEL_ERROR);
       return -1;
      }
     }
     else
     {     
       sprintf (FANTASY_DEBUG_STRING, "Material %s no valido. No es opaque ni transp\n",matpointer[curmat].matname);
       Debug(DEBUG_LEVEL_ERROR);
       return -1;
     }       
     // Hasta aquí tenemos el nombre del material, que es estático, su opacidad y el tipo de blend si aplica.
     // Nos falta leer el número de texturas y las texturas

     token = strtok(NULL,separadores);
     token2string(token,cadena2);
     ntextures= atoi(cadena2);

     if(ntextures < 0)
     {     
       sprintf (FANTASY_DEBUG_STRING, "Material %s no valido. No puede tener menos de 0 texturas\n",matpointer[curmat].matname);
       Debug(DEBUG_LEVEL_ERROR);
       return -1;
     }
     else if(ntextures == 0)
     {
         matpointer[curmat].textures=NULL;
         matpointer[curmat].multitexblend=NULL;
         matpointer[curmat].texcoordgen=NULL;
     }
     else if(ntextures == 1)
     {
         matpointer[curmat].textures = (int *) malloc (ntextures * sizeof(int));
         matpointer[curmat].multitexblend = (int *) malloc ((ntextures) * sizeof(int));
         matpointer[curmat].texcoordgen = (int *) malloc ((ntextures) * sizeof(int));;
         matpointer[curmat].linearmultiplier = (float *)malloc((ntextures) * sizeof(float));
     }
     else
     {
         matpointer[curmat].textures = (int *) malloc (ntextures * sizeof(int));
         matpointer[curmat].multitexblend = (int *) malloc ((ntextures) * sizeof(int));
         matpointer[curmat].texcoordgen = (int *) malloc ((ntextures) * sizeof(int));
         matpointer[curmat].linearmultiplier = (float *)malloc((ntextures) * sizeof(float));
     }
  
     matpointer[curmat].ntextures = ntextures;

     // Cargamos las texturas
     for(i=0;i<ntextures;i++)
     {
      token = strtok(NULL,separadores);
      token2string(token,cadena2);

      // La carga de la textura la haremos después de conocer si hay cubemap o no!
      
      strcpy(texname,cadena2);
     
      // Carga del tipo de coordenadas de textura
      // En caso de usar shader, aunque se ignoren los tipos de coordenada, lo usamos
      // para saber cuándo una textura es un cubemap...
      token = strtok(NULL,separadores);
      token2string(token,cadena2);

      if((!strcmp(cadena2,"same")) || (!strcmp(cadena2,"uv")))
      {
       matpointer[curmat].texcoordgen[i]=TEXGEN_UV;
      }
      else if(!strcmp(cadena2,"eyelinear"))
      {
       matpointer[curmat].texcoordgen[i]=TEXGEN_EYE_LINEAR;
       if(matpointer[curmat].shader_based == 1 )
       {
        sprintf (FANTASY_DEBUG_STRING, "Warning: el material %s está basado en shader, pero se ha seleccionado texgen tipo eyelinear. Pongo UV\n",matpointer[curmat].matname);
        Debug(DEBUG_LEVEL_ERROR);
        matpointer[curmat].texcoordgen[i]=TEXGEN_UV;
       }
      }
      else if(!strcmp(cadena2,"objectlinear"))
      {
       matpointer[curmat].texcoordgen[i]=TEXGEN_OBJECT_LINEAR;
       if(matpointer[curmat].shader_based == 1 )
       {
        sprintf (FANTASY_DEBUG_STRING, "Warning: el material %s está basado en shader, pero se ha seleccionado texgen tipo objectlinear. Pongo UV\n",matpointer[curmat].matname);
        Debug(DEBUG_LEVEL_ERROR);
        matpointer[curmat].texcoordgen[i]=TEXGEN_UV;
       }
      }
      else if(!strcmp(cadena2,"spheremap"))
      {
       matpointer[curmat].texcoordgen[i]=TEXGEN_SPHERE_MAP;
       if(matpointer[curmat].shader_based == 1 )
       {
        sprintf (FANTASY_DEBUG_STRING, "Warning: el material %s está basado en shader, pero se ha seleccionado texgen tipo spheremap. Pongo UV\n",matpointer[curmat].matname);
        Debug(DEBUG_LEVEL_ERROR);
        matpointer[curmat].texcoordgen[i]=TEXGEN_UV;
       }
      }     	
      else if(!strcmp(cadena2,"cubemap"))
      {
       matpointer[curmat].texcoordgen[i]=TEXGEN_CUBE_MAP;
      }    
      // Ahora podemos cargar la textura
      if(matpointer[curmat].texcoordgen[i] != TEXGEN_CUBE_MAP)
      {
     	matpointer[curmat].textures[i] = ConjuntoTexturas->CargaTextura(texname);
     	        
      }
      else
      {      	
      	matpointer[curmat].textures[i] = ConjuntoTexturas->CargaCubeMap(texname);
      }

      // Si las coordenadas son eyelinear u objectlinear, cogemos el multiplicador
      if((matpointer[curmat].texcoordgen[i]==TEXGEN_OBJECT_LINEAR) ||
      (matpointer[curmat].texcoordgen[i]==TEXGEN_EYE_LINEAR))
      {
       token = strtok(NULL,separadores);
       token2string(token,cadena2);
       matpointer[curmat].linearmultiplier[i]=atof(cadena2);
      }  
      // Finalmente, si no es la primera unidad de textura, vemos qué tipo de blend
      // utiliza esta unidad de textura
      if((i>0) && (matpointer[curmat].shader_based != 1)) // Para tipo shader no se carga
      {
	token = strtok(NULL,separadores);
      	token2string(token,cadena2);

      	if(!strcmp(cadena2,"add"))  	// Blend aditivo
      	{
       	 matpointer[curmat].multitexblend[i]=BLEND_ADD;
      	}
      	else if(!strcmp(cadena2,"modulate"))  // Modulate
      	{
       	 matpointer[curmat].multitexblend[i]=BLEND_MODULATE;
      	}
      	else if(!strcmp(cadena2,"mask"))	// Mask
      	{
	 matpointer[curmat].multitexblend[i]=BLEND_MASK;
      	}
      	else if(!strcmp(cadena2,"mask2"))	// Mask
      	{
      	 matpointer[curmat].multitexblend[i]=BLEND_MASK2;
      	}
      	else if(!strcmp(cadena2,"dot3"))	// Dot3 bump mapping
      	{
	 matpointer[curmat].multitexblend[i]=BLEND_DOT3;
      	}      	
      	else // Por defecto dejamos replace, para darnos cuenta del error
       {     
        sprintf (FANTASY_DEBUG_STRING, "Material %s no valido. Multitex blend no valido\n",matpointer[curmat].matname);
        Debug(DEBUG_LEVEL_ERROR);
        return -1;
       }

      }
      else
        matpointer[curmat].multitexblend[i]=BLEND_MODULATE;
     	
     } // Fin del bucle for

     // Ahora vamos a cargar los parámetros relativos a la iluminación. Ojo, que son opcionales,
     // y pueden ir en cualquier orden!!!
     for(i=0;i<3;i++)
     {
      token = strtok(NULL,separadores);
      if(token == NULL) break;
      token2string(token,cadena2);
      if(!strcmp(cadena2,"specular"))
      {
       	for(j=0;j<3;j++)  // Leemos las 4 componentes
       	{
         token = strtok(NULL,separadores);
         token2string(token,cadena2);
         matpointer[curmat].specular[j]=2.0f*atof(cadena2);
       	}
       	matpointer[curmat].specular[3] = 1.0f;
      }
      else if(!strcmp(cadena2,"emission"))
      {
       	for(j=0;j<3;j++)  // Leemos las 4 componentes
       	{
         token = strtok(NULL,separadores);
         token2string(token,cadena2);
         matpointer[curmat].emission[j]=atof(cadena2);
       	}
       	matpointer[curmat].emission[j] = 1.0f;
      }
      else if((!strcmp(cadena2,"shininess")) || (!strcmp(cadena2,"glossiness")))
      {
         token = strtok(NULL,separadores);
         token2string(token,cadena2);
         matpointer[curmat].shininess=atof(cadena2);
      }
      else 
      {     
       sprintf (FANTASY_DEBUG_STRING, "Material %s no valido. Parámetros de iluminación no válidos\n",matpointer[curmat].matname);
       Debug(DEBUG_LEVEL_ERROR);
       return -1;
      }

     }

     
    }
    curmat++;
   
 }

 materiales = matpointer;
 numero_materiales = nmats;
 pak_close(p);

 if(curmat < nmats)
 {     
   sprintf (FANTASY_DEBUG_STRING, "Fallo al leer el archivo de materiales %s. Hay menos materiales de los definidos en nmats\n",filename);
   Debug(DEBUG_LEVEL_ERROR);
   return -1;
 }
 return nmats;
}


//
// Función para localizar un material a partir de su cadena
//

int Material::FindMaterial(char *cadena)
{
 int i;

 for(i=0;i<numero_materiales;i++)
 {
   //sprintf (FANTASY_DEBUG_STRING, "Comparando %s con %s.\n",cadena,materiales[i].matname);
   //Debug();
  	
 	
  if(!strcmp(cadena,materiales[i].matname))
    return i;
 }

 return -1; // -1 significa que el material no existe (textura 0)
}


//
// Función para seleccionar el material
//

void Material::SelectMaterial(int nmat, int force)
{
    int i;
  
#ifdef MEGADEBUG
   sprintf (FANTASY_DEBUG_STRING, "->Me piden seleccionar material %d\n",nmat);
   Debug(DEBUG_LEVEL_OVERDEBUG);
#endif  
    
    if((nmat == last_used_material) && (!force))
      return;
    last_used_material = nmat;
    
    
    if((nmat < numero_materiales)  && (nmat >= 0))
    {
            if(materiales[nmat].opaque)		// Materiales opacos
            {

#ifdef MEGADEBUG
   sprintf (FANTASY_DEBUG_STRING, "->Es un material opaco\n");
   Debug(DEBUG_LEVEL_OVERDEBUG);
#endif  

                if(blend_enabled)
                {
                    if(force_always_blend)
                        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                    else
                        glDisable(GL_BLEND);

                    glDepthMask(GL_TRUE);
                    blend_enabled=0;
                }
               
            }
            else					// Materiales traslúcidos
            {
                if(!blend_enabled)
                {
                    if(!force_always_blend) glEnable(GL_BLEND);
                    glDepthMask(GL_FALSE);
                    blend_enabled=1;
                }
                
                if(blend_substract_supported) f_glBlendEquationEXT(GL_FUNC_ADD_EXT);
                
                switch(materiales[nmat].blend_type)
                {
                case BLEND_BLEND: glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
                break;
                case BLEND_ADD:   glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                break;
                case BLEND_MULTIPLY: glBlendFunc(GL_DST_COLOR,GL_ZERO);
                break;
                case BLEND_SUB: if(blend_substract_supported)
                		{
                		 glBlendFunc(GL_SRC_ALPHA,GL_ONE);
                	         f_glBlendEquationEXT(GL_FUNC_REVERSE_SUBTRACT_EXT);                	         
                	        }
                break;
                }
            }


            if(materiales[nmat].ntextures)
            {
                    for(i=0;i<materiales[nmat].ntextures;i++)
                    {
                     if(EXTGL_multitexturing)
                      f_glActiveTextureARB(TextureUnits[i]);
		     
		     if(materiales[nmat].texcoordgen[i] != TEXGEN_CUBE_MAP)
		     {
		      glEnable(GL_TEXTURE_2D);
                      ConjuntoTexturas->SeleccionaTextura(materiales[nmat].textures[i]);
                     }
                     
                     
                     // Seleccionamos el tipo de blend de texturas
                     switch(materiales[nmat].multitexblend[i])
                     {
                       case BLEND_ADD: glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
                                       break;
                       case BLEND_MODULATE: glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                                       break;
                       case BLEND_MASK: if(EXTGL_texture_env_combine_supported)
                        		 {                         	
                        		  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
                        		  glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);                      		  
                                          
                                          glTexEnvf(GL_TEXTURE_ENV,GL_SOURCE0_RGB_ARB,GL_PREVIOUS_ARB);
                                          glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND0_RGB_ARB,GL_SRC_COLOR);
                                          glTexEnvf(GL_TEXTURE_ENV,GL_SOURCE1_RGB_ARB,GL_TEXTURE);
                                          glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND1_RGB_ARB,GL_SRC_COLOR);
                                          glTexEnvf(GL_TEXTURE_ENV,GL_SOURCE2_RGB_ARB,GL_TEXTURE);
                                          glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND2_RGB_ARB,GL_ONE_MINUS_SRC_ALPHA);                                        
                                         }
                                        break;
                       case BLEND_MASK2: if(EXTGL_texture_env_combine_supported)
                        		 {                         	
                        		  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
                        		  glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);                      		  
                                          
                                          glTexEnvf(GL_TEXTURE_ENV,GL_SOURCE0_RGB_ARB,GL_PREVIOUS_ARB);
                                          glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND0_RGB_ARB,GL_SRC_COLOR);
                                          glTexEnvf(GL_TEXTURE_ENV,GL_SOURCE1_RGB_ARB,GL_TEXTURE);
                                          glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND1_RGB_ARB,GL_SRC_COLOR);
                                          glTexEnvf(GL_TEXTURE_ENV,GL_SOURCE2_RGB_ARB,GL_PREVIOUS_ARB);
                                          glTexEnvf(GL_TEXTURE_ENV,GL_OPERAND2_RGB_ARB,GL_SRC_ALPHA);                                        
                                         }
                                        break;                                        
                       case BLEND_DOT3: if(EXTGL_texture_env_dot3_supported)
                        		{
                                // UNIT 0 
         			// find dot product of N (stored in the texture map) and L (stored
         			// as the PRIMARY_COLOR).
                                 	f_glActiveTextureARB(TextureUnits[i-1]);
                                 	ConjuntoTexturas->SeleccionaTextura(materiales[nmat].textures[i]);
         		         	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
         			 	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_DOT3_RGB_ARB); 
               			 	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
         			 	glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
           			 	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR_ARB);
         			 	glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
                                // UNIT 1
         			// modulate the base texture by N.L        
         			 	f_glActiveTextureARB(TextureUnits[i]);
         			 	ConjuntoTexturas->SeleccionaTextura(materiales[nmat].textures[i-1]);
         			 	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
         			 	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
         			 	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_EXT);
         			 	glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
         			 	glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
         			 	glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);                                                         			 
               				}
                            		break;                                                        
                       case BLEND_REPLACE: glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
                                        break;
                        }
                
                        // Seleccionamos el tipo de generaci¢n de coord. 
                        switch(materiales[nmat].texcoordgen[i])
                        {
                        case TEXGEN_UV:
                            glDisable(GL_TEXTURE_GEN_S);
                            glDisable(GL_TEXTURE_GEN_T);
                            glDisable(GL_TEXTURE_GEN_R);
                            if(EXTGL_texture_cube_map_supported) glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                            break;
                        case TEXGEN_EYE_LINEAR:
                            {
                        	GLfloat xplane[]={1.0f,0.0f,0.0f,0.0f};
                        	GLfloat yplane[]={0.0f,1.0f,0.0f,0.0f};
                        	
                             glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
                             glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
                             glEnable(GL_TEXTURE_GEN_S);                         
                             glEnable(GL_TEXTURE_GEN_T);
                             glDisable(GL_TEXTURE_GEN_R);
                             if(EXTGL_texture_cube_map_supported) glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                             xplane[0]=materiales[nmat].linearmultiplier[i];
                             yplane[1]=materiales[nmat].linearmultiplier[i];

			// Lo del glpushmatrix y demás lo cogí de un ejemplo de sgi
			// que estaba colgado por internet. Aún tengo que saber por que
			// funciona asi :)
                             
                             glPushMatrix();
	    		     glMatrixMode(GL_MODELVIEW);
	    		     glLoadIdentity();
	    		     glTexGenfv(GL_S, GL_EYE_PLANE, xplane);
	    		     glTexGenfv(GL_T, GL_EYE_PLANE, yplane);
	    		     glPopMatrix();
                            }
                            break;
                        case TEXGEN_OBJECT_LINEAR:
                            {
                       	     GLfloat xplane[]={1.0f,0.0f,0.0f,0.0f};
                             GLfloat yplane[]={0.0f,1.0f,0.0f,0.0f};
                        	
                             glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
                             glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
                             glEnable(GL_TEXTURE_GEN_S);                         
                             glEnable(GL_TEXTURE_GEN_T);
                             glDisable(GL_TEXTURE_GEN_R);
                             if(EXTGL_texture_cube_map_supported) glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                             xplane[0]=materiales[nmat].linearmultiplier[i];
                             yplane[1]=materiales[nmat].linearmultiplier[i];
                             glTexGenfv(GL_S, GL_OBJECT_PLANE, xplane);
        		     glTexGenfv(GL_T, GL_OBJECT_PLANE, yplane);
                            }
                            
                            break;
                        case TEXGEN_SPHERE_MAP:
                            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                            glEnable(GL_TEXTURE_GEN_S);
                            glEnable(GL_TEXTURE_GEN_T);
                            glDisable(GL_TEXTURE_GEN_R);
                            if(EXTGL_texture_cube_map_supported) glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                            break;
                        case TEXGEN_CUBE_MAP:              
                            if(EXTGL_texture_cube_map_supported)              
                            {                                                        	
                             glEnable(GL_TEXTURE_CUBE_MAP_ARB); 
                             glDisable(GL_TEXTURE_2D);                            	
                             ConjuntoTexturas->SeleccionaCubeMap(materiales[nmat].textures[i]);
			     glEnable(GL_TEXTURE_GEN_S); 
			     glEnable(GL_TEXTURE_GEN_T); 
			     glEnable(GL_TEXTURE_GEN_R);
                             glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB); 
			     glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB); 
			     glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB); 			     
			    }
			    break;
                        }
                    }
                if(EXTGL_multitexturing) // Deshabilitamos todas menos la 1¦
                {
                    for(i=materiales[nmat].ntextures;i<EXTGL_number_of_texture_units;i++)
                    {
                        f_glActiveTextureARB(TextureUnits[i]);
                        glDisable(GL_TEXTURE_2D);
                        if(EXTGL_texture_cube_map_supported) glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                    }
                }   
                    
	    }

            else
            {
                if(EXTGL_multitexturing)
                {
                    for(i=1;i<EXTGL_number_of_texture_units;i++)
                    {
                        f_glActiveTextureARB(TextureUnits[i]);
                        glDisable(GL_TEXTURE_2D);
                    }
                    f_glActiveTextureARB(TextureUnits[0]);
                }
                ConjuntoTexturas->SeleccionaTextura(0);
            }
        
   
        if(materiales[nmat].twosided)
        {
            if(culling_enabled)
            {
                glDisable(GL_CULL_FACE); // Deshabilitamos el testeo de profundidad
                culling_enabled=0;
            }
        }
        else
        {
            if(!culling_enabled)
            {
                glCullFace(GL_BACK);
                glEnable(GL_CULL_FACE); // Habilitamos el testeo de profundidad
                culling_enabled=1;
            }
        }
      // Finalmente tenemos en cuenta los parámetros para la iluminación
      glMaterialfv(GL_FRONT,GL_SPECULAR,materiales[nmat].specular);  
      glMaterialfv(GL_FRONT,GL_EMISSION,materiales[nmat].emission);
      glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,materiales[nmat].shininess);

      // Ok, la textura está puesta. Sólo nos falta el shader
      // If there is no shader, shader 0 (NULL) will be selected, meaning no shader program

      
      GLSL_SelectShader(&(materiales[nmat]));
    }
}



//
// Esta función simplemente nos dice si un material es opaco o no
//


int Material::isOpaque(int nmat)
{
 if((nmat < numero_materiales)  && (nmat >= 0))
 {
  return materiales[nmat].opaque;
 }
 return 1;
}

void Material::ResetMatParameters(void)
{
 culling_enabled=1;
 blend_enabled=0;
 last_used_material=-1;
}

void Material::SetForceAlwaysBlend(int yesno)
{
 force_always_blend = yesno;
}

void Material::CleanupTextures(void)
{
    int i,j;
    
    for(i=0;i<numero_materiales;i++)
    {
        for(j=0;j<materiales[i].ntextures;j++)
            ConjuntoTexturas->EliminaTextura(materiales[i].textures[j]);
    }
}

void Material::CleanupShaderObjects(void)
{
    int i,j;
    
    for(i=0;i<numero_materiales;i++)
    {   	
    	GLSL_DeleteShaders(&(materiales[i]));
    }
}

int Material::NumTextures(int nmat)
{
 return (materiales[nmat].ntextures);
}

