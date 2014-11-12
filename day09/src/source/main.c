/* bkerndev - Bran's Kernel Development Tutorial
 *  By:   Brandon F. (friesenb@gmail.com)
 *  Desc: main.o: C code Entry
 *
 *  Notes: No warranty expressed or implied. Use at own risk. 
 *
 *  modified: for tutorial of kernel.wuzx.me
 *  By:Westion westion717@gmail.com
 */

#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <timer.h>
#include <scrn.h>

int main()
{
	gdt_install();
	init_video();
	idt_install();
	isr_install();
	irq_install();
	timer_install();
	__asm__ __volatile__ ("sti");
	for (;;);
	return 0;
}

