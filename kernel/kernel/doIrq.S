.code32

#----|-----entry------|-errorcode-|-----id-----|---handler---|
.globl vec0;    vec0:   pushl $0;  pushl    $0; jmp asmDoIrq
.globl vec1;    vec1:   pushl $0;  pushl    $1; jmp asmDoIrq
.globl vec2;    vec2:   pushl $0;  pushl    $2; jmp asmDoIrq
.globl vec3;    vec3:   pushl $0;  pushl    $3; jmp asmDoIrq
.globl vec4;    vec4:   pushl $0;  pushl    $4; jmp asmDoIrq
.globl vec5;    vec5:   pushl $0;  pushl    $5; jmp asmDoIrq
.globl vec6;    vec6:   pushl $0;  pushl    $6; jmp asmDoIrq
.globl vec7;    vec7:   pushl $0;  pushl    $7; jmp asmDoIrq
.globl vec8;    vec8:              pushl    $8; jmp asmDoIrq
.globl vec9;    vec9:   pushl $0;  pushl    $9; jmp asmDoIrq
.globl vec10;   vec10:             pushl   $10; jmp asmDoIrq
.globl vec11;   vec11:             pushl   $11; jmp asmDoIrq
.globl vec12;   vec12:             pushl   $12; jmp asmDoIrq
.globl vec13;   vec13:             pushl   $13; jmp asmDoIrq
.globl vec14;   vec14:             pushl   $14; jmp asmDoIrq


.global irqEmpty
irqEmpty:
	cli
	pushl $0 // push dummy error code
	pushl $-1 // push interrupt vector into kernel
	jmp asmDoIrq

.global irqGProtectFault
irqGProtectFault:
	cli
	pushl $0
	pushl $0xd
	jmp asmDoIrq

.global irqSyscall
irqSyscall:
	cli
	pushl $0 // push dummy error code
	pushl $0x80 // push interrupt vector into kernel stack
	jmp asmDoIrq

.global irq0
irq0:
	cli
	pushl $0 // push dummy error code
	pushl $0x20 // push interrupt vector into kernel stack
	jmp asmDoIrq

.global irq1
irq1:
	cli
	pushl $0 // push dummy error code
	pushl $0x21 // push interrupt vector into kernel stack
	jmp asmDoIrq

.global asmDoIrq
asmDoIrq:
	pushal // push process state into kernel stack
	
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs

	movw $16,%ax
	movw %ax,%ds
	movw %ax,%es
	movw %ax,%fs

	pushl %esp
	call irqHandle
	addl $4, %esp

	movl currentstacktop, %esp

	popl %gs
	popl %fs
	popl %es
	popl %ds

	popal

	addl $4, %esp //interrupt vector is on top of kernel stack
	addl $4, %esp //error code is on top of kernel stack
	iret
