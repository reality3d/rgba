#
# PLUGIN Makefile helper
# Linux

DLL_PREFIX  = lib
DLL_SUFFIX  = so
OBJ_SUFFIX  = o
PLUGIN_SUFFIX = epx

#DST      = $(DLL_PREFIX)$(PLUGIN_NAME).$(DLL_SUFFIX)
DST      = $(PLUGIN_NAME).$(PLUGIN_SUFFIX)
# Epopeia headers and libs
H_GENERAL   = ../../include
#LIB_EPOPEIA = ../../lib/libepopeia.so

LIBS 		= -lGL -lGLU
LIBS_EPOPEIA	= -lepopeia
LIBS_SYS 	=  

# Compiler options
CC		= gcc -c
CCOPTS  	= -O3 -march=pentium -ffast-math -DLINUX -DFX_EXPORTS -I$(H_GENERAL)
CCOUT        	= -o

CCPP		= g++ -c
CCPPOPTS	= $(CCOPTS)
CCPPOUT		= $(CCOUT)

# Linker options
LINK		= gcc
LINKOPTS 	= -shared -L../../lib
LINKOUT        	= -o
LINKSRC		= $(OBJ_FULL) $(LIBS) $(LIBS_SYS) $(LIBS_EPOPEIA)

# Commands
DEL    	= rm

$(DST): $(OBJ_FULL)
	$(LINK) $(LINKOUT)$(DST) $(LINKOPTS) $(LINKSRC) 

install: $(DST)
	cp $(DST) ../../../rgba-bin/demosystem/plugin
