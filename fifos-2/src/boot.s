	.globl stack
	.bss
	.align 0x1000
	.comm 	stack, 0x1000
	.comm idt_start, 0x1000
	.text	
	.global context_init
	.type context_init, @function
	.global func_exe 
	.type context_switch, @function
	.global context_switch
	.type eoi_reset, @function
	.global eoi_reset
	.type save_stack, @function
	.global save_stack
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

idt_ptr:
	.short 0x7FF
	.long idt_start


idt_init: 
	movl %cs, %edx			#dx --> code segment
	popl %ecx				#return adrress --> temp		
	movl %ecx, temp
	movl $(idt_start), %ecx	
	addl $256, %ecx
idt_loop:	
	movl $(ISR_8253), %eax	#interrupt handler in EAX
	andl $0xFFFF, %eax	    #extracting the lower 16-bits of the segment handler		
	shld $16, %edx, %edx	#getting higher byte for
	orl %edx, %eax			#lower s  
	movl %eax, (%ecx)		#move the lower 4 bytes to idt_start
	addl $0x04, %ecx
	movl $0x00008E00, %edx
	movl $(ISR_8253), %eax	#interrupt handler in EAX
	andl $0xFFFF0000, %eax	#extracting the upper 16-bits of the segment handler	
	orl %eax, %edx			#oring upper 16 bits and 8E00
	movl %edx, (%ecx)
	addl $0x04, %ecx
	lidt idt_ptr
	pushl temp 
	ret
	
	
pic_init:
	mov $0x11, %al		#ICW1(master)
	outb %al, $0x20		#Send ICW1 to the master
	mov $0x13, %al		#ICW1(slave)
	outb %al, $0xA0		#send ICW1 to the slave		
	mov $0x20, %al		#ICW2(master)
	outb %al, $0x21		#Send ICW2 to the Master
	mov $0x28, %al		#ICW2(slave)
	outb %al, $0xA1		#Send ICW2 to the Slave
	mov $0x4, %al		#ICW3(master)
	outb %al, $0x21		#sending the ICW3 to the master
	mov 0x02, %al		#ICW3(slave)
	outb %al, $0xA1		#sending to the slave
	mov $0x0D, %al		#ICW4
	outb %al, $0x21		#Move that value to the master
	outb %al, $0xA1		#Move the value to the slave send 0x09 
	ret


pit_init:
	mov $0x34, %al
	outb %al, $0x43	
	mov $65535 , %ax      # 1193181 / 100
	outb %al, $0x40
	xchg %ah, %al
	outb %al, $0x40
	ret	

code:
	call gdt_done
	call idt_init
	call pic_init
	call pit_init
	call scheduler_init
	sti
code_loop:
	jmp code_loop
	
	
context_init:
	movl %esp, %edx
	popl %eax					#return address
	popl %eax					#Stack pointer
	popl %ebx					#function pointer
	movl %eax, %esp	
	pushf						#pushing the flags
	pushw %cs					#push code segment value
	pushl %ebx					#pushing function address at the new calling fucntion
	pushal						#pushing the registers
	pushw %ds					#pushing the segment registers 
	pushw %es
	pushw %ss
	pushw %gs
	pushw %fs
	movl %esp, %eax		#return new stack pointer value to the calling function
	movl %edx, %esp		#restore stack to calling functions's stack
	ret			

ISR_8253:
	cli
	pushal						#pushing the registers
	pushw %ds					#pushing the segment registers 
	pushw %es
	pushw %ss
	pushw %gs
	pushw %fs
	call scheduler				#call
	popw %fs					#pop the segment registers
	popw %gs
	popw %ss
	popw %es
	popw %ds	
	popal
eoi_reset:
	mov $0x20, %al
	outb %al, $0x20			#Reset EOI
	sti
	nop
	ret
	
	
context_switch:
	popl %eax						#return address of the scheduler_return
	popl %ebx						#popping stack pointer of the thread to switch to
	#popl %ecx						#popping ISR's return address
	movl %ebx, %esp					#switching to thread's stack
	popw %fs						#pop the segment registers
	popw %gs
	popw %ss
	popw %es
	popw %ds	
	popal
	iret		

save_stack:
	popl %eax						#return address of the scheduler
	popl %ebx 						#return address of the ISR
	movl %eax, temp1 
	movl %esp, %eax					#stack_ptr to save
	movl $1000, %esp
	pushl temp1
	ret
	
.section .data
scheduler_sp: .long 0x00
sidt_loc: .long 0x01
scheduler_return: .long 0x00
thread_sp: .long 0x00
isr_return: .long 0x00
temp: .long 0x00
temp1: .long 0x00
