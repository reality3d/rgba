Notas para compilar Epopeia
---------------------------

Es necesario definir la variable de entorno PLATFORM a uno de los
siguientes:

linux
win32

De esa manera el makefile incluye el submakefile que corresponde a cada
plataforma.

Para poder compilar la version win32 hace falta:

- GNU make (por ejemplo el del DJGPP)
- nasmw (NASM win32)

enlar
