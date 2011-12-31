/* ******** netUstad: Network Ustadi (Network Master) *********/
/* This software is Copyright (C) 2004 by Ozkan KIRIK.        */
/* Permission is granted to redistribute and modify this      */
/* software under the terms of the GNU General Public License */
/* available at http://www.gnu.org/                           */
/**************************************************************/

#include "common.h"

/******************/
/* Manage Request */
/******************/
void
accept_request(nunetwork_socket client, struct nunetwork_headerstruct *headerbuf)
{
	int auth_form = 0;
	int p = 0,i,t;
	char path[512], parameters[512];
	char env_str[512];
	struct stat st;
	int cgi = 0;		/* becomes true if server decides this is a CGI program */
	char *query_string = NULL;
	char *tmp_str = NULL;
	char *tmp_url = NULL;
	char *client_sid = " ";
	char method[5];
	char url[256];
	char header[256];
	char value[256];
	/* Vars for processing HTTP 304*/
	/*enum en_day { Sun = 0, Mon, Tue, Wed, Thu, Fri, Sat} days;
	enum en_month { Jan = 0, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec } months;*/
	/*char *withoutsession[] = {
			"/netustad.css",
			"/authform",
			"/images/bg.png",
			"/images/buton_0.png",
			"/images/buton_1.png",
			"/images/buton_2.png",
			"/images/buton_3.png",
			"/images/buton_4.png",
			"/images/buton_5.png",
			"/images/buton_6.png",
			"/images/buton_7.png",
			"/images/banner.png",
			"/images/banner-alt.png",
			"/images/menu_login.png",
			"/images/menu_rulelist.png",
			"/images/menu_addrule.png",
			"/images/menu_logout.png",
			NULL
			};
	int ifwithoutsession=0;*/
	/*struct tm zamanstr;
	time_t zaman_t;
	char str4[5]="123";*/	
	char *start;
	/*****************************************/
	
	/* Processing the header buffer */
	/* GET request is saved to be processed at the end*/
	for (t=0; t< headerbuf->number_of_headers; t++)  {
		
		if (!strcasecmp(headerbuf->headers[t].header_id, "POST") ) {	
			/* sorry maybe next time */
			unimplemented(client);
			return;
		} else if (!strcasecmp(headerbuf->headers[t].header_id, "HEAD")) {
			tmp_url = strdup(headerbuf->headers[t].header_value);
			
			/*Getting the requested url */
			start = headerbuf->headers[t].header_value;
			while (*start!=' ' || *start!='\t') start++;
			tmp_url = strstr(start, "HTTP/") - 1;
			while ( *tmp_url!= ' ' || *tmp_url != '\t' ) tmp_url--;
			snprintf(url, tmp_url - start + 1, "script%s", start );
			headers(client, url);
			return;
		} else if (!strcasecmp(headerbuf->headers[t].header_id, "GET")) {
			/* this is it ... getting the method and the url without HTTP id */
			strncpy(method, headerbuf->headers[t].header_id, sizeof(method));
			strncpy(url, headerbuf->headers[t].header_value, sizeof(url)); 
			i=0; while(url[i++]!=' ');
			url[i-1]='\0';
		/*} else if (!strcasecmp(headerbuf->headers[t].header_id, "If-Modified-Since")) {*/

			/* Processing If-Modified-Since header value if its not related with cgis */
			/*if ( (strstr(url, ".png")!=NULL && strstr(url, "images")!=NULL) || 
				strstr(url, "netustad.css") ) {*/
			
				/* Getting Day */
				/*	HTTP/1.1 304 Not Modified
					Date: Fri, 31 Dec 1999 23:59:59 GMT	*/

				/*start = headerbuf->headers[t].header_value;
				if (strlen(start)>10) {
					while (*start==' ') start++;
					start+=5;
					snprintf(str4, 2, start, "%s");
					zamanstr.tm_mday=atoi(start);
					
					start+=2;
					snprintf(str4, 3, start, "%s");
					zamanstr.tm_mon=months[str4];
					
					start+=4;
					snprintf(str4, 4, start, "%s");
					zamanstr.tm_year= atoi(str4)-1900;
					
					start+=5;
					snprintf(str4, 2, start, "%s");
					zamanstr.tm_hour=atoi(str4);
					
					start+=3;
					snprintf(str4, 2, start, "%s");
					zamanstr.tm_min=atoi(str4);
					
					start+=3;
					snprintf(str4, 2, start, "%s");
					zamanstr.tm_sec=atoi(str4);
					zamanstr.tm_isdst=-1;
					
					zaman_t = mktime(&zamanstr);

				}
						
			
			}*/	

		}
	} 

	query_string = url;
	while ((*query_string != '?') && (*query_string != '\0'))
		query_string++;

	if (*query_string == '?') {
		cgi = 1;
		*query_string = '\0';
		query_string++;
	}

	  /*******************/
	 /* Parse SessionID */
	/*******************/
	if (query_string[0] == 0) {
		tmp_str = url;
		tmp_url = strtok(tmp_str, "$\n");
		while (tmp_url != NULL && (int) p <= (int) strlen(value))
			value[p++] = *tmp_url++;
	} else {
		tmp_str = strdup(query_string);
		query_string = strtok(tmp_str, "$\n");
	}
	client_sid = strtok(NULL, "$\n");
	if (client_sid == 0)
		client_sid = "$";

	  /****************************/
	 /* add script folder to url */
	/****************************/
	snprintf(path, sizeof (path)-1, "script%s", url);

   	  /*****************************/
	 /* Check Unwanted Characters */
 	/*****************************/
	if (strstr(path, "..") != NULL) {
		server_sid[0] = '$';
		server_sid[1] = '\0';
		snprintf(parameters, sizeof (parameters)-1,
			 " -- %s: %s. %s: %s\n",
			 (char *) gettext("Bad Request From"), 
			 (char *) inet_ntoa(client_name.sin_addr),
			 (char *) gettext("The Request is"),
			 (char *) path);
		log_msg(mainlogfile, parameters, 1);
		log_msg(mainlogfile, gettext("Session Killed Because of Bad Request\n"), 1);
		snprintf(auth_msg, sizeof (auth_msg)-1, " ");
		auth_form = 1;
	}

  	  /************************/
	 /* Check Session Expire */
	/************************/
	time(&currentsec);
	if ((long) currentsec - (long) lastactionsec <= sesexpiretime) {	/* Session Not Expired */
		time(&lastactionsec);
	} else {		/* Session Expired, close session */
		server_sid[0] = '$';
		server_sid[1] = '\0';
		if (!(rightcmp(path, "/netustad.css") ||\
		rightcmp(path, "/authform") ||\
		rightcmp(path, "/images/bg.png") ||\
		rightcmp(path, "/images/buton_0.png") ||\
		rightcmp(path, "/images/buton_1.png") ||\
        	rightcmp(path, "/images/buton_2.png") ||\
        	rightcmp(path, "/images/buton_3.png") ||\
        	rightcmp(path, "/images/buton_4.png") ||\
        	rightcmp(path, "/images/buton_5.png") ||\
        	rightcmp(path, "/images/buton_6.png") ||\
        	rightcmp(path, "/images/buton_7.png") ||\
        	rightcmp(path, "/images/banner.png") ||\
        	rightcmp(path, "/images/banner-alt.png") ||\
        	rightcmp(path, "/images/editor.png") ||\
        	rightcmp(path, "/images/eraser.png") ||\
		rightcmp(path, "/images/menu_rulelist.png") ||\
		rightcmp(path, "/images/menu_addrule.png") ||\
		rightcmp(path, "/images/menu_logout.png") ||\
        	rightcmp(path, "/images/menu_login.png"))) {
			log_msg(mainlogfile, gettext("Administrator's Session Expired!\n"), 1);
		}
		/*t=0;
		while (withoutsession[t]!=NULL && !rightcmp(path, withoutsession[t])) {
			t++;
		}
		if (withoutsession[t]!=NULL && !rightcmp(path, withoutsession[t])) {
			log_msg(mainlogfile, gettext("Administrator's Session Expired!\n"), 1);
		} else {
			ifwithoutsession=1;
		}*/
		snprintf(auth_msg, sizeof (auth_msg)-1, gettext("Session Expired!"));
		auth_form = 1;
	}

	  /********************/
	 /* Check Session ID */
	/********************/
	if (strcmp(server_sid, "$") != 0 && *client_sid != '$')
		if (strcmp(client_sid, server_sid) != 0 ||
		    strcmp(client_ip,
			   (char *) inet_ntoa(client_name.sin_addr)) != 0) {
			snprintf(auth_msg, sizeof (auth_msg)-1, gettext("You are not logged in!"));
			auth_form = 1;
		} else
			auth_form = 0;
	else {
		if ((long) lastactionsec == 0)
			snprintf(auth_msg, sizeof (auth_msg)-1, " ");
		auth_form = 1;
	}

 	  /********************************/
	 /* Define Environment Variables */
 	/********************************/
	snprintf(env_str, sizeof (env_str)-1, "netustadversion=%s", NUVERSION);
	env_p[0] = strdup(env_str);	/* netustad version */

	snprintf(env_str, sizeof (env_str)-1, "work_path=%s/script", workdir);
	env_p[1] = strdup(env_str);	/* work_path */

	env_p[2] = "auth_msg=";		/* authform message */

	snprintf(env_str, sizeof (env_str)-1, "server_sid=%s", server_sid);
	env_p[3] = strdup(env_str);	/* server session id */

	snprintf(env_str, sizeof (env_str)-1, "fw_cmd=%s", fw_cmd);
	env_p[4] = strdup(env_str);	/* iptables command */

	snprintf(env_str, sizeof (env_str)-1, "TEXTDOMAIN=%s", PACKAGE);
	env_p[5] = strdup(env_str);	/* TEXTDOMAIN (for gettext) */

	snprintf(env_str, sizeof (env_str)-1, "TEXTDOMAINDIR=%s", PACKAGE_LOCALE_DIR);
	env_p[6] = strdup(env_str);	/* TEXTDOMAINDIR (for gettext) */

	snprintf(env_str, sizeof (env_str)-1, "LC_ALL=%s", lc_all );
	env_p[7] = strdup(env_str);	/* Choose locale */

	snprintf(env_str, sizeof (env_str)-1, "gettext_cmd=%s", gettext_cmd);
	env_p[8] = strdup(env_str);	/* gettext command */

	snprintf(env_str, sizeof (env_str)-1, "cat_cmd=%s", cat_cmd);
	env_p[9] = strdup(env_str);	/* full path of cat command */

	snprintf(env_str, sizeof (env_str)-1, "awk_cmd=%s", awk_cmd);
	env_p[10] = strdup(env_str);	/* full path of awk command */

	snprintf(env_str, sizeof (env_str)-1, "MM_CHARSET=%s", gettext("charset"));
	env_p[11] = strdup(env_str);	/* charset environ */

	snprintf(env_str, sizeof (env_str)-1, "LANG=%s", lc_all );
	env_p[12] = strdup(env_str);	/* LANG environ */

	snprintf(env_str, sizeof (env_str)-1, "netstat_cmd=%s", netstat_cmd );
	env_p[13] = strdup(env_str);	/* netstat_cmd environ */

	snprintf(env_str, sizeof (env_str)-1, "grep_cmd=%s", grep_cmd );
	env_p[14] = strdup(env_str);	/* grep_cmd environ */

	snprintf(env_str, sizeof (env_str)-1, "tr_cmd=%s", tr_cmd );
	env_p[15] = strdup(env_str);	/* tr_cmd environ */

	snprintf(env_str, sizeof (env_str)-1, "sed_cmd=%s", sed_cmd );
	env_p[16] = strdup(env_str);	/* sed_cmd environ */

	snprintf(env_str, sizeof (env_str)-1, "tail_cmd=%s", tail_cmd );
	env_p[17] = strdup(env_str);	/* tail_cmd environ */

	snprintf(env_str, sizeof (env_str)-1, "route_cmd=%s", route_cmd );
	env_p[18] = strdup(env_str);	/* route_cmd environ */

	snprintf(env_str, sizeof (env_str)-1, "QUERY_STRING=a");
	env_p[19] = strdup(env_str);	/* QUERY_STRING environ */

	snprintf(env_str, sizeof (env_str)-1, "ifconfig_cmd=%s", ifconfig_cmd );
	env_p[20] = strdup(env_str);	/* ifconfig_cmd environ */

	env_p[21] = NULL;	/*temporary variable */
	env_p[22] = NULL;	/*temporary variable */
	env_p[23] = NULL;	/*parameter terminate */

 	  /***************************/
	 /* Parse Requested Address */
	/***************************/

	/* If Logout Requested Close Session and Goto Auth */
	if (rightcmp(path, "/logout")) {
		server_sid[0] = '$';
		server_sid[1] = '\0';
		snprintf(auth_msg, sizeof (auth_msg)-1, gettext("You are logged out"));
		log_msg(mainlogfile, gettext("Administrator logged out\n"), 1);
		auth_form = 1;
	/*} else if (ifwithoutsession && strcmp(withoutsession[t], "/authform")!=0) {
		snprintf(path, sizeof(path)-1, "script%s", withoutsession[t]);
	}*/
	} else if (rightcmp(path, "/images/bg.png")) {
		snprintf(path, sizeof (path)-1, "script/images/bg.png");
        } else if (rightcmp(path, "/images/buton_0.png")) {
                snprintf(path, sizeof(path)-1, "script/images/buton_0.png");
        } else if (rightcmp(path, "/images/buton_1.png")) {
                snprintf(path, sizeof(path)-1, "script/images/buton_1.png");
        } else if (rightcmp(path, "/images/buton_2.png")) {
                snprintf(path, sizeof(path)-1, "script/images/buton_2.png");
        } else if (rightcmp(path, "/images/buton_3.png")) {
                snprintf(path, sizeof(path)-1, "script/images/buton_3.png");
        } else if (rightcmp(path, "/images/menu_login.png")) {
                snprintf(path, sizeof(path)-1, "script/images/menu_login.png");
        } else if (rightcmp(path, "/images/menu_logout.png")) {
                snprintf(path, sizeof(path)-1, "script/images/menu_logout.png");
        } else if (rightcmp(path, "/images/menu_addrule.png")) {
                snprintf(path, sizeof(path)-1, "script/images/menu_addrule.png");
        } else if (rightcmp(path, "/images/menu_rulelist.png")) {
                snprintf(path, sizeof(path)-1, "script/images/menu_rulelist.png");
        } else if (rightcmp(path, "/images/buton_4.png")) {
                snprintf(path, sizeof(path)-1, "script/images/buton_4.png");
        } else if (rightcmp(path, "/images/buton_5.png")) {
                snprintf(path, sizeof(path)-1, "script/images/buton_5.png");
        } else if (rightcmp(path, "/images/buton_7.png")) {
                snprintf(path, sizeof(path)-1, "script/images/buton_7.png");
        } else if (rightcmp(path, "/images/banner.png")) {
                snprintf(path, sizeof(path)-1, "script/images/banner.png");
        } else if (rightcmp(path, "/images/banner-alt.png")) {
                snprintf(path, sizeof(path)-1, "script/images/banner-alt.png");
        } else if (rightcmp(path, "/images/editor.png")) {
                snprintf(path, sizeof(path)-1, "script/images/editor.png");
        } else if (rightcmp(path, "/images/eraser.png")) {
                snprintf(path, sizeof(path)-1, "script/images/eraser.png");
        } else if (rightcmp(path, "/netustad.css")) {
                snprintf(path, sizeof(path)-1, "script/netustad.css");
	}

	/* Other Requests */
	if (auth_form == 1 && \
	    !(rightcmp(path, "/auth")) && \
            !(rightcmp(path, "/netustad.css")) && \
            !(rightcmp(path, "/images/bg.png")) && \
            !(rightcmp(path, "/images/buton_0.png")) && \
            !(rightcmp(path, "/images/buton_1.png")) && \
            !(rightcmp(path, "/images/buton_2.png")) && \
            !(rightcmp(path, "/images/buton_3.png")) && \
            !(rightcmp(path, "/images/buton_4.png")) && \
            !(rightcmp(path, "/images/buton_5.png")) && \
            !(rightcmp(path, "/images/buton_6.png")) && \
            !(rightcmp(path, "/images/buton_7.png")) && \
            !(rightcmp(path, "/images/banner.png")) && \
            !(rightcmp(path, "/images/banner-alt.png")) && \
            !(rightcmp(path, "/images/editor.png")) && \
            !(rightcmp(path, "/images/eraser.png")) && \
	    !(rightcmp(path, "/images/menu_logout.png")) && \
	    !(rightcmp(path, "/images/menu_addrule.png")) && \
	    !(rightcmp(path, "/images/menu_rulelist.png")) && \
            !(rightcmp(path, "/images/menu_login.png"))) {
		snprintf(env_str, sizeof (env_str)-1, "auth_msg=%s", auth_msg);
		env_p[2] = strdup(env_str);
		snprintf(path, sizeof (path)-1, "script/authform");
	} else if (path[strlen(path) - 1] == '/') {
		strncat(path, "showrule", (size_t) (sizeof (path) - 1 - strlen(path)));
	} else if (rightcmp(path, "/edit")) {
		snprintf(env_str, sizeof (env_str)-1, "ruleno=%s", strtok(query_string,"@\n"));
		env_p[21] = strdup(env_str);
	#ifdef LINUX
		snprintf(env_str, sizeof (env_str)-1, "chain=%s", strtok(NULL,"@\n"));
		env_p[22] = strdup(env_str);
	#endif
		strncat(path, "form", (size_t) (sizeof (path) - 1 - strlen(path)));

	} else if (rightcmp(path, "/if_edit")) {
		snprintf(env_str, sizeof (env_str)-1, "if_name=%s", strtok(query_string,"@\n"));
		env_p[21] = strdup(env_str);
		snprintf(path, sizeof(path)-1, "script/if_edit");
	
	} else if (rightcmp(path, "/nat_del")) {
		snprintf(env_str, sizeof (env_str)-1, "nat_name=%s", strtok(query_string,"@\n"));
		env_p[21] = strdup(env_str);
		snprintf(path, sizeof(path)-1, "script/nat_del");
	
	} else if (rightcmp(path, "/rt_del")) {
		snprintf(env_str, sizeof (env_str)-1, "route=%s", strtok(query_string,"@\n"));
		env_p[21] = strdup(env_str);
		snprintf(path, sizeof(path)-1, "script/rt_del");
	
	} else if (rightcmp(path, "/auth") && auth_form == 1) {
		snprintf(path, sizeof (path)-1, "%s", auth(query_string));
		snprintf(env_str, sizeof (env_str)-1, "auth_msg=%s", auth_msg);
		env_p[2] = strdup(env_str);
		snprintf(env_str, sizeof (env_str)-1, "server_sid=%s", server_sid);
		env_p[3] = strdup(env_str);
	#ifdef FREEBSD
	} else if (rightcmp(path, "/write")) {
		snprintf(path, sizeof(path)-1, "script/writeconfig");
	#endif	
	}

	/* If Rule Deleting Requested ("del" word) */
	if (rightcmp(path, "/del")) {
		strncpy(path, fw_cmd, sizeof (path)-1);
		deleterule(client, path, header, query_string);
	} else if (rightcmp(path, "/addnew")) {
		strncpy(path, fw_cmd, sizeof (path)-1);
		addnewrule(client, path, header, query_string);
	} else if (rightcmp(path, "/applyedit")) {
		strncpy(path, fw_cmd, sizeof (path)-1);
		editrule(client, path, header, query_string);
	} else if (stat(path, &st) == -1) {
		snprintf(log_msg_text, sizeof(log_msg_text)-1, gettext("File not found: %s"), path);
		log_msg(logfile, log_msg_text, 1);
		not_found(client);
	} else {
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			strncat(path, "/showrule",
				(size_t) (sizeof (path) - 1 - strlen(path)));
		if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP)
		    || (st.st_mode & S_IXOTH))
			cgi = 1;
		if (!cgi)
			serve_file(client, path);
		else
			snprintf(env_str, sizeof (env_str)-1, "QUERY_STRING=%s", query_string);
			env_p[19] = strdup(env_str);	/* QUERY_STRING environ */
			execute_cgi(client, path, header, query_string, env_p, 1);
	}
	return;
}

/******************************/
/* Send client a regular file */
/******************************/
void
serve_file(nunetwork_socket client, char *filename)
{
	FILE *resource = NULL;
	char buf[1024];

	buf[0] = 'A';
	buf[1] = '\0';
	resource = fopen(filename, "r");
	if (resource == NULL)
		not_found(client);
	else {
		headers(client, filename);
		binarycat(client, resource);
	}
	fclose(resource);
}

/*********************************/
/* Compare two string from right */
/*********************************/
/* For example:                  */
/*      str="abcd"; look4str="cd"*/
/* rightcmp(str,look4str) = 1    */
/*      str="abcd"; look4str="ab"*/
/* rightcmp(str,look4str) = 0    */
/*********************************/
int
rightcmp(char *str, char *look4str)
{
	int i = 0;
	if (look4str == NULL || str == NULL) {
		return 0;
	}			/* Oopps, invalid number of args */
	if (strlen(look4str) > strlen(str)) {	/* If look4str is longer then str, compare cannot be done */
		return 0;
	}
	for (i = strlen(look4str); i >= 1; i--) {
		if ((char) str[strlen(str) - i] !=
		    (char) look4str[strlen(look4str) - i]) {
			return 0;
		}
	}
	return 1;
}
