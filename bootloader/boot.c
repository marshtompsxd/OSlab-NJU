#include "boot.h"

#define SECTSIZE 512
#define KERNEL_OFFSET_IN_DISK 512


void bootMain(void) {
	/* 加载内核至内存，并跳转执行 */

	struct ELFHeader *elf;
	struct ProgramHeader *ph, *ph_end;
	uint8_t *paddr;

	elf = (struct ELFHeader*)0x8000;

	readBytes((uint8_t*)elf, KERNEL_OFFSET_IN_DISK, 4096);
	ph = (struct ProgramHeader*)((uint8_t *)elf + elf->phoff);

	for(ph_end = ph + elf->phnum; ph < ph_end; ph++){		
		if(ph->type == 1){
			paddr = (uint8_t*)(ph->paddr);
			readBytes(paddr,KERNEL_OFFSET_IN_DISK+ph->off,ph->filesz);
			uint8_t* temp;
			for(temp = paddr + ph->filesz; temp < paddr + ph->memsz; temp++){
				*temp = 0;
			}
		}
	}

	((void(*)(void))elf->entry)(); 
	while(1);
}

void waitDisk(void) { // waiting for disk
	while((inByte(0x1F7) & 0xC0) != 0x40);
}

void readSect(void *dst, int offset) { // reading a sector of disk
	int i;
	waitDisk();
	outByte(0x1F2, 1);
	outByte(0x1F3, offset);
	outByte(0x1F4, offset >> 8);
	outByte(0x1F5, offset >> 16);
	outByte(0x1F6, (offset >> 24) | 0xE0);
	outByte(0x1F7, 0x20);

	waitDisk();
	for (i = 0; i < SECTSIZE / 4; i ++) {
		((int *)dst)[i] = inLong(0x1F0);
	}
}


void readBytes(uint8_t* addr, int diskoff, int cont){ // reading cont bytes from diskoff in disk to addr
	uint8_t* addr_end = addr + cont;
	int section;
	
	// addr step back
	addr -= diskoff%SECTSIZE;
	section = (diskoff/SECTSIZE);
	for(; addr < addr_end; addr+=SECTSIZE, section++)
		readSect(addr, section);
}


