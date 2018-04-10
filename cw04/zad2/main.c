#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <memory.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int N, K, n=0, k=0;
pid_t *children_pid, *child_sentUSR_pid;

void SIGINT_handler(int, siginfo_t*, void*);
void SIGUSR1_handler(int, siginfo_t*, void*);
void rt_handler(int, siginfo_t*, void*);
void SIGCHLD_handler(int, siginfo_t*, void*);
void child();
void child_handler(int);
void removeChild(pid_t);
int checkIfChildren(pid_t);

#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_RESET   "\x1b[0m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"


int main(int args, char **argv){
    if (args!=3){
        printf("Must be 2 args: N & K ");
        exit(EXIT_FAILURE);
    }
    N = atoi(argv[1]);
    K = atoi(argv[2]);

    children_pid = calloc(N, sizeof(pid_t));
    child_sentUSR_pid = calloc(N, sizeof(pid_t));

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags=SA_SIGINFO;

    act.sa_sigaction = SIGINT_handler;
    sigaction(SIGINT,&act,NULL);

    act.sa_sigaction = SIGUSR1_handler;
    sigaction(SIGUSR1,&act,NULL);
    
    act.sa_sigaction = SIGCHLD_handler;
    sigaction(SIGCHLD,&act,NULL);

    act.sa_sigaction = rt_handler;
    for (int i = SIGRTMIN; i <= SIGRTMAX; i++){
        sigaction(i,&act,NULL);
    }

    for (int i=0; i<N; i++){
        pid_t pid = fork();
        if (!pid){
            printf(ANSI_GREEN "Begin child process -> "ANSI_BLUE"PID: %d\n" ANSI_RESET,getpid());
            child();
        }
        else{
            children_pid[n] = pid;
            n++;
        }
    }
    while(wait(NULL));
    return 0;
}

void SIGINT_handler(int signum, siginfo_t *info, void *ucontext){
    printf("Parent received "ANSI_YELLOW"SIGINT\n"ANSI_RESET);
    for (int i=0; i< N; i++){
        kill(children_pid[i],SIGKILL);
        waitpid(children_pid[i],NULL,0);
    }
    exit(EXIT_SUCCESS);
}

void SIGUSR1_handler(int signum, siginfo_t *info, void *ucontext){
    printf(ANSI_RESET"Parent received "ANSI_YELLOW"SIGUSR1"ANSI_RESET" from Child -> PID: %d\n",info->si_pid);
    //if (checkIfChildren(info->si_pid) == -1) return;
    if (k < K)
        child_sentUSR_pid[k++] = info->si_pid;
    else if (k == K){
        for (int i=0;i<K;i++){
            printf("Parent is sending permission "ANSI_YELLOW"(SIGUSR2)"ANSI_RESET" to Child -> PID: %d\n",child_sentUSR_pid[i]);
            kill(child_sentUSR_pid[i], SIGUSR2);
            waitpid(child_sentUSR_pid[i],NULL,0);
        }
        k++;
    }  
    else {
        printf("Parent is sending permission "ANSI_YELLOW"(SIGUSR2)"ANSI_RESET" to Child -> PID: %d\n",info->si_pid);
        kill(info->si_pid, SIGUSR2);
        waitpid(info->si_pid,NULL,0);
    }
}

void rt_handler(int signum, siginfo_t *info, void *ucontext){
    printf("Parent received RT signal: SIGRTMIN+%d from Child -> PID: %d\n",signum-SIGRTMIN, info->si_pid);
}

void SIGCHLD_handler(int signum, siginfo_t *info, void *ucontext){
    n--;
    printf(ANSI_RED"Child (PID: %d) terminated, with exit status: %d\n"ANSI_RESET,info->si_pid,info->si_status);
    if (n == 1){
        printf("\nAll children terminated.\nTerminating program...\n");
        exit(EXIT_SUCCESS);
    }

    //removeChild(info->si_pid);
}

void child(){
    srand(getpid());

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR2);

    signal(SIGUSR2, child_handler);
    int sleep_time = rand() % 10000001;
    printf("PID: %d is sleeping for %d sec...\n",getpid(),sleep_time/1000000);
    usleep(sleep_time);
    //THEN SEND SIGUSR1
    kill(getppid(),SIGUSR1);

    sigsuspend(&mask);
    exit(sleep_time/1000000); 
}

void child_handler(int signum){
    kill(getppid(),SIGRTMIN + (rand() % (SIGRTMAX - SIGRTMIN)));
}

void removeChild(pid_t pid) {
    for (int i = 0; i < N; i++)
        if (children_pid[i] == pid) {
            children_pid[i] = -1;
            return;
        }
}

int checkIfChildren(pid_t pid) {
    for (int i = 0; i < N; i++)
        if (children_pid[i] == pid) return i;
    return -1;
}
