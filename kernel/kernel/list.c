#include "x86.h"
#include "device.h"
#include "common.h"

/* functions to manage the process list */
void list_add(PCB* head, PCB* data){
	PCB* p;
	for(p=head;p->next!=NULL;p = p->next);
	data->next = p->next;
	p->next = data;
}

void list_delete(PCB* head, PCB* data){
	PCB* p;
	PCB* q;
	for(p=head;p->next!=NULL;p = p->next){
		if(p->next == data)break;
	}
	if(p->next==NULL)assert(0);
	q = p->next;
	p->next = q->next;
}

PCB* list_head(PCB* head){
	PCB* p = head->next;
	if(p==NULL)assert(0);
	return p;
}
