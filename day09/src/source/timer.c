#include <system.h>
#include <scrn.h>
#include <timer.h>
#include <irq.h> 

/* 纪录时钟节拍 */
PRIVATE int timer_ticks = 0;

PRIVATE void timer_handler(struct regs *r);
PRIVATE void timer_phase(int hz);

PRIVATE void timer_phase(int hz)
{
	int divisor;
	if(hz<19)
	{
		divisor=0xFFFF;
	}
	else{
		divisor = 1193180 / hz;   /*计算初值*/
	}      
	outportb(0x43, 0x36);             /* 写命令方式字 */
	outportb(0x40, divisor & 0xFF);   /* 设置除数的低8位 */
	outportb(0x40, divisor >> 8);     /* 设置除数的高8位 */
}

/* 这是时钟中断，也就是IR0的处理程序*/
PRIVATE void timer_handler(struct regs *r)
{
    /* 时钟节拍＋1*/
    timer_ticks++;

    /* 每18个节拍，大约1秒，在屏幕上打出一行字 */
    if (timer_ticks % 18 == 0)
    {
        puts("One second has passed\n");
    }
}

/* 把IR0中断处理函数安装到irq上*/
PUBLIC void timer_install()
{
    timer_phase(18);
    irq_install_handler(0, timer_handler);
}

