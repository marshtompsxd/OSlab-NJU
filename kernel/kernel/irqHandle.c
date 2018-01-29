#include "x86.h"
#include "device.h"
#include "common.h"

#define NR_exit			1
#define NR_fork			2
#define NR_read 		3
#define NR_write 		4
#define NR_open 		5
#define NR_close 		6

#define NR_sem_init		7
#define NR_sem_destroy	8
#define NR_sem_wait		9
#define NR_sem_post		10

#define NR_seek			11
#define NR_remove		12
#define NR_ls 			13
#define NR_cat			14

#define NR_sleep		162

#define SEEK_SET 		0
#define SEEK_CUR 		1
#define SEEK_END 		2


void irqHandle(struct TrapFrame *tf) {
	/*
	 * intr handlers
	 */

	disableInterrupt();	

	switch(tf->irq) {
		case -1:break;
		case 0xd: GProtectFaultHandle(tf);break;
		case 0x20:timerHandle(tf);break;
		case 0x21:keyboardHandle(tf);break;
		case 0x80:syscallHandle(tf);break;
		default:assert(0);
	}
	if(!intr_nesting){
		currentstacktop = (int)(current->stack) + MAX_STACK_SIZE*4;
		assert(currentstacktop == (int)(&(current->tf)));
	}
	else{
		currentstacktop = (int)tf;
	}	
}

int getAvlFilepointer()
{
	int i;
	for(i=3;i<MAX_FP_NUM;i++)
	{
		if(current->filepointer[i] == -1)return i;
	}
	return -1;
}

int getAvlSystemFile()
{
	int i;
	for(i=0;i<MAX_SYS_FILE_NUM;i++)
	{
		if(sysFileTable[i].inode_index == -1)return i;
	}
	return -1;
}

void ls_aux(char* path){
	int index = findCurrentDirPos(path);
	int i;
	int cont = 2;
	for(i=2;cont<fs_inodepointer[index].filenum;i++)
	{
		if(strcmp(fs_inodepointer[index].dirdata[i].filename, "\0")!=0)
		{
			printQ("%s\t", fs_inodepointer[index].dirdata[i].filename);
			cont++;
		}
		
	}
	printQ("\n");
}

void cat_aux(char* path){
	int index = findCurrentFLPos(path);
	
	int i;
	for(i = 0;i<fs_inodepointer[index].filesz;i++)
	{
		printQ("%c", fs_inodepointer[index].datablock[i]);
	}
	printQ("\n");
}

void open_aux(char *path, int flags){
	if(!isFLExist(path)){
		//printk("path is %s", path);
		makeFL(path, NULL);
		//printk("make file\n");
	}

	int index = findCurrentFLPos(path);
	int sys_file_index = getAvlSystemFile();
	int fp_index = getAvlFilepointer();

	if(sys_file_index == -1 || fp_index == -1)assert(0);

	sysFileTable[sys_file_index].inode_index = index;
	sysFileTable[sys_file_index].link_num = 1;
	sysFileTable[sys_file_index].offset = 0;

	current->filepointer[fp_index] = sys_file_index;
	current->tf.eax = fp_index;

	//printk("sys file index is %d and fd is %d\n", sys_file_index, fp_index);
	sys_file_num++;
	current->fp_num++;
}

void write_aux(int fd, char* buf, int len){
	if(current->pid == 2){
		buf = (char *)((int)buf + Child_Data_Offst);
	}
	if(fd == stdout){
		int i;
		for(i=0; i<len; i++){
			uint8_t ch = buf[i];
			screen_put(ch, default_forecolor, default_backcolor);
		}
	}
	else{
		//printk("fd is %d len is %d\n", fd, len);
		//printk("the offset is %d\n", sysFileTable[current->filepointer[fd]].offset);
		int index = sysFileTable[current->filepointer[fd]].inode_index;
		int offset = sysFileTable[current->filepointer[fd]].offset;
		
		current->tf.eax = writeF(index, buf, len, offset);
		
		sysFileTable[current->filepointer[fd]].offset += len;
		//printk("the offset is %d\n", sysFileTable[current->filepointer[fd]].offset);
	}
}

void close_aux(int fd){
	int sys_file_index = current->filepointer[fd];
	sysFileTable[sys_file_index].link_num--;
	if(sysFileTable[sys_file_index].link_num == 0)
	{
		sysFileTable[sys_file_index].inode_index = -1;
		sysFileTable[sys_file_index].offset = 0;
		sys_file_num--;
	}
	current->filepointer[fd] = -1;

	current->fp_num--;
	current->tf.eax = 0;
}

void remove_aux(char * path){
	rmFL(path);
	current->tf.eax = 0;
}

void wait_for_keyboard(int char_num){
    while(1) {
    	if(buf_out + char_num >= buf_in){
    		intr_nesting = true;
			enableInterrupt();
			waitForInterrupt();
			disableInterrupt();
    	}
    	else{
    		intr_nesting = false;
			break;
    	}			
    }	
}

void read_aux(int fd, char* buf, int len){
	if(current->pid == 2){
		buf = (char *)((int)buf + Child_Data_Offst);
	}
	if(fd == stdin){
		wait_for_keyboard(len - 1);
		int i;
		for(i=0; i<len; i++){
			buf[i] = BUF[i + buf_out];
		}
		buf_out += len;
	}
	else{
		//printk("fd is %d len is %d\n", fd, len);
		//printk("the offset is %d\n", sysFileTable[current->filepointer[fd]].offset);
		int index = sysFileTable[current->filepointer[fd]].inode_index;
		int offset = sysFileTable[current->filepointer[fd]].offset;
		
		//printk("inode is %d\n", index);
		current->tf.eax = readF(index, buf, len, offset);

		int filesz = fs_inodepointer[index].filesz;
		if(offset + len > filesz) sysFileTable[current->filepointer[fd]].offset = filesz;
		else sysFileTable[current->filepointer[fd]].offset += len;
		//printk("the offset is %d\n", sysFileTable[current->filepointer[fd]].offset);
	}
}

void lseek_aux(int fd, int offset, int whence){
	if(whence == SEEK_SET){
		sysFileTable[current->filepointer[fd]].offset = offset;
		//printk("the offset is %d\n", sysFileTable[current->filepointer[fd]].offset);
	}
	else if(whence == SEEK_CUR){
		sysFileTable[current->filepointer[fd]].offset += offset;
	}
	else if(whence == SEEK_END)
	{
		int index = sysFileTable[current->filepointer[fd]].inode_index;
		sysFileTable[current->filepointer[fd]].offset = offset + fs_inodepointer[index].filesz;
	}
	else assert(0);
}

void try_to_wakeup(){
	PCB* p;
	PCB* q;
	p=blocked_list->next;
	while(p!=NULL){
		p->sleepTime--;
		if(p->sleepTime == 0){
			q = p;
			p = p->next;
			list_delete(blocked_list, q);
			q->state = RUNNABLE;
			list_add(runnable_list, q);
		}
		else p = p->next;
	}
}

void schedule(){
	if(runnable_list->next!=NULL){
		if(current->state == RUNNING){
			current->state = RUNNABLE;
			if(current!=&idle) list_add(runnable_list, current);
		}
		else if(current->state == DEAD){
			list_add(free_list, current);
		}
		else if(current->state == BLOCKED){
			list_add(blocked_list, current);
		}
		else if(current->state == SEM_BLOCKED){
			list_add(current->psem->list, current);
		}

		PCB* p = list_head(runnable_list);
		list_delete(runnable_list, p);
		p->state = RUNNING;
		p->timeCount = 10;
		p->sleepTime = 0;
		tss.esp0 = (int)(p->stack) + MAX_STACK_SIZE*4 + sizeof(struct TrapFrame);
		current = p;
	}
	else{
		if(current->state == RUNNING){
			current->timeCount = 10;
		}
		else if(current->state == DEAD){
			list_add(free_list, current);
			current = &idle;
			current->state = RUNNING;
			current->timeCount = 10;
		}
		else if(current->state == BLOCKED){
			list_add(blocked_list, current);
			current = &idle;
			current->state = RUNNING;
			current->timeCount = 10;
		}
		else if(current->state == SEM_BLOCKED){
			list_add(current->psem->list, current);
			current = &idle;
			current->state = RUNNING;
			current->timeCount = 10;
		}
	} 
	//char ch = '0' + current->pid;
	//putChar(ch);
	printk("%d", current->pid);
}

void GProtectFaultHandle(struct TrapFrame *tf){
	assert(&(current->tf) == tf);
	assert(0);
	return;
}

void timerHandle(struct TrapFrame *tf) {
	if(!intr_nesting){
		assert(&(current->tf) == tf);
		current->timeCount--;
		try_to_wakeup();
		if(current->timeCount == 0)schedule();
	}
}

void keyboardHandle(struct TrapFrame *tf){
	char ch = keyboard_event();
	if(ch != '\0'){
		screen_put(ch, default_forecolor, default_backcolor);
		BUF[buf_in++] = ch;
	} 

}

void syscallHandle(struct TrapFrame *tf) {
	assert(&(current->tf) == tf);
	/* implement system call */
	switch(tf->eax){
		case NR_exit: exit_aux();break;
		case NR_fork: fork_aux();break;
		case NR_read: read_aux(tf->ebx, (char *)tf->ecx, tf->edx);break;
		case NR_write: write_aux(tf->ebx, (char *)tf->ecx, tf->edx);break;
		case NR_seek: lseek_aux(tf->ebx, tf->ecx, tf->edx);break;
		case NR_open: open_aux((char*)tf->ebx, tf->ecx);break;
		case NR_close: close_aux(tf->ebx);break;
		case NR_remove: remove_aux((char *)tf->ebx);break;
		case NR_sem_init: sem_init_aux((sem_t *)tf->ebx, tf->ecx);break;
		case NR_sem_destroy: sem_destroy_aux((sem_t *)tf->ebx);break;
		case NR_sem_wait: sem_wait_aux((sem_t *)tf->ebx);break;
		case NR_sem_post: sem_post_aux((sem_t *)tf->ebx);break;
		case NR_ls: ls_aux((char*)tf->ebx);break;
		case NR_cat:cat_aux((char*)tf->ebx);break;
		case NR_sleep: sleep_aux((int)tf->ebx);break;
		default: assert(0);
	}
	return;
}

