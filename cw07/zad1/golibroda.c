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
#include <signal.h>

#include "functions.h"

key_t key;
int semID, shmID;
Fifo* fifo;
int sem_count = 6;
int seats_no = 0;

void at_exit(void){
    remove_sem(semID);
    shmdt(fifo);
    shmctl(shmID,IPC_RMID,NULL);
}

void sig_handler(int signo){
    exit(signo);
}

int main(int args, char **argv){
    atexit(at_exit);

    if (args < 2){
        printf("Podaj liczbe siedzen w poczekalni!\n");
        return EXIT_FAILURE;
    }

    signal(SIGINT,sig_handler);
    signal(SIGTERM,sig_handler);

    seats_no = atoi(argv[1]);
    key = create_key(getenv("HOME"),PROJ_ID);
    semID = create_sem(key,sem_count);
    shmID = create_shared_mem(key,sizeof(Fifo));

    fifo = (Fifo *) shared_mem_pointer(shmID);
    fifo_init(fifo,seats_no);

    add_sem(semID, QUEUE, 1);
    add_sem(semID, BARBER, 1);
    printf("GETVAL_BARBER: %d\n",semctl(semID,BARBER,GETVAL,0));

    while(1){
        add_sem(semID, QUEUE, -1);
        int is_empty = is_fifo_empty(fifo);
        add_sem(semID, QUEUE, 1);
        if(is_empty){
            printf("BARBER is falling asleep.   Time:%ld\n",get_time());
            printf("GETVAL_BARBER: %d\n",semctl(semID,BARBER,GETVAL,0));
            add_sem(semID,BARBER,-1);
            printf("GETVAL_BARBER: %d\n",semctl(semID,BARBER,GETVAL,0));
            add_sem(semID,BARBER,-1);
            printf("GETVAL_BARBER: %d\n",semctl(semID,BARBER,GETVAL,0));
            printf("BARBER is waking up.   Time:%ld\n",get_time());
        }
        add_sem(semID,QUEUE,-1);
        int pid = fifo_get(fifo);
        add_sem(semID,QUEUE,1);
        printf("BARBER is welcoming client on chair (PID: %d).   Time:%ld\n",pid,get_time());
        kill(pid,SIGUSR1);
        printf("GETVAL_CHAIR: %d\n",semctl(semID,CHAIR,GETVAL,0));
        add_sem(semID,CHAIR,-1);
        printf("GETVAL_CHAIR: %d\n",semctl(semID,CHAIR,GETVAL,0));
        //add_sem(semID,BARBER,-1);
        //printf("GETVAL_BARBER: %d\n",semctl(semID,BARBER,GETVAL,0));
        printf("BARBER is cutting client (PID: %d).   Time:%ld\n",pid,get_time());
        printf("BARBER has just finished cutting (PID: %d).   Time:%ld\n",pid,get_time());
        kill(pid,SIGINT);
    }
    return 0;
}