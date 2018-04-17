#define _BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define BOLD_TEXT          "\x1b[1m"

#define ARGS_N 5
#define PIPES_NO 5

int line_parser (char* , char**);
int arguments_parser(char*, char**);
void execute_line(char *line);



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
    while (fgets(line,sizeof(line),file)){
        pid_t pid = fork();
        if (pid == 0){
            execute_line(line);
            exit(0);
        }
        wait(NULL);
    }
    fclose(file);
    return 0;
}

int line_parser (char * line, char **programs){
    int i = 1;
    char *arg = strtok(line,"|\n");

    if (arg[0]==' ') arg++;
    if(arg[strlen(arg)-1] == 32) arg[strlen(arg)-1] = '\0';

    programs[0] = malloc(strlen(arg) * sizeof(char));
    strcpy(programs[0],arg);
    printf(ANSI_COLOR_YELLOW"Command: " ANSI_COLOR_RED"%s",programs[0]);

    while((arg=strtok(NULL,"|\n")) != NULL && i <= PIPES_NO){
        if (arg[0]==' ') arg++;
        if(arg[strlen(arg)-1] == 32) arg[strlen(arg)-1] = '\0';

        programs[i] = malloc(strlen((char*) arg) * sizeof(char));
        strcpy(programs[i],arg);
        printf(ANSI_COLOR_YELLOW" | "ANSI_COLOR_RED"%s",programs[i]);
        i++;
    }
    printf(ANSI_COLOR_RESET"\n\n");
    return i;
}

int arguments_parser(char *program, char **arguments){
    char *arg;
    int i = 1;
    arguments[0] = strtok(program," \n");
    while((arg=strtok(NULL," \n")) != NULL && i <= ARGS_N){
        arguments[i] = arg;
        i++;
    }
    arguments[i] = NULL;
    return i;
}

void execute_line(char *line){
    printf(ANSI_COLOR_BLUE"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    char * programs[PIPES_NO+1];
    int progs_count = line_parser(line,programs);
    char* arguments[ARGS_N+2];

    int pipes[2][2];
    int i;
    
    for (i=0; i<progs_count; i++){
        if (i>0){
            close(pipes[i%2][0]);
            close(pipes[i%2][1]);
        }

        if(pipe(pipes[i % 2])== -1){
            printf("Error with \"pipe\"\n");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid==0){
            arguments_parser(programs[i],arguments);

            if (i != progs_count-1){
                close(pipes[i%2][0]);
                if(dup2(pipes[i%2][1],STDOUT_FILENO) < 0) exit(EXIT_FAILURE);
            }
            if (i != 0){
                close(pipes[(i+1)%2][1]);
                if(dup2(pipes[(i+1)%2][0],STDIN_FILENO) < 0) exit(EXIT_FAILURE);
            }

            execvp(*arguments,arguments);
            exit(EXIT_FAILURE);
        }
    }
    close(pipes[i%2][0]);
    close(pipes[i%2][1]);
    while(wait(NULL) > 0);
}

