#include <idt.h> //因为要使用idt中的设置描述符函数
#include <scrn.h>
#include <irq.h>
/*导入start.asm中32-47外部中断处理函数*/
extern void isr32();
extern void isr33();
extern void isr34();
extern void isr35();
extern void isr36();
extern void isr37();
extern void isr38();
extern void isr39();
extern void isr40();
extern void isr41();
extern void isr42();
extern void isr43();
extern void isr44();
extern void isr45();
extern void isr46();
extern void isr47();


/* 这以下是一个函数指针的数组，可以用来存放处理中断的程序的函数*/
PRIVATE void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

PRIVATE void irq_remap();

/* 安装对应IR中断的处理程序的函数*/
PUBLIC void irq_install_handler(int irq, void (*handler)(struct regs *r))
{
    irq_routines[irq] = handler;
}

/* 卸载对应IR中断的处理程序的函数 */
PUBLIC void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}

/* 初始化8259A,映射中断号 */
PRIVATE void irq_remap()
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}

/* 先调用irq_remap把中断号修改好后。再设置IDT中32-47号中断对应的中断子程序入口地址 */
PUBLIC void irq_install()
{
    irq_remap();

    idt_set_gate(32, (u32)isr32, 0x08, 0x8E);
    idt_set_gate(33, (u32)isr33, 0x08, 0x8E);
    idt_set_gate(34, (u32)isr34, 0x08, 0x8E);
    idt_set_gate(35, (u32)isr35, 0x08, 0x8E);
    idt_set_gate(36, (u32)isr36, 0x08, 0x8E);
    idt_set_gate(37, (u32)isr37, 0x08, 0x8E);
    idt_set_gate(38, (u32)isr38, 0x08, 0x8E);
    idt_set_gate(39, (u32)isr39, 0x08, 0x8E);
    idt_set_gate(40, (u32)isr40, 0x08, 0x8E);
    idt_set_gate(41, (u32)isr41, 0x08, 0x8E);
    idt_set_gate(42, (u32)isr42, 0x08, 0x8E);
    idt_set_gate(43, (u32)isr43, 0x08, 0x8E);
    idt_set_gate(44, (u32)isr44, 0x08, 0x8E);
    idt_set_gate(45, (u32)isr45, 0x08, 0x8E);
    idt_set_gate(46, (u32)isr46, 0x08, 0x8E);
    idt_set_gate(47, (u32)isr47, 0x08, 0x8E);
}

/* 响应外部中断的处理程序 
 * 处理完成后要对8259A发送中断结束指令。以便第二次请求这个级别的请求
 */
void irq_handler(struct regs *r)
{
    /* 先定义一个空的函数指针 */
    void (*handler)(struct regs *r);

    /* 根据中断号调用相应的处理函数，如果有的话*/
    handler = irq_routines[r->int_no - 32];
    if (handler)
    {
        handler(r);
    }

    /* 如果是从片产生的中断，那么完成处理后要对从片发送中断结束 */
    if (r->int_no >= 40)
    {
        outportb(0xA0, 0x20);
    }

    /*不管是哪个片产生的中断，完成后都需要对主片发送结束*/
    outportb(0x20, 0x20);
}

