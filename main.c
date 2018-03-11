#include <stdio.h>
#include <stdlib.h>
#include "array.h"
#include <time.h>



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




int main(){
    srand(time(NULL));
    char *random = randomString(10);
    
}