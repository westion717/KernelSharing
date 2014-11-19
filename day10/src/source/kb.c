#include <kb.h>
#include <irq.h>
#include <scrn.h>
#include <system.h>

/*这是一个美式键盘的映射表*/
/*还有部分非显示字符部分用0代替*/
PRIVATE char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};
		
PRIVATE void keyboard_handler(struct regs *r);

/* 键盘中断处理程序 */
PRIVATE void keyboard_handler(struct regs *r)
{
    unsigned char scancode;

    /*从键盘的数据寄存器端口取得扫描码 */
    scancode = inportb(0x60);

    /* 检测是否是释放按键 */
    if (scancode & 0x80)
    {
        /* 这里可以来处理释放的是什么按键，用来标记
        比如shift，alt，or control是否被按下
         */
    }
    else
    {
        /* 这里处理按下的字符，我们做简单处理，只是输出按下的字符*/
        /*你也可以设置标志符号来判断shift键是否按下，那么需要一个更大的映射表*/
        putch(kbdus[scancode]);
    }
}

PUBLIC void keyboard_install()
{
    irq_install_handler(1, keyboard_handler);
}

	
