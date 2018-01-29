#include "x86.h"
#include "device.h"
#include "common.h"


void exit_aux(){
	current->state = DEAD;
	p_num--;
	writeBack();
	schedule();
}

void fork_aux(){
	list_delete(free_list, &pcb[p_num]);
	pcb[p_num].fp_num = current->fp_num;

	int i;
	for(i=0;i<MAX_FP_NUM;i++)
	{
		pcb[p_num].filepointer[i] = current->filepointer[i];
		if(current->filepointer[i]!= -1)
		{
			int sys_file_index = current->filepointer[i];
			sysFileTable[sys_file_index].link_num++;
		}
	}

	pcb[p_num].pid = p_num;
	pcb[p_num].state = RUNNABLE;
	pcb[p_num].timeCount = 10;
	pcb[p_num].sleepTime = 0;
	pcb[p_num].psem = current->psem;
	pcb[p_num].ppid = current->pid;

	pcb[p_num].tf.eax = 0;
	pcb[p_num].tf.ebx = current->tf.ebx;
	pcb[p_num].tf.ecx = current->tf.ecx;
	pcb[p_num].tf.edx = current->tf.edx;
	pcb[p_num].tf.ebp = current->tf.ebp;
	pcb[p_num].tf.edi = current->tf.edi;
	pcb[p_num].tf.esi = current->tf.esi;
	pcb[p_num].tf.xxx = current->tf.xxx;

	pcb[p_num].tf.irq = current->tf.irq;
	pcb[p_num].tf.error = current->tf.error;
	pcb[p_num].tf.eip = current->tf.eip;
	pcb[p_num].tf.eflags = current->tf.eflags;
	pcb[p_num].tf.esp = current->tf.esp;

	pcb[p_num].tf.cs = USEL(SEG_UCODE2);
	pcb[p_num].tf.ds = USEL(SEG_UDATA2);
	pcb[p_num].tf.es = USEL(SEG_UDATA2);
	pcb[p_num].tf.gs = KSEL(SEG_KDISPLAY);
	pcb[p_num].tf.ss = USEL(SEG_UDATA2);
	pcb[p_num].tf.fs = USEL(SEG_UDATA2);

	char* src = (char*)0x200000;
	char* dest = (char*)0x400000;
	
	for(i=0;i<0x200000;i++){
		dest[i] = src[i];	
	}

	current->tf.eax = p_num;
	list_add(runnable_list,&pcb[p_num]);

	p_num++;
}

void sleep_aux(int sleepTime){
	
	current->state = BLOCKED;
	current->sleepTime = sleepTime;
	schedule();
}