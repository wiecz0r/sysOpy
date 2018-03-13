#include <stdio.h>
#include <stdlib.h>
#include "array.h"
#include <time.h>
#include <sys/times.h>
#include <unistd.h>

#define TIME_COUNT 6
#define REPEAT 1000
#define FILENAME "./raport2a.txt"

#ifndef DLL
void fillBlockArray(BlockArray * bArray);
void deleteAndAdd(BlockArray * myarr, int blocksNo, int startIndex);
void seqDeleteAndAdd(BlockArray * myarr, int blocksNo, int startIndex);
double tDiff(clock_t end, clock_t start);
void printTime(clock_t start, clock_t end, struct tms tms_start, struct tms tms_end, FILE *f, char *message);
#endif

int main(int argc, char **argv){
    srand(time(NULL));

    if (argc < 4){
        printf("at least 3 args required:\n arraySize, blockSize, static|dynamic\n");
        exit(EXIT_FAILURE);
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

    for (int i=0; i<TIME_COUNT;i++){
        free(tmss[i]);
    }
    fprintf(file,"---------------------------------------------------------------\n");
    fclose(file);

    return 0;
}
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////////////////////////////////////////////////////////
void fillBlockArray(BlockArray * bArray){
    for (int i=0; i<bArray->size; i++)
        addBlock(bArray,randomString(bArray->blockSize),i);
}

void deleteAndAdd(BlockArray * myarr, int blocksNo, int startIndex){
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
//do zmiany!!!!!!!!!!
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