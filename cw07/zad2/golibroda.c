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

char *shm = "/shm";
sem_t *queue, *barber, *chair, *client;
int shmID;
Fifo* fifo;
int seats_no = 0;

void at_exit(void){
    remove_sem(queue,QUEUE);
    remove_sem(barber,BARBER);
    remove_sem(chair,CHAIR);
    remove_sem(client,CLIENT);
 
    munmap(fifo,sizeof(Fifo));
    shm_unlink(shm);
    printf("\nRemoved shared data and semaphores. Terminating...\n");
    fflush(stdout);
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
    queue = create_sem(QUEUE,1);
    barber = create_sem(BARBER,1);
    client = create_sem(CLIENT,1);
    chair = create_sem(CHAIR,0);
    shmID = create_shared_mem(shm,sizeof(Fifo));

    fifo = (Fifo *) shared_mem_pointer(shmID,sizeof(Fifo));
    fifo_init(fifo,seats_no);

    while(1){
        take_sem(queue);
        int is_empty = is_fifo_empty(fifo);
        release_sem(queue);
        if(is_empty){
            printf("BARBER is falling asleep.                             Time:%ld\n",get_time());
            take_sem(barber);
            take_sem(barber);
            printf("BARBER is waking up.                                  Time:%ld\n",get_time());
        }
        take_sem(queue);
        int pid = fifo_get(fifo);
        release_sem(queue);
        if(pid==0){
            printf("AAAAAAAA -ZERO\n");
            exit(1);
        }
        printf("BARBER is welcoming client on chair (PID: %d).      Time:%ld\n",pid,get_time());
        kill(pid,SIGUSR1);
        take_sem(chair);
        printf("BARBER is cutting client (PID: %d).                 Time:%ld\n",pid,get_time());
        printf("BARBER has just finished cutting (PID: %d).         Time:%ld\n",pid,get_time());
        kill(pid,SIGUSR2);
    }
    return 0;
}