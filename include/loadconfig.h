#ifndef LOADCONFIG_H
#define LOADCONFIG_H
#include "common.h"

/* Configuration File - Specific Strings */

char logfile[256];
char mainlogfile[256];
char adminuser[256];
char adminpasswd[256];
#ifndef WITHOUT_SSL
char cert_file[256];
char key_file[256];
#endif
char lc_all[32];
int  sesexpiretime;


void loadconfig(char *);
void readconfig(char *);

#endif
