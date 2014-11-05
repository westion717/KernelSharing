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
