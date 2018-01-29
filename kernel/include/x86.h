#ifndef __X86_H__
#define __X86_H__

#include "x86/cpu.h"
#include "x86/memory.h"
#include "x86/io.h"
#include "x86/irq.h"
#include "x86/fs.h"

#define Father_Data_Offset 	0
#define Father_Code_Offset 	0
#define Child_Code_Offst 	0x200000
#define Child_Data_Offst 	0x200000

#define DISPLAY_BASE_ADDR	0xB8000

#define max_size 		1024

#define black 			0
#define blue 			1
#define green 			2
#define cyan 			3
#define red 			4
#define magenta 		5
#define brown 			6
#define light_grey		7 
#define dark_grey 		8
#define light_blue 		9
#define light_green 	10
#define light_cyan 		11
#define light_red 		12
#define light_magenta 	13
#define light_brown 	14
#define white 			15

#define ColorAttr(foreColour, backColour) ((backColour << 4) | (foreColour & 0x0F))

#define default_forecolor white
#define default_backcolor black

typedef uint8_t bool;

#define true 1
#define false 0

#define stdin			0
#define stdout			1
#define stderr			2

#define SECTSIZE 512
#define USER_OFFSET_IN_DISK (512 + 102400)
#define FS_OFFSET_IN_DISK   (512 + 102400 + 10240)
#define FS_OFFSET_IN_MEM	0x01000000

char BUF[max_size];
volatile int buf_in;
volatile int buf_out;
volatile bool intr_nesting;

void initSeg();
void initPcb();
void initScreen();
void initFS();
void loadUMain();
void loadFS();

void printk(const char *format,...);
void printQ(const char *format,...);
int strlen(char* src);
void strcpy(char* dst, char* src);
int strcmp(char* dst, char* src);

void syscallHandle(struct TrapFrame *tf);
void timerHandle(struct TrapFrame *tf);
void keyboardHandle(struct TrapFrame *tf);
void GProtectFaultHandle(struct TrapFrame *tf);
void schedule();
void try_to_wakeup();

void write_aux(int fd, char* buf, int len);

void exit_aux();
void fork_aux();
void sleep_aux(int sleepTime);

void P(sem_t *sem);
void V(sem_t *sem);
void sem_init_aux(sem_t* sem, int value);
void sem_destroy_aux(sem_t *sem);
void sem_wait_aux(sem_t *sem);
void sem_post_aux(sem_t *sem);

void move_cursor();
void screen_put(char c, uint8_t foreColour, uint8_t backColour);
void screen_clear();

void list_add(PCB* head, PCB* data);
void list_delete(PCB* head, PCB* data);
PCB* list_head(PCB* head);

char press_key(int scan_code);
char keyboard_event();

void waitDisk(void);

void readSect(void *dst, int offset);
void readBytes(uint8_t* addr, int diskoff, int cont);
void writeSect(void *src, int offset);
void writeBytes(uint8_t* addr, int diskoff, int cont);
#endif
