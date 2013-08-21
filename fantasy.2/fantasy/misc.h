#ifndef MISC_H

#define MISC_H
#include "3dworld.h" 
 
 extern char FANTASY_DEBUG_STRING[1024];
 
 void Debug(int debuglevel);
 void DumpWorld(world_type *world);
 int strmatch(char *pattern, char *cadena);


 #define DEBUG_LEVEL_ERROR   0
 #define DEBUG_LEVEL_WARNING 1
 #define DEBUG_LEVEL_INFO    2
 #define DEBUG_LEVEL_OVERDEBUG 3
#endif

