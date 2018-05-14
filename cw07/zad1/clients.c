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

int clients_count=0;
int cuts_count=0;
Fifo * fifo;
pid_t pid;

int semID;

void sigusr1_handler(int signo){
    printf("[PID: %ld] Sitting on barber's chair.   Time:%ld\n",pid,get_time());
    add_sem(semID,BARBER,2);
}

void sigint_handler(int signo){
    cuts_count--;
    printf("[PID: %ld] Finished cut.%d more cuts remaining.   Time:%ld\n",pid,cuts_count,get_time());
    if(!cuts_count){
        printf("[PID: %ld] Leaving the barber's.   Time:%ld\n",pid,get_time());
        exit(EXIT_SUCCESS);
    }
}

int main(int args, char **argv){
    atexit(at_exit());

    if (args < 3){
        printf("Podaj liczbe klientow oraz strzyzen dla klienta!\n");
        return EXIT_FAILURE;
    }
    signal(SIGINT,sigint_handler);

    clients_count = atoi(argv[1]);
    cuts_count = atoi(argv[2]);
    key_t key = create_key(getenv("HOME"),PROJ_ID);
    semID = get_sem(key);
    int shmID = get_shared_mem(key);
    fifo = (Fifo *) shared_mem_pointer(shmID);

    signal(SIGUSR1,sigusr1_handler);
    for(int i=0; i<clients_count; i++){
        if(fork()==0){
            pid = getpid();
            printf("Client [PID: %ld] is entering the Barber Shop.   Time:%ld\n",pid,get_time());
            while(1){
                add_sem(semID, QUEUE, -1);
                int putted = fifo_put(fifo,pid);
                add_sem(semID, QUEUE, 1);
                if(semval_zero(semID,BARBER)){
                    printf("[PID: %ld] Waking up barber.   Time:%ld\n",pid,get_time());
                    add_sem(semID,BARBER, 2);
                }
                else if(putted == -1){
                    printf("[PID: %ld] Queue full. Going to exit.   Time:%ld\n",pid,get_time());
                    exit(0);
                }
                else {
                    printf("[PID: %ld] Sitting in queue.   Time:%ld\n",pid,get_time());
                }
                pause();
            }
            break;
        }
    }

}