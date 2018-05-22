#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define MAX(X, Y) (X > Y ? X : Y)
#define MIN(X, Y) (X < Y ? X : Y)

struct picture {
    int ** pic;
    int width;
    int height;
    int range;
}picture;

struct filter {
    float ** filter;
    int size;
}filter;

struct threads_info {
    int n_threads;
    int threads_done;

}threads_info;

struct picture pic;
struct picture filtered_pic;
struct filter fil;
struct threads_info thr_info;

int main(int argc, char ** argv){
    if (argc < 5){
        printf("Too few arguments!\n");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT,int_handler);

    thr_info.n_threads=atoi(argv[1]);
    thr_info.threads_done=0;

    char * filtered_name = argv[4];

    parse_picture(argv[2]);
    parse_filter(argv[3]);

    


    return 0;
}

void * int_handler(int signum){
    //...
}


void parse_picture(char * path){
    FILE * file = fopen(path,"r");
    if (file == NULL){
        perror("Error when opening a picture file");
        exit(EXIT_FAILURE);
    }

    fscanf(file,"P2\n");
    fscanf(file, "%d*[ ]", &pic.width);
    fscanf(file, "%d*[ \n]", &pic.height);
    fscanf(file, "%d*[ \n]", &pic.range);
    filtered_pic = pic;

    pic.pic = (int **) malloc(sizeof(int*) * pic.height);
    filtered_pic.pic = (int **) malloc(sizeof(int*) * filtered_pic.height);

    for(int i=0; i<pic.height; i++){
        pic.pic[i] = (int *) malloc(sizeof(int) * pic.width);
        filtered_pic.pic[i] = (int *) malloc(sizeof(int) * filtered_pic.width);
        for(int j=0; j<pic.width; i++){
            fscanf(file, "%d*[ \n]", &pic.pic[i][j]);
        }
    }
    fclose(file);
}

void parse_filter(char * path){
    FILE * file = fopen(path,"r");
    if (file == NULL){
        perror("Error when opening a picture file");
        exit(EXIT_FAILURE);
    }

    fscanf(file,"%d*[ \n]",&fil.size);

    fil.filter = (float **) malloc(sizeof(float*) * fil.size);

    for(int i=0; i<fil.size; i++){
        fil.filter[i] = (float *) malloc(sizeof(float) * fil.size);
        for(int j=0; j<fil.size; i++){
            fscanf(file, "%f[ \n]", &fil.filter[i][j]);
        }
    }
    fclose(file);
}