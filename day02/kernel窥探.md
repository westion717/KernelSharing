OK,今天我们就开始进入内核的初步开发了，想想还有点小激动呢。

##内容概述
---

- C语言与汇编混合使用
	- 方法1:设置链接标志（label）
	- 方法2:直接内嵌入C代码中

- 内核初始化
	- Start
	- Link
	- Build

- iso镜像生成
	- GRUB介绍
	- iso镜像生成
	- 运行内核

##知识点分析
---

内核，其实简单的说就是一个二进制可执行文件。那么一个二进制可执行文件的生成，经历3个过程。 
 
- **源代码编写** 用文本编辑器编写C代码或者汇编代码的过程
- **编译** 将源代码经过编译器生成目标代码或者叫做目标文件的过程
- **链接** 将目标文件链接成可执行文件的过程

***二进制的可执行文件也具有一定的格式，不只是可执行的二进制机器指令部分，还有一些头部，作为描述信息***

本教程的二进制可执行文件的格式为elf（executable linkedble file），32位。elf是Linux下的二进制可执行文件格式




####C语言与汇编混合使用

一些需要**准确操作寄存器**或**端口**的代码，是无法直接用C语言来写的。所以一些初始化硬件的操作需要汇编来辅助，通常的情况是C语言与汇编混合使用。  
下面介绍如何将C语言与汇编混合使用。

  
**该例子仅为演示，代码不规范也不完整，仅作说明作用**  
**该例子中的链接使用的是gcc的链接接口，之后的正式代码中链接将用单独的ld命令实现**

如下是一段汇编代码


	;C与汇编混合编程测试
	;code.asm

	global mytest
	
	extern myprint
	mytest:
	        push dword 20
	        push dword 10
	        call myprint
	        add esp,8
	        ret

如下是一段C代码

	;C与汇编混合编程测试
	;main.c

	#include <stdio.h>
	
	extern void mytest();
	
	void myprint(int a,int b)
	{
	
	        printf("a=%d,b=%d\n",a,b);
	}
	
	int main()
	{
	        mytest();
	        return 0;
	}

使用如下代码生成可执行文件

	nasm -f elf code.asm
	gcc -o main.o -c main.c
	gcc -o a.out main.o code.o ;gcc的链接接口

运行

	./a.out

执行结果

	a=10,b=20

