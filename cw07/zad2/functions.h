#ifndef _FUNC_H
#define _FUNC_H

//#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define QUEUE "/queue"
#define BARBER "/barber"
#define CHAIR "/chair"
#define CLIENT "/client"

typedef struct Fifo{
    int el[512];
    int size;
    int in;
    int out;
}Fifo;

long get_time(void);

void fifo_init(Fifo* f,int size);
int fifo_put(Fifo* f, int element);
int fifo_get(Fifo* f);
int is_fifo_empty(Fifo* f);

sem_t* create_sem(const char* name, unsigned int val);
void remove_sem(sem_t * sem, const char* name);
sem_t * get_sem(const char* name);
void take_sem(sem_t * sem);
void release_sem(sem_t * sem);
int create_shared_mem(const char * name, off_t size);
int get_shared_mem(const char * name);
int * shared_mem_pointer(int shm_descriptor, off_t size);

#endif