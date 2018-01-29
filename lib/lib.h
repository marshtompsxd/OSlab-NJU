#ifndef __lib_h__
#define __lib_h__

#include "types.h"

void printf(const char *format,...);
char getchar();
void gets(char* s);
void scanf(const char *format,...);

int fork();
void exit();
void sleep(int time_cont);

int sem_init(sem_t* sem, int value);
int sem_destroy(sem_t* sem);
int sem_wait(sem_t* sem);
int sem_post(sem_t* sem);

void ls(char* path);
void cat(char* path);

int open(char *path, int flags);
int read(int fd, void *buffer, int size);
int write(int fd, void *buffer, int size);
int lseek(int fd, int offset, int whence);
int close(int fd);
int remove(char *path);

#define SEEK_SET 		0
#define SEEK_CUR 		1
#define SEEK_END 		2

#define O_RDWR			0x1
#define O_RDONLY		0x2
#define O_WRONLY		0x4
#define O_APPEND		0x8
#define O_CREAT			0x10
#define O_EXCL			0x20
#define O_TRUNC			0x40
#define O_NOCTTY		0x80
#define O_NONBLOCK		0x100

#endif
