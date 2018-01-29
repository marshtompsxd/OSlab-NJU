#include "common.h"
#include "x86.h"
#include "device.h"

void kEntry(void) {

	
	disableInterrupt();
	initSerial(); // initialize serial port
	initIdt(); // initialize idt
	initIntr(); // iniialize 8259a
	initTimer(); // initialize timer
	initSeg(); // initialize gdt, tss
	initPcb(); // initialize pcb
	initScreen(); // initialize screen
	
	loadFS();
	initFS();
	loadUMain(); // load user program, enter user space

	int idle_esp = (int)(idle.stack) + MAX_STACK_SIZE*4 + sizeof(struct TrapFrame) - 2*4;
	asm volatile("movl %%eax, %%esp"::"a"(idle_esp));
    enableInterrupt();
    while(1) {
		waitForInterrupt();
    }
	assert(0);
}
