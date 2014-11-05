/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: Global function declarations and type definitions
*
*  Notes: manipulation of screen. 
*  modified: for tutorial of kernel.wuzx.me
*  By:Westion westion717@gmail.com
*/

#include <type.h>
#ifndef __SCRN_H
#define __SCRN_H

/*先定义下我们的字符属性,然后是行和列*/

PUBLIC void cls();
PUBLIC void putch(unsigned char c);
PUBLIC void init_video();
PUBLIC void puts(const char * text);
PUBLIC void settextcolor(u8 forecolor,u8 backcolor);
#endif
