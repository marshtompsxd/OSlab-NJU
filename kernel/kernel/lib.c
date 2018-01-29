#include "common.h"
#include "x86.h"
#include "device.h"


int strlen(char* src)
{
	int i = 0;
	while(src[i]!='\0') i++;
	return i;
}

void strcpy(char* dst, char* src)
{
	int i;
	int len = strlen(src);
	for(i = 0;i<len;i++)
	{
		dst[i] = src[i];
	}

	dst[len] = '\0';
}

int strcmp(char* dst, char* src)
{
	int i;
	int len1 = strlen(dst);
	int len2 = strlen(src);
	int len = (len1 < len2)?len1:len2;
	for(i=0; i<len; i++)
	{
		if(dst[i] > src[i])return 1;
		else if(dst[i]<src[i])return -1;
	}
	if(len1 == len2) return 0;
	else if(len1 > len2) return 1;
	else return -1;
}


void print_ch(uint8_t arg){
	char ch = arg;
	//outbuf[outlen++] = ch;
	putChar(ch);
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


void printk(const char *format,...){
	
	uint32_t *stack_point = (uint32_t*)&format;
	stack_point ++;
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
	//syscall(NR_write, stdout, (uint32_t)outbuf, outlen, 0, 0);
}



void print_ch_Q(uint8_t arg){
	char ch = arg;
	//outbuf[outlen++] = ch;
	//putChar(ch);
	screen_put(ch, default_forecolor, default_backcolor);
}

void print_string_Q(uint8_t* arg){
	while(*arg){
		print_ch_Q(*(arg++));
	}
}

void print_dec_Q(uint32_t arg){
	int integer = arg;
	if(integer == 0x80000000){
		int8_t* str = "-2147483648"; 
		print_string_Q((uint8_t*)str);
	} 
	else{ 
		if(integer < 0){
			integer = -integer;
			print_ch_Q('-');
		}
		if(integer/10) print_dec_Q(integer/10);
		print_ch_Q('0' + integer%10);
	}
}

void print_hex_Q(uint32_t arg){
	uint32_t integer = arg;
	if(integer/0x10)print_hex_Q(integer/0x10);
	if(integer%0x10 < 10)print_ch_Q( (uint8_t)('0' + integer%0x10));
	else print_ch_Q((uint8_t)('a' + integer%0x10 - 10));
}


void printQ(const char *format,...){
	
	uint32_t *stack_point = (uint32_t*)&format;
	stack_point ++;
	while(*format){
		if(*format == '%'){
			format++;
			while(*format == ' ')format++;
			switch(*format){
				case 'c': print_ch_Q(*((uint8_t*)stack_point)); stack_point++; break;
				case 'd': print_dec_Q(*((uint32_t*)stack_point)); stack_point++; break;
				case 'X': print_hex_Q(*((uint32_t*)stack_point)); stack_point++; break;
				case 'x': print_hex_Q(*((uint32_t*)stack_point)); stack_point++; break;
				case 's': print_string_Q(*((uint8_t**)stack_point)); stack_point++; break;
				case '%': print_ch_Q('%'); break;
				default: break;
			}
			format++;
		}
		else{
			print_ch_Q(*format);
			format++;
		}
	}
	//syscall(NR_write, stdout, (uint32_t)outbuf, outlen, 0, 0);
}