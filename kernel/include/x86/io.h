#ifndef __X86_IO_H__
#define __X86_IO_H__
/* ELF32二进制文件头 */

#define	PT_NULL			0		/* Program header table entry unused */
#define PT_LOAD			1		/* Loadable program segment */
#define PT_DYNAMIC		2		/* Dynamic linking information */
#define PT_INTERP		3		/* Program interpreter */
#define PT_NOTE			4		/* Auxiliary information */
#define PT_SHLIB		5		/* Reserved */
#define PT_PHDR			6		/* Entry for header table itself */
#define PT_TLS			7		/* Thread-local storage segment */
#define	PT_NUM			8		/* Number of defined types */
#define PT_LOOS			0x60000000	/* Start of OS-specific */
#define PT_GNU_EH_FRAME	0x6474e550	/* GCC .eh_frame_hdr segment */
#define PT_GNU_STACK	0x6474e551	/* Indicates stack executability */
#define PT_GNU_RELRO	0x6474e552	/* Read-only after relocation */
#define PT_LOSUNW		0x6ffffffa
#define PT_SUNWBSS		0x6ffffffa	/* Sun Specific segment */
#define PT_SUNWSTACK	0x6ffffffb	/* Stack segment */
#define PT_HISUNW		0x6fffffff
#define PT_HIOS			0x6fffffff	/* End of OS-specific */
#define PT_LOPROC		0x70000000	/* Start of processor-specific */
#define PT_HIPROC		0x7fffffff	/* End of processor-specific */


struct ELFHeader {
	unsigned int   magic;
	unsigned char  elf[12];
	unsigned short type;
	unsigned short machine;
	unsigned int   version;
	unsigned int   entry;
	unsigned int   phoff;
	unsigned int   shoff;
	unsigned int   flags;
	unsigned short ehsize;
	unsigned short phentsize;
	unsigned short phnum;
	unsigned short shentsize;
	unsigned short shnum;
	unsigned short shstrndx;
};

/* ELF32 Program header */
struct ProgramHeader {
	unsigned int type;
	unsigned int off;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int filesz;
	unsigned int memsz;
	unsigned int flags;
	unsigned int align;
};


static inline int inLong(short port) {
	int data;
	asm volatile("in %1, %0" : "=a" (data) : "d" (port));
	return data;
}

static inline void outLong(uint16_t port, uint32_t data) {
    asm volatile("out %0, %1" : : "a"(data), "d"(port));
}

/* 读I/O端口 */
static inline uint8_t inByte(uint16_t port) {
	uint8_t data;
	asm volatile("in %1, %0" : "=a"(data) : "d"(port));
	return data;
}

/* 写I/O端口 */
static inline void outByte(uint16_t port, int8_t data) {
	asm volatile("out %%al, %%dx" : : "a"(data), "d"(port));
}

#endif
