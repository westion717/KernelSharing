---
layout: post
title:  使用强大的C语言
categories: day03
---
##内容概述
---

- C语言使用
	- 工具函数编写
	- 主函数编写
	- 并入内核

##知识点分析
---

####工具函数编写

前几天的课程中讲到，能不用汇编的地方就不用汇编，所以我们接下来将使用C语言来编写内核，除了必修使用汇编的地方。在一般的C语言编程中，main函数是程序的主要入口，想必这对大家来说已经非常熟悉了。  
好，接下来我们将在start.asm中调用C语言的main函数，然后就进入了C语言入口，感受C语言的强大。^_^!

在那之前我们先需要一些工具函数，把它当做工具函数。然后在主函数中只写一个死的for循环，停在那。我们先来看看这几个工具函数。


{% highlight c linenos%}

/*lib/system.c */

//内存拷贝，从原地址拷贝count个字节到目标地址
PUBLIC void *memcpy(void *dest, const void *src, size_t count)

//内存赋值,从指定开始每个内存单元都赋值为val
PUBLIC void *memset(void *dest, char val, size_t count)

//内存赋值（字）从指定开始每个内存字单元都赋值为val
PUBLIC short *memsetw(void *dest, short val, size_t count)

//计算字符串长度
PUBLIC size_t strlen(const char *str)

//IO操作，端口读值
PUBLIC char inportb (u16 _port)

//IO操作，端口赋值
PUBLIC void outportb (u16 _port, char _data)
{% endhighlight %}

具体实现参看github的源代码[system.c](https://github.com/westion717/KernelSharing/tree/master/day03/src/source/system.c)

PUBLIC的宏定义和u16的类型定义参看[system.h](https://github.com/westion717/KernelSharing/tree/master/day03/src/include/system.h)和[type.h](https://github.com/westion717/KernelSharing/tree/master/day03/src/include/type.h)

PUBLIC的宏定义和u16的类型定义详细分析看下文。


然后我们还需要一个system.h头文件。在头文件的前后可以加上头文件卫士，防止头文件被重复预处理
	
	#ifndef __SYSTEM_H
	#define __SYSTEM_H

	...

	#endif


####主函数编写

最后是主函数，非常简单。就一个for的死循环。
没有返回值的main函数。因为我们平常写的main函数有返回值，是返回操作系统。而我们现在就是在写操作系统。没必要有返回值。

{% highlight c linenos%}
void main()
{
    for (;;);
}
{% endhighlight %}


####并入内核

那么如何在start.asm中进入main函数呢？其实和之前C与混合使用的教程里一样。我们在`jmp $`这句之前加上如下两句
	
 	extern main
    call main

就进入了main函数。是不是很简单呢


##具体操作

在编译之前，我们把文件分分结构。也便于之后增添。

![dir](http://mykernel.qiniudn.com/day03_dir.png)

看到`.c`的文件都放在`source`目录了，而`.h`头文件放在了`include`里。工具类的`.c`文件都放在了`lib`里。  
另外你也可以建一个`build`目录，来存放编译过程中生成的`.o`文件

好了，我们将新增的编译语句加入到`build.sh`里吧。别忘了链接的时候要把所有生成的文件要一起链接起来

{% highlight bash linenos%}

#build.sh

echo 开始编译、链接，生成内核
nasm -f elf -o ./build/start.o start.asm

gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o ./build/main.o ./source/main.c
gcc -Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I./include -c -o ./build/lib/system.o ./lib/system.c

echo OK，这里开始链接代码
echo 所有的C和汇编生成的目标文件都需要链接进来哦！
ld -T link.ld -o kernel.bin ./build/start.o ./build/lib/system.o ./build/main.o
echo 完成了!

{% endhighlight %}

gcc的编译参数部分稍作解释  
`-Wall` 是打开所有警告信息，在编译的时候显示所有的警告信息  
`-nostdinc` 不使用标准的头文件  
`-fno-builtin` 不使用内建的函数。比如printf这类，不用指定头文件，编译器默认也知道链接这些库，而我们现在是在开发内核，不是在操作系统上编程  
`-I./include ` 指定我们头文件的位置
`-c` 只编译成目标文件，不要链接

其他参数读者自行搜索

Ok按照前一天课程，运行`build.sh`

编译结果可能会有一条警告信息，因为按照标准main函数要返回int，不过这不影响，可以忽略  
然后就按上次说的方法把`kernel.bin`文件复制到`cdrom`里，然后制作成iso镜像，放在虚拟机里运行，这里就不详述了。

![mykernel](http://mykernel.qiniudn.com/day02_mykernel.png)


运行时的画面和上次课一样，卡在了那个位置。这不奇怪，因为main函数写的是一个for的死循环。虽然看起来好像一样，但是已经进入了main函数。

##注意提醒
---

- `type.h`里将`unsigned char`,`unsigned short`和`unsigned int`定义为对应的**u+**位数大小，是为了更直观地体现类型占的空间并且存的数都是无符号数，一般是用在表示存的数是地址，端口，所以都是正数。
- `PUBLIC`与`PRIVATE`对应的宏的作用是是否在函数前`static`关键字。作用就是来区别能否被其他.c文件调用。这是C语言static关键字的知识，这里不做详述
- `outportb`和`inportb`函数的具体实现中用到了上节课没讲的第二种使用汇编的方法。就是在C语言中直接嵌入汇编。你也可以用第一种方式在汇编里写好方法，然后在C语言中调用。因为第二种方式用的不多，这里不做解释。