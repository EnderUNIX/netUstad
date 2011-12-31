/* ******** netUstad: Network Ustadi (Network Master) *********/
/* This software is Copyright (C) 2004 by Ozkan KIRIK.        */
/* Permission is granted to redistribute and modify this      */
/* software under the terms of the GNU General Public License */
/* available at http://www.gnu.org/                           */
/**************************************************************/


/* This code Derived from: */
/* J. David's webserver */
/* This is a simple webserver. */
/* Created November 1999 by J. David Blackstone. */
/* CSE 4344 (Network concepts), Prof. Zeigler */
/* University of Texas at Arlington */
/* tinyhttpd project: http://tinyhttpd.sourceforge.net */

/**************************/
/* Start of derieved code */
/**************************/

/**********************************************************************/
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 * Parameters: client socket descriptor
 *             path to the CGI script */
/**********************************************************************/
#include "common.h"
#include "execcgi.h"


void
execute_cgi(nunetwork_socket client, char *path,
	    char *method, char *query_string, char *envp[], int http_ok)
{
	char buf[1024];
	int cgi_output[2];
	int cgi_input[2];
	pid_t pid;
	int status;
	int i;
	char c;
	int content_length = -1;

	buf[0] = 'A';
	buf[1] = '\0';
/*
	if (strcasecmp(method, "GET") == 0)
		nunetwork_discardheaders(client);
	else {			/ POST *\/

		numchars = get_line(client, buf, sizeof (buf));
		while ((numchars > 0) && strcmp("\n", buf)) {
			buf[15] = '\0';
			if (strcasecmp(buf, "Content-Length:") == 0)
				content_length = atoi(&(buf[16]));
			numchars = get_line(client, buf, sizeof (buf));
		}
		if (content_length == -1) {
			bad_request(client);
			return;
		}
	}
*/
	if (http_ok) {
		snprintf(buf, sizeof (buf)-1, "HTTP/1.0 200 OK\r\n");
		nunetwork_send(client, buf, strlen(buf), 0);
	}

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
	if (pid == 0) {		/* child: CGI script */
		char meth_env[255];
		char query_env[255];
		char length_env[255];

		dup2(cgi_output[1], 1);
		dup2(cgi_input[0], 0);
		close(cgi_output[0]);
		close(cgi_input[1]);
		snprintf(meth_env, sizeof (meth_env)-1, "REQUEST_METHOD=%s",
			 method);
		/*putenv(meth_env);*/
		if (!(strcasecmp(method, "GET") == 0)) {
			snprintf(length_env, sizeof (length_env)-1,
				 "CONTENT_LENGTH=%d", content_length);
			/*putenv(length_env);*/
		}
		execle(path, path, NULL, envp);
		exit(0);
	} else {		/* parent */
		close(cgi_output[1]);
		close(cgi_input[0]);
		if (strcasecmp(method, "POST") == 0)
			for (i = 0; i < content_length; i++) {
				nunetwork_recv(client, &c, 1, 0);
				write(cgi_input[1], &c, 1);
			}
		while (read(cgi_output[0], &c, 1) > 0)
			nunetwork_send(client, &c, 1, 0);

		close(cgi_output[0]);
		close(cgi_input[1]);
		waitpid(pid, &status, 0);
	}
}



