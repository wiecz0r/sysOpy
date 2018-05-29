#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

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

struct picture pic;
struct picture filtered_pic;
struct filter fil;
int n_threads;

void int_handler(int);
void parse_picture(char *);
void parse_filter(char *);
void *thread_action(void *);
void save_picture(char *);
void calculate_and_save_time(struct timeval[3], struct timeval[3]);
long time_diff(struct timeval, struct timeval);
void free_mem(void);



int main(int argc, char ** argv){
    if (argc < 5){
        printf("Too few arguments!\n");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT,int_handler);

    n_threads=atoi(argv[1]);

    char * filtered_name = argv[4];

    parse_picture(argv[2]);
    parse_filter(argv[3]);

    //timeval[] indexes: 0-realtime, 1-usertime, 2-systemtime
    struct timeval start[3];
    struct timeval end[3];
    struct rusage * ru = NULL;

    pthread_t * threads_id = (pthread_t*) malloc(sizeof(pthread_t) * n_threads);

    gettimeofday(&start[0],0);
    getrusage(RUSAGE_SELF, ru);
    start[1]=ru->ru_utime;
    start[2]=ru->ru_stime;

    for (int i=0; i<n_threads; i++){
        int * arg = (int *) malloc(sizeof(int));
        *arg=i;
        pthread_create(&threads_id[i],NULL,thread_action,arg);
    }
    for (int i=0; i<n_threads;i++){
        void *nth;
        pthread_join(threads_id[i],&nth);
    }

    free(threads_id);

    gettimeofday(&end[0],0);
    getrusage(RUSAGE_SELF, ru);
    end[1]=ru->ru_utime;
    end[2]=ru->ru_stime;

    save_picture(filtered_name);
    calculate_and_save_time(start, end);

    free_mem();

    return 0;
}

void int_handler(int signum){
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

void *thread_action(void * args){
    int thr_num = *(int*)args;
    for (int x=thr_num*pic.width/n_threads;x<(thr_num+1)*pic.width/n_threads;x++){
        for(int y=0;y<pic.height;y++){
            double new_val = 0;

            for (int j=0;j<fil.size;j++){
                for (int k=0;k<fil.size;k++){
                    int pos_x = MIN(MAX(0,x-ceil(fil.size/2)+j),pic.width-1);
                    int pos_y = MIN(MAX(0,y-ceil(fil.size/2)+k),pic.width-1);
                    new_val += pic.pic[pos_y][pos_x] * fil.filter[x][y];
                }
            }
            filtered_pic.pic[y][x] = (int) round(new_val); 
        }
    }
    return NULL;
}

void save_picture(char * path){
    FILE * file = fopen(path,"w");
    if (file == NULL){
        perror("Error when opening a filtered picture file");
        exit(EXIT_FAILURE);
    }
    fprintf(file,"P2\n%d %d\n255\n",filtered_pic.width,filtered_pic.height);
    for (int i=0;i<pic.height;i++){
        for (int j=0;j<pic.width;i++){
            fprintf(file,"%d ",filtered_pic.pic[i][j]);
        }
        fprintf(file,"\n");
    }
    fclose(file);
}

void calculate_and_save_time(struct timeval start[3], struct timeval end[3]){
    long real_time = time_diff(start[0],end[0]);
    long user_time = time_diff(start[1],end[1]);
    long sys_time = time_diff(start[2],end[2]);

    FILE * file = fopen("Times.txt","a");
    if (file == NULL){
        perror("Error when opening a \"Times.txt\" file");
        exit(EXIT_FAILURE);
    }
    fprintf(file,"Number of threads: %d\n",n_threads);
    fprintf(file,"Picture size: %dx%d\n",pic.width,pic.height);
    fprintf(file,"Filter size: %dx%d\n",fil.size,fil.size);
    fprintf(file,"-----TIME INFO-----\n");
    fprintf(file,"Real: %-10ld\nUser: %-20ld\nSystem: %-20ld\n\n",real_time,user_time,sys_time);
    fclose(file);
}

long time_diff(struct timeval start, struct timeval end){
    return (end.tv_sec-start.tv_sec) * 1000000 + (end.tv_usec-start.tv_usec);
}

void free_mem(void){
    for (int i=0; i<pic.height;i++){
        free(pic.pic[i]);
        free(filtered_pic.pic[i]);
    }
    free(pic.pic);
    free(filtered_pic.pic);
    for (int i=0; i<fil.size;i++){
        free(fil.filter[i]);
    }
    free(fil.filter);
}