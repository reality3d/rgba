# Definiciones para Linux de variables estandar make

OBJ_SUFFIX	= o
LIB_PREFIX	= lib
LIB_SUFFIX	= a
DLL_PREFIX	= lib
DLL_SUFFIX	= so

CC		= gcc -c
CCOPTS		= -O2 -Wall -DLINUX -DEPOPEIA_EXPORTS -Irgba -I../include
CCOUT        	= -o

LINK		= gcc
LINKOPTS    	= -shared --export-dynamic -g -Wl,-soname,$(DLL_PREFIX)$(TARGET).$(DLL_SUFFIX).$(VER_MAJOR) $(LIBS_DIR)
LINKOUT        	= -o
LINKSRC		= $(OBJ) $(LIBS) $(SYSLIBS) $(RES) $(LIBS_WIN)

LIBS_DIR	= -L../lib
LIBS 		= -lSDL -lGL -lGLU -lfmod-3.4  #-ljpeg /usr/lib/libpng.a #-lpng da problemas de versiones
LIBS_WIN	= 
SYSLIBS 	= 

