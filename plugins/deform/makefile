PLUGIN_NAME = deform
OBJ         = deform_epopeia deform wave vector interp 
OBJ_FULL    = $(addsuffix .$(OBJ_SUFFIX), $(OBJ))
LIBS	    = expr/libexpreval.a
LIBS_MINGW32= ../../lib/glt.lib
LIBS_LINUX  = -lglt

include ../main.mak

expr/libexpreval.a : $(OBJ_EXPR)
	cd expr; make clean; make; cd ..

