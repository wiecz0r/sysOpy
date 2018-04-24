#ifndef HEADER
#define HEADER

#define MIRROR  1
#define CALC    2
#define TIME    3
#define END     4
#define INIT    5
#define STOP    6
#define RPLY    7
#define ERR     8
#define UNDEF   9

#define MAX_MSG_TXT 4096
#define MSG_SIZE     sizeof(struct msg_buf)-sizeof(long)
#define MAX_CLIENTS 10

typedef struct msg_buf 
{
    long msg_type;
    char msg_text[MAX_MSG_TXT];
    int client_id;
} msg_buf;

#endif