#!/bin/sh
#i686-w64-mingw32-gcc -v #for 32bit
#x86_64-w64-mingw32-gcc -v #for 64bit
x86_64-w64-mingw32-gcc -c -o kunten_main.o kunten_main.c
x86_64-w64-mingw32-gcc -c -o my_math.o my_math.c
x86_64-w64-mingw32-gcc -c -o utf_string.o utf_string.c
x86_64-w64-mingw32-gcc -c -o tex_command.o tex_command.c
x86_64-w64-mingw32-gcc -c -o kunten.o kunten.c
x86_64-w64-mingw32-gcc -o kunten.exe kunten_main.o tex_command.o my_math.o utf_string.o kunten.o
rm *.o
