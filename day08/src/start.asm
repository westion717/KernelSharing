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
; 每一个标号为`isr＋数字`的标号都代表对应中断号的服务子程序的入口地址
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31
global isr32
global isr33
global isr34
global isr35
global isr36
global isr37
global isr38
global isr39
global isr40
global isr41
global isr42
global isr43
global isr44
global isr45
global isr46
global isr47

;  0: Divide By Zero Exception
isr0:
    cli            ; 关中断，尽管CPU跳转到中断程序之后会自动关闭
    push byte 0    ; 人为push一个错误码，
                   ; 以便于栈内数据分布统一
    push byte 0    ; push一个中断号便于知道是几号中断
    jmp isr_common_stub

;  1: Debug Exception
isr1:
    cli
    push byte 0
    push byte 1
    jmp isr_common_stub
    
;  2: Non Maskable Interrupt Exception
isr2:
    cli
    push byte 0
    push byte 2
    jmp isr_common_stub

;  3: Int 3 Exception
isr3:
    cli
    push byte 0
    push byte 3
    jmp isr_common_stub

;  4: INTO Exception
isr4:
    cli
    push byte 0
    push byte 4
    jmp isr_common_stub

;  5: Out of Bounds Exception
isr5:
    cli
    push byte 0
    push byte 5
    jmp isr_common_stub

;  6: Invalid Opcode Exception
isr6:
    cli
    push byte 0
    push byte 6
    jmp isr_common_stub

;  7: Coprocessor Not Available Exception
isr7:
    cli
    push byte 0
    push byte 7
    jmp isr_common_stub



;  8: Double Fault Exception (With Error Code!)
isr8:
    cli
    push byte 8       ; 这个8号中断不需要人为push一个错误码
    					; 因为本身就有我们只需要push一个中断号
    jmp isr_common_stub

;  9: Coprocessor Segment Overrun Exception
isr9:
    cli
    push byte 0
    push byte 9
    jmp isr_common_stub

; 10: Bad TSS Exception (With Error Code!)
isr10:
    cli
    push byte 10
    jmp isr_common_stub

; 11: Segment Not Present Exception (With Error Code!)
isr11:
    cli
    push byte 11
    jmp isr_common_stub

; 12: Stack Fault Exception (With Error Code!)
isr12:
    cli
    push byte 12
    jmp isr_common_stub

; 13: General Protection Fault Exception (With Error Code!)
isr13:
    cli
    push byte 13
    jmp isr_common_stub

; 14: Page Fault Exception (With Error Code!)
isr14:
    cli
    push byte 14
    jmp isr_common_stub

; 15: Reserved Exception
isr15:
    cli
    push byte 0
    push byte 15
    jmp isr_common_stub

; 16: Floating Point Exception
isr16:
    cli
    push byte 0
    push byte 16
    jmp isr_common_stub

; 17: Alignment Check Exception
isr17:
    cli
    push byte 0
    push byte 17
    jmp isr_common_stub

; 18: Machine Check Exception
isr18:
    cli
    push byte 0
    push byte 18
    jmp isr_common_stub

; 19: Reserved
isr19:
    cli
    push byte 0
    push byte 19
    jmp isr_common_stub

; 20: Reserved
isr20:
    cli
    push byte 0
    push byte 20
    jmp isr_common_stub

; 21: Reserved
isr21:
    cli
    push byte 0
    push byte 21
    jmp isr_common_stub

; 22: Reserved
isr22:
    cli
    push byte 0
    push byte 22
    jmp isr_common_stub

; 23: Reserved
isr23:
    cli
    push byte 0
    push byte 23
    jmp isr_common_stub

; 24: Reserved
isr24:
    cli
    push byte 0
    push byte 24
    jmp isr_common_stub

; 25: Reserved
isr25:
    cli
    push byte 0
    push byte 25
    jmp isr_common_stub

; 26: Reserved
isr26:
    cli
    push byte 0
    push byte 26
    jmp isr_common_stub

; 27: Reserved
isr27:
    cli
    push byte 0
    push byte 27
    jmp isr_common_stub

; 28: Reserved
isr28:
    cli
    push byte 0
    push byte 28
    jmp isr_common_stub

; 29: Reserved
isr29:
    cli
    push byte 0
    push byte 29
    jmp isr_common_stub

; 30: Reserved
isr30:
    cli
    push byte 0
    push byte 30
    jmp isr_common_stub

; 31: Reserved
isr31:
    cli
    push byte 0
    push byte 31
    jmp isr_common_stub

; IR0
isr32:
    cli
    push byte 0
    push byte 32
    jmp isr_common_stub

; IR1
isr33:
    cli
    push byte 0
    push byte 33
    jmp isr_common_stub

; IR2
isr34:
    cli
    push byte 0
    push byte 34
    jmp isr_common_stub

; IR3
isr35:
    cli
    push byte 0
    push byte 35
    jmp isr_common_stub

; IR4
isr36:
    cli
    push byte 0
    push byte 36
    jmp isr_common_stub

; IR5
isr37:
    cli
    push byte 0
    push byte 37
    jmp isr_common_stub

; IR6
isr38:
    cli
    push byte 0
    push byte 38
    jmp isr_common_stub

; IR7
isr39:
    cli
    push byte 0
    push byte 39
    jmp isr_common_stub

; IR8
isr40:
    cli
    push byte 0
    push byte 40
    jmp isr_common_stub

; IR9
isr41:
    cli
    push byte 0
    push byte 41
    jmp isr_common_stub

; IR10
isr42:
    cli
    push byte 0
    push byte 42
    jmp isr_common_stub

; IR11
isr43:
    cli
    push byte 0
    push byte 43
    jmp isr_common_stub

; IR12
isr44:
    cli
    push byte 0
    push byte 44
    jmp isr_common_stub

; IR13
isr45:
    cli
    push byte 0
    push byte 45
    jmp isr_common_stub

; IR14
isr46:
    cli
    push byte 0
    push byte 46
    jmp isr_common_stub

; IR15
isr47:
    cli
    push byte 0
    push byte 47
    jmp isr_common_stub

; 下面的代码要保存中断时刻的上下文（各个寄存器的值），然后把各个段寄存器切换到ring0，内核级别的数据段来
; 然后调用C中的一个函数来处理异常
; 最后复原保存时候的上下文

extern isr_handler
isr_common_stub:
    pusha
    push  ds
    push  es
    push  fs
    push  gs
    mov ax, 0x10   ; 中断之后程序从用户态已经转到内核的代码
    mov ds, ax	     ; 当然数据段也要用内核的数据段
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   
    push eax		  ; 作为C中处理异常函数的参数，这样那个函数可以通过esp的值作为指针找到前面保存的寄存器的值还有错误码等
    mov eax, isr_handler
    call eax       ;
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; 删除错误码和中断号，每个4字节
    iret           ; 中断返回。将出栈5个值CS, EIP, EFLAGS, SS, and ESP!




; 这里定义了BSS段，用作存储数据的栈。记住栈是从低地址到高地址的
SECTION .bss
    resb 8192 ; 从低到高向下走了8KB的空间
_sys_stack:  ; 所以这里的'_sys_stack'标记的地址就是现在的栈顶的位置
