#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

int main(int args, char ** argv){
    if (args < 4){
        printf("Too few args\n");
        exit(EXIT_FAILURE);
    }

    int slave_no = atoi(argv[3]);
    int child_pid;
    pid_t master_pid;

    master_pid = fork();
    if(!master_pid){
        execlp("./master","master",argv[1],NULL);
        printf("Error creating Master\n");
        exit(EXIT_FAILURE);
    }
    if(master_pid < 0){
        printf("Error when forking Master\n");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    for(int i=0; i<slave_no; i++){
        child_pid = fork();
        if(child_pid < 0){
            printf("Error when forking Slave\n");
            exit(EXIT_FAILURE);
        }
        if(!child_pid){
            execlp("./slave","slave",argv[1],argv[2],NULL);
            printf("Error creating Slave\n");
            exit(EXIT_FAILURE);
        }
    }
    while(wait(NULL) != -1){

    }

    return 0;
}