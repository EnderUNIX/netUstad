/* ******** netUstad: Network Ustadi (Network Master) *********/
/* This software is Copyright (C) 2004 by Ozkan KIRIK.        */
/* Permission is granted to redistribute and modify this      */
/* software under the terms of the GNU General Public License */
/* available at http://www.gnu.org/                           */
/**************************************************************/

#include "auth.h"
#include "netustad.h"
#include "loadconfig.h"

  /***********************************/
 /* Check AdminUser and AdminPasswd */
/***********************************/
char *
auth(char *query_string)
{
	char query_user[32];
	char query_passwd[128];
	char plain_passwd[32];
	char salt[12];
	int tmp;
	char *username=NULL;
	char *password=NULL;
	int passerror = 0;

	if (strlen(query_string) < 3)
		passerror = 1;
	else {
		username = (char *) strtok(query_string, ":\n");
		password = (char *) strtok(NULL, ":\n");
	}

	if ((username == NULL) || (password == NULL))
		passerror = 1;
	else if ((strlen(username) > 31) || (strlen(password) > 31))
		passerror = 1;

	if (passerror) {
		snprintf(log_msg_text, sizeof (log_msg_text)-1, "IP:%s %s",
			 client_ip,
			 (char *) gettext("Auth Failure: Long user or password.\n"));
		log_msg(mainlogfile, log_msg_text, 1);
		snprintf(auth_msg, sizeof (auth_msg)-1, "%s", 
			 (char *) gettext("Invalid Username or Password"));
		return "script/authform";
	}

	snprintf(query_user, sizeof (query_user)-1, "%s", username);
	snprintf(plain_passwd, sizeof (plain_passwd)-1, "%s", password);

	for (tmp = 0; tmp < 12; tmp++) {
		if (tmp > 3)
			if (adminpasswd[tmp] == '$')
				break;
		salt[tmp] = adminpasswd[tmp];
	}
	salt[tmp] = '\0';

	strncpy(query_passwd, (char *)crypt(plain_passwd, salt), sizeof (query_passwd)-1);

	if (strncmp((char *) adminuser, query_user, strlen(adminuser)) == 0
	    && strlen(adminuser) == strlen(query_user))
		if (strncmp
		    ((char *) adminpasswd, query_passwd,
		     strlen(adminpasswd)) == 0
		    && strlen(adminpasswd) == strlen(query_passwd))
			if (createsession() == 1) {
				snprintf(log_msg_text, sizeof (log_msg_text)-1, "IP: %s %s",
					 client_ip,
					 (char *) gettext("Administrator logged in\n"));
				log_msg(mainlogfile, log_msg_text, 1);
				return "script/loadindex";
			} else
				snprintf(log_msg_text, sizeof (log_msg_text)-1, "%s",
					 (char *) gettext("Cannot Create Session!\n"));
		else
			snprintf(log_msg_text, sizeof (log_msg_text)-1, "IP:%s %s=%s\n",
				 client_ip,
				 (char *) gettext("Auth Failure: Invalid Password"),
				 plain_passwd);
	else
		snprintf(log_msg_text, sizeof (log_msg_text)-1, "IP:%s %s=%s\n",
			 client_ip,
			 (char *) gettext("Auth Failure: Invalid Username"),
			 query_user);
	log_msg(mainlogfile, log_msg_text, 1);
	snprintf(auth_msg, sizeof (auth_msg)-1, "%s", gettext("Invalid Username or Password"));
	return "script/authform";
}

  /*********************/
 /* Create Session ID */
/*********************/
int
createsession(void)
{
	char sessionid[22];
	int cgi_output[2];
	int cgi_input[2];
	int position = 0;
	int status;
	pid_t pid;
	char c;
	if (pipe(cgi_output) < 0) {
		return -1;
	}
	if (pipe(cgi_input) < 0) {
		return -1;
	}
	if ((pid = fork()) < 0) {
		return -1;
	}

	if (pid == 0) {		/* child: execute script */
		dup2(cgi_output[1], 1);
		dup2(cgi_input[0], 0);
		close(cgi_output[0]);
		close(cgi_input[1]);
		execl("/bin/date", "/bin/date", "+%y%H%m%M%d%S%s", NULL);
		exit(0);
	} else {		/* parent */
		close(cgi_output[1]);
		close(cgi_input[0]);

		while (read(cgi_output[0], &c, 1) > 0 && position < 22) {
			sessionid[position] = c;
			server_sid[position] = sessionid[position];
			position++;
		}
		sessionid[position] = 0;
		server_sid[position] = sessionid[position];

		close(cgi_output[0]);
		close(cgi_input[1]);
		waitpid(pid, &status, 0);
	}
	client_ip = (char *)strdup((char *) inet_ntoa(client_name.sin_addr));
	time(&lastactionsec);
	if (sessionid[0] != 0)
		return 1;
	else
		return -1;
}
