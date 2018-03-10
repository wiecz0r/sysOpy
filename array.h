#ifndef ARRAY_HEADER
#define ARRAY_HEADER

typedef struct{
    char ** array;
    size_t size;
    size_t blockSize;
    bool dynamic;
} BlockArray;


BlockArray * createArray(size_t arraySize, size_t blockSize, bool dynamic);
void deleteArray(BlockArray * array);
void addBlock(BlockArray * array, char *block);
void deleteBlock(BlockArray * array);

#endif