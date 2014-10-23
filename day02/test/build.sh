nasm -f elf code.asm
gcc -o main.o -c main.c
gcc -o a.out main.o code.o
