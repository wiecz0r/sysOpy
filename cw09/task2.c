#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

#define _BSD_SOURCE
#define LINE_MAX_SIZE 256
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"


int P, K, N, L, nk, verbose;
char search_type, filename[FILENAME_MAX];
char **buffer;
int P_index=0, K_index=0, P_term=0;
sem_t P_sem, K_sem, *buffer_sem, P_wait, K_wait;
pthread_t *P_threads, *K_threads;
int L_count=0;

void handler(int);
void read_config(char*);
FILE *allocate_and_initialize();
void *producer(void*);
void *consumer(void*);
void length_checker(char *);


int main(int argc, char **argv){
    if(argc<2){
        printf(RED"Please give path to the config file!\n");
        exit(EXIT_FAILURE);
    }
    read_config(argv[1]);
    sleep(2);

    if(nk > 0){
        signal(SIGALRM,handler);
    }
    else if (nk == 0){
        signal(SIGINT,handler);
    }
    else if (nk < 0){
        printf(RED"Wrong \"nk\" number: it must be >= 0\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = allocate_and_initialize();

    if (nk > 0) alarm(nk);
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
    sem_post(&K_wait);
    for (int i=0; i<K; i++){
        pthread_join(K_threads[i], NULL);
    }
    //close file and destroy all mutexes and conditions
    printf(YELLOW"All threads terminated. Closing file and destroying mutexes.\nTerminating program...\n"RESET);
    fclose(file);

    for (int i=0; i<N; i++){
        if (buffer[i]) free(buffer[i]);
        sem_destroy(&buffer_sem[i]);
    }
    free(buffer);
    free(buffer_sem);
    sem_destroy(&P_sem);
    sem_destroy(&K_sem);
    sem_destroy(&P_wait);
    sem_destroy(&K_wait);
    printf("Terminated\n");
    return 0;
}

void read_config(char *path){
    FILE *config_file = fopen(path,"r");
    if(config_file==NULL){
        perror(RED"Error while opening the config file!\n");
        exit(EXIT_FAILURE);
    }

    fscanf(config_file,"%d\n%d\n%d\n%s\n%d\n%c\n%d\n%d",&P,&K,&N,filename,&L,&search_type,&verbose,&nk);
    printf(BLUE"Variables:\nP: %d, K: %d, N: %d,\nFilename: %s, L: %d,\nSearch type: %c, Verbose: %d,\nnk: %d\n"RESET,P,K,N,filename,L,search_type,verbose,nk);
    fclose(config_file);
}

FILE *allocate_and_initialize(){
    FILE * file = fopen(filename,"r");
    if(file == NULL){
        perror(RED"Error while opening a file to be read!\n");
        exit(EXIT_FAILURE);
    }
    buffer = malloc(N * sizeof(char *));
    buffer_sem = malloc(N * sizeof(sem_t));

    P_threads = malloc(P * sizeof(pthread_t));
    K_threads = malloc(K * sizeof(pthread_t));

    for(int i=0; i<N; i++){
        sem_init(&buffer_sem[i], 0, 1);
    }
    sem_init(&P_sem, 0, 1);
    sem_init(&K_sem, 0, 1);
    sem_init(&K_wait, 0, 0);
    sem_init(&P_wait, 0, N);

    //pthread_cond_init(&P_cond, NULL);
    //pthread_cond_init(&K_cond, NULL);

    return file;
}

void *producer(void *args){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    FILE *file = (FILE*) args;
    char line[LINE_MAX_SIZE];
    int index;

    while(fgets(line, LINE_MAX_SIZE, file) != NULL){
        sem_wait(&P_sem);
        if(verbose){
            printf("Producer [TID: %ld] is processing file line.\n",(long) pthread_self());
        }

        /*while(buffer[P_index] != NULL){
            pthread_cond_wait(&P_cond, &P_sem);
        }*/
        sem_wait(&P_wait);

        if(verbose){
            printf("Producer [TID: %ld] is taking last used buffer index.\n",(long) pthread_self());
        }
        index=P_index;
        P_index = (P_index + 1) % N;

        sem_wait(&buffer_sem[index]);
        buffer[index] = malloc((strlen(line)+1) * sizeof(char));
        strcpy(buffer[index], line);
        if(verbose){
            printf("Producer [TID: %ld] just copied line from \"%s\" to buffer (index: %d).\n",(long) pthread_self(),filename,index);
        }

        //pthread_cond_broadcast(&K_cond);
        sem_post(&buffer_sem[index]);
        sem_post(&P_sem);
        sem_post(&K_wait);
        if(verbose){
            printf("Producer [TID: %ld] just finished his job!\n",(long) pthread_self());
        }
    }
    if(verbose){
        printf(MAGENTA"Producer [TID: %ld] is terminating\n"RESET,(long) pthread_self());
    }
    return NULL;
}

void *consumer(void *args){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int index;

    while(1){
        sem_wait(&K_sem);
        //while(buffer[K_index]==NULL){
        
        if(P_term){
            sem_post(&K_wait);
            sem_post(&K_sem);
            if(verbose){
                printf(MAGENTA"Consumer [TID: %ld] is terminating.\n"RESET,(long) pthread_self());
            }
            return NULL;
        }
        sem_wait(&K_wait);
            //pthread_cond_wait(&K_cond,&K_sem);
        //}
        if(verbose){
            printf("Consumer [TID: %ld] is taking last used buffer index.\n",(long) pthread_self());
        }
        index=K_index;
        K_index = (K_index + 1) % N;

        sem_wait(&buffer_sem[index]);
        
        
        char *line = buffer[index];
        buffer[index]=NULL;
        
        if(verbose){
            printf("Consumer [TID: %ld] just took line from buffer (index: %d).\n",(long) pthread_self(),index);
        }

        length_checker(line);
        sem_post(&buffer_sem[index]);
        sem_post(&K_sem);
        sem_post(&P_wait);//pthread_cond_broadcast(&P_cond);
        
        free(line);
    }
}

void handler(int signum){
    sem_wait(&P_sem);
    sem_wait(&K_sem);
    printf("Received %s !\nCancelling all threads...\n",signum == SIGINT ? "SIGINT" : "SIGALRM");
    for(int i=0; i<P; i++){
        pthread_cancel(P_threads[i]);
    }
    for(int i=0; i<K; i++){
        pthread_cancel(K_threads[i]);
    }
    printf("All threads cancelled!\nTerminating...\n");
    exit(EXIT_SUCCESS);
}

void length_checker(char *line){
    int ok_len = 0;
    switch (search_type){
        case '<':
            if (strlen(line)<L){
                ok_len = 1;
            }
            break;
        case '=':
            if (strlen(line)==L){
                ok_len = 1;
            }
            break;
        case '>':
            if (strlen(line)>L){
                ok_len = 1;
            }
            break;
        default:
            printf(RED"Wrong search type: %c. (should be one of following: '<' '=' '>')\n",search_type);
            exit(EXIT_FAILURE);
    }
    if (ok_len) {
        L_count++;
        printf(GREEN"Consumer "YELLOW"[TID: %ld]"GREEN" just found line which length is %c %d.\n"RESET,(long) pthread_self(),search_type,L);
        fflush(stdout);
    }
}
