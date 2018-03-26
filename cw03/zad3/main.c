#define _BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define BOLD_TEXT          "\x1b[1m"

#define ARGS_N 10

void make_limits(char*, char*);
void print_usage(struct rusage*);

int main(int argc, char ** argv){
    if (argc < 4){
        printf(ANSI_COLOR_RED"Wrong number of arguments.\nPlease give: FILEPATH CPU_TIME_LIMIT DATA_SIZE_LIMIT(in MB)\n"ANSI_COLOR_RESET);
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1],"r");
    if (file == NULL){
        printf("Something gone wrong with opening file\n");
        exit(EXIT_FAILURE);
    }
    char line[256];
    char **arguments = malloc( (ARGS_N + 2) * sizeof(char *));
    struct rusage *rUsage = malloc(2 * sizeof(struct rusage));

    while (fgets(line,sizeof(line),file)){
        char *arg;
        int i = 1;
        arguments[0] = strtok(line," \n");
        while((arg=strtok(NULL," \n")) != NULL && i <= ARGS_N){
            arguments[i] = arg;
            i++;
        }
        arguments[i] = NULL;

        getrusage(RUSAGE_CHILDREN,&rUsage[0]);
        int procResult;
        pid_t pid = vfork();
        if (pid==0){
            printf("##################################################################################\n");
            printf(ANSI_COLOR_YELLOW "PID: "ANSI_COLOR_MAGENTA"%d\n"ANSI_COLOR_YELLOW"CMD: "ANSI_COLOR_RED BOLD_TEXT,getpid());
            for (int j = 0; j < i ; ++j) {
                printf("%s ",arguments[j]);
            }
            printf(ANSI_COLOR_RESET"\n\n");
            make_limits(argv[2],argv[3]);
            execvp(*arguments,arguments);
            exit(EXIT_FAILURE);
        }
        else if (pid == -1) exit(EXIT_FAILURE);
        else {
            if (pid != wait(&procResult)){
                exit(EXIT_FAILURE);
            }
            if (procResult != 0){
                printf("\nError with command: %s\n",arguments[0]);
                exit(EXIT_FAILURE);
            }
            printf("\n");
        }
        getrusage(RUSAGE_CHILDREN,&rUsage[1]);
        print_usage(rUsage);
    }
    free(arguments);
    fclose(file);
    return 0;
}

void make_limits(char *time, char *size){
    struct rlimit rLimit;
    int time_limit = (int) strtol(time,NULL,10);
    int size_limit = (int) strtol(time,NULL,10);

    rLimit.rlim_cur = rLimit.rlim_max = (rlim_t) time_limit;
    if (setrlimit(RLIMIT_CPU,&rLimit)!=0){
        printf("Unable to set CPU time limit!");
        exit(EXIT_FAILURE);
    }
    rLimit.rlim_cur = rLimit.rlim_max = (rlim_t) size_limit * 1024 * 1024; // MB to B
    if (setrlimit(RLIMIT_DATA,&rLimit)!=0){
        printf("Unable to set memory limit!");
        exit(EXIT_FAILURE);
    }
}

void print_usage(struct rusage * rUsage){
    float sec = rUsage[1].ru_utime.tv_sec - rUsage[0].ru_utime.tv_sec;
    float usec = rUsage[1].ru_utime.tv_usec - rUsage[0].ru_utime.tv_usec;
    printf(ANSI_COLOR_GREEN"User time:"ANSI_COLOR_CYAN"      %f\n",(sec + (usec/1000000)));
    sec = rUsage[1].ru_stime.tv_sec - rUsage[0].ru_stime.tv_sec;
    usec = rUsage[1].ru_stime.tv_usec - rUsage[0].ru_stime.tv_usec;
    printf(ANSI_COLOR_GREEN"System time:   "ANSI_COLOR_CYAN" %f\n\n"ANSI_COLOR_RESET,(sec + (usec/1000000)));
}