/* bkerndev - Bran's Kernel Development Tutorial
 *  By:   Brandon F. (friesenb@gmail.com)
 *  Desc: main.o: C code Entry
 *
 *  Notes: No warranty expressed or implied. Use at own risk. 
 *
 *  modified: for tutorial of kernel.wuzx.me
 *  By:Westion westion717@gmail.com
 */

#include <scrn.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>

int main()
{
	gdt_install();
	idt_install();
	isrs_install();
	init_video();
__asm__ __volatile__ ("sti");
	puts("hello world\n");
	int i = 3/0;
	i++;
	puts("hello world\n");
	for (;;);
	return 0;
}
