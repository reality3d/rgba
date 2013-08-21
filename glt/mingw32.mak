# definiciones Mingw32 sobre linux

DLL_PREFIX	= 
DLL_SUFFIX	= dll 
OBJ_SUFFIX	= o
PLUGIN_SUFFIX   = epl

DST      = glt.dll

# Compiler settings
CC		= i586-mingw32msvc-gcc -c 
CCOPTS		= -O2 -march=pentium -ffast-math -Wall -DMINGW32 -DDISABLE_DEBUG -I../include-mingw32 -I../include
#CCOPTS   	= -O2 -march=pentium -ffast-math -Wall -DMINGW32 -g -I../include-mingw32 -I../include
CCOUT        	= -o

CCPP		= i586-mingw32msvc-g++ -c
CCPPOPTS	= $(CCOPTS)
CCPPOUT		= $(CCOUT)

# Linker settings
LINK		= i586-mingw32msvc-gcc 
LINKOPTS    	= -shared --export-dynamic -Wl,--output-def,glt.def,--out-implib,libgltdll.a -L../lib $(LIBS_DIR) --enable-auto-import
LINKOUT        	= -o
LIB_IMPLICIT    = -lepopeiadll -lopengl32 -lglu32 -lgdi32 -lwinmm

# Assembler settings
NASM		= nasm -f win32
NASMOUT		= -o

# Commands
DEL    	= rm

$(DST): $(OBJ_FULL)
	$(LINK) $(LINKOPTS) $(LINKOUT)$(DST) $(OBJ_FULL) $(LIB_SYS) $(LIB_IMPLICIT) $(LIBS)

install: $(DST)
	cp $(DST) ../../rgba-bin/demosystem/plugin
