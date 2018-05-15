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
    f->out = (f->out + 1) % f-> size;
    return element;
}

int is_fifo_empty(Fifo* f){
    return (f->in == f->out);
}

// %%%%%%%%%% SEMAPHORES & SHARED MEM %%%%%%%%%%% 

key_t create_key(char *pathname, int proj_id){
    key_t k = ftok(pathname,proj_id);
    if(k==-1){
        printf("Error when creating a new key!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return k;
}

int create_sem(key_t k, int no){
    int semID = semget(k,no,0666 | IPC_CREAT);
    if(semID==-1){
        printf("Error when creating a sempahore!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    union semun uni;
    uni.array = (unsigned short *) malloc(no * sizeof(unsigned short));
    for (int i=0; i<no; i++)
        uni.array[i] = 0;
    if(semctl(semID, 0, SETALL, uni)==-1){
        printf("Error when using semctl!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return semID;
}

void remove_sem(int semID){
    if(semctl(semID,0,IPC_RMID,0)==-1){
        printf("Error when removing semaphores!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
}

int get_sem(key_t k){
    int semID;
    if((semID=semget(k,0,0))==-1){
        printf("Error when accesing semaphores!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return semID;
}

int create_shared_mem(key_t k, int size){
    int shmID;
    if((shmID=shmget(k,size,0666|IPC_CREAT))==-1){
        printf("Error when creating shared memory!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return shmID;
}

int get_shared_mem(key_t k){
    int shmID;
    if((shmID=shmget(k,0,0))==-1){
        printf("Error when creating shared memory!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return shmID;
}

int * shared_mem_pointer(int shmID){
    int *mem = (int*) shmat(shmID,NULL,0);
    if(*mem==-1){
        printf("Error when creating pointer to shared memory!\n");
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    return mem;
}

void add_sem(int semID, int n, int value){
    struct sembuf sbuf;
    sbuf.sem_num = n;
    sbuf.sem_flg = 0;
    sbuf.sem_op = value;
    if(semop(semID,&sbuf,1)==-1){
        printf("Error when adding to semaphore! %d\n",n);
        perror(NULL);
        exit(EXIT_FAILURE);
    }
}

int semval_zero(int semID, int n){
    /*struct sembuf sbuf;
    sbuf.sem_num = n;
    sbuf.sem_flg = IPC_NOWAIT;
    sbuf.sem_op = 0;
    if(semop(semID,&sbuf,1)<0 && errno==EAGAIN){
        return 1;
    }
    return 0;
    */
   if (semctl(semID,n,GETVAL)==0) return 1;
   else return 0;
}

void sem_wait_till_zero(int semID, int n){
    struct sembuf sbuf;
    sbuf.sem_num = n;
    sbuf.sem_flg = 0;
    sbuf.sem_op = 0;
    semop(semID,&sbuf,1);
}
