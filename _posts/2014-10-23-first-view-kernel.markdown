---
layout: post
title:  kernel窥探
categories: day02
---
OK,今天我们就开始进入内核的初步开发了，想想还有点小激动呢。但在那之前，还需要做一些准备工作。了解一下C语言如何和汇编混用还有内核是怎么被加载和运行的。

##内容概述
---
- ELF
- C语言与汇编混合使用
	- 方法1:设置链接标志（label）
	- 方法2:直接内嵌入C代码中

- 计算机启动流程
	- boot和Bootloader

- kernel初步编程
	- Start
	- Link
	- Build

- iso镜像生成
	- GRUB介绍
	- iso镜像生成
	- 运行内核

##知识点分析
---

####ELF（Executable and Linkable Format）

内核，其实简单的说就是一个二进制可执行文件。那么一个二进制可执行文件的生成，经历3个过程。 

![编译链接](http://mykernel.qiniudn.com/day02_compile.png)

***文件后缀名只是为了标示文件格式，便于人识别，对于机器无影响。汇编文件后缀也常用`.s`或`.S`标示***
 
- **源代码编写** 用文本编辑器编写C代码或者汇编代码的过程
- **编译** 将源代码经过编译器生成目标代码或者叫做目标文件的过程
- **链接** 将目标文件链接成可执行文件的过程

二进制的可执行文件也具有一定的格式，包含可执行的二进制机器指令部分，还有一些头部，作为描述信息。  
比如，程序的**加载地址**，程序的**执行入口地址**。加载该文件的程序根据这些描述信息将程序加载到指定的位置，并从程序的入口开始执行。

ELF作为二进制可执行文件格式中的一种，是Linux的主要可执行文件格式。像Windows平台的二进制可执行文件格式叫做PE。




####C语言与汇编混合使用

一些需要**准确操作寄存器**或**端口**的代码，是无法直接用C语言来写的。所以一些初始化硬件的操作需要汇编来辅助，通常的情况是C语言与汇编混合使用。  
C语言通过编译器也是生成汇编代码之后，再编译成目标文件，所以本质上两者是一样的。
下面介绍如何将C语言与汇编混合使用。


**下面例子仅为演示，代码不规范也不完整，仅作说明作用**  
**下面例子中的链接也使用的是gcc，之后的正式代码中链接将用单独的ld命令实现**

如下是一段汇编代码

;C与汇编混合编程测试  
;code.asm
{% highlight asm linenos %}
global mytest ;导出链接标记被外部调用
extern myprint ;导入外部函数
mytest:
	push dword 20 ; 将20作为4字节压栈
	push dword 10
	call myprint ;调用外部的myprint函数
	add esp,8 ;清理栈
	ret; 函数返回
{% endhighlight %}

	
如下是一段C代码


;C与汇编混合编程测试  
;main.c
{% highlight c linenos%}
#include <stdio.h>
	
void mytest();
	
void myprint(int a,int b)
{
	
	printf("a=%d,b=%d\n",a,b);
}
	
int main()
{
        mytest(); //调用汇编中的mytest函数
        return 0;
}
{% endhighlight %}
	
	
使用如下代码生成可执行文件

	
	nasm -f elf code.asm ;编译生成code.o目标文件
	gcc -o main.o -c main.c 
	gcc -o a.out main.o code.o ;gcc的链接接口，生成a.out二进制可执行文件

运行

	./a.out

执行结果

	a=10,b=20


先解释一下汇编代码。  
第7行`mytest`标记之后到`ret`之间的代码是一个子程序。而`mytest：`是一个链接标记（label），本质是一个地址，也是一个子程序的名字，在C语言中也叫**函数**。在链接器链接时，通过函数名找到对应的链接标记。而第4行的`global`关键字的目的是将mytest这个标记导出，以便在C语言中调用。`extern`关键字目的是说明myprint这个函数是从外部引进的，以便在第10行调用。

再看C语言代码。  
main函数中调用了`mytest`函数，该函数的过程是在汇编文件中实现。也就是`mytest`标记之后的语句，直到`ret`语句返回为止。而更具C语言函数的规则，使用前必须声明。所以在main函数之前声明了`void mytest()`。main函数之前还定义了一个`myprint`函数，该函数在汇编中被调用`call myprint`。如前面所说汇编文件中在调用之前用`extern`进行声明该函数来自外部（来自别的汇编文件也可以）。

所以综上，在汇编中的程序如果需要**被外部调用**，就需要用`global`关键字，将标记导出。汇编中需要**调用外部函数**则在调用之前就要用`extern`声明该函数的名字。

那么，在函数调用的过程中的参数传递和返回值是如何解决的？通过遵循**C调用约定**（C Calling convention）来解决  

- 函数中后面的参数先入栈
- 函数的返回值通过**EAX**寄存器传递
- 由调用者清理栈

所以上述汇编例子中，push了两个4字节（dword）的整数20和10。而在C语言中接收到的顺序是相反的，a=10,b=20。这也验证了函数中后面的参数先入栈的原则。  
同样的，当在C语言中调用汇编中定义的函数时，如调用下面的函数  

	func(a,b);（a,b为4字节类型）

先将b压入栈，再将a压入栈，最后压入eip**（下一条程序地址，便于返回时从栈中取出返回至原程序）**  
则在汇编函数中，[esp+4]存储的是a,[esp+8]存储的是b。栈图如下

![stack](http://mykernel.qiniudn.com/day02_stack.png)

可通过
	
	mov eax [esp+4]
	mov ebx [esp+8]

将a放入eax中，将b放入ebx中

返回值的处理则较为简单，只需在eax放入返回值，然后调用汇编的`ret`指令,就可在C语言中收到返回值。这里就不过多叙述。

以上是C和汇编互相调用的一种方法。另一种在**C中调用汇编**的方法将在以后运用到的地方再说。


####计算机启动流程

当计算机上电后，CPU中寄存器会被复位至初始状态。然后开始执行指令。初始状态CPU的各寄存器都为0，**除了**CS寄存器是`FFFFH`。所以第一条指令的物理地址是`FFFF0H`。该地址对应系统的**BIOS(Basic Input Output System)**。系统的最基础的输入输出系统。每台计算机都具备的基础功能的软件，它被固化在主板上的系统ROM里。  
所以计算机启动之后，运行ROM里的指令。做的主要工作是**POST（Power On Self Test）**，**自检**。自检是检查计算机的完整性（比如外设是否正常工作，键盘是否连接等）。这些工作完成之后，会听到PC喇叭发出“滴”的一声。如果一切正常，BIOS就会选择一个启动设备，并且读取该设备的第一扇区（即启动扇区）。这也就是平常所说的从什么设备启动，平常一般默认从硬盘启动，读硬盘的第一个扇区。或者也可以通过设置从U盘或者光盘启动。

选择的设备的第一个扇区（启动扇区）中的内容，我们把它叫做boot（引导）。该扇区最后两个字节内容为`0x55AA`。这样就能被BIOS识别，并把该扇区内容加载到地址为`0x7c00`处，然后执行。

说到这里你是不是已经知道了boot里的内容是干什么工作了呢？把盘上的内核程序载入内存，然后执行内核。

很好，你已经掌握了启动和载入内核的流程了。但是不幸地告诉你，在载入内核之前，还有很多工作要做，一个扇区根本容不下这么多的代码。那怎么办？

所以我们要有个东西叫做Bootloader。由于boot的大小只有512B，能做的工作有限，所以我们在boot里只需做将Bootloader载入内存并运行Bootloader的工作。然后让Bootloader做复杂的载入内核前的准备工作，然后载入内核，并运行。整个流程如下图

![load](http://mykernel.qiniudn.com/day02_load.png)


为了简化本教程，boot与Bootloader将不自己编写。  
本教程中Bootloader将采用现成的软件，GRUB。之后会介绍。而将GRUB加载并运行的boot在mkisofs命令制作iso启动镜像时就已经完成，即**GRUB启动盘的制作**。之后制作iso镜像时会详细说明。

综上，本书的重点内容放在kernel的开发上。而之前的启动工作则交由现成的软件，GRUb，来完成。

####kernel初步编程

在之前的C与汇编混合使用中介绍到，C语言操作底层硬件的局限性，所以内核的编写中，必须会用到汇编。会在汇编中编写一些底层的操作，然后在C语言中调用。并且我们引导内核用的是GRUB，那么为了能够被GRUB引导，内核要做好对应的接口。所以接口格式的定义还有内核的入口都在汇编文件中指定。好了废话不多说，来建立一个真正的小内核吧。

先看以下汇编代码

;start.asm
	
{% highlight asm linenos %}
[BITS 32]
global start
start: ;开始标记处
    mov esp, _sys_stack ;设置栈指针，指向的地址标记在下面有定义
    jmp stublet ;跳转到下方标记的地址处执行
	
	;这部分作为GRUB启动的接口必须在内存中以4字节对齐
	ALIGN 4
	mboot:
	;mboot到stublet之间的代码都是为了定义GRUB加载的接口

	MULTIBOOT_PAGE_ALIGN	equ 1<<0
	MULTIBOOT_MEMORY_INFO	equ 1<<1
	MULTIBOOT_AOUT_KLUDGE	equ 1<<16
	MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
	MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
	MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
	EXTERN code, bss, end
	
	; GRUB引导接口头定义
	dd MULTIBOOT_HEADER_MAGIC
	dd MULTIBOOT_HEADER_FLAGS
	dd MULTIBOOT_CHECKSUM
	    
	    
	dd mboot
	dd code
	dd bss
	dd end
	dd start
	
; 现在这里的jmp $是一个死循环，让程序停在这。
; 之后我们会在这句前加上'extern main'和'call main',
; 跳转至C语言的main函数
stublet:
	jmp $
	
	
; 之后将在这里加入初始化程序
	
	
; 之后将在这里加入中断服务程序的代码
	
	
	
; 这里定义了BSS段，用作存储数据的栈。记住栈是从低地址到高地址的
SECTION .bss
    resb 8192 ; 从低到高向下走了8KB的空间
	_sys_stack:	 ; 所以这里的'_sys_stack'标记的地址就是现在的栈顶的位置
	
{% endhighlight %}

以上代码，设置了GRUB启动的对接接口，然后设置了栈指针（esp）的位置
和预留出8KB的栈空间

然后我们用nasm编译这个start.asm文件，生成elf格式的目标文件start.o，然后用ld链接器链接start.o生成kernel.bin这个二进制可执行文件

在链接的过程中还需要指定这个生成的二进制可执行文件的格式，还有内核被加载到内存中的位置等等信息。所以我们用一个链接脚本来完成这个工作


	OUTPUT_FORMAT("binary")
	ENTRY(start)
	phys = 0x00100000;
	SECTIONS
	{
	  .text phys : AT(phys) {
	    code = .;
	    *(.text)
	    *(.rodata)
	    . = ALIGN(4096);
	  }
	  .data : AT(phys + (data - code))
	  {
	    data = .;
	    *(.data)
	    . = ALIGN(4096);
	  }
	  .bss : AT(phys + (bss - code))
	  {
	    bss = .;
	    *(.bss)
	    . = ALIGN(4096);
	  }
	  end = .;
	}
	

对于链接脚本，本教程就不细讲，详细的解读需要读者自行查询资料。简单点两个地方。其他不懂的地方并不影响后续的学习。  
`phys = 0x00100000` 指定了内核加载到内存中的位置，位于1MB处。
`ENTRY(start)` 指明了入口地址的标记，start也可以改成别的，相应的`start.asm`中也需要修改与之匹配。

另外，用nasm编译start.asm并且链接生成kernel的过程需要使用好多条指令，并且有一定的顺序。之后的C代码也需要编译链接，整个过程比较繁琐。为了简便，我们需要一个帮我们编译和链接的脚本。最完善的办法是使用Makefile。考虑到Makefile编写和学习有一定难度，所以我们暂时只用bash脚本来处理。我们建立一个build.sh文件，将需要执行的操作写入，便可以一口气执行。

以下是bash的内容

{% highlight bash linenos %}
	#build.sh
	
	echo 开始编译、链接，生成内核
	nasm -f elf -o start.o start.asm
	
	echo 我们之后将在这个位置加入C代码
	
	echo OK，这里开始链接代码
	echo 所有的C和汇编生成的目标文件都需要链接进来哦！
	ld -T link.ld -o kernel.bin start.o
	echo 完成了!
{% endhighlight %}

##具体操作
---
将编写好的三个文件放在一个目下  
运行以下命令将build.sh变为可执行
	
	chmod +x build.sh

然后在终端运行

	./build.sh

*是不是在目下生成了start.o还有kernel.bin呢？*

然后我们将制作GRUB的iso启动镜像

	mkdir cdrom
	cd cdrom
	mkdir -p boot/grub

复制`stage2_eltorito`到boot/grub中。该文件存在/usr/lib/grub/i386中（或/usr/lib/grub/x86_64。如果该文件不存在则，在Ubuntu下则apt-get install grub）  
新建文件,boot/grub/menu.lst  
内容如下

	default 0 #默认第一个
	title mykernel #标题
	root (cd) #设置根目录
	kernel /kernel/kernel.bin #内核位置

menu.lst中的内容是配置GRUB起启动时的参数和信息，并指定了要加载的内核的位置。GRUB启动后如下图显示

![grub](http://mykernel.qiniudn.com/day02_grub.png)


然后将之前生成的内核kernel.bin复制到cdrom/kernel/通过命令

	cp kernel.bin cdrom/kernel/

目录结构如下

![dir](http://mykernel.qiniudn.com/day02_dir.png)


回到cdrom文件夹的父目录，也就是build.sh所在目录。运行以下命令

	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o kernel.iso cdrom

调用iso镜像生成命令`mkisofs`,指定了GRUB的文件位置（相对目录cdrom),生成镜像的目录`cdrom`,镜像名字`kernel.iso`  
执行后即生成`kernel.iso`

最后将此iso文件从虚拟机中拿出（如果不是在虚拟机里开发就不需要）。新建一个虚拟机系统，并将此iso镜像加载到光驱运行

是否看见期待已久的GRUB的启动画面了呢？如上图

选中mykernel回车进入后如下图

![mykernel](http://mykernel.qiniudn.com/day02_mykernel.png)

运行结果打出了一行字，然后光标就停在了第二行。这行字是GRUB运行你的kernel之前打出的字，然后就运行了你的kernel。你的`start.asm`中`jmp $`一句执行了死循环，光标当然就停在那了。但是证明你的内核已经被运行了。  

怎么样？有没有很激动。为了这一刻，我们之前做了很多的准备。虽然现在显示的结果觉得很无趣，但这已经迈出了一大步。我们完成了boot启动，并且加载了Bootloader，并且还通过Bootloader将内核加载并运行了。所以现在这个iso已经可以算是一个内核而不仅仅是一个引导了。  
是不是很有成就感！！！

PS：虚拟机里mykernel只有强制退出了。^-^!


##注意提醒
---

- C与汇编混合使用中有**[由调用者清理栈]**原则，所以当在汇编代码中调用C的函数时，记得将压入栈中的参数退栈。其实只要将esp指针重新指到压栈前的位置即可。上述例子中`add esp,8`即将栈指针重新向下移动8个字节（2个int大小)。C调用汇编中的函数就不需要，C编译器已经帮你做了这个工作
- X86平台堆栈是由高地址向低地址衍生
- 使用GRUB作引导软件，在加载内核成功到内存中，准备执行内核之前的硬件状态如下。
	1. EAX是魔数**0X2BADB002**,表明是用GRUB引导的
	2. EBX是Bootloader提供的引导信息结构的所在的32位物理地址。可以在进入main之前把这两个参数通过压栈传过去。可以获取硬件的一些信息，比如总内存大小
	3. CS指向一个以0为基址，4GB－1为界限的**代码段**描述符。
	4. DS，SS，ES，FS和GS指向一个以0为基址，4GB－1为界限的**数据段**描述符。
	5. A20地址线被打开
	6. CR0的PE位置位，PG位清除，即保护模式开启，分页关闭。
	7. EFLAGS的VM位必须清除，IF位必须清除。
	8. GDTR这个时候可能不合法，所以在设置成指向正确的GDT之前，即使段寄存器像上面一样设置好了也不要用。
	9. 中断被关闭。


	**3,4,6,8都涉及到保护模式的知识，之后会介绍保护模式
	参考自[http://module77.is-programmer.com/posts/16190.html](http://module77.is-programmer.com/posts/16190.html)**