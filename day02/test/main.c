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
