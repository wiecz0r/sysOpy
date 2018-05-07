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

#include "posix.h"

int client_q = 0;
char client_path[128];
int server_q;
int client_id;

void generate_and_open_queues(void);
void init_client(void);
void receive_msg(msg_buf*);
void send_msg(msg_buf*,char*);
/////////////
void delete_client_queue(void){
    if (client_q == 0) return;
    if(mq_close(client_q)==-1) printf ("Error when closing client queue\n");
    else printf("Closed client queue\n");
    if(mq_unlink(client_path)==-1) printf("Error when deleting client queue\n");
    else printf("Deleted client queue!\n");
}
//////////
void int_handler(int signo){
    printf("\nReceived SIGINT, terminating CLIENT process...\n");
    exit(SIGINT);
}

int main(void){
    atexit(delete_client_queue);
    signal(SIGINT,int_handler);

    generate_and_open_queues();
    init_client();

    msg_buf msg;
    msg.client_id = client_id;
    char command[64];
    msg.client_PID=getpid();

    while(1){
        printf("\nWrite command: \n");
        fgets(command,64,stdin);
        if(strlen(command)==1) continue;
        char *type = strtok(command," \n");
        char *arg = NULL;
        char text[100];
        sprintf(text,"%s","");
        while((arg=strtok(NULL," \n"))!=NULL){
            strcat(text,arg);
            strcat(text," ");
        }
        if (strlen(text)>=1) text[strlen(text)-1]=0;
        if (strcmp(type,"mirror")==0){
            msg.msg_type = MIRROR;
            sprintf(msg.msg_text,"%s",text);
            send_msg(&msg,"MIRROR");
            receive_msg(&msg);
            printf("\n");
        }
        else if (strcmp(type,"calc")==0){
            msg.msg_type = CALC;
            sprintf(msg.msg_text,"%s",text);
            send_msg(&msg,"CALC");
            receive_msg(&msg);
        }
        else if (strcmp(type,"time")==0){
            msg.msg_type = TIME;
            sprintf(msg.msg_text,"%s","");
            send_msg(&msg,"TIME");
            receive_msg(&msg);
        }
        else if (strcmp(type,"end")==0){
            msg.msg_type = END;
            sprintf(msg.msg_text,"%s","");
            send_msg(&msg,"END");
        }
        else if (strcmp(type,"quit")==0){
            exit(EXIT_SUCCESS);
        }
        else printf ("Wrong cmd!\n");
    }


}

void generate_and_open_queues(void){
    struct mq_attr state;
    state.mq_maxmsg = 10;
    state.mq_msgsize = MSG_SIZE;

    server_q = mq_open("/server", O_WRONLY);
    if (server_q == -1){
        printf("SERVER not working. Can't open server queue.\n");
        exit(EXIT_FAILURE);
    }
    sprintf(client_path,"/%d",getpid());
    client_q = mq_open(client_path, O_RDONLY | O_CREAT | O_EXCL, 0666, &state);
    if (client_q == -1){
        perror(NULL);
        printf("Error when opening client queue from client\n");
        exit(EXIT_FAILURE);
    }
}

void init_client(void){
    msg_buf msg;
    msg.msg_type = INIT;
    msg.client_PID = getpid();
    sprintf(msg.msg_text,"%s",client_path);
    if (mq_send(server_q,(char *) &msg,MSG_SIZE,0)==-1){
        perror(NULL);
        printf("Error when sending client_key to server\n");
        exit(EXIT_FAILURE);
    }
    if (mq_receive(client_q,(char *) &msg,MSG_SIZE,NULL)==-1){
        printf("Error with INIT response from server\n");
        exit(EXIT_FAILURE);
    }
    if (msg.msg_type == TMC){
        printf("Too much clients for SERVER!\n");
        exit(TMC);
    }
    client_id = msg.client_id;
    printf("Client %d initialised. PID: %d",msg.client_id+1,getpid());
}

void receive_msg(msg_buf *msg){
    if(mq_receive(client_q,(char *) msg,MSG_SIZE,NULL)==-1){
        printf("Error when receiving message\n");
        exit(EXIT_FAILURE);
    }
    printf("%s",msg->msg_text);
}

void send_msg(msg_buf *msg,char *fname){
    if(mq_send(server_q,(char *) msg,MSG_SIZE,0)==-1){
        printf("Error when sending message from client to server: %s\n",fname);
        exit(EXIT_FAILURE);
    }
}