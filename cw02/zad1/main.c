#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>

#define _XOPEN_SOURCE 500

#define RESULTS_FILE "wyniki.txt"


void exitFail(char *msg){
    printf("%s\n",msg);
    exit(EXIT_FAILURE);
}

void generate(char *path, int amount, int length){
    char *string = malloc((length+1) * sizeof(char));
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
        if (fread(str, sizeof(char), (size_t) length + 1, src) != length + 1)
            exitFail("Error when reading sourcefile to buffer!");
        if (fwrite(str, sizeof(char), (size_t) length + 1, trg) != length + 1)
            exitFail("Error when writing from buffer to target file!");
    }
    fclose(src);
    fclose(trg);
    free(str);
}

void sysCopy(char *from, char *to, int amount, int length){
    char *str = malloc(length * sizeof(char));
    int src = open(from, O_RDONLY);
    int trg = open(to, O_CREAT | O_WRONLY | O_TRUNC,S_IRUSR | S_IWUSR);

    for (int i=0; i<amount; i++){
        if (read(src, str, (size_t) (length+1) * sizeof(char)) != length+1)
            exitFail("Error when reading sourcefile to buffer!");
        if (write(trg, str, (size_t) (length+1) * sizeof(char)) != length+1)
            exitFail("Error when writing from buffer to target file!");
    }
    close(src);
    close(trg);
    free(str);
}

void libSort(char *path, int amount, int length){
    FILE *f = fopen(path,"r+");
    char *str1 = malloc((length+1) * sizeof(char));
    char *str2 = malloc((length+1) * sizeof(char));
    int j;

    for (int i=1; i<amount; i++){
        fseek(f, i*(length+1),0);
        if (fread(str1,sizeof(char),(size_t) length+1,f) != length+1)
            exitFail("Error with \"fread\" function - copying to buffer str1");
        j=i-1;
        while (j>=0){
            fseek(f, j*(length+1),0);
            if (fread(str2,sizeof(char),(size_t) length+1,f) != length+1)
                exitFail("Error with \"fread\" function - copying to buffer str2");
            if (str2[0] > str1[0]){
                fseek(f,(j+1)*(length+1),0);
                if (fwrite(str2,sizeof(char),(size_t) length+1,f) != length+1)
                    exitFail("Error with \"fwrite\" function - writing from buffer str2 to file");
                j--;
                fseek(f,(j+1)*(length+1),0);
                if (fwrite(str1,sizeof(char),(size_t) length+1,f) != length+1)
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
    char *str1 = malloc((length+1) * sizeof(char));
    char *str2 = malloc((length+1) * sizeof(char));
    int j;

    for (int i=1; i<amount; i++){
        lseek(f, i*(length+1),SEEK_SET);
        if (read(f,str1,sizeof(char)*(length+1)) != length+1)
            exitFail("Error with \"fread\" function - copying to buffer str1");
        j=i-1;
        while (j>=0){
            lseek(f, j*(length+1),SEEK_SET);
            if (read(f,str2,sizeof(char)*(length+1)) != length+1)
                exitFail("Error with \"fread\" function - copying to buffer str2");
            if (str2[0] > str1[0]){
                lseek(f,(j+1)*(length+1),SEEK_SET);
                if (write(f,str2,sizeof(char)*(length+1)) != length+1)
                    exitFail("Error with \"fwrite\" function - writing from buffer str2 to file");
                j--;
                lseek(f,(j+1)*(length+1),SEEK_SET);
                if (write(f,str1,sizeof(char)*(length+1)) != length+1)
                    exitFail("Error with \"fwrite\" function - writing from buffer str1 to file");
            }
            else break;
        }

    }
    close(f);
    free(str1);
    free(str2);
}

double tDiff(clock_t end, clock_t start){
    return (double)(end-start) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char **argv){
    struct tms tmsEnd;
    struct tms tmsStart;
    char *msg =" ";
    int records=0;
    int bytes = 0;
    
    if (argc < 5){
        printf("Too few arguments");
        return 1;
    }
    if (strcmp(argv[1],"generate")==0){
        generate(argv[2],atoi(argv[3]),atoi(argv[4]));
        return 0;
    }
    times(&tmsStart);
    if (strcmp(argv[1],"sort")==0){
        records = atoi(argv[3]);
        bytes = atoi(argv[4]);
        if (strcmp(argv[5],"sys") == 0){
            sysSort(argv[2],records,bytes);
            msg = "Sortowanie SYS";
            
        }
        else if (strcmp(argv[5],"lib") == 0){
            libSort(argv[2],records,bytes);
            msg = "Sortowanie LIB";
        }
        else {
            printf("Wrong type of command given (sys|lib)\n");
            return 1;
        }
    }
    else if (strcmp(argv[1],"copy")==0){
        records = atoi(argv[4]);
        bytes = atoi(argv[5]);
        if (strcmp(argv[6],"sys") == 0){
            sysCopy(argv[2],argv[3],records,bytes);
            msg = "Kopiowanie SYS";
        }
        else if (strcmp(argv[6],"lib") == 0){
            libCopy(argv[2],argv[3],records,bytes);
            msg = "Kopiowanie LIB";
        }
        else {
            printf("Wrong type of command given (sys|lib)\n");
            return 1;
        }
    }
    else{
        exitFail("Wrong command");
    }
    times(&tmsEnd);

    FILE * file = fopen(RESULTS_FILE,"a");
    /*
    printf("\n%s   Records count: %i   Record size in bytes: %i\n",msg,records,bytes);
    printf("User:   %5.2lf\n", tDiff(tmsEnd.tms_utime,tmsStart.tms_utime));
    printf("System: %5.2lf\n", tDiff(tmsEnd.tms_stime,tmsStart.tms_stime));
    printf("-----------------------------------------------------------------------\n");
    */
    fprintf(file,"\n%s   Records count: %i   Record size in bytes: %i\n",msg,records,bytes);
    fprintf(file,"User:   %5.2lf\n", tDiff(tmsEnd.tms_utime,tmsStart.tms_utime));
    fprintf(file,"System: %5.2lf\n", tDiff(tmsEnd.tms_stime,tmsStart.tms_stime));
    fprintf(file,"-----------------------------------------------------------------------\n");

    fclose(file);

    return 0;
}