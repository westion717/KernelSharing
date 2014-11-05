//把`system.h`include进来,这样可以使用那些工具函数
#include <system.h>
#include <scrn.h>

PRIVATE u16 *textmemptr;
PRIVATE int attrib = 0x0F; //黑底白字
PRIVATE int column = 0, row = 0; //列和行，主要用来定位屏幕的位置。这样可以移动光标到特定位置。
PRIVATE void move_csr();
PRIVATE void scroll();

/* 初始化屏幕。即在main函数中调用  */
PUBLIC void init_video()
{
	textmemptr = (u16 *)0xB8000;
	cls();
}

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

/* 更新光标位置，就是一闪一闪的短的白色下滑线*/
PRIVATE void move_csr()
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

/* 输出字符串 */
PUBLIC void puts(const char *text)
{
	int i;

	for (i = 0; i < strlen(text); i++)
	{
		putch(text[i]);
	}
}

/*修改字符颜色*/
PUBLIC void settextcolor(u8 forecolor, u8 backcolor)
{
	attrib = (backcolor << 4) | (forecolor & 0x0F);
}

