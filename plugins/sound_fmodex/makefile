PLUGIN_NAME = sound_fmodex
OBJ        = sound_fmodex sound_fmodex_epopeia
OBJ_FULL   = $(addsuffix .$(OBJ_SUFFIX), $(OBJ))
LIBS	    = ../../lib/fmodvc.lib
#LIBS_MINGW32= ../../lib/
LIBS_LINUX  = -Lfmodex -lfmodex
CCOPTS_LINUX = -Ifmodex/ -fPIC
CCDEBUG   = -g


include ../main.mak

