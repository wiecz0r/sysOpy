#ifndef _FUNC_H
#define _FUNC_H

#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define QUEUE 0
#define BARBER 1
#define PROJ_ID 1234

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

key_t create_key(char *pathname, int proj_id);
int create_sem(key_t k, int no);
void remove_sem(int semID);
int get_sem(key_t k);
int create_shared_mem(key_t k, int size);
int get_shared_mem(key_t k);
int * shared_mem_pointer(int shmID);
void add_sem(int semID, int n, int value);
int semval_zero(int semID, int n);
void sem_wait_till_zero(int semID, int n);

#endif