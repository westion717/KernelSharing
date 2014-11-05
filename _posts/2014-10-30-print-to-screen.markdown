---
layout: post
title:  屏幕输出
categories: day04
---
##内容概述
---

- 目录结构调整
- makefile
- VGA

##知识点分析
---

####目录结构调整

需要将目录结构调整一下，便于makefile书写。调整为如下

![newdir](http://mykernel.qiniudn.com/day04_newdir.png)

删除了`lib`目录，把里面的system.c放入source

####makefile编写

之前我们编译程序，生成iso镜像，等每一条指令的执行是通过**build.sh**来执行。现在我们把它放到一个叫**makefile**的文件里。因为makefile功能更强大，为了长远的发展，我们转入使用makefile。但是makefile的语法书写功能很多，我们不想花太多时间在这里，所以我们所做只是满足了需求，之后再发展变复杂了，再改进。

直接来看写好的makefile

{% highlight make linenos%}

include=include
source=source
build=build
options=-Wall -O -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -I${include} -c -o
compiler=gcc
ld=ld
cd=cdrom

objects=start.o system.o main.o

kernel: ${objects}
        ${ld} -T link.ld -o kernel.bin ${objects}
        mv ${objects} ${build}
        mv kernel.bin ${cd}/kernel/
main.o:
        $(compiler) $(options) main.o $(source)/main.c
system.o:
        ${compiler} $(options) system.o ${source}/system.c
start.o:
        nasm -f elf -o start.o start.asm
clean:
        rm $(build)/*.o kernel.iso
iso:
        mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o kernel.iso $(cd)
        
{% endhighlight %}

简单解释一下，一开始的几行`xx=yy`,xx就是变量名，在下文中通过`${xx}`即可引用。为什么要用变量的形式，就是避免重复写同样的东西，便于之后修改。然后`xxx:`是一个构建标记，标记右边同一行的内容是构建这个标记所需要的前提标记。比如`kernel: ${objects}`，经过变量替换后`kernel:start.o system.o main.o`。意思就是kernel这个构建目标需要start.o,system.o,main.o这3个构建目标。而这三个构建目标在下文中列出了。所以在构建kernel时候会先去构建其他三个目标。以此类推。目标下方语句是构建该目标需要执行的。然后写好makefile怎么使用呢？只需要在makefile所以在目录执行`make`，它就会自动构建第一个目标，也就是kernel。如果构建别的目标就要显示指出`make clean`。
  
然后我们再改写`build.sh`

{% highlight bash linenos%}
#build.sh

echo 开始清理
make clean
echo 开始编译
make
echo OK
echo 生成iso
make iso
echo 完成了
{% endhighlight %}

####VGA video card

好了，进入今天的正式内容。我们要介绍怎么让我们的内核在屏幕上输出字符。  
计算机中的基础显卡会把`物理地址`为`0xB8000`开始的80x25个字符自动输出到屏幕，只要这段内存中的值有变化，它就会自动将变化后的值显示在屏幕上。所以我们要在屏幕某个位置显示字符，只需要在对应的内存单元中写入数值即可。


其实呢，在这段显存中，每个字符占据1字，2个内存单元，也就是16位。低8位表示字符的ascii码，而高8位则是这个字符的颜色。高8位中的高4位是表示字符的背景色，低4位是字符的前景色

![color](http://mykernel.qiniudn.com/day04_color.png)

为了能更好的定位字符在内存的哪个位置，这里有一个公式

	索引 = (行 * 80) + 列
	
**注意：这里行和列都是从0计算的，所以［0,24］行，[0,79]列。也就是屏幕从上到下有25行，从左到右有80列，都是从零开始数数。**

所以你要在第4行，第三列显示一个字符，那么对应的索引index如下  

	index ＝ 4 * 80 + 3 = 323

然后我们可以通过这样的C代码将数据写入内存
	
	short* where = (short*)0xB8000 + 323;
	*where = character | (attribute << 8);

where是我们的内存地址，character是字符的ascii码，attribute是字符的属性，也就是表示颜色的8位数。attribute左移8位后和character`或操作`便成了我们需要的16位的值。

**注意：这里的`0xB8000`已经转换成了short类型的指针，所以每加1是移动两个内存单元，所以323对应的是646，地址前进了646个字节。这也是指针方便的地方**

好了，接下来，我们专门建立一个`scrn.c`源文件，来处理跟屏幕显示有关的操作。

{% highlight c linenos%}

/*scrn.c
* 把`system.h`include进来,这样可以使用那些工具函数
*/

#include <system.h>

/*先定义下我们的字符属性,然后是行和列*/
PRIVATE u16 *textmemptr;
PRIVATE int attrib = 0x0F; //黑底白字
PRIVATE int column = 0, row = 0; //列和行，主要用来定位屏幕的位置。这样可以移动光标到特定位置。

PRIVATE void move_csr();
PRIVATE void scroll();


//先看一个简单的清屏函数
/* Clears the screen */
PUBLIC void cls()
{
    u16 blank;
    int i;
    /* ascii为0x20的空格符，因为空格符看不见，所以起了清屏作用*/
    blank = 0x20 | (attrib << 8);

    for(i = 0; i < 25; i++)
        memsetw (textmemptr + i * 80, blank, 80);

    /* 修改列和行*/
    column = 0;
    row = 0;
    
    move_csr();//移动光标
}

/* 具体操作端口更新光标位置，就是一闪一闪的短的白色下滑线*/
PRIVATE void move_csr(void)
{
   int temp;

    /*用公式算出索引*/
    temp = row * 80 + column;

	/*以下4句，对显卡接口的寄存器进行输出，达到移动光标的作用*/
    outportb(0x3D4, 14);
    outportb(0x3D5, temp >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, temp);
}


/*用于滚屏的函数*/

PRIVATE void scroll()
{
    u16 blank;
    int temp;

    blank = 0x20 | (attrib << 8);
    if(row >= 25)
    {
        temp = row - 25 + 1;
        memcpy (textmemptr, textmemptr + temp * 80, (25 - temp) * 80 * 2);
        memsetw (textmemptr + (25 - temp) * 80, blank, 80);
        row = 25 - 1;
    }
}

/* 输出单个字符*/
PUBLIC void putch(unsigned char c)
{
    u16 *where;
    u16 att = attrib << 8;

    /* 处理退格键*/
    if(c == 0x08)
    {
        if(column != 0) column--;
    }
    /* 处理tab键*/
    else if(c == 0x09)
    {
        column = (column + 8) & ~(8 - 1);
    }
    /* 处理回显示键*/
    else if(c == '\r')
    {
        column = 0;
    }
    /* 这里处理换行符，这里得到换行符的处理时既换行，光标也移动到开头*/
    else if(c == '\n')
    {
        column = 0;
        row++;
    }
    /* 大于等于空格符的ascii码都要被显示。显示的索引用上面介绍的方法找到内存对应的位置 */
    else if(c >= ' ')
    {
        where = textmemptr + (row * 80 + column);
        *where = c | att;	
        column++;
    }

    /* 如果游标超出了一行，就移动到下一行，并且移到最头上*/
    if(column >= 80)
    {
        column = 0;
        row++;
    }

    /*滚屏，如果需要的话，滚屏函数里会判断是否需要*/
    scroll();
    /*执行移动游标*/
    move_csr();
}

/* 初始化屏幕。即在main函数中调用 */
PUBLIC void init_video()
{
    textmemptr = (u16 *)0xB8000;
    cls();
}


/* 输出字符串 */
PUBLIC void puts(const char *text)
{
    int i;

    for (i = 0; i < strlen(text); i++)
    {
        putch(text[i]);
    }
}

/*最后，我们再添加一个可以修改颜色的函数*/
PUBLIC void settextcolor(u8 forecolor, u16 backcolor)
{
    attrib = (backcolor << 4) | (forecolor & 0x0F);
}

{% endhighlight %}


##具体操作
---
好，分析完了之后我们来具体操作。


除了上面在`source`目录建立了`scrn.c`文件以外，我们别忘记建立`scrn.h`文件，把函数的生命和变量的声明都放在里面。

{% highlight c linenos%}
/*scrn.h*/
include <type.h>
#ifndef __SCRN_H
#define __SCRN_H

PUBLIC void cls();
PUBLIC void putch(unsigned char c);
PUBLIC void init_video();
PUBLIC void puts(const char * text);
PUBLIC void settextcolor(u8 forecolor,u8 backcolor);
#endif

{% endhighlight %}

然后我们要在makefile添加关于`scrn.c`的编译

添加如下两行
{% highlight make linenos%}
scrn.o:
        $(compiler) $(options) scrn.o $(source)/scrn.c
{% endhighlight %}

并在objects变量里加上`scrn.o`

	objects=start.o system.o  scrn.o main.o
	
最后为了显示出效果，我们在main函数先调用`init_video`函数初始化屏幕，也就是清屏。然后用`puts`函数输出两行helloword。并用for循环停着

{% highlight c linenos%}

#include <scrn.h>

void main()
{
        init_video();//初始化
        //按国际惯例输出helloword
        puts("hello world!\n");
        puts("hello world!\n");
    for (;;);
}
{% endhighlight %}

最后执行build.sh建立出iso镜像，从虚拟机中取出。用virtualbox运行，这里就不再多说了。怎么样是不是看见如下的情景

![hello](http://mykernel.qiniudn.com/day04_helloworld.png)

怎么样。是不是特别有成就感?!

##注意提醒
---
- 在按位操作的过程中，变量的有无符号类型，其实并不太重要。主要关心变量类型所占的空间，也就是位数。
- 有关键字`PRIVATE`修饰的变量和函数是**静态**的，不能被对应的**.c文件之外**的文件里调用。所以**声明和定义不要放在头文件里**，放在**.c**文件里。虽然不能被其他文件调用，但因为这些变量和函数会被**自己所在的.c文件使用**，所以把这些变量和函数的`声明`放在.c文件**最前面**,遵循C语言的先声明后调用原则，而`定义`则可以在.c文件的任意位置。当然声明变量的时候也可以直接赋值。***没有赋值，默认为0***
- 如果你想消除编译时候main函数因返回值不会int消除的警告，那么你就将返回值改成int，并`return 0;`Init