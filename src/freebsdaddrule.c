/* ******** netUstad: Network Ustadi (Network Master) ******** */
/* This software is Copyright (C) 2004 by Ozkan KIRIK.        */
/* Permission is granted to redistribute and modify this      */
/* software under the terms of the GNU General Public License */
/* available at http://www.gnu.org/                           */
/**************************************************************/


#include "addnewrule.h"
#include "netustad.h"
#include "loadconfig.h"
/***************************************************************/
/* Adding A New IPFW Rule                                      */
/***************************************************************/
void 
addnewrule(nunetwork_socket client, char *path,
	   char *method, char *query_string)
{
	char		buf       [1024],
			tmpbuf        [255];
	int		cgi_output [2];
	int		cgi_input  [2];
	pid_t		pid;
	int		status;
	char		c;
	char           *param[64];
	char           *tempstr;
	int		pos = 2;


	buf[0] = 'A';
	buf[1] = '\0';

	if (strlen(query_string) < 13) {
		unimplemented(client);
		return;
	}
	snprintf(buf, sizeof(buf),
	 "HTTP/1.1 200 OK\r\nContent-Type: text/html\n\n");
	nunetwork_send(client, buf, strlen(buf), 0);

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
	if (pid == 0) {		/* child: Add rule */
		dup2(cgi_output[1], 1);
		dup2(cgi_input[0], 0);
		close(cgi_output[0]);
		close(cgi_input[1]);

		param[0] = strdup(path);
		param[1] = "add";
		tempstr = strdup(query_string);

		param[pos] = strtok(tempstr, "@\n");
		while (param[pos++] != NULL) {
			param[pos] = strtok(NULL, "@\n");
		}

		execv(path, param);
		exit(0);
	} else {		/* parent */
		close(cgi_output[1]);
		close(cgi_input[0]);
		pos = 0;
		while (read(cgi_output[0], &c, 1) > 0) {
			tmpbuf[pos++] = c;
			/* nunetwork_send(client, &c, 1, 0); */

		}
		tmpbuf[pos] = '\0';
		snprintf(log_msg_text, sizeof(log_msg_text), "IP: %s %s: %s", \
			 (char *)inet_ntoa(client_name.sin_addr), \
			 gettext("Add Rule"),\
			 (char *)tmpbuf);
		log_msg(logfile, log_msg_text, 1);
		
		snprintf(buf, sizeof (buf)-1, "<html>\n<head>\n<meta http-equiv='Refresh' content='2; URL=/$%s'>\n\
                                <meta http-equiv='Pragma' content='no-cache'>\n\
                                <meta http-equiv=\"Content-Type\" content=\"text/html; charset=%s\">\n\
                                <title>netUstad</title></head>\n",
                	        server_sid,
                        	(char *) gettext("charset"));
  		nunetwork_send(client, buf, strlen(buf), 0);
  		snprintf(buf,sizeof(buf)-1,"<link rel='stylesheet' type='text/css' href='/netustad.css'>\n");
  		nunetwork_send(client, buf, strlen(buf), 0);
  		snprintf(buf,sizeof(buf)-1,"<body><center><br><b>%s<b><br>%s</body></html>\n",
                        (char *) gettext("Rule added"),
                        (char *) gettext("Redirecting to Rule List"));
  		nunetwork_send(client, buf, strlen(buf), 0);

		close(cgi_output[0]);
		close(cgi_input[1]);

		waitpid(pid, &status, 0);
	}
}
