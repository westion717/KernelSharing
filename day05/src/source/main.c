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

int main()
{
	gdt_install();
	init_video();
	puts("hello world\n");
	puts("hello world\n");
	settextcolor(12,13);
	puts("hello world\n");
	for (;;);
	return 0;
}
