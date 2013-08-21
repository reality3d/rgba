PLUGIN_NAME = sound_fmod
OBJ        = sound_fmod sound_fmod_epopeia
OBJ_FULL   = $(addsuffix .$(OBJ_SUFFIX), $(OBJ))
LIBS	    = ../../lib/fmodvc.lib
#LIBS_MINGW32= ../../lib/
LIBS_LINUX  = -Lfmod4 -lfmodex
CCOPTS_LINUX = -Ifmod4/ -fPIC


include ../main.mak

