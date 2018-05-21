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
int count_all=0;
Fifo * fifo;
pid_t pid;
int on_chair=0, finished=0;
char *shm = "/shm";
sem_t *queue, *barber, *chair, *client;

void print_msg(char *string, int pid, long gtime){
    printf("[PID: \x1b[33m%d\x1b[0m] %sTime:%ld\n",pid,string,gtime);
}

void sigusr1_handler(int signo){
    take_sem(client);
    on_chair=1;
    print_msg("Sitting on barber's chair.           ",pid,get_time());
    fflush(stdout);
    release_sem(chair);
}

void sigusr2_handler(int signo){
    cuts_count--;
    printf("[PID: \x1b[34m%d\x1b[0m] Finished cut.%d more cuts remaining.  Time:%ld\n",pid,cuts_count,get_time());
    fflush(stdout);
    release_sem(client);
    if(!cuts_count){
        print_msg("\x1b[31mLeaving the barber's.\x1b[0m                ",pid,get_time());
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
    finished=1;
}

int main(int args, char **argv){
    if (args < 3){
        printf("Podaj liczbe klientow oraz strzyzen dla klienta!\n");
        return EXIT_FAILURE;
    }
    signal(SIGUSR2,sigusr2_handler);

    clients_count = atoi(argv[1]);
    cuts_count = atoi(argv[2]);

    queue = get_sem(QUEUE);
    barber = get_sem(BARBER);
    client = get_sem(CLIENT);
    chair = get_sem(CHAIR);
    int shmID = get_shared_mem(shm);

    fifo = (Fifo *) shared_mem_pointer(shmID, sizeof(Fifo));

    signal(SIGUSR1,sigusr1_handler);
    printf("\n");
    for(int i=0; i<clients_count; i++){
        if(fork()==0){
            pid = getpid();
            print_msg("is entering the Barber Shop.         ",pid,get_time());
            while(1){
                take_sem(queue);
                int putted = fifo_put(fifo,getpid());
                release_sem(queue);
                int semval;

                if(sem_getvalue(barber,&semval)==-1){
                    printf("Error when getting value (sem_getvalue)\n");
                    perror(NULL);
                    exit(EXIT_FAILURE);
                }
                printf("%d\n",semval); fflush(stdout);
                if(semval==0){
                    print_msg("\x1b[32mWaking up barber.\x1b[0m                    ",pid,get_time());
                    fflush(stdout);
                    release_sem(barber);
                    //release_sem(barber);
                }
                else {
                    if(putted == -1){
                        print_msg("\x1b[31mQueue full. Going to exit.\x1b[0m   ",getpid(),get_time());
                        fflush(stdout);
                        exit(0);
                    }
                    if (on_chair!=1){
                        print_msg("Sitting in queue.                    ",pid,get_time());
                        fflush(stdout);
                    }
                }
                while(!on_chair){
                    //do nothing
                }
                on_chair=0;
                while(!finished){
                    //do nothing
                }
                finished=1;
            }
        }
    }
    while(wait(NULL)>0);
    printf("All clients left barbers'. Terminating clients manager...\n");
    return 0;

}