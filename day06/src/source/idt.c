/*idt.c*/

#include <system.h>
#include <idt.h>
/* IDT条目定义 */
struct idt_entry
{
	u16 base_lo;
	u16 sel;        /* 段选择子 */
	u8 always0;     /* 总为0 */
	u8 flags;       /* type属性见GDT章*/
	u16 base_hi;
} __attribute__((packed));

/*IDTR结构*/
struct idt_ptr
{
	u16 limit;
	u32 base;
} __attribute__((packed));

/* 定义了256个中断条目*/
PRIVATE struct idt_entry idt[256];
struct idt_ptr idtp;

/*这个函数在start.asm中，载入idt*/
extern void idt_load();

/* 设置条目函数*/
PUBLIC void idt_set_gate(int num, u32 base, u16 sel, u8 flags)
{
	idt[num].base_lo = (base & 0xFFFF);
	idt[num].base_hi = (base >> 16) & 0xFFFF;
	idt[num].sel = sel;
	idt[num].always0 = 0;
	idt[num].flags = flags;
}

/* 安装IDT供main函数调用*/
PUBLIC void idt_install()
{
	/* 设置IDTR结构，把大小和IDT的地址设，像GDT中设置GDTR结构一样 */
	idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
	idtp.base = (u32)idt;

	/* 先全部初始化为0*/
	memset(idt, 0, sizeof(struct idt_entry) * 256);

	/* 之后可以在这里设置对应的条目 */

	/* 加载 */
	idt_load();
}

