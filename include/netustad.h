#ifndef NETUSTAD_H
#define NETUSTAD_H
#include "common.h"

/* Configuration File - Specific Strings */
u_short port;
int server_sock;
int client_sock;
char server_sid[22];
char *client_ip;
char logintime[10];
char currtime[10];
struct sockaddr_in client_name;
time_t currentsec;
time_t lastactionsec;
char auth_msg[50];
char *timenow;
char *nameoftty;
char *conffile;
char log_msg_text[256];

void daemonize(void);
void signal_handler(int);
void log_msg(char *,char *, char );
void load_config_file(void);
void error_die(char *);
int startup(u_short *);

#endif
