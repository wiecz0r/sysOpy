#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define _XOPEN_SOURCE 500

void exitFail(char *msg){
    printf("%s\n",msg);
    exit(EXIT_FAILURE);
}

void generate(char *path, int amount, int length){
    char *string = malloc(length * sizeof(char) + 1);
    FILE *f = fopen(path, "w+");
    FILE *rand0m = fopen("/dev/random","r");

    for (int i=0; i<amount; i++){
        if (fread(string,sizeof(char),(size_t) length + 1, rand0m) != length + 1)
            exitFail("Problem with reading chars from /dev/random");

        for (int k=0;k<length;k++)
            string[k]= (char) abs(string[k]) % 94 + 33;

        string[length] = 10;
        if (fwrite(string,sizeof(char),(size_t) length + 1,f) != length + 1)
            exitFail("Problem with writing generated string to file");
    }
    fclose(rand0m);
    fclose(f);
    free(string);
}

void libCopy(char *from, char *to, int amount, int length){
    char *str = malloc(length * sizeof(char));
    FILE *src = fopen(from,"r");    //source file
    FILE *trg = fopen(to,"w+");     //target file

    for (int i=0; i<amount; i++){
        if (fread(trg, sizeof(char), (size_t) length, src) != length)
            exitFail("Error when reading sourcefile to buffer!");
        if (fwrite(str, sizeof(char), (size_t) length, trg) != length)
            exitFail("Error when writing from buffer to target file!");
    }
    fclose(trg);
    fclose(src);
    free(str);
}

void sysCopy(char *from, char *to, int amount, int length){
    char *str = malloc(length * sizeof(char));
    int src = open(from, O_RDONLY);
    int trg = open(to, O_CREAT | O_WRONLY | O_TRUNC);

    for (int i=0; i<amount; i++){
        if (read(src, str, (size_t) length * sizeof(char)) != length)
            exitFail("Error when reading sourcefile to buffer!");
        if (write(trg, str, (size_t) length * sizeof(char)) != length)
            exitFail("Error when writing from buffer to target file!");
    }
    close(trg);
    close(src);
    free(str);
}

void libSort(char *path, int amount, int length){
    FILE *f = fopen(path,"r+");
    char *str1 = malloc(length * sizeof(char));
    char *str2 = malloc(length * sizeof(char));
    int j;

    for (int i=1; i<amount; i++){
        fseek(f, i*length,0);
        if (fread(str1,sizeof(char),length,f) != length)
            exitFail("Error with \"fread\" function - copying to buffer str1");
        j=i-1;
        while (j>=0){
            fseek(f, j*length,0);
            if (fread(str2,sizeof(char),length,f) != length)
                exitFail("Error with \"fread\" function - copying to buffer str2");
            if (str2[0] > str1[0]){
                fseek(f,(j+1)*length,0);
                if (fwrite(str2,sizeof(char),length,f) != length)
                    exitFail("Error with \"fwrite\" function - writing from buffer str2 to file");
                j--;
                fseek(f,(j+1)*length,0);
                if (fwrite(str1,sizeof(char),length,f) != length)
                    exitFail("Error with \"fwrite\" function - writing from buffer str1 to file");
            }
            else break;
        }

    }
    fclose(f);
    free(str1);
    free(str2);
}

void sysSort(char *path, int amount, int length){
    int f = open(path, O_RDWR);
    char *str1 = malloc(length * sizeof(char));
    char *str2 = malloc(length * sizeof(char));
    int j;

    for (int i=1; i<amount; i++){
        lseek(f, i*length,SEEK_SET);
        if (read(f,str1,sizeof(char)*length) != length)
            exitFail("Error with \"fread\" function - copying to buffer str1");
        j=i-1;
        while (j>=0){
            lseek(f, j*length,SEEK_SET);
            if (read(f,str2,sizeof(char)*length) != length)
                exitFail("Error with \"fread\" function - copying to buffer str2");
            if (str2[0] > str1[0]){
                lseek(f,(j+1)*length,SEEK_SET);
                if (write(f,str2,sizeof(char)*length) != length)
                    exitFail("Error with \"fwrite\" function - writing from buffer str2 to file");
                j--;
                lseek(f,(j+1)*length,SEEK_SET);
                if (write(f,str2,sizeof(char)*length) != length)
                    exitFail("Error with \"fwrite\" function - writing from buffer str1 to file");
            }
            else break;
        }

    }
    close(f);
    free(str1);
    free(str2);
}


int main(int argc, char **argv){
    return 0;
}