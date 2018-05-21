#include "functions.h"

long get_time(void){
    struct timespec marker;
    if (clock_gettime(CLOCK_MONOTONIC, &marker) == -1){
        printf("ERROR when getting time!\n");
        exit(EXIT_FAILURE);
    }
    return marker.tv_nsec / 1000;
}

void fifo_init(Fifo* f,int size){
    f->size=size;
    f->in=f->out=0;
}

int fifo_put(Fifo* f, int element){
    if(f->in == (f->out + f->size -1)%f->size)
        return -1;
    f->el[f->in]=element;
    f->in = (f->in + 1) % f->size;
    return 0;
}

int fifo_get(Fifo* f){
    int element = f->el[f->out];
    f->el[f->out] = 0;
    f->out = (f->out + 1) % f-> size;
    return element;
}

int is_fifo_empty(Fifo* f){
    return (f->in == f->out);
}

// %%%%%%%%%% SEMAPHORES & SHARED MEM %%%%%%%%%%% 
/*
key_t create_key(char *pathname, int proj_id){
    key_t k = ftok(pathname,proj_id);
    if(k==-1){
        printf("Error when creating a new key!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return k;
}
*/
sem_t* create_sem(const char* name, unsigned int val){
    sem_t* sem = sem_open(name, O_CREAT|O_RDWR, 0666, val);
    if(sem==SEM_FAILED){
        printf("Error when creating a sempahore!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return sem;
}

void remove_sem(sem_t * sem, const char* name){
    if(sem_close(sem)){
        printf("Error when: Sem_close\n");
        perror(NULL);
    } 
    if(sem_unlink(name)==-1) {
        printf("Failed to remove semaphore!\n");
        perror(NULL);
    }
}

sem_t * get_sem(const char* name){
    sem_t* sem = sem_open(name,O_RDWR);
    if(sem==SEM_FAILED){
        printf("Error when getting a sempahore!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return sem;
}

void take_sem(sem_t * sem){
    if(sem_wait(sem)==-1){
        printf("Problem when taking semaphore\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
}

void release_sem(sem_t * sem){
    if(sem_post(sem)==-1){
        printf("Problem when releasing semaphore\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
}

int create_shared_mem(const char * name, off_t size){
    int shm_descriptor;
    if((shm_descriptor=shm_open(name,O_RDWR|O_CREAT,0666))==-1){
        
        printf("Error when creating shared memory!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    if(ftruncate(shm_descriptor, size)==-1){
        printf("Error when ftruncate!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return shm_descriptor;
}

int get_shared_mem(const char * name){
    int shm_descriptor;
    if((shm_descriptor=shm_open(name,O_RDWR,0666))==-1){
        printf("Error when getting shared memory!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return shm_descriptor;
}

int * shared_mem_pointer(int shm_descriptor, off_t size){
    int * addr = (int *) mmap(NULL,(size_t) size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_descriptor, 0);
    if(addr==(int*) -1){
        printf("Error when mmap |shared_mem_pointer function|!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return addr;
}
