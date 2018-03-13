#ifndef ARRAY_HEADER
#define ARRAY_HEADER

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct BlockArray{
    char ** array;
    size_t size;
    size_t blockSize;
    int dynamic;
} BlockArray;

char staticBlockArray[1000][200];

BlockArray * createArray(size_t arraySize, size_t blockSize, int dynamic);
void deleteArray(BlockArray * array);
void addBlock(BlockArray * array, char *block, int index);
void deleteBlock(BlockArray * array, int index);
int getASCIIvalue (char *block);
char * closestBlock (BlockArray * bArr, int value);
void printArray(BlockArray * myArr);


#endif