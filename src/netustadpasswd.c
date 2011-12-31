/* ******** netUstad: Network Ustadi (Network Master) *********/
/* This software is Copyright (C) 2004 by Ozkan KIRIK.        */
/* Permission is granted to redistribute and modify this      */
/* software under the terms of the GNU General Public License */
/* available at http://www.gnu.org/                           */
/**************************************************************/

  /****************************/
 /* Thanks to M. Onur YALAZI */
/****************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <stdio.h>

#ifdef __GLIBC__ 
	#include <crypt.h>
#endif

#include "snprintf.h"

int
parseline(char *line[], char *passwd)
{
	int _break = 0, tmp = 0;
	char *search = "adminpasswd";

	for (; (unsigned) tmp <= strlen((char *) line); tmp++) {
		switch (((char *) line)[tmp]) {
		case ' ':
			continue;
			break;
		case '#':
		case '\n':
			return 0;
		default:
			_break = 1;
			break;
		}
		if (_break)
			break;
	}

	if (!strncmp((char *) line, search, strlen(search))) {
		snprintf((char *) line, strlen(passwd) + 18,
			 "adminpasswd = \"%s\"\n", passwd);
		return 1;
	} else {
		return 0;
	}

}

void
writeconf(char *conffile, char *passwd)
{
	char buf[255], tmpline[1024];
	/* int parse=0; */
	FILE *fptr;
	FILE *tmpptr;

	fptr = fopen(conffile, "r");
	if (fptr == NULL) {
		printf("\n Config File is not found.\n");
		exit(1);
	}
	snprintf(buf, strlen(conffile) + 4, "%s_tmp", conffile);
	tmpptr = fopen(buf, "w");

	if (tmpptr == NULL) {

		printf("\nCan not open temporary file\n");
		fclose(fptr);
		exit(1);

	}

	while (fgets(tmpline, 1024, fptr)) {
		parseline((char **) tmpline, passwd);
		fputs(tmpline, tmpptr);
	}

	fclose(fptr);
	fclose(tmpptr);

	unlink(conffile);
	rename(buf, conffile);

}

int
main(int argc, char *argv[])
{

	char salt[12];
	char passwd[100];
	time_t currentsec;
	FILE *logfile;

	if (argc != 4) {

		printf("\n Usage: %s conffile password salt\n", argv[0]);
		printf
		    ("\t conffile is the netustad config file that you use\n");
		printf("\t Password is the password that you wish to use\n");
		printf("\t salt is used as an cryptographic key.\n");
		printf("\t\t salt length must be between 2 and 8 chars\n");

		exit(0);

	}

	if ((strlen(argv[3]) > 8) || (strlen(argv[3]) < 2)) {
		printf("\n Salt length must be between 2 and 8 chars\n");
		exit(0);
	}

	snprintf(salt, 12, "$1$%s", argv[3]);
	strncpy(passwd, (char *)crypt(argv[2], salt), sizeof (passwd));
	writeconf(argv[1], passwd);
	printf("Password changed successfully\n");

	/* Log that the password is changed */
	time(&currentsec);
	logfile = fopen("/var/log/netustad.log", "a");
	if (!logfile)
		return -1;
	fprintf(logfile, "%s -- Password changed\n",
		(char *) strtok((char *)ctime(&currentsec), "\n"));
	fclose(logfile);
	return 0;
}
