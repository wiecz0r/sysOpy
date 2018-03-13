#include <stdio.h>
#include <stdlib.h>
#include "array.h"
#include <time.h>
#include <sys/times.h>
#include <unistd.h>

#define TIME_COUNT 6

char *randomString(size_t maxBlockSize){
    if (maxBlockSize<1) return NULL;
    char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,/?!@#$^&*()_+";
    int outStrLen = (rand() % strlen(charset)) % maxBlockSize;
    char *outString = (char *) malloc( (size_t) (outStrLen + 1) * sizeof(char) );
    for (int i=0; i< outStrLen; i++){
        outString[i] = charset[ rand() % strlen(charset) ];
    }
    outString[outStrLen]='\0';
    return outString;
}

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

double tDiff(clock_t start, clock_t end){
    return (double)(end-start) / sysconf(_SC_CLK_TCK);
}

void printTime(clock_t *clocks, struct tms *tmss, char *message, FILE *f, int index){
    printf("%s\n",message);
    printf("Real:   %lf  ", tDiff(clocks[index+1], clocks[index]));
    printf("User:   %lf  ", tDiff(tmss[index+1].tms_utime,tmss[index].tms_utime));
    printf("System: %lf\n\n", tDiff(tmss[index+1].tms_stime,tmss[index].tms_stime));

    fprintf(f,"%s\n",message);
    fprintf(f,"Real:   %lf  ", tDiff(clocks[index+1], clocks[index]));
    fprintf(f,"User:   %lf  ", tDiff(tmss[index+1].tms_utime,tmss[index].tms_utime));
    fprintf(f,"System: %lf\n\n", tDiff(tmss[index+1].tms_stime,tmss[index].tms_stime));
}


int main(int argc, char **argv){
    srand(time(NULL));

    int arrSize = atoi(argv[1]);
    int blcSize = atoi(argv[2]);
    int isDynamic;

    if (argc < 4){
        printf("at least 3 args required:\n arraySize, blockSize, s|d (static|dynamic)\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[3], "s")==0) isDynamic = 1;
    else if (strcmp(argv[3], "d")==0) isDynamic = 0;
    else {
        printf("Wrong type of memory allocation given!\nUse \"s\" for static or \"d\" for dynamic\n");
        exit(EXIT_FAILURE);
    }

    clock_t clocks[TIME_COUNT] = {0,0,0,0,0,0};
    struct tms *tmss[TIME_COUNT];
    for (int i=0; i<TIME_COUNT;i++){
        tmss[i] = calloc(1, sizeof(struct tms *));
    }
    int curr_time = 0;

    FILE * file = fopen("./raport2a.txt","a");
    if (file == NULL){
        printf("Error with file!\n");
        exit(EXIT_FAILURE);
    }

    printf("Array size: %d  Block size: %d  Allocation:  %s\n",arrSize,blcSize,argv[3]);
    fprintf(file,"Array size: %d  Block size: %d  Allocation:  %s\n",arrSize,blcSize,argv[3]);

    //start measuring time
    clocks[curr_time] = times(tmss[curr_time]);
    curr_time++;
    //create table
    BlockArray * blockArray;
    blockArray = createArray(arrSize,blcSize,isDynamic);
    fillBlockArray(blockArray);
    //interval
    clocks[curr_time] = times(tmss[curr_time]);
    curr_time++;

    printTime(clocks,*tmss,"CREATING TABLE", file, 0);








    
    
}