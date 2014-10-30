//把`system.h`include进来,这样可以使用那些工具函数
#include <system.h>
#include <scrn.h>
/*先定义下我们的字符属性,然后是行和列*/
PRIVATE unsigned short *textmemptr;
PRIVATE int attrib = 0x0F; //黑底白字
PRIVATE int column = 0, row = 0; //列和行，主要用来定位屏幕的位置。这样可以移动光标到特定位置。

//先看一个简单的清屏函数
/* Clears the screen */
PUBLIC void cls()
{
	short blank;
	int i;
	textmemptr=(unsigned short*)0xB8000;
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
PUBLIC void move_csr()
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
