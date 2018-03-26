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

#define ARGS_N 5

int main(int argc, char ** argv){
    if (argc != 2){
        printf("Wrong number of arguments.\nGive just path to the file as parameter\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(argv[1],"r");
    if (file == NULL){
        printf("Something gone wrong with opening file\n");
        exit(EXIT_FAILURE);
    }
    char line[256];
    char **arguments = malloc( (ARGS_N + 2) * sizeof(char *));

    while (fgets(line,sizeof(line),file)){
        char *arg;
        int i = 1;
        arguments[0] = strtok(line," \n");
        while((arg=strtok(NULL," \n")) != NULL && i <= ARGS_N){
            arguments[i] = arg;
            i++;
        }
        arguments[i] = NULL;

        int procResult;
        pid_t pid = vfork();
        if (pid==0){
            printf("##################################################################################\n");
            printf(ANSI_COLOR_YELLOW "PID: "ANSI_COLOR_MAGENTA"%d\n"ANSI_COLOR_YELLOW"CMD: "ANSI_COLOR_RED BOLD_TEXT,getpid());
            for (int j = 0; j < i ; ++j) {
                printf("%s ",arguments[j]);
            }
            printf(ANSI_COLOR_RESET"\n");
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
    }
    free(arguments);
    fclose(file);
    return 0;
}