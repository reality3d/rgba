PLUGIN_NAME = menu_gtk12
OBJ         = dlg menu_gtk12_epopeia #image img_png 
OBJ_FULL    = $(addsuffix .$(OBJ_SUFFIX), $(OBJ))
LIBS	    = 
#LIBS_MINGW32= ../../lib/
LIBS_LINUX  = -L/usr/X11R6/lib -lXi -lglib -lgtk -lgdk # el -Xi es un work-around para FC1
CCOPTS_LINUX= -I/usr/include/gtk-1.2 -I/usr/include/glib-1.2 -I/usr/lib/glib/include


include ../main.mak

