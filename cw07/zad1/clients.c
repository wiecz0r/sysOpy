#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
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

void print_msg(char *string, int pid, long gtime){
    //add_sem(semID,MSG,-1);
    printf("[PID: %d] %sTime:%ld\n",pid,string,gtime);
    //add_sem(semID,MSG,1);
}

void sigusr1_handler(int signo){
    add_sem(semID,CLIENT,-1);
    print_msg("Sitting on barber's chair.           ",pid,get_time());
    fflush(stdout);
    //printf("GETVAL_CHAIR: %d\n",semctl(semID,CHAIR,GETVAL,0));
    fflush(stdout);
    add_sem(semID,CHAIR,1);
}

void sigint_handler(int signo){
    cuts_count--;
    printf("[PID: %d] Finished cut.%d more cuts remaining.  Time:%ld\n",pid,cuts_count,get_time());
    fflush(stdout);
    add_sem(semID,CLIENT,1);
    //add_sem(semID,MSG,1);
    if(!cuts_count){
        print_msg("Leaving the barber's.                ",pid,get_time());
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
}

int main(int args, char **argv){
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
    add_sem(semID,CLIENT,1);
    add_sem(semID,MSG,1);
    sleep(1);
    printf("\n");
    for(int i=0; i<clients_count; i++){
        if(fork()==0){
            pid = getpid();
            print_msg("is entering the Barber Shop.         ",pid,get_time());
            while(1){
                //add_sem(semID,MSG,-1);
                add_sem(semID, QUEUE, -1);
                int putted = fifo_put(fifo,getpid());
                add_sem(semID, QUEUE, 1);
                if(semval_zero(semID,BARBER)){
                    print_msg("Waking up barber.                    ",pid,get_time());
                    fflush(stdout);
                    add_sem(semID,BARBER,2);
                }
                else if(putted == -1){
                    print_msg("Queue full. Going to exit.   ",pid,get_time());
                    fflush(stdout);
                    exit(0);
                }
                else {
                    print_msg("Sitting in queue.                    ",pid,get_time());
                    fflush(stdout);
                }
                //add_sem(semID,MSG,-1);
                pause();
            }
        }
    }
    while(wait(NULL)>0);
    return 0;

}