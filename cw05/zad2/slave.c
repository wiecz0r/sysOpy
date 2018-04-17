#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 256

int main(int args, char **argv){
    if (args < 3){
        printf("Too few args\n");
        exit(EXIT_FAILURE);
    }

    int lines_no = atoi(argv[2]);
    srand(getpid()*time(NULL));

    FILE *fifo = fopen(argv[1],"w");
    printf("Slave PID: %i\n",getpid());

    char buffer[BUFFER_SIZE];
    char date_str[BUFFER_SIZE];
    FILE *date;
    for (int i=0; i<lines_no; i++){
        date = popen("date","r");
        fgets(date_str,BUFFER_SIZE,date);
        pclose(date);

        sprintf(buffer,"Slave (PID:%i) -> date: %s",getpid(),date_str);
        fputs(buffer,fifo);
        sleep(rand()%4 + 2);
    }
    fclose(fifo);

    return 0;
}