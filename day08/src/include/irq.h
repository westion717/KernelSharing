/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: handler interruption of 0 to 31
*
*  Notes: manipulation of screen. 
*  modified: for tutorial of kernel.wuzx.me
*  By:Westion westion717@gmail.com
*/

#include <type.h>
#include <system.h>
#ifndef __IRQ_H
#define __IRQ_H

/*安装前32个描述符*/
PUBLIC void irq_install();

PUBLIC void irq_install_handler(int irq, void (*handler)(struct regs *r));
PUBLIC void irq_uninstall_handler(int irq);
#endif
