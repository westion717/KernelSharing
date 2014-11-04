#include <gdt.h>

/* GDT条目结构
*  __attribute__((packed)) 结构是防止编译器优化
* 成员从上到下地址以此递增，从低位到高位
*/
PRIVATE struct gdt_entry
{
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
}__attribute__((packed));

/* GDTR的机构 limit记得是长度－1*/
PRIVATE struct gdt_ptr
{
    u16 limit;
    u32 base;
}__attribute__((packed));

/* GDT条目 */
PRIVATE struct gdt_entry gdt[3];
/*GDTR结构*/
PUBLIC struct gdt_ptr gp;

/* 设置gdt中条目内容 
*  num是对于的条目号，从0开始
*/
PRIVATE void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran);

/* 这个函数定义在汇编中，调用lldt更新gdt
* extern 来表示这个函数在汇编中
* 其实这里extern 加不加都可
 */
extern void gdt_flush();

PRIVATE void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran)
{
    /* 设置基地址 */
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    /* 设置段长 */
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    /* 设置权限粒度等属性 */
    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

PUBLIC void gdt_install()
{
    /* 设置GDTR结构 */
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (u32)&gdt;

    /* 0号描述符要为null*/
    gdt_set_gate(0, 0, 0, 0, 0);
   /*
    *  1号描述符。是一个代码段，基址为0,范围4GB,使用4KB的粒度
    *  使用32位操作符
    */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /*  2号条目。这是一个数据段，其他属性与上面代码段一样 */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* 更新gdt */
    gdt_flush();
}
