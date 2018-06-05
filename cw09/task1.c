#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/resource.h>
#include <unistd.h>
#include <string.h>

#define LINE_MAX_SIZE 256

int P, K, N, L, nk, verbose;
char search_type, filename[FILENAME_MAX];
char **buffer;
int P_index=0, K_index=0, P_term=0;
pthread_mutex_t P_mutex, K_mutex, *buffer_mutex;
pthread_cond_t P_cond, K_cond;
pthread_t *P_threads, *K_threads;

void handler(int signum){
    printf("Received signal!\nCancelling all threads...\n");
    for(int i=0; i<P; i++){
        pthread_cancel(P_threads[i]);
    }
    for(int i=0; i<K; i++){
        pthread_cancel(K_threads[i]);
    }
    printf("All threads cancelled!\nTerminating...\n");
    exit(EXIT_SUCCESS);
}

void read_config(char*);
FILE *allocate_and_initialize();
void *producer(void*);
void *consumer(void*);


int main(int argc, char **argv){
    if(argc<2){
        printf("Please give path to the config file!\n");
        exit(EXIT_FAILURE);
    }
    read_config(argv[1]);

    if(nk > 0){
        signal(SIGALRM,handler);
    }
    else if (nk < 0){
        printf("Wrong \"nk\" number: it must be >= 0\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = allocate_and_initialize();

    if (nk > 0) alarm(nk);
    usleep(5000);
    for (int i=0; i<P; i++){
        pthread_create(&P_threads[i], NULL, producer, file);
    }
    for (int i=0; i<K; i++){
        pthread_create(&K_threads[i], NULL, consumer, NULL);
    }

    for (int i=0; i<P; i++){
        pthread_join(P_threads[i], NULL);
    }
    P_term = 1;
    pthread_cond_broadcast(&K_cond);
    for (int i=0; i<K; i++){
        pthread_join(K_threads[i], NULL);
    }
    //close file and destroy all mutexes and conditions
    fclose(file);

    for (int i=0; i<N; i++){
        if (buffer[i]) free(buffer[i]);
        pthread_mutex_destroy(&buffer_mutex[i]);
    }
    free(buffer);
    free(buffer_mutex);
    pthread_mutex_destroy(&P_mutex);
    pthread_mutex_destroy(&K_mutex);
    pthread_cond_destroy(&P_cond);
    pthread_cond_destroy(&K_cond);
    sleep(5);
    return 0;
}

void read_config(char *path){
    FILE *config_file = fopen(path,"r");
    if(config_file==NULL){
        perror("Error while opening the config file!\n");
        exit(EXIT_FAILURE);
    }

    fscanf(config_file,"%d\n%d\n%d\n%s\n%d\n%c\n%d\n%d",&P,&K,&N,filename,&L,&search_type,&verbose,&nk);
    printf("Variables:\nP: %d, K: %d, N: %d,\nFilename: %s, L: %d,\nSearch type: %c, Verbose: %d,\nnk: %d\n",P,K,N,filename,L,search_type,verbose,nk);
    fclose(config_file);
}

FILE *allocate_and_initialize(){
    FILE * file = fopen(filename,"r");
    if(file == NULL){
        perror("Error while opening a file to be read!\n");
        exit(EXIT_FAILURE);
    }
    buffer = malloc(N * sizeof(char *));
    buffer_mutex = malloc(N * sizeof(pthread_mutex_t));

    P_threads = malloc(P * sizeof(pthread_t));
    K_threads = malloc(K * sizeof(pthread_t));

    for(int i=0; i<N; i++){
        pthread_mutex_init(&buffer_mutex[i], NULL);
    }
    pthread_mutex_init(&P_mutex, NULL);
    pthread_mutex_init(&K_mutex, NULL);

    pthread_cond_init(&P_cond, NULL);
    pthread_cond_init(&K_cond, NULL);

    return file;
}

void *producer(void *args){
    //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    FILE *file = (FILE*) args;
    char line[LINE_MAX_SIZE];
    int index;
    usleep(1000);

    while(fgets(line, LINE_MAX_SIZE, file) != NULL){
        pthread_mutex_lock(&P_mutex);
        if(verbose){
            printf("Producer [TID: %d] is processing file line.\n",(int) pthread_self());
        }

        while(buffer[P_index] != NULL){
            pthread_cond_wait(&P_cond, &P_mutex);
        }
        if(verbose){
            printf("Producer [TID: %d] is taking last used buffer index.\n",(int) pthread_self());
        }
        index=P_index;
        P_index = (P_index + 1) % N;

        pthread_mutex_lock(&buffer_mutex[index]);
        buffer[index] = malloc((strlen(line)+1) * sizeof(char));
        strcpy(buffer[index], line);
        if(verbose){
            printf("Producer [TID: %d] just copied line from \"%s\".\n",(int) pthread_self(),filename);
        }

        pthread_cond_broadcast(&K_cond);
        pthread_mutex_unlock(&buffer_mutex[index]);
        pthread_mutex_unlock(&P_mutex);
        if(verbose){
            printf("Producer [TID: %d] just finished his job!\n",(int) pthread_self());
        }
        usleep(20);
    }
    return NULL;
}

void *consumer(void *args){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int index;

    while(1){
        pthread_mutex_lock(&K_mutex);
        while(buffer[K_index]==NULL){
            if(P_term){
                pthread_mutex_unlock(&K_mutex);
                if(verbose){
                    printf("Consumer [TID: %d] is terminating.\n",(int) pthread_self());
                }
                return NULL;
            }
            pthread_cond_wait(&K_cond,&K_mutex);
        }
        if(verbose){
            printf("Consumer [TID: %d] is taking last used buffer index.\n",(int) pthread_self());
        }
        index=K_index;
        K_index = (K_index + 1) % N;

        pthread_mutex_lock(&buffer_mutex[index]);
        pthread_mutex_unlock(&K_mutex);
        char *line = buffer[index];
        buffer[index]=NULL;
        
        if(verbose){
            printf("Consumer [TID: %d] just took line from buffer (index: %d).\n",(int) pthread_self(),index);
        }
        pthread_cond_broadcast(&P_cond);
        pthread_mutex_unlock(&buffer_mutex[index]);
        usleep(20);
        //free(line);
    }
}
