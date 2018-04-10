#define _DEFAULT_SOURCE
#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int paused = 0;
int killed = 0;
char* cmd[] = {"./time.sh",0};

void SIGTSTP_handler(int signal){
    if (!paused)
        printf("\nOczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    paused = (paused+1)%2;
}

void SIGINT_handler(int signal){
    printf("\nOdebrano sygnał SIGINT\n");
    exit(EXIT_SUCCESS);
}

void child(){
    pid_t pid = fork();
    if(!pid) execvp(cmd[0],cmd);

    while(1){
        if (!paused){
            if(killed){
                killed = 0;
                pid = fork();
                if(!pid) execvp(cmd[0],cmd);
            }
        }
        else if (!killed){
            kill(pid,SIGKILL);
            killed = 1;
        }
    }
}

int main(int args, char **argv){
    if (args != 2){
        printf("Podaj argument: 1 - wersja normalna, 2 - wersja z procesami\n");
        exit(EXIT_FAILURE);
    }
    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_handler = SIGTSTP_handler;
    sigact.sa_flags = 0;

    sigaction(SIGTSTP,&sigact,NULL);
    signal(SIGINT,SIGINT_handler);
    
    int arg = atoi(argv[1]);

    // WERSJA NORMALNA
    if (arg == 1){
        printf("Wersja normalna\n");
        time_t myTime;
        struct tm *tmInfo;
        
        while(1){
            if(!paused){
                myTime = time(NULL);
                tmInfo = localtime(&myTime);
                printf("\nAktualny czas: %s",asctime(tmInfo));
            }
            sleep(1);
        }
        return 0;
    }

    //WERSJA Z PROCESAMI
    else if (arg == 2){
        printf("Wersja z procesami\n");
        child();
        return 0;
    }
    return 1;
}

