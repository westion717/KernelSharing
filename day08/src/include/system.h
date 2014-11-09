/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: Global function declarations and type definitions
*
*  Notes: No warranty expressed or implied. Use at own risk. 
*  modified: for tutorial of kernel.wuzx.me
*  By:Westion westion717@gmail.com
*/

#include <type.h>

#ifndef __SYSTEM_H
#define __SYSTEM_H


struct regs
{
    u16 gs, fs, es, ds;      /* 最后push的几个段寄存器*/
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pusha 命令push的通用寄存器*/
    u8 int_no, err_code;    /* 我们push的中断号和错误代码 */

    /*以下5个值是CPU硬件自动push进来的，当发生中断的时候
      保留了中断之前的程序的地址还有状态寄存器，还有中断之前的主程序的栈段选择子，还有esp指针。因为主程序可能不是ring0，所以有自己的栈
    */
    u32 eip;
    u16 cs;
    u32 eflags, useresp;
    u16 userss; 
};

/*system.c*/
//内存拷贝，从原地址拷贝count个字节到目标地址
PUBLIC void *memcpy(void *dest, const void *src, size_t count);
//内存赋值,从指定开始每个内存单元都赋值为val
PUBLIC void *memset(void *dest, u8 val, size_t count);
//内存赋值（字节）从指定开始每个内存单元都赋值为val
PUBLIC void *memsetw(void *dest, u16 val, size_t count);
//计算字符串长度
PUBLIC size_t strlen(const char *str);
//IO操作，端口读值
PUBLIC u8 inportb (u16 _port);
//IO操作，端口赋值
PUBLIC void outportb (u16 _port, u8 _data);

#endif
