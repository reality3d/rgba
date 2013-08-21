@echo off
bison --verbose --defines -o parser.c parser.y
flex -i -oscanner.c scanner.l
rem xchg scanner.c /getc/mygetc/ferror/myferror/fread/myfread/
sed "s/getc/mygetc/g" scanner.c > scanner.tmp
move scanner.tmp scanner.c /q
sed "s/ferror/myferror/g" scanner.c > scanner.tmp
move scanner.tmp scanner.c /q
sed "s/fread/myfread/g" scanner.c > scanner.tmp
move scanner.tmp scanner.c /q

cl main.c scanner.c parser.c expr.c ../turb.c
lib /OUT:expreval.lib scanner.obj parser.obj expr.obj turb.obj
