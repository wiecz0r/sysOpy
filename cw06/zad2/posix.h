#ifndef HEADER
#define HEADER

#define INIT    1
#define MIRROR  2
#define CALC    3
#define TIME    4
#define END     5
#define TMC     6

#define MAX_MSG_TXT 300
#define MAX_CLIENTS 10

#define PROJECT_ID 12
#define SRV_PATH "/server"

typedef struct msg_buf 
{
    long msg_type;
    char msg_text[MAX_MSG_TXT];
    int client_id;
    int client_PID;
} msg_buf;

const int MSG_SIZE = 500;

#endif