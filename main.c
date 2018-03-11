#include <stdio.h>
#include <stdlib.h>
#include "array.h"


int main(){
    BlockArray *myArray = createArray(100,12,0);
    printf("%d\n",(int) myArray->size);
    addBlock(myArray, "alamakota",0);
    addBlock(myArray,"akotmaale", 1);
    //printf("%s\n", closestBlock(myArray, 120));
    printArray(myArray);
    return 0;
}