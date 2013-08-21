# definiciones Mingw32 sobre linux
# Lo primero, debemos saber cual es el prefijo MING32 de la máquina
# Lo mejor seria que se nos indique en la variable de entorno MINGW32_PREFIX

ifeq ($(MINGW32_PREFIX),)
	MINGW32_PREFIX=i386-mingw32
endif

# en endymion/debian es: MINGW32_PREFIX=i586-mingw32msvc
# ¿un poco caprichoso no?


DLL_PREFIX	= 
DLL_SUFFIX	= dll 
OBJ_SUFFIX	= o
PLUGIN_SUFFIX   = epl

DST      = $(PLUGIN_NAME).$(PLUGIN_SUFFIX)

# Compiler settings
include ../debug-gcc.mak

CC		= $(MINGW32_PREFIX)-gcc -c 
CCOPTS		= -O2 -march=pentium -ffast-math -Wall -DMINGW32 -D_WIN32_IE=0x0300 $(CCDEBUG) -I../../include-mingw32 -I../../include $(CCOPTS_MINGW32)
CCOUT        	= -o

CCPP		= $(MINGW32_PREFIX)-g++ -c
CCPPOPTS	= $(CCOPTS)
CCPPOUT		= $(CCOUT)

RC		= $(MINGW32_PREFIX)-windres

# Linker settings
LINK		= $(MINGW32_PREFIX)-gcc 
LINKOPTS    	= -shared -L../../lib $(LIBS_DIR)
LINKOUT        	= -o
LIB_IMPLICIT    = -lepopeiadll -lopengl32 -lglu32 -lgdi32 -lwinmm -lcomctl32

# Assembler settings
NASM		= nasm -f win32
NASMOUT		= -o

# Commands
DEL    	= rm

$(DST): $(OBJ_FULL)
	$(LINK) $(LINKOPTS) $(LINKOUT)$(DST) $(OBJ_FULL) $(LIB_SYS) $(LIB_IMPLICIT) $(LIBS) $(LIBS_MINGW32)
	$(MINGW32_PREFIX)-strip $@

install: $(DST)
	cp $(DST) ../../../rgba-bin/demosystem/plugin

%.$(OBJ_SUFFIX) : %.rc
	$(RC) $< -o $@
