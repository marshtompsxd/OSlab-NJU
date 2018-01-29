#include "lib.h"
#include "types.h"
/*
 * io lib here
 * 库函数写在这
 */
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

#define max_size 		1024

#define stdin			0
#define stdout			1
#define stderr			2


char outbuf[max_size];
char inbuf[max_size];
int outlen;
int inlen;
void print_ch(uint8_t arg);
void print_string(uint8_t* arg);
void print_dec(uint32_t arg);
void print_hex(uint32_t arg);


int32_t syscall(int num, uint32_t a1,uint32_t a2,
		uint32_t a3, uint32_t a4, uint32_t a5)
{
	int32_t ret = 0;

	asm volatile("int $0x80" : "=a"(ret): "a"(num), "b"(a1), "c"(a2), "d"(a3));
		
	return ret;
}

void gets(char* s){
	inlen = 0;
	syscall(NR_read, stdin, (uint32_t)inbuf, 1, 0, 0);
	char ch = inbuf[inlen];
	while(ch != '\n'){
		s[inlen] = inbuf[inlen];
		inlen ++;
		syscall(NR_read, stdin, (uint32_t)(&inbuf[inlen]), 1, 0, 0);
		ch = inbuf[inlen];
	}
	s[inlen] = '\0';
}

char getchar(){
	inlen = 0;
	syscall(NR_read, stdin, (uint32_t)inbuf, 1, 0, 0);
	return inbuf[inlen];
}

void scan_ch(char* arg){
	volatile char ch = ' ';
	syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	while(ch == '\n' || ch == ' '){
		syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	}
	*arg = ch;
}

void scan_string(char* arg){
	int len = 0;
	volatile char ch = ' ';
	syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	while(ch == '\n' || ch == ' '){
		syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	}
	while(ch != '\n' && ch != ' '){
		arg[len] = ch;
		len ++;
		syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	}
	arg[len] = '\0';

}

void scan_dec(uint32_t* arg){
	volatile char ch = ' ';
	uint32_t num = 0;
	syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	while(ch == '\n' || ch == ' '){
		syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	}
	while(ch >= '0' && ch <= '9'){
		num *= 10;
		num += ch - '0';
		syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	}
	*arg = num;
}

void scan_hex(uint32_t* arg){
	volatile char ch = ' ';
	uint32_t num = 0;
	syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	while(ch == '\n' || ch == ' '){
		syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	}
	while((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')){
		num *= 16;
		if( ch >= '0' && ch <= '9' ) num += ch - '0';
		else num += ch - 'a' + 10;
		syscall(NR_read, stdin, (uint32_t)(&ch), 1, 0, 0);
	}
	*arg = num;
}

void scanf(const char *format,...){
	uint32_t *stack_point = (uint32_t*)&format;
	stack_point ++;
	outlen = 0;
	while(*format){
		if(*format == '%'){
			format++;
			while(*format == ' ')format++;
			switch(*format){
				case 'c': scan_ch((char* )(*stack_point));   	stack_point++; break;
				case 'd': scan_dec((uint32_t* )(*stack_point)); stack_point++; break;
				case 'x': scan_hex((uint32_t* )(*stack_point)); stack_point++; break;
				case 's': scan_string((char* )(*stack_point)); 	stack_point++; break;
				default: break;
			}
			format++;
		}
		else{
			printf("%c", *format);
			format++;
		}
	}
}

void print_ch(uint8_t arg){
	char ch = arg;
	outbuf[outlen++] = ch;
}

void print_string(uint8_t* arg){
	while(*arg){
		print_ch(*(arg++));
	}
}

void print_dec(uint32_t arg){
	int integer = arg;
	if(integer == 0x80000000){
		int8_t* str = "-2147483648"; 
		print_string((uint8_t*)str);
	} 
	else{ 
		if(integer < 0){
			integer = -integer;
			print_ch('-');
		}
		if(integer/10) print_dec(integer/10);
		print_ch('0' + integer%10);
	}
}

void print_hex(uint32_t arg){
	uint32_t integer = arg;
	if(integer/0x10)print_hex(integer/0x10);
	if(integer%0x10 < 10)print_ch( (uint8_t)('0' + integer%0x10));
	else print_ch((uint8_t)('a' + integer%0x10 - 10));
}


void printf(const char *format,...){
	
	uint32_t *stack_point = (uint32_t*)&format;
	stack_point ++;
	outlen = 0;
	while(*format){
		if(*format == '%'){
			format++;
			while(*format == ' ')format++;
			switch(*format){
				case 'c': print_ch(*((uint8_t*)stack_point)); stack_point++; break;
				case 'd': print_dec(*((uint32_t*)stack_point)); stack_point++; break;
				case 'X': print_hex(*((uint32_t*)stack_point)); stack_point++; break;
				case 'x': print_hex(*((uint32_t*)stack_point)); stack_point++; break;
				case 's': print_string(*((uint8_t**)stack_point)); stack_point++; break;
				case '%': print_ch('%'); break;
				default: break;
			}
			format++;
		}
		else{
			print_ch(*format);
			format++;
		}
	}
	syscall(NR_write, stdout, (uint32_t)outbuf, outlen, 0, 0);
}


int fork(){
	return syscall(NR_fork,0,0,0,0,0);
}

void exit(){
	syscall(NR_exit,0,0,0,0,0);
}

void sleep(int time_cont){
	syscall(NR_sleep,time_cont,0,0,0,0);
}

int sem_init(sem_t* sem, int value){
	return syscall(NR_sem_init, (uint32_t)sem, value,0,0,0);
}

int sem_destroy(sem_t* sem){
	return syscall(NR_sem_destroy, (uint32_t)sem,0,0,0,0);
}

int sem_wait(sem_t* sem){
	return syscall(NR_sem_wait, (uint32_t)sem,0,0,0,0);
}

int sem_post(sem_t* sem){
	return syscall(NR_sem_post, (uint32_t)sem,0,0,0,0);
}

void ls(char* path){
	syscall(NR_ls, (uint32_t)path, 0, 0, 0, 0);
}

void cat(char* path){
	syscall(NR_cat, (uint32_t)path, 0, 0, 0, 0);

}

int open(char *path, int flags){
	return syscall(NR_open, (uint32_t)path, flags, 0, 0, 0);
}

int read(int fd, void *buffer, int size){
	return syscall(NR_read, fd, (uint32_t)buffer, size, 0, 0);
}

int write(int fd, void *buffer, int size){
	return syscall(NR_write, fd, (uint32_t)buffer, size, 0, 0);
}

int lseek(int fd, int offset, int whence){
	return syscall(NR_seek, fd, offset, whence, 0, 0);
}

int close(int fd){
	return syscall(NR_close, fd, 0, 0, 0, 0);
}

int remove(char *path){
	return syscall(NR_remove, (uint32_t)path, 0, 0, 0 ,0);
}