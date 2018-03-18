#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>

#define TIME_COUNT 6
#define REPEAT 1000
#define FILENAME "./results3b.txt"

#ifndef DLL
#include "array.h"
#endif

#ifdef DLL
typedef struct BlockArray{
    char ** array;
    size_t size;
    size_t blockSize;
    int dynamic;
} BlockArray;

char staticBlockArray[50000][500];
#endif

void fillBlockArray(BlockArray * bArray){
    #ifdef DLL
    void *lib = dlopen("./libarray.so",RTLD_LAZY);
    void (*addBlock)(BlockArray*, char*, int) = dlsym(lib,"addBlock");
    char* (*randomString)(size_t) = dlsym(lib,"randomString");
    #endif
    for (int i=0; i<bArray->size; i++)
        addBlock(bArray,randomString(bArray->blockSize),i);
}

void deleteAndAdd(BlockArray * myarr, int blocksNo, int startIndex){
    #ifdef DLL
    void *lib = dlopen("./libarray.so",RTLD_LAZY);
    void (*addBlock)(BlockArray*, char*, int) = dlsym(lib,"addBlock");
    void (*deleteBlock)(BlockArray*, int) = dlsym(lib,"deleteBlock");
    char* (*randomString)(size_t) = dlsym(lib,"randomString");
    #endif
    int index = startIndex;
    for (int i=0; i < blocksNo && index < myarr->size; i++){
        deleteBlock(myarr,index);
        index++;
    }
    char *block;
    for (int i=0; i < blocksNo && startIndex < myarr->size; i++){
        block = randomString(myarr->blockSize);
        addBlock(myarr,block,startIndex);
        startIndex++;
    }
    free(block);
}

void seqDeleteAndAdd(BlockArray * myarr, int blocksNo, int startIndex){
    #ifdef DLL
    void *lib = dlopen("./libarray.so",RTLD_LAZY);
    void (*addBlock)(BlockArray*, char*, int) = dlsym(lib,"addBlock");
    void (*deleteBlock)(BlockArray*, int) = dlsym(lib,"deleteBlock");
    char* (*randomString)(size_t) = dlsym(lib,"randomString");
    #endif
    char *block;
    for (int i=0; i < blocksNo && startIndex < myarr->size; i++){
        deleteBlock(myarr,startIndex);
        block = randomString(myarr->blockSize);
        addBlock(myarr,block,startIndex);
        startIndex++;
    }
}

double tDiff(clock_t end, clock_t start){
    return (double)(end-start) / sysconf(_SC_CLK_TCK);
}

void printTime(clock_t start, clock_t end, struct tms tms_start, struct tms tms_end, FILE *f, char *message){
    printf("\n%s\n",message);
    printf("Real: %lf  ", tDiff(end,start));
    printf("User: %lf  ", tDiff(tms_end.tms_utime,tms_start.tms_utime));
    printf("System: %lf\n", tDiff(tms_end.tms_stime,tms_start.tms_stime));

    fprintf(f,"\n%s\n",message);
    fprintf(f,"Real: %lf  ", tDiff(end,start));
    fprintf(f,"User: %lf  ", tDiff(tms_end.tms_utime,tms_start.tms_utime));
    fprintf(f,"System: %lf\n", tDiff(tms_end.tms_stime,tms_start.tms_stime));
}

int main(int argc, char **argv){
    #ifdef DLL
    void *lib = dlopen("./libarray.so",RTLD_LAZY);
    if(!lib){
        printf("Problem with library!");
        return 1;
    }
    BlockArray* (*createArray)(size_t, size_t, int) = dlsym(lib,"createArray");
    char* (*closestBlock) (BlockArray*, int) = dlsym(lib,"closestBlock");
    void (*deleteArray)(BlockArray *) = dlsym(lib,"deleteArray");
    #endif
    srand(time(NULL));

    if (argc < 4){
        printf("at least 3 args required:\n arraySize, blockSize, static|dynamic\n");
        return 1;
    }

    int arrSize = atoi(argv[1]);
    int blcSize = atoi(argv[2]);
    int isDynamic;

    if (strcmp(argv[3], "static")==0) isDynamic = 0;
    else if (strcmp(argv[3], "dynamic")==0) isDynamic = 1;
    else {
        printf("Wrong type of memory allocation given!\nUse \"static\" or \"dynamic\"\n");
        exit(EXIT_FAILURE);
    }

    clock_t clocks[TIME_COUNT] = {0,0,0,0,0,0};
    struct tms *tmss[TIME_COUNT];
    for (int i=0; i<TIME_COUNT;i++){
        tmss[i] = calloc(1, sizeof(struct tms *));
    }
    int curr_time = 0;

    FILE * file = fopen(FILENAME,"a");
    if (file == NULL){
        printf("Error with file!\n");
        exit(EXIT_FAILURE);
    }

    printf("Program name: %s\nArray size: %d  Block size: %d  Allocation: %s  No of repeats:  %d\n",argv[0],arrSize,blcSize,argv[3],REPEAT);
    fprintf(file,"Program name: %s\nArray size: %d  Block size: %d  Allocation: %s  No of repeats:  %d\n",argv[0],arrSize,blcSize,argv[3],REPEAT);

    //start measuring time
    clocks[curr_time] = times(tmss[curr_time]);
    curr_time++;
    //create table
    BlockArray * blockArray;
    for(int i=0; i<REPEAT; i++){
        blockArray = createArray(arrSize,blcSize,isDynamic);
        fillBlockArray(blockArray);
    }
    //interval
    clocks[curr_time] = times(tmss[curr_time]);
    curr_time++;

    printTime(clocks[0],clocks[1],*tmss[0],*tmss[1],file,"CREATING TABLE");

    char *block;
    char **message = (char **) calloc(2,sizeof(char*));
    int k = 0;
    for(int i=4; i<argc-1 && i<7; i+=2){
        int no = atoi(argv[i+1]);
        clocks[curr_time] = times(tmss[curr_time]);
        curr_time++;
        if (strcmp(argv[i],"ascii")==0){
            for(int j=0; j<REPEAT; j++){
                block=closestBlock(blockArray,no);
            }
            char *msg = "FIND BY ASCII";
            message[k] = msg;
            k++;
        }
        else if (strcmp(argv[i],"da")==0){
            for(int j=0; j<REPEAT; j++){
                deleteAndAdd(blockArray,no,0);
            }
            char *msg1 = "DELETE AND ADD";
            message[k] = msg1;
            k++;
        }
        else if (strcmp(argv[i],"sda")==0){
            for(int j=0; j<REPEAT; j++){
                seqDeleteAndAdd(blockArray,no,0);
            }
            char *msg2 = "DELETE AND ADD SEQUENTLY";
            message[k] = msg2;
            k++;
        }
        else{
            printf("\nWrong command!\n\"ascii\"-Find by ascii number\n\"da\"-Delete and add\n\"sda\"-Delete and add sequently\n");
            exit(EXIT_FAILURE);
        }
    }

    clocks[curr_time] = times(tmss[curr_time]);

    for(int i=2; i<curr_time; i++){
        printTime(clocks[i],clocks[i+1],*tmss[i],*tmss[i+1],file,message[i-2]);
    }
    free(message);

    fprintf(file,"---------------------------------------------------------------\n");
    fclose(file);
    deleteArray(blockArray);
    return 0;
}
