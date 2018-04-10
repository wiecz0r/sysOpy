#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <memory.h>
#include <sys/wait.h>

pid_t child_pid;
int type, p_sent=0, c_received=0, p_received=0;
int L = 0;

void parent();
void c_handler(int, siginfo_t*, void*);
void p_handler(int, siginfo_t*, void*);

void stats(){
    printf("Signals sent from Parent to Child: %d\n",p_sent);
    printf("Signals received by Parent: %d\n",p_received);
}

int main(int args, char **argv){
    if (args != 3){
        printf("Please provide 2 args: `number of signals` and `type`\n");
        return(EXIT_FAILURE);
    }
    L = atoi(argv[1]);
    type = atoi(argv[2]);
    if (type < 1 || type > 3) return(EXIT_FAILURE);

    child_pid = fork();
    if (child_pid < 0){
        printf("Error when fork()\n");
        return 1;
    }
    if (!child_pid){
        sigset_t m;
        sigfillset(&m);
        sigdelset(&m, SIGUSR1); sigdelset(&m, SIGUSR2); sigdelset(&m, SIGRTMIN); sigdelset(&m, SIGRTMAX);
        sigprocmask(SIG_SETMASK,&m,NULL);

        struct sigaction sigact;
        sigemptyset(&sigact.sa_mask);
        sigact.sa_flags = SA_SIGINFO;
        sigact.sa_sigaction = c_handler;

        sigaction(SIGUSR1,&sigact, NULL);
        sigaction(SIGRTMIN,&sigact, NULL);
        sigaction(SIGRTMAX,&sigact, NULL);
        sigaction(SIGUSR2,&sigact, NULL);

        while (1) sleep(1);
    }
    else parent();

    stats();

    return 0; 
}

void parent(){
    struct sigaction sigact;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    sigact.sa_sigaction = p_handler;

    sigaction(SIGINT,&sigact, NULL);
    sigaction(SIGUSR1,&sigact, NULL);
    sigaction(SIGRTMIN,&sigact, NULL);

    int sig1, sig2;
    sig1 = type == 3 ? SIGRTMIN : SIGUSR1;
    sig2 = type == 3 ? SIGRTMAX : SIGUSR2;
    for (int i=0; i < L; i++){
        p_sent = i;
        printf("PARENT sending %s\n",type == 3 ? "SIGRTMIN" : "SIGUSR1");
        kill(child_pid,sig1);
        if (type == 2){
            sigset_t m; sigfillset(&m);
            sigdelset(&m,SIGUSR1); sigdelset(&m,SIGINT);
            sigsuspend(&m);
        }
    }
    p_sent++;
    printf("PARENT sending %s\n",type == 3 ? "SIGRTMAX" : "SIGUSR2");
    kill(child_pid,sig2);
    wait(NULL);
}

void p_handler(int signum, siginfo_t *info, void *c){
    if (signum == SIGINT){
        p_received++;
        printf("PARENT received SIGINT\nTerminating...\n");
        kill(child_pid,SIGUSR2);
        exit(p_received);
    }
    if (info->si_pid != child_pid) return;
    if (signum == SIGRTMIN){
        p_received++;
        printf("PARENT received SIGRTMIN from Child: %d\n",child_pid);
    }
    else if (signum == SIGUSR1){
        p_received++;
        printf("PARENT received SIGUSR1 from Child: %d\n",child_pid);
    }
}

void c_handler(int signum, siginfo_t *info, void *c){
    if(info->si_pid != getppid()) return;
    
    if (signum == SIGUSR1){
        c_received++;
        printf("CHILD recevied SIGUSR1, sending back...\n");
        kill(getppid(),SIGUSR1);
    } 
    else if (signum == SIGUSR2){
        c_received++;
        printf("CHILD recevied SIGUSR2\nTERMINATING...\n");
        printf("\n\nSignals received by Child: %d\n",c_received);
        exit(c_received);
    }
    else if (signum == SIGRTMIN){
        c_received++;
        printf("CHILD recevied SIGRTMIN, sending back...\n");
        kill(getppid(),SIGRTMIN);
    }
    else if (signum == SIGRTMAX){
        c_received++;
        printf("CHILD recevied SIGRTMAX\nTERMINATING...\n");
        printf("\n\nSignals received by Child: %d\n",c_received);
        exit(c_received);
    }
}