;start.asm

[BITS 32]
global start
start: ;开始标记处
    mov esp, _sys_stack ;设置栈指针，指向的地址标记在下面有定义
    jmp stublet ;跳转到下方标记的地址处执行

;这部分作为GRUB启动的接口必须在内存中以4字节对齐
ALIGN 4
mboot:
    ;mboot到stublet之间的代码都是为了定义GRUB加载的接口

    MULTIBOOT_PAGE_ALIGN    equ 1<<0
    MULTIBOOT_MEMORY_INFO   equ 1<<1
    MULTIBOOT_AOUT_KLUDGE   equ 1<<16
    MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
    MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
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
    extern main
    call main
    jmp $


; 以下代码我们更新我们的GDT，使得代码段和数据段等段寄存器都更新为我们自己设置的代码段和数据段
global gdt_flush     ; 导出该函数，以便在C中调用，C中用extern void gdt_flush();
extern gp            ; 引入GDTR结构的变量gp，在C中定义
gdt_flush:
    lgdt [gp]        ; 载入GDT，GDTR内容用gp所指向的内容更新
    mov ax, 0x10      ; 0x10 是数据段的选择子的值，前13位的值是2，索引对应是条目2
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax        ; 除CS其余段寄存器都指向上述数据段
    jmp 0x08:flush2   ; 同理0x08 是代码段选择子，前13位的值是1，索引对应GDT中的条目1
		      ; 这里`选择子:标号`的jmp形式，使用的是绝对地址跳转。因为标号已经是虚拟地址，所以代码段的基址要为0才能定位到正确位置。在C的gdt.c中定义的代码段就是基址为0
flush2:
    ret               ; 返回到C代码中

; 载入idt。idt_load函数将在idt.c被中调用。以下代码可以加在start.asm的加载GDT之后。
global idt_load
extern idtp
idt_load:
    lidt [idtp]
    ret


; 之后将在这里加入中断服务程序的代码



; 这里定义了BSS段，用作存储数据的栈。记住栈是从低地址到高地址的
SECTION .bss
    resb 8192 ; 从低到高向下走了8KB的空间
_sys_stack:  ; 所以这里的'_sys_stack'标记的地址就是现在的栈顶的位置
