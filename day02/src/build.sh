#build.sh

echo 开始编译、链接，生成内核
nasm -f elf -o start.o start.asm

echo 我们之后将在这个位置加入C代码

echo OK，这里开始链接代码
echo 所有的C和汇编生成的目标文件都需要链接进来哦！
ld -T link.ld -o kernel.bin start.o
echo 完成了!
