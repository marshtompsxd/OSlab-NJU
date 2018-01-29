.code16

.global start
start:

	cli

	xorw %ax, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss

	inb $0x92, %al
	orb $0x02, %al
	outb %al, $0x92

	lgdt gdtDesc

	movl %cr0, %eax
	orb $0x01, %al
	movl %eax, %cr0

	ljmp $0x08, $start32


.code32
start32:

	# initialize ds, es, ss, fs, gs and esp
	movw $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw %ax, %fs

	movw $0x18, %ax
	movw %ax, %gs

	movl $0x8000, %esp

    # jump to bootMain in boot.c
	jmp bootMain

.p2align 2
gdt: # 8 bytes for each table entry, at least 1 entry
	.word 0,0 # empty entry
	.byte 0,0,0,0

	.word 0xffff,0 # code stack description
	.byte 0,0x9a,0xcf,0

	.word 0xffff,0 # data stack description
	.byte 0,0x92,0xcf,0

	.word 0xffff,0x8000 # display stack desc
	.byte 0x0b,0x92,0xcf,0


gdtDesc: # 6 bytes in total
	.word (gdtDesc - gdt -1)
	.long gdt
