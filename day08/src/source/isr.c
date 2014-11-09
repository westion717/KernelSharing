#include <system.h>
#include <idt.h> //因为要使用idt中的设置描述符函数
#include <scrn.h>
#include <irq.h>

/*导入start.asm中前32个中断处理函数*/
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();







/* 接下来用上次课的设置IDT中描述符的方法，将32个isr中断子程序入口对应每个条目设置到IDT中。即偏移地址处设置为对应的函数名。
*  0x08是我们内核代码段的段选择子*  
*  flags是0x8E，代表这个中断门是存在于内存中的*  set to the。特权级是0，代表是内核态的。type是E，代表386中断门
*/

PUBLIC void isr_install()
{
	idt_set_gate(0, (u32)isr0, 0x08, 0x8E);
	idt_set_gate(1, (u32)isr1, 0x08, 0x8E);
	idt_set_gate(2, (u32)isr2, 0x08, 0x8E);
	idt_set_gate(3, (u32)isr3, 0x08, 0x8E);
	idt_set_gate(4, (u32)isr4, 0x08, 0x8E);
	idt_set_gate(5, (u32)isr5, 0x08, 0x8E);
	idt_set_gate(6, (u32)isr6, 0x08, 0x8E);
	idt_set_gate(7, (u32)isr7, 0x08, 0x8E);

	idt_set_gate(8, (u32)isr8, 0x08, 0x8E);
	idt_set_gate(9, (u32)isr9, 0x08, 0x8E);
	idt_set_gate(10, (u32)isr10, 0x08, 0x8E);
	idt_set_gate(11, (u32)isr11, 0x08, 0x8E);
	idt_set_gate(12, (u32)isr12, 0x08, 0x8E);
	idt_set_gate(13, (u32)isr13, 0x08, 0x8E);
	idt_set_gate(14, (u32)isr14, 0x08, 0x8E);
	idt_set_gate(15, (u32)isr15, 0x08, 0x8E);

	idt_set_gate(16, (u32)isr16, 0x08, 0x8E);
	idt_set_gate(17, (u32)isr17, 0x08, 0x8E);
	idt_set_gate(18, (u32)isr18, 0x08, 0x8E);
	idt_set_gate(19, (u32)isr19, 0x08, 0x8E);
	idt_set_gate(20, (u32)isr20, 0x08, 0x8E);
	idt_set_gate(21, (u32)isr21, 0x08, 0x8E);
	idt_set_gate(22, (u32)isr22, 0x08, 0x8E);
	idt_set_gate(23, (u32)isr23, 0x08, 0x8E);

	idt_set_gate(24, (u32)isr24, 0x08, 0x8E);
	idt_set_gate(25, (u32)isr25, 0x08, 0x8E);
	idt_set_gate(26, (u32)isr26, 0x08, 0x8E);
	idt_set_gate(27, (u32)isr27, 0x08, 0x8E);
	idt_set_gate(28, (u32)isr28, 0x08, 0x8E);
	idt_set_gate(29, (u32)isr29, 0x08, 0x8E);
	idt_set_gate(30, (u32)isr30, 0x08, 0x8E);
	idt_set_gate(31, (u32)isr31, 0x08, 0x8E);
}



/* 这是一个字符串数组，用来保存出错信息。我们现在不处理错误而只在屏幕上显示一行字 */
PRIVATE char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};



/* 目前我们前面32个异常的处理全部用这个函数，而且只输出对应的字符串。然后死循环在那 */
PRIVATE void fault_handler(struct regs *r)
{
        /* 打出错误信息，然后停止在那*/
        puts(exception_messages[r->int_no]);
	putch('\n');
	puts("int_no=");
	putch(r->int_no);
	putch('\n');
	while(1);
}

extern void irq_handler(struct regs *r);
void isr_handler(struct regs *r)
{

	/* 判断这个异常是不是0-31的,不是则是外部中断，调用外部中断处理程序*/
	if (r->int_no < 32)
	{
		fault_handler(r);
	}
	else
	{
		irq_handler(r);
	}

}


