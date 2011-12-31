/* ******** netUstad: Network Ustadi (Network Master) *********/
/* This software is Copyright (C) 2004 by Ozkan KIRIK.        */
/* Permission is granted to redistribute and modify this      */
/* software under the terms of the GNU General Public License */
/* available at http://www.gnu.org/                           */
/**************************************************************/

#include "mimetype.h"
#include "netustad.h"

/***************************************/
/* Determine The MIME Type of filename */
/***************************************/
char *
mimetype(char *filename)
{
	if ((filename[strlen(filename) - 5] == '.' &&
	     filename[strlen(filename) - 4] == 'j' &&
	     filename[strlen(filename) - 3] == 'p' &&
	     filename[strlen(filename) - 2] == 'e' &&
	     filename[strlen(filename) - 1] == 'g')
	    ||
	    (filename[strlen(filename) - 4] == '.' &&
	     filename[strlen(filename) - 3] == 'j' &&
	     filename[strlen(filename) - 2] == 'p' &&
	     filename[strlen(filename) - 1] == 'g'))
		strncpy(type, "image/jpeg", sizeof (type)-1);
	else if (filename[strlen(filename) - 4] == '.' &&
		 filename[strlen(filename) - 3] == 'p' &&
		 filename[strlen(filename) - 2] == 'n' &&
		 filename[strlen(filename) - 1] == 'g')
		strncpy(type, "image/png", sizeof (type)-1);
	else if (filename[strlen(filename) - 4] == '.' &&
		 filename[strlen(filename) - 3] == 'g' &&
		 filename[strlen(filename) - 2] == 'i' &&
		 filename[strlen(filename) - 1] == 'f')
		strncpy(type, "image/gif", sizeof (type)-1);
	else if (filename[strlen(filename) - 4] == '.' &&
		 filename[strlen(filename) - 3] == 'c' &&
		 filename[strlen(filename) - 2] == 's' &&
		 filename[strlen(filename) - 1] == 's')
		strncpy(type, "text/css", sizeof (type)-1);
	else
		strncpy(type, "text/html", sizeof (type)-1);
	return (type);
}
