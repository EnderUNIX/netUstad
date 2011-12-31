/* This code Derived from EnderUNIX's isoqlog project */
/* http://www.enderunix.org */

#include "loadconfig.h"
#include "netustad.h"

/*************************************/
/* Load netUstad Configuration File */
/*************************************/
void
loadconfig(char *cfgfile)
{
	FILE *fd;
	char buf[1024];
	char keyword[64];
	char value[256];
	char *cp1, *cp2;
	char *variables[] = {
		"accesslogfile",
		"mainlogfile",
		"adminuser",
		"adminpasswd",
		"listenport",
		"sesexpiretime",
		"lc_all"
	#ifndef WITHOUT_SSL	
		,
		"cert_file",
		"key_file"
	#endif
	};

	int i, key, line, keyword_nums = sizeof (variables) / sizeof (char *);

	if ((fd = fopen(cfgfile, "r")) == NULL) {
		fprintf(stderr,
			"\nnetUstad: cannot open configuration file %s,\nExiting...\n\n",
			cfgfile);
		exit(-1);
	}
	line = 0;
	while ((fgets(buf, sizeof (buf), fd)) != NULL) {
		line++;
		if (buf[0] == '#')
			continue;
		if ((strlen(buf)) <= 1)
			continue;
		cp1 = buf;
		cp2 = keyword;
		while (isspace((int) *cp1))
			cp1++;
		while (isgraph((int) *cp1) && (*cp1 != '='))
			*cp2++ = *cp1++;
		*cp2 = '\0';
		cp2 = value;
		while ((*cp1 != '\0') && (*cp1 != '\n') && (*cp1 != '='))
			cp1++;
		cp1++;
		while (isspace((int) *cp1))
			cp1++;
		if (*cp1 == '"')
			cp1++;
		while ((*cp1 != '\0') && (*cp1 != '\n') && (*cp1 != '"'))
			*cp2++ = *cp1++;
		*cp2-- = '\0';
		if (keyword[0] == '\0' || value[0] == '\0')
			continue;
		key = 0;
		for (i = 0; i < keyword_nums; i++) {
			if ((strcmp(keyword, variables[i])) == 0) {
				key = i;
				break;
			}
		}

		switch (key) {
		case 0:
			strncpy(logfile, value, sizeof (logfile)-1);
			break;
		case 1:
			strncpy(mainlogfile, value, sizeof (mainlogfile)-1);
			break;
		case 2:
			strncpy(adminuser, value, sizeof (adminuser)-1);
			break;
		case 3:
			strncpy(adminpasswd, value, sizeof (adminpasswd)-1);
			break;
		case 4:
			port = (u_short) atoi(value);
			break;
		case 5:
			sesexpiretime = (int) atoi(value);
			break;
		#ifndef WITHOUT_SSL
		case 7:
			strncpy(cert_file, value, sizeof(cert_file)-1);
			break;
		case 8:
			strncpy(key_file, value, sizeof(key_file)-1);
			break;
		#endif
		case 6:
			strncpy(lc_all, value, sizeof (lc_all)-1);
			break;
		}
	}
	fclose(fd);
}

void
readconfig(char *cfgfile)
{
	loadconfig(cfgfile);

	if ((strlen(logfile)) <= 0) {
		fprintf(stderr, "logfile variable doesn't seems defined\n");
		exit(-1);
	}
	if (strlen(mainlogfile) <= 0) {
		fprintf(stderr, "mainlogfile variable doesn't seems defined\n");
		exit(-1);
	}
	if (strlen(adminuser) <= 0) {
		fprintf(stderr, "adminuser variable doesn't seems defined\n");
		exit(-1);
	}
	if (strlen(adminpasswd) <= 0) {
		fprintf(stderr, "adminpasswd variable doesn't seems defined\n");
		exit(-1);
	}
	if (port == 0) {
		fprintf(stderr, "listenport variable doesn't seems defined\n");
		exit(-1);
	}
	if (sesexpiretime == 0) {
		fprintf(stderr,
			"sesexpiretime variable doesn't seems defined\n");
		exit(-1);
	}
	#ifndef WITHOUT_SSL
	if (strlen(cert_file) <= 0) {
                fprintf(stderr, "cert_file variable doesn't seems defined\n");
                exit(-1);
        }
	if (strlen(key_file) <= 0) {
                fprintf(stderr, "key_file variable doesn't seems defined\n");
                exit(-1);
        }
	#endif
}
