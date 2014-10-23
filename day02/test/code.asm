global mytest

extern myprint
mytest:
	push dword 20
	push dword 10
	call myprint
	add esp,8
	ret

