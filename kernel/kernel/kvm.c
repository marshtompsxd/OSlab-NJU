#include "x86.h"
#include "device.h"

SegDesc gdt[NR_SEGMENTS];



static uint8_t buffer[4096]; //restore elf


void initPcb(){

	sys_file_num = 0;
	int i;
	for(i=0;i<MAX_SYS_FILE_NUM;i++)
	{
		sysFileTable[i].inode_index = -1;
	}

	intr_nesting = false;

	dummy1.next = NULL;
	dummy2.next = NULL;
	dummy3.next = NULL;

	runnable_list = &dummy1;
	blocked_list = &dummy2;
	free_list = &dummy3;

	p_num = 1;

	
	for(i = 1;i < MAX_PCB_NUM;i++){
		pcb[i].pid = -1;
		pcb[i].ppid = -1;
		pcb[i].state = DEAD;
		pcb[i].sleepTime = 0;
		pcb[i].timeCount = 0;
		pcb[i].psem = NULL;
		pcb[i].fp_num = 3;

		int j;
		for(j=0;j<MAX_FP_NUM;j++)
		{
			pcb[i].filepointer[j] = -1;
		}

		list_add(free_list, &pcb[i]);
	}
	


	idle.pid = 0;
	idle.state = RUNNING;
	idle.timeCount = 10;
	idle.sleepTime = 0;
	idle.psem = NULL;

	current = &idle;
}

void initSeg() {
	gdt[SEG_KCODE] 	= 	SEG(STA_X | STA_R, 	0,       	0xffffffff, DPL_KERN);
	gdt[SEG_KDATA] 	= 	SEG(STA_W,         	0,       	0xffffffff, DPL_KERN);
	gdt[SEG_UCODE1] = 	SEG(STA_X | STA_R, 	0,       	0xffffffff, DPL_USER);
	gdt[SEG_UDATA1] = 	SEG(STA_W,         	0,       	0xffffffff, DPL_USER);
	gdt[SEG_UCODE2] = 	SEG(STA_X | STA_R, 	0x200000,   0xffffffff, DPL_USER);
	gdt[SEG_UDATA2] = 	SEG(STA_W,         	0x200000,   0xffffffff, DPL_USER);
	gdt[SEG_TSS] 	= 	SEG16(STS_T32A,     &tss, 		sizeof(TSS)-1, DPL_KERN);
	gdt[SEG_TSS].s 	= 	0;
	gdt[SEG_KDISPLAY] = SEG(STA_W,			0xB8000,	0xffffffff,	DPL_USER);
	setGdt(gdt, sizeof(gdt));

	/*
	 * init TSS
	 */

	
	tss.esp0 = (int)(pcb[p_num].stack) + MAX_STACK_SIZE*4 + sizeof(struct TrapFrame);
	tss.ss0 = KSEL(SEG_KDATA);
	asm volatile("ltr %%ax":: "a" (KSEL(SEG_TSS)));

	/*set segment regs*/
	asm volatile("movw %%ax,%%es":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ds":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%ss":: "a" (KSEL(SEG_KDATA)));
	asm volatile("movw %%ax,%%fs":: "a" (KSEL(SEG_KDATA)));

	asm volatile("movw %%ax,%%gs":: "a" (KSEL(SEG_KDISPLAY)));

	lLdt(0);
	
}

void initScreen(){
	buf_in = buf_out = 0;
	vm_addr = (uint16_t *)0xB8000;
	screen_clear();
}

void enterUserSpace(uint32_t entry) {
	/*
	 * Before enter user space 
	 * you should set the right segment registers here
	 * and use 'iret' to jump to ring3
	 */	
	asm volatile("pushl %0"::"a"(USEL(SEG_UDATA1)));			//ss
	asm volatile("pushl $0x300000");							//esp

    asm volatile("movw %%ax,%%gs":: "a" (KSEL(SEG_KDISPLAY))); 	//gs
    asm volatile("movw %%ax,%%fs":: "a" (USEL(SEG_UDATA1))); 	//fs
    asm volatile("movw %%ax,%%ds":: "a" (USEL(SEG_UDATA1))); 	//ds
    asm volatile("movw %%ax,%%es":: "a" (USEL(SEG_UDATA1))); 	//es
 
	asm volatile("pushl $0x202");								//eflags
	asm volatile("pushl %0"::"a"(USEL(SEG_UCODE1)));			//cs
	asm volatile("pushl %0"::"a"(entry));						//eip
	asm volatile("iret");
}

void mem_set(uint8_t* beg, uint8_t* end, uint8_t bt){
	uint8_t* temp;
	for(temp = beg; temp < end; temp++){
		*temp = bt;
	}
}

void preparePCB(uint32_t entry){
	list_delete(free_list, &pcb[p_num]);
	
	pcb[p_num].pid = p_num;
	pcb[p_num].ppid = -1;
	pcb[p_num].state = RUNNABLE;
	pcb[p_num].timeCount = 10;
	pcb[p_num].sleepTime = 0;
	pcb[p_num].tf.eip = entry;
	pcb[p_num].tf.eflags = 0x202;
	pcb[p_num].tf.esp = 0x300000;
	pcb[p_num].tf.cs = USEL(SEG_UCODE1);
	pcb[p_num].tf.ds = USEL(SEG_UDATA1);
	pcb[p_num].tf.es = USEL(SEG_UDATA1);
	pcb[p_num].tf.ss = USEL(SEG_UDATA1);
	pcb[p_num].tf.fs = USEL(SEG_UDATA1);
	pcb[p_num].tf.gs = KSEL(SEG_KDISPLAY);
	list_add(runnable_list, &pcb[p_num]);

	PCB* p = list_head(runnable_list);
	assert(p->pid == 1);

	p_num++;	
}

void loadUMain(void) {
	printk("loading user app from disk...\n");
	/*load app to the memory*/
	struct ELFHeader *elf;
	struct ProgramHeader *ph, *ph_end;
	uint8_t *paddr;

	elf = (struct ELFHeader*)buffer;
	readBytes((uint8_t*)elf, USER_OFFSET_IN_DISK, 4096);
	ph = (struct ProgramHeader*)((uint8_t*)elf + elf->phoff);

	for(ph_end = ph + elf->phnum; ph < ph_end; ph++){
		if(ph->type == PT_LOAD){
			paddr = (uint8_t*)(ph->paddr);
			readBytes(paddr, USER_OFFSET_IN_DISK + ph->off, ph->filesz);
			mem_set(paddr + ph->filesz, paddr + ph->memsz, 0);
		}
	}
	
	preparePCB(elf->entry);
}

void loadFS(void) {
	fs_addr = (uint8_t*)0x01000000;
	mem_set(fs_addr, fs_addr + FS_SIZE_MB*1024*1024, 0);	
	printk("loading fs from disk...\n");
	readBytes(fs_addr, FS_OFFSET_IN_DISK, FS_SIZE_MB*1024*1024);
}
