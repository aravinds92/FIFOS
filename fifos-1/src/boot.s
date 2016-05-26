    .globl stack
	.bss
	.align 0x1000
	.comm 	stack, 0x1000
	.text	
	.global context_init
	.type context_init, @function
	.global func_exe 
	.type func_exe, @function
	.global yield 
	.type yield, @function
	.globl _start 
	 
_start:
        jmp first

        /* Multiboot header -- Safe to place this header in 1st page of memory for GRUB */
        .align 4
        .long 0x1BADB002 /* Multiboot magic number */
        .long 0x00000003 /* Align modules to 4KB, req. mem size */
                         /* See 'info multiboot' for further info */
        .long 0xE4524FFB /* Checksum */
 
gdt_ptr: 
	.short 0x7FF
	.long gdt

gdt:
	.long 0
	.long 0
	.long 0xFFFF
	.long 0x00CF9A00
	.long 0x0000FFFF
	.long 0x00CF9200
	         
first:
	movl $stack+0x1000, %esp
	pushl %ebx
	call mem
	popl %ebx
	call gdt_init
	hlt
	
gdt_init:
    lgdt gdt_ptr       
    mov $0x10, %ax
    movw %ax, %es
	movw %ax, %ds
	movw %ax, %fs
    movw %ax, %ss
    movw %ax, %gs        
    ljmp $0x08, $code  
    
code:
	call gdt_done
	call scheduler_init

loop:
	jmp loop
	
	
context_init:
	movl %esp, %edx
	popl %eax						#return address
	popl %eax						#Stack pointer
	popl %ebx						#function pointer
	movl %eax, %esp	
	pushf							#pushing the flags
	pushl %cs						#push code segment value
	pushl %ebx						#pushing function address at the new calling fucntion
	pushal							#pushing the registers
	pushw %ds						#pushing the segment registers 
	pushw %es
	pushw %ss
	pushw %gs
	pushw %fs
	movl %esp, %eax		#return new stack pointer value to the calling function
	movl %edx, %esp		#restore stack to calling functions's stack
	ret			
	
func_exe:
	popl %eax			#return address
	popl %ebx			#stack pointer value
	movl %esp, scheduler_sp	#save main stack ptr
	movl %ebx, %esp		#stack = new stack pointer
	popw %fs			#pop the segment registers
	popw %gs
	popw %ss
	popw %es
	popw %ds	
	popal
	iret

yield:
	popl %ebx
	pushf			#pushing the flags
	pushl %cs		#push code segment value
	pushl %ebx		#pushing function address at the new calling fucntion
	pushal			#pushing the registers
	pushw %ds		#pushing the segment registers 
	pushw %es
	pushw %ss
	pushw %gs
	pushw %fs
	movl %esp, %eax
	movl scheduler_sp, %esp
	pushl %eax
	call detour	
	hlt

.section .data
scheduler_sp: .long 0x00
