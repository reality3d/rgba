#ifdef WIN32
 #include <windows.h>
#endif
#ifdef LINUX
 #define _strdup strdup
#endif

#include <math.h>
#include <malloc.h>
#include <string.h>
#include <packer/packer.h>

#include "fsprite.h"


static int strmatch(char *pattern, char *cadena)
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

static char **generate_filelist(char *filename,int *nfiles)
{
   int files_left;
   char *tmp;
   char **filelist=NULL;
   int i;
   
   *nfiles=0;
      
   files_left = pak_list_reset();
   if(files_left)   
   {
   	// Convertimos el formato del path
   	tmp=filename;
   	while(*tmp != '\0')
   	{
    		if(*tmp == '/') *tmp='\\';
    		tmp++;	
   	}

    	while(files_left)
    	{
    		tmp=pak_list_current_filename();    		
 		if(strmatch(filename,tmp))
 		{
 			(*nfiles)++;
 		}
 		files_left = pak_list_next();	
    	}
    	// En *nfiles tenemos todos los ficheros que hacen match
    	// Ahora hacemos el malloc y tiramos p'alante;
    	if (*nfiles == 0) return NULL;
    	filelist = (char **)malloc((*nfiles) * sizeof (char *));
    	i=0;
   	files_left = pak_list_reset(); 	
    	while(files_left)
    	{
    		tmp=pak_list_current_filename();
 		if(strmatch(filename,tmp))
 		{
 			filelist[i]=_strdup(tmp);
 			i++;
 		}
 		files_left = pak_list_next();	
    	}    	    	
   }	
    else
    {
    	#ifdef WIN32	
    	WIN32_FIND_DATA FileData;
    	HANDLE hSearch;
    	char szDirPath[MAX_PATH];
        char szNewPath[MAX_PATH];        
        int length;
        int j;
   	
   	// Convertimos el formato del path
   	tmp=filename;
   	while(*tmp != '\0')
   	{
    		if(*tmp == '/') *tmp='\\';
    		tmp++;	
   	}
        
        length=strlen(filename);
        tmp=filename;
        j=length;
        while((tmp[j] != '\\') && (j>0)) j--;
        length=j+1;
        
        for(j=0;j<length;j++) szDirPath[j] = tmp[j];
        szDirPath[length]='\0';
        
    	hSearch = FindFirstFile(filename, &FileData);
    	if (hSearch == INVALID_HANDLE_VALUE)
    	{
         	return NULL;
    	}
    	else
    	{
    		while(1)
    		{    		
    			(*nfiles)++;
    			if (!FindNextFile(hSearch, &FileData)) 
    			{
			  FindClose(hSearch);
			  break;        
			}
        	}
    		// En *nfiles tenemos todos los ficheros que hacen match
    		// Ahora hacemos el malloc y tiramos p'alante;
    		if (*nfiles == 0) return NULL;
    		filelist = (char **)malloc((*nfiles) * sizeof (char *));
    		i=0;
		hSearch = FindFirstFile(filename, &FileData);
    		while(1)
    		{
    			strcpy(szNewPath,szDirPath);
    			strcat(szNewPath,FileData.cFileName);
			//filelist[i]=_strdup(FileData.cFileName);
			filelist[i]=_strdup(szNewPath);
			i++;
			if (!FindNextFile(hSearch, &FileData))
			{
				FindClose(hSearch);
				break;        
			}
		}
	}

        #endif
#ifdef LINUX
#warning "no-pak pattern matching not implemented!"
#endif
    	// TODO: añadir el mismo código para cuando se carga de fichero en linux!!!!!
    }

 return filelist;
}

// Voy a hacer un algoritmo clásico de burbuja para esto, no tengo ganas de buscar un qsort :P

static void sort_filelist(char **filelist, int nfiles)
{
	int i,j;
	char *tmp;
	
	for(i=0;i<nfiles;i++)
	{
		for(j=i+1;j<nfiles;j++)
		{
			if(strcmp(filelist[j],filelist[i])<0)
			{
				tmp=filelist[i];
				filelist[i]=filelist[j];
				filelist[j]=tmp;	
			}
		}		
	}		
}

TSprite* FantasySprite_New(char *filename,char *texid)
{
    TSprite* self;
    int i;
    char *token;
    int max_image_size;
 
    char **filelist;
    int nfiles;
    
    
    filelist=generate_filelist(filename,&nfiles);
    if(nfiles == 0)
    {
      return 0;	
    }
    sort_filelist(filelist,nfiles);
    /*printf("Nfiles: %d\n",nfiles);
    for(i=0;i<nfiles;i++)
    {
    	printf("%s\n",filelist[i]);	
    }*/
  
    
    self = (TSprite *)malloc(sizeof(TSprite));    
    self->num_sprites=nfiles;
    
    // Alloc sprites
    self->sprite = (TSpritePriv *) malloc (self->num_sprites * sizeof (TSpritePriv));
    // Alloc Images
    self->ima = (TImaPriv *) malloc (self->num_sprites * sizeof (TImaPriv));
    if ((!self->sprite) || (!self->ima))
    {
    	printf("FSPRITE: Error when malloc\n");
        return 0;
    }

    max_image_size = 0;
    // Leemos todos los sprites
    for(i=0;i<nfiles;i++)
    {
    	self->ima[i].imagen = IMG_Read (filelist[i]);
        if (!self->ima[i].imagen)
        {
            printf("FSPRITE: Error reading image %s\n",filelist[i]);
            return 0;
         }   
        self->sprite[i].texture = TEX_New (self->ima[i].imagen->width,
                                       self->ima[i].imagen->height,
                                       self->ima[i].imagen->bits_per_pixel,
                                       self->ima[i].imagen->data,
                                       TEX_BUILD_MIPMAPS);

	if (!self->sprite[i].texture)
	{
		printf("FSPRITE: Error in TEX_New\n");
        	return 0;
        }
        	
        if(self->ima[i].imagen->width*self->ima[i].imagen->height > max_image_size)
            max_image_size = self->ima[i].imagen->width*self->ima[i].imagen->height;
    }
     

    CACHE_InsertTexture (texid,self->sprite[0].texture);

    self->actual_sprite = 0;
    self->ultimo_tick = 0;
    self->spriteanterior= 0;

    // 1 cambio de sprite por por segundo
    self->loop_mode    = 0; // No loop
    self->change_speed = 1000;
    self->base_time    = Epopeia_GetTimeMs ( );
    self->anterior_alpha = 1.0;//Alpha global
    self->alpha = 1.0;//Alpha global
    // Reservamos espacio en memoria para buffer de pixeles intermedio (alpha)
    self->pixel_buffer = NULL;
    self->max_image_size = max_image_size;

    for(i=0;i<nfiles;i++) free(filelist[i]);
    free(filelist);

    return self;
}


void FantasySprite_Kill  (TSprite* self)
{
    int i;

    CACHE_FreeTexture (self->sprite[0].texture);

    for(i=0;i<self->num_sprites;i++)
        IMG_Delete(self->ima[i].imagen);
    
    if(self->pixel_buffer != NULL)
        free(self->pixel_buffer);
        
    free(self);
}
