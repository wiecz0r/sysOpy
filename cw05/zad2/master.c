#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 256

int main(int args, char **argv){
    if (args < 2){
        printf("Too few args\n");
        exit(EXIT_FAILURE);
    }
    int count = 0;

    mkfifo(argv[1], S_IWUSR | S_IRUSR);
    FILE *fifo = fopen(argv[1],"r");
    char buff[BUFFER_SIZE];

    while(fgets(buff, BUFFER_SIZE, fifo)){
        if (!count) printf("\n");
        printf("%s",buff);
        count++;
    }

    fclose(fifo);
    remove(argv[1]);

    printf("\n\nLICZBA LINII: %i\n",count);

    return 0;
}