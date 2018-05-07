#define _GNU_SOURCE

#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "posix.h"

#include <errno.h>

int end = 0;
int clients_q[MAX_CLIENTS];
int clients_PID[MAX_CLIENTS];

int id = 0;
int server_q;

void init(msg_buf*);
void mirror(msg_buf*);
void timef(msg_buf*);
void calcf(msg_buf*);

void delete_server_queue(void){
    for (int i=0; i<id;i++){
        if(mq_close(clients_q[id])==-1) printf("Error when closing queue ID: %d\n",i);
        if(kill(clients_PID[i],SIGINT)==-1) printf("Error when trying to kill client! PID: %d\n",clients_PID[i]);
    }
    if(mq_close(server_q)==-1) printf ("Error when closing server queue\n");
    else printf("Closed server queue\n");
    if(mq_unlink(SRV_PATH)==-1) printf("Error when deleting server queue\n");
    else printf("Deleted server queue!\n");
}

void int_handler(int signo){
    printf("\nReceived SIGINT, terminating SERVER process...\n");
    exit(SIGINT);
}

int main(void){
    atexit(delete_server_queue);
    signal(SIGINT,int_handler);

    struct mq_attr state;
    state.mq_maxmsg = 10;
    state.mq_msgsize = MSG_SIZE;

    
    server_q = mq_open(SRV_PATH, O_RDONLY | O_CREAT | O_EXCL, 0666, &state);
    if (server_q == -1){
        printf("Error while making server queue!\n");
        exit(EXIT_FAILURE);
    }
    printf("Server queue: %s\n",SRV_PATH);

    msg_buf msg;
    struct mq_attr stat;
    while(1){
        if (mq_getattr(server_q,&stat) == -1){
            perror(NULL);
            printf("Error when getting current state of server queue\n");
            exit(EXIT_FAILURE);
        }
        if (end && stat.mq_curmsgs == 0) exit(EXIT_SUCCESS);
        if (stat.mq_curmsgs == 0){
            continue;
        }
        if (mq_receive(server_q,(char*) &msg, MSG_SIZE, NULL) < 0){
            printf("Receiving message from SERVER queue failed!\n");
            exit(EXIT_FAILURE);
        }
        printf("Received message!\n Type: %ld\n Text: %s\n",msg.msg_type, msg.msg_text);
        switch(msg.msg_type){
            case INIT:
                init(&msg);
                break;
            case MIRROR:
                mirror(&msg);
                break;
            case CALC:
                calcf(&msg);
                break;
            case TIME:
                timef(&msg);
                break;
            case END:
                end = 1;
                break;
            default:
                printf("Error! Received unknown msg!\n");
                break;
        }
    }
    return 0;
}

void init(msg_buf *msg){
    msg->msg_type=msg->client_PID;
    msg->client_id = id;
   
    char path[64];
    sprintf(path,"/%d",msg->client_PID);
    int client_queue_id = mq_open(path,O_WRONLY);

    if (id==MAX_CLIENTS-1){
        msg->msg_type=TMC;
        if(mq_send(client_queue_id,(char *)msg,MSG_SIZE,0)==-1){
            printf("Error when trying to respond from INIT func!\n");
            exit(EXIT_FAILURE);
        }
        end=1;
        return;
    }

    clients_q[id] = client_queue_id;
    clients_PID[id] = msg->client_PID;

    sprintf(msg->msg_text, "%d", id);
    id++;
    if(mq_send(client_queue_id,(char *)msg,MSG_SIZE,0)==-1){
        printf("Error when trying to respond from INIT func!\n");
        exit(EXIT_FAILURE);
    }

}

void mirror(msg_buf *msg){
    int client_q = clients_q[msg->client_id];
    msg->msg_type=msg->client_PID;

    int length = (int)strlen(msg->msg_text);
    char tmp;
    for (int i = 0; i < length/2; ++i)
    {
        tmp = msg->msg_text[i];
        msg->msg_text[i] = msg->msg_text[length-i-1];
        msg->msg_text[length-i-1] = tmp;
    }
    if(mq_send(client_q,(char *) msg,MSG_SIZE,1)==-1){
        printf("Error when trying to respond from MIRROR func!\n");
        exit(EXIT_FAILURE);
    }

}

void timef(msg_buf *msg){
    int client_q = clients_q[msg->client_id];
    msg->msg_type=msg->client_PID;
    
    FILE *date = popen("date","r");
    fgets(msg->msg_text,MAX_MSG_TXT,date);
    pclose(date);

    if(mq_send(client_q,(char *) msg,MSG_SIZE,1)==-1){
        printf("Error when trying to respond from TIME func!\n");
        exit(EXIT_FAILURE);
    }
}

void calcf(msg_buf *msg){
    int client_q = clients_q[msg->client_id];
    msg->msg_type=msg->client_PID;

    char buff[MAX_MSG_TXT+12];
    sprintf(buff,"echo '%s' | bc",msg->msg_text);
    FILE * run = popen(buff,"r");
    fgets(msg->msg_text,MAX_MSG_TXT,run);
    pclose(run);

    if(mq_send(client_q,(char *) msg,MSG_SIZE,1)==-1){
        printf("Error when trying to respond from CALC func!\n");
        exit(EXIT_FAILURE);
    }
}

