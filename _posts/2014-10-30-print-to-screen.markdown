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

好了，接下来，我们专门建立一个`scrn.c`源文件，来处理跟屏幕显 有关的操作。

{% highlight c linenos%}

/*scrn.c
* 把`system.h`include进来,这样可以使用那些工具函数
*/

#include <system.h>

/*先定义下我们的字符属性,然后是行和列*/
STATIC unsigned short *textmemptr;
STATIC int attrib = 0x0F; //黑底白字
STATIC int column = 0, row = 0; //列和行，主要用来定位屏幕的位置。这样可以移动光标到特定位置。

//先看一个简单的清屏函数
/* Clears the screen */
PUBLIC void cls()
{
    short blank;
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

/* 更新光标位置，就是一闪一闪的短的白色下滑线*/
PUBLIC void move_csr(void)
{
    unsigned short temp;

    /*用公式算出索引*/
    temp = row * 80 + column;

	/*以下4句，对显卡接口的寄存器进行输出，达到移动光标的作用*/
    outportb(0x3D4, 14);
    outportb(0x3D5, temp >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, temp);
}

{% endhighlight %}

##具体操作

##注意提醒
---
