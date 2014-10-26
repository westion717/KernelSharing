#build.sh

echo 开始编译、链接，生成内核
nasm -f elf -o ./build/start.o start.asm

gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o ./build/main.o ./source/main.c
gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o ./build/lib/system.o ./lib/system.c

echo OK，这里开始链接代码
echo 所有的C和汇编生成的目标文件都需要链接进来哦！
ld -T link.ld -o kernel.bin ./build/start.o ./build/lib/system.o ./build/main.o
echo 完成了!
