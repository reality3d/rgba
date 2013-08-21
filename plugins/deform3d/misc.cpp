#include <string.h>

extern "C"
{

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

}