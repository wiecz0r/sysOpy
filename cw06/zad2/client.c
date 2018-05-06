#define _GNU_SOURCE

#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>

#include "sV.h"

int client_q;
int server_q;
int client_id;

key_t generate_and_open_queues(void);
void init_client(key_t);
void receive_msg(msg_buf*);
void send_msg(msg_buf*,char*);
/////////////
void delete_client_queue(void){
    if(msgctl(client_q, IPC_RMID,NULL)==-1)
        printf("Error when deleting client queue\n");
    else printf("Deleted client queue\n");
}
//////////
void int_handler(int signo){
    printf("\nReceived SIGINT, terminating CLIENT process...\n");
    exit(SIGINT);
}

int main(void){
    atexit(delete_client_queue);
    signal(SIGINT,int_handler);

    init_client(generate_and_open_queues());

    msg_buf msg;
    msg.client_id = client_id;
    char command[64];
    msg.client_PID=getpid();

    while(1){
        printf("\nWrite command: \n");
        fgets(command,64,stdin);
        char *type = strtok(command," \n");
        char *cmd2 = strtok(NULL," \n");
        if (strcmp(type,"mirror")==0){
            msg.msg_type = MIRROR;
            sprintf(msg.msg_text,"%s",cmd2);
            send_msg(&msg,"MIRROR");
            receive_msg(&msg);
        }
        else if (strcmp(type,"calc")==0){
            msg.msg_type = CALC;
            sprintf(msg.msg_text,"%s",cmd2);
            send_msg(&msg,"CALC");
            receive_msg(&msg);
        }
        else if (strcmp(type,"time")==0){
            msg.msg_type = TIME;
            sprintf(msg.msg_text,"");
            send_msg(&msg,"TIME");
            receive_msg(&msg);
        }
        else if (strcmp(type,"end")==0){
            msg.msg_type = END;
            sprintf(msg.msg_text,"");
            send_msg(&msg,"END");
        }
        else if (strcmp(type,"quit")==0){
            exit(EXIT_SUCCESS);
        }
        else printf ("Wrong cmd!\n");
    }


}

key_t generate_and_open_queues(void){
    key_t server_key = ftok(getenv("HOME"),PROJECT_ID);

    if (server_key == -1){
        printf("Error while generating server queue key!\n");
        exit(EXIT_FAILURE);
    }

    server_q = msgget(server_key,0);
    if (server_q == -1){
        printf("Error when opening server queue from client\n");
        exit(EXIT_FAILURE);
    }

    key_t client_key = ftok(getenv("HOME"),getpid());
    if (client_key == -1){
        printf("Error while generating client queue key!\n");
        exit(EXIT_FAILURE);
    }

    client_q = msgget(client_key,IPC_CREAT | IPC_EXCL | 0666);
    if (client_q == -1){
        perror(NULL);
        printf("Error when opening client queue from client\n");
        exit(EXIT_FAILURE);
    }
    return client_key;
}

void init_client(key_t client_key){
    msg_buf msg;
    msg.msg_type = INIT;
    msg.client_PID = getpid();
    sprintf(msg.msg_text,"%d",client_key);
    if (msgsnd(server_q,&msg,MSG_SIZE,0)==-1){
        perror(NULL);
        printf("Error when sending client_key to server\n");
        exit(EXIT_FAILURE);
    }
    if (msgrcv(client_q,&msg,MSG_SIZE,0,0)==-1){
        printf("Error with INIT response from server\n");
        exit(EXIT_FAILURE);
    }
    client_id = msg.client_id;
    printf("Client initialised. PID: %d",getpid());
}

void receive_msg(msg_buf *msg){
    if(msgrcv(client_q,msg,MSG_SIZE,0,0)==-1){
        printf("Error when receiving message\n");
        exit(EXIT_FAILURE);
    }
    printf("%s\n",msg->msg_text);
}

void send_msg(msg_buf *msg,char *fname){
    if(msgsnd(server_q,msg,MSG_SIZE,0)==-1){
        printf("Error when sending message from client to server: %s\n",fname);
        exit(EXIT_FAILURE);
    }
}