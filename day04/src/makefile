include=include
source=source
build=build
options=-Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I${include} -c -o
compiler=gcc
ld=ld
cd=cdrom

objects=start.o system.o main.o scrn.o

kernel: ${objects}
	${ld} -T link.ld -o kernel.bin ${objects}
	mv ${objects} ${build}
	mv kernel.bin ${cd}/kernel/
scrn.o:
	$(compiler) $(options) scrn.o $(source)/scrn.c
main.o:
	$(compiler) $(options) main.o $(source)/main.c
system.o:
	${compiler} $(options) system.o ${source}/system.c
start.o:
	nasm -f elf -o start.o start.asm
clean: 
	rm $(build)/*.o kernel.iso
iso:
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o kernel.iso $(cd)
