#define _GNU_SOURCE

#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "sV.h"

int end = 0;
int clients[MAX_CLIENTS];
int id = 0;
int server_q;

void init(msg_buf*);
void mirror(msg_buf*);
void timef(msg_buf*);

void delete_server_queue(){
    msgctl(server_q, IPC_RMID,NULL);
    printf("Deleted server queue\n");
}

int main(){
    atexit(delete_server_queue);
    
    key_t queue_key = ftok(getenv("HOME"),0);
    if (queue_key == -1){
        printf("Error while generating queue key!\n");
        exit(EXIT_FAILURE);
    }
    server_q = msgget(queue_key, IPC_CREAT | IPC_EXCL);
    if (server_q == -1){
        printf("Error while making server queue!\n");
        exit(EXIT_FAILURE);
    }

    msg_buf msg;
    struct msqid_ds stat;

    while(1){
        if(end){
            if (msgctl(server_q,IPC_STAT,&stat) == -1){
                printf("Error when getting current state of server queue\n");
                exit(EXIT_FAILURE);
            }
            if (stat.msg_qnum == 0) break;
        }
        if (msgrcv(server_q,&msg,MSG_SIZE,0,0) < 0){
            printf("Error when receiving msg from client!\n");
            exit(EXIT_FAILURE);
        }
        switch(msg.msg_type){
            case INIT:
                init(&msg);
                break;
            case MIRROR:
                mirror(&msg);
                break;
            case CALC:
                //calcf(&msg);
                break;
            case TIME:
                timef(&msg);
                break;
            case END:
                end = 1;
                break;
            default:
                printf("Error! Received unknown msg!\n");
                exit(EXIT_FAILURE);
        }
    }
    return 0;
}

void init(msg_buf *msg){
    msg->client_id = id;
    key_t client_queue_key =(key_t)strtol(msg->msg_text,NULL,10);
    int client_queue_id = msgget(client_queue_key,0);
    clients[id] = client_queue_id;
    sprintf(msg->msg_text, "%d", id);
    if(msgsnd(client_queue_id,msg,MSG_SIZE,0)==-1){
        printf("Error when trying to respond from INIT func!\n");
        exit(EXIT_FAILURE);
    }
    id++;

}

void mirror(msg_buf *msg){
    int client_q = clients[msg->client_id];

    int length = (int)strlen(msg->msg_text);
    char tmp;
    for (int i = 0; i < length/2; ++i)
    {
        tmp = msg->msg_text[i];
        msg->msg_text[i] = msg->msg_text[length-i-1];
        msg->msg_text[length-i-1] = tmp;
    }
    if(msgsnd(client_q,msg,MSG_SIZE,0)==-1){
        printf("Error when trying to respond from MIRROR func!\n");
        exit(EXIT_FAILURE);
    }

}

void timef(msg_buf *msg){
    int client_q = clients[msg->client_id];
    time_t actualtime = time(NULL);
    strcpy(msg->msg_text,ctime(&actualtime));
    if(msgsnd(client_q,msg,MSG_SIZE,0)==-1){
        printf("Error when trying to respond from TIME func!\n");
        exit(EXIT_FAILURE);
    }
}

