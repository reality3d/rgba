#
# main Makefile template for PLUGINS
#

include ../$(PLATFORM).mak

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
