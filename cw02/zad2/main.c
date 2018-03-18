#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define _XOPEN_SOURCE 500

void generate(char *path, int amount, int length){
    char *string = malloc(length * sizeof(char) + 1);
    FILE *f = fopen(path, "w+");
    FILE *rand0m = fopen("/dev/random","r");

    for (int i=0; i<amount; i++){
        if (fread(string,sizeof(char),(size_t) length + 1, rand0m) != length + 1){
            printf("Problem with reading chars from /dev/random\n");
            exit(EXIT_FAILURE);
        }

        for (int k=0;k<length;k++)
            string[k]= (char) abs(string[k]) % 94 + 33;

        string[length] = 10;
        if (fwrite(string,sizeof(char),(size_t) length + 1,f) != length + 1){
            printf("Problem with writing generated string to file\n");
            exit(EXIT_FAILURE);
        }
    }
    fclose(rand0m);
    fclose(f);
    free(string);
}



int main(int argc, char **argv){
    nftw();
    return 0;
}