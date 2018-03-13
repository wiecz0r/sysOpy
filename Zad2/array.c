#include "array.h"

char staticBlockArray[50000][500];

BlockArray * createArray(size_t arraySize, size_t blockSize, int dynamic){
    BlockArray *myArray = (BlockArray *) calloc(1, sizeof(BlockArray));
    if (dynamic) {
        char **arr = (char **) calloc(arraySize, sizeof(char *));
        myArray->array = arr;
    }
    else
        myArray->array =(char **) staticBlockArray;
    myArray->size=arraySize;
    myArray->blockSize=blockSize;
    myArray->dynamic=dynamic;
    return myArray;
}
void deleteArray(BlockArray * array){
    if (array == NULL) return;
    if (array->dynamic) {
        for (int i = 0; i < array->size; i++) {
            free(array->array[i]);
        }
    }
    else {
        for (int i=0; i< array->size;i++){
            array->array[i]=NULL;
        }
    }
    free(array->array);
    free(array);
}

void addBlock(BlockArray * array, char *block, int index){
    if (array == NULL) return;
    if (index >= array->size)
        printf("Index out of range\n");
    else if (strlen(block)>=array->blockSize)
        printf("Length of string is too big!\n");
    else {
        if (!array->dynamic)
            array->array[index]=block;
        else {
            if (array->array[index] != NULL) printf("Item with this index already exists in array. Cannot add.\n");
            array->array[index] = (char *) calloc(array->blockSize, sizeof(char));
            strcpy(array->array[index], block);
        }
    }
}

void deleteBlock(BlockArray * array, int index){
    if (array == NULL || array->array == NULL || array->array[index] == NULL) return;
    if (!array->dynamic)
        array->array[index] = NULL;
    else {
        free(array->array[index]);
        array->array[index] = NULL;
    }
}

int getASCIIvalue (char *block){
    int sum = 0;
    for (int i=0;i<strlen(block);i++){
        sum += (int) block[i];
    }
    return sum;
}

char * closestBlock (BlockArray * bArr, int value){
    char * closest = NULL;
    int minDiff = INT_MAX;
    char * currBlock;
    for (int i=0; i < bArr->size; i++){
        currBlock = bArr->array[i];
        if (currBlock == NULL) continue;
        int diff = abs(getASCIIvalue(currBlock)-value);
        if(diff < minDiff){
            minDiff = diff;
            closest = currBlock;
        }
    }
    return closest;
}

void printArray(BlockArray * myArr){
    if (myArr == NULL){
        printf("No array");
        return;
    }
    for (int i=0; i<myArr->size; i++){
        if(myArr->array[i] == NULL) continue;
        printf("%s  %d \n", myArr->array[i], i);
    }
}
