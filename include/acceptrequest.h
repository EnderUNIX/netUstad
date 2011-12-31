#ifndef ACCEPTREQ_H
#define ACCEPTREQ_H

#include "common.h"
#include "network.h"
/* Configuration File - Specific Strings */

void accept_request(nunetwork_socket , struct nunetwork_headerstruct *);
void serve_file(nunetwork_socket , char *);
int rightcmp(char *, char *);

char *env_p[24];
#endif
