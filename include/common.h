#ifndef COMMON_H
#define COMMON_H
/* ******** netUstad: Network Ustadi (Network Master) *********/
/* This software is Copyright (C) 2004 by Ozkan KIRIK.        */
/* Permission is granted to redistribute and modify this      */
/* software under the terms of the GNU General Public License */
/* available at http://www.gnu.org/                           */
/**************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <syslog.h>
#include <stdarg.h>
#include <libgen.h>
#include <sys/param.h>                                                          


#include "acceptrequest.h"
#include "addnewrule.h"
#include "auth.h"
#include "common.h"
#include "config.h"
#include "deleterule.h"
#include "editrule.h"
#include "execcgi.h"
#include "loadconfig.h"
#include "mimetype.h"
#include "netustad.h"
#include "network.h"
#include "snprintf.h"



#define u_short unsigned short
#define ISspace(x) isspace((int)(x))


#ifdef __GLIBC__
	#include <crypt.h>
#endif


#include <libintl.h>
#include <locale.h>
#define PACKAGE "netustad"
#define gettext(S) gettext(S)	
/****************************************
	Workaround for 
	implicit declaration of strdup
	
	any other ideas?
****************************************/
#ifdef NO_STRDUP
char *strdup(const char *s1);
#endif

/****************************************
	Workaround for implicit declaration of snprintf
	this is achieved by using Mark Martinec's 
	portable snprintf implementation
	

	Mark Martinec <mark.martinec@ijs.si>, April 1999, June 2000
	Copyright © 1999,2000,2001,2002 Mark Martinec. All rights reserved.
****************************************/



#endif
