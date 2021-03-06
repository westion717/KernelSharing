/* bkerndev - Bran's Kernel Development Tutorial
 *  By:   Brandon F. (friesenb@gmail.com)
 *  Desc: system.o: tool function
 *
 *  Notes: No warranty expressed or implied. Use at own risk. 
 *
 *  modified: for tutorial of kernel.wuzx.me
 *  By:Westion westion717@gmail.com
 */


#include <system.h>

PUBLIC void *memcpy(void *dest, const void *src, size_t count)
{
	const u8 *sp = (const u8 *)src;
	u8 *dp = (u8 *)dest;
	for(; count != 0; count--) *dp++ = *sp++;
	return dest;
}

PUBLIC void *memset(void *dest, u8 val, size_t count)
{
	u8 *temp = (u8 *)dest;
	for( ; count != 0; count--) *temp++ = val;
	return dest;
}

PUBLIC void *memsetw(void *dest, u16 val, size_t count)
{
	u16 *temp = (u16 *)dest;
	for( ; count != 0; count--) *temp++ = val;
	return dest;
}

PUBLIC size_t strlen(const char *str)
{
	size_t retval;
	for(retval = 0; *str != '\0'; str++) retval++;
	return retval;
}

PUBLIC u8 inportb (u16 _port)
{
	u8 rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

PUBLIC void outportb (u16 _port, u8 _data)
{
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}
