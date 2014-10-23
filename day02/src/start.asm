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
    jmp $


; 之后将在这里加入初始化程序


; 之后将在这里加入中断服务程序的代码



; 这里定义了BSS段，用作存储数据的栈。记住栈是从低地址到高地址的
SECTION .bss
    resb 8192 ; 从低到高向下走了8KB的空间
_sys_stack:  ; 所以这里的'_sys_stack'标记的地址就是现在的栈顶的位置
