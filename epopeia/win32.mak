# Definiciones para Win32 de variables estandar make

OBJ_SUFFIX	= obj
LIB_PREFIX	= 
LIB_SUFFIX	= lib
DLL_PREFIX	= 
DLL_SUFFIX	= dll

CC		= cl
CCOPTS		= /c /Ox /DWIN32 /DEPOPEIA_EXPORTS /Irgba /I../include
CCOUT        	= /Fo

LINK		= link
LINKOPTS    	= /DLL /nologo /incremental:no /subsystem:windows /NODEFAULTLIB:LIBC #/DEF:glt/glt.def
LINKOUT        	= /OUT:
LINKSRC		= $(OBJ) $(LIBS) $(SYSLIBS) $(RES) $(LIBS_WIN)

LIBS_DIR	= /LIBPATH=../lib
LIBS 		= gdi32.lib opengl32.lib glu32.lib glaux.lib winmm.lib
LIBS_WIN	= ../lib/fmodvc.lib ../lib/libpng.lib ../lib/glt.lib
SYSLIBS 	= user32.lib kernel32.lib advapi32.lib msvcrt.lib


