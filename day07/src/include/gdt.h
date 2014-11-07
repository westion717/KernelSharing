/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: GDT entry definition and some init functions
*
*  Notes: manipulation of GDT
*  modified: for tutorial of kernel.wuzx.me
*  By:Westion westion717@gmail.com
*/

#include <type.h>

#ifndef __GDT_H
#define __GDT_H

/* 载入新的GDT，函数定义在汇编中。在供给main函数调用*/
PUBLIC void gdt_install();


#endif
