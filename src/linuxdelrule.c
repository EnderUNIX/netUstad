/* ******** netUstad: Network Ustadi (Network Master) *********/
/* This software is Copyright (C) 2004 by Ozkan KIRIK.        */
/* Permission is granted to redistribute and modify this      */
/* software under the terms of the GNU General Public License */
/* available at http://www.gnu.org/                           */
/**************************************************************/

#include "deleterule.h"
#include "netustad.h"
#include "loadconfig.h"

/***************************************************************/
/* Deleting An IPFW Rule                                       */
/***************************************************************/
void
deleterule(nunetwork_socket client,  char *path,
	   char *method, char *query_string)
{
	char buf[1024];
	int cgi_output[2];
	int cgi_input[2];
	char *param[5];
	pid_t pid;
	int status;
	/*char c; */

	buf[0] = 'A';
	buf[1] = '\0';
/*	if (strcasecmp(method, "GET") == 0)
		nunetwork_discardheaders(client);
	else {			/* POST * /

		unimplemented(client);
		return;
	}
*/
	snprintf(buf, sizeof (buf)-1,
		 "HTTP/1.1 200 OK\r\nContent-Type: text/html\n\n");
	nunetwork_send(client, buf, strlen(buf), 0);

	param[0] = (char *)strdup(path);
	param[1] = "-D";
	param[2] = strtok(query_string, "@\n");
	param[3] = strtok(NULL, "@\n");
	param[4] = NULL;

	if (pipe(cgi_output) < 0) {
		cannot_execute(client);
		return;
	}
	if (pipe(cgi_input) < 0) {
		cannot_execute(client);
		return;
	}

	if ((pid = fork()) < 0) {
		cannot_execute(client);
		return;
	}
	if (pid == 0) {		/* child: Delete rule */
		/*dup2(cgi_output[1], 1);
		dup2(cgi_input[0], 0);
		close(cgi_output[0]);
		close(cgi_input[1]);*/

		execv(path, param);

		exit(0);
	} else {		/* parent */
		/*close(cgi_output[1]);
		close(cgi_input[0]);
		close(cgi_output[0]);
		close(cgi_input[1]);*/
		waitpid(pid, &status, 0);

		snprintf(buf, sizeof (buf)-1, "<html>\n<head>\n<meta http-equiv='Refresh' content='2; URL=/$%s'>\n\
						<meta http-equiv='Pragma' content='no-cache'>\n\
						<meta http-equiv=\"Content-Type\" content=\"text/html; charset=%s\">\n\
						<title>netUstad</title></head>\n",
			 			server_sid,
						(char *) gettext("charset"));
		nunetwork_send(client, buf, strlen(buf), 0);
		snprintf(buf, sizeof (buf)-1, "<link rel='stylesheet' type='text/css' href='/netustad.css'>\n");
		nunetwork_send(client, buf, strlen(buf), 0);
		snprintf(log_msg_text, sizeof (log_msg_text)-1,
			 "IP: %s %s: %s - %s\n",
			 (char *) inet_ntoa(client_name.sin_addr),
			 (char *) gettext("Deleted Rule"),
			 (char *) param[2],
			 (char *) param[3]);
		log_msg(logfile, log_msg_text, 1);

		snprintf(buf, sizeof (buf)-1,
			 "<body>\n<center>\n<br><b>%s: %s - %s: %s %s <b><br>%s</body></html>\n",
			 (char *) gettext("Chain"),
			 (char *) param[2],
			 (char *) gettext("Rule"),
			 (char *) param[3],
			 (char *) gettext("Deleted"),
			 (char *) gettext("Redirecting to Rule List"));
		nunetwork_send(client, buf, strlen(buf), 0);
	}
}
