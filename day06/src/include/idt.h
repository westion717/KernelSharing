/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: idt installation functions
*
*  Notes: installation of idt. 
*  modified: for tutorial of kernel.wuzx.me
*  By:Westion westion717@gmail.com
*/

#include <type.h>
#ifndef __IDT_H
#define __IDT_H

PUBLIC void idt_install();
PUBLIC void idt_set_gate(int num, u32 base, u16 sel, u8 flags);
#endif
