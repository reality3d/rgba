PLUGIN_NAME = glt
OBJ        = cache texture
OBJ_FULL   = $(addsuffix .$(OBJ_SUFFIX), $(OBJ))
LIBS	   = imagelib/libimage.a

include $(PLATFORM).mak

#
# Main
#

clean:
	$(DEL) $(OBJ_FULL)
	$(DEL) $(PLUGIN_NAME).$(PLUGIN_SUFFIX)

#
# Objs
#

%.$(OBJ_SUFFIX): %.c
	$(CC) $(CCOPTS) $< $(CCOUT)$@

%.$(OBJ_SUFFIX): %.cpp
	$(CCPP) $(CCPPOPTS) $< $(CCPPOUT)$@
