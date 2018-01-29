#include "x86.h"
#include "device.h"
#include "common.h"


void P(sem_t *sem){
    sem->value --;
    if(sem->value < 0){
    	current->state = SEM_BLOCKED;
    	schedule();
    }
}

void V(sem_t *sem){
    if(sem->value < 0){
    	PCB *p = list_head(sem->list);
    	list_delete(sem->list, p);
    	p->state = RUNNABLE;
    	list_add(runnable_list, p);
    }
    sem->value ++;
}


void sem_init_aux(sem_t* sem, int value){
	if(current->pid == 2){
		sem = (sem_t *)((int)sem + Child_Data_Offst);
	}

	current->psem = sem;
	current->psem->value = value;

	dummysem.next = NULL;
	sem->list = &dummysem;

	current->tf.eax = 0;
}

void sem_destroy_aux(sem_t *sem){
	current->tf.eax = 0;
	current->psem = NULL;
}

void sem_wait_aux(sem_t *sem){
	current->tf.eax = 0;
	P(sem);
}

void sem_post_aux(sem_t *sem){
	current->tf.eax = 0;
	V(sem);
}
