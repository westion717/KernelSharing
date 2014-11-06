---
layout: post
title:  中断机制
categories: day06
---
##内容概述
---

- 什么是中断
- IDT
- 装载IDT

##知识点分析
---

####什么是中断
下面我们要讲内核中又一重要的模块，中断。那么什么是中断呢？  
中断就是CPU在执行`主程序`的时候，被其他东西打扰之后，转而去处理其他程序(`中断子程序`)去了。目的是为了有交互性，这样能及时响应你的操作。比如你在键盘按下一个键，CPU就会马上响应你在屏幕上打出一个字来。如果没有中断那你将会等很久才能看到响应。等处理完对你的响应之后，CPU又转回主程序继续执行。这样的一个过程叫做中断。最通俗的理解就是你在做一件你主要做的事，比如看书，这时候电话响了，给了你一个中断，然后你就去接电话，打完电话，回来继续看书。这个过程就是一个中断。  
当然，计算机中的中断也不仅仅是用来响应用户，在发生某些情况的时候也需要中断。比如计算机内部出现了一些错误，比如除法中除数为0了。这些情况的时候也会让CPU中断。  
如上说所，CPU内部会产生中断，CPU外部的请求也会产生中断。所以上面我们所说的中断只是一个广义的中断。分细一点的话，一些内部的错误我们可以把它叫做`fault`，`abort`，或`trap`，而外部的一些请求，比如键盘，打印机等的请求,还有用软件发的中断指令，我们可以把它叫做`interrupt`。这些在广义上都可以叫中断，但具体还是有一些区别。  


####IDT
中断之后当然会要去处理中断子程序了，那么中断子程序在哪呢？你总要告诉计算机中断处理程序在哪，并且要事先把中断子程序都写好。这样发生中断的时候就可以被执行，就像写好预案一样。  
这就好比你有3个锦囊，遇见情况1，打开锦囊1来处理。遇见情况2，打开锦囊2来处理。同理，中断就好比各种情况，锦囊就是对应的中断处理程序。那怎么把中断和中断处理程序对应起来呢？自然想到，查表。这个表叫做`IDT`（Interruption Descriptor Table），中断描述符表。每个中断发生的时候，都有唯一一个中断号，这样CPU就会根据这个中断号去IDT里找对应的中断程序。比如中断号为0的中断发生，CPU就会看IDT中0号条目，来找到中断程序在哪。然后让CPU里的CS，IP寄存器指向中断程序，然后执行。这个过程CPU自动会完成。  

接下来看IDT中的条目结构是怎么样的。IDT的描述符和GDT中的描述符比较相似。大小都是8个字节，64位。  

![idtdescriptor](http://mykernel.qiniudn.com/day06_idtdescriptor.png)

最低两个字节和最高两个字节是偏移地址。byte2，byte3是段选择子，选代码段用的，也就是中断子程序在哪个代码段。然后再根据偏移地址就可以找到。byte4全0就好。最后byte5和之前GDT中相关属性的解释是一样的。只不过现在我们的描述符是**门描述符**，不是之前GDT中代码段或数据段描述符。所以type属性要看上次图的第二列。是代码段／数据段还是系统段／门是S位设置的。所以这里S位是0。

计算机中我们一般定义256个中断，所以IDT中有256个条目。所以`idt_entry`结构会创建一个256个元素的数组。

####装载IDT

IDT的装载和GDT类似，也有一个IDTR寄存器来存放IDT的位置和大小，用汇编指令'lidt [idt_ptr]'来记载。整个IDTR的结构和GDTR一模一样。

因为过程和上节的GDT加载和初始化差不多。只不过这次的IDT表里有256项。接下来，我们就直接操作了。这次课，我们只做IDT的初始化，还没有写对应的中断子程序。所以256条目先全部写**0**吧。

##具体操作

{% highlight c linenos%}
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
PRIVATE void idt_set_gate(int num, u32 base, u16 sel, u8 flags);

/* 设置条目函数*/
PRIVATE void idt_set_gate(int num, u32 base, u16 sel, u8 flags)
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
    idtp.base = (u32)&idt;

    /* 先全部初始化为0*/
    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    /* 之后可以在这里设置对应的条目 */

    /* 加载 */
    idt_load();
}

{% endhighlight %}

{% highlight as linenos%}

; 载入idt。idt_load函数将在idt.c被中调用。以下代码可以加在start.asm的加载GDT之后。
global idt_load
extern idtp
idt_load:
    lidt [idtp]
    ret
{% endhighlight %}

最后别忘记在makefile加上idt.0的编译和链接。还有'idt.h'的建立，把`idt_install()`函数声明添加进去。然后在main函数中gdt初始化之后加上调用idt的初始化。

OK。最后编译链接封装生成iso，加载运行。得到的结果和上次课一样（就不上图了）。现在也看不出我们的idt是否正常（不过你也可以在main函数里除以0，试试会不会崩）。下次课我们将按上我们的中断处理程序，这样就除以0之后就可以让他在屏幕上显示出一行字，来证明我们的idt安装成功了。

##注意提醒
---
- 因为我们还没有添加中断子程序，也就是说中断之后CPU不知道去哪执行对应的程序。这样的结果是如果错误发生系统就会崩掉。你可以在main函数中做一个除数为0的除法试一下。
