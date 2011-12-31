/* ******** netUstad: Network Ustadi (Network Master) *********/
/* This software is Copyright (C) 2004 by Ozkan KIRIK.        */
/* Permission is granted to redistribute and modify this      */
/* software under the terms of the GNU General Public License */
/* available at http://www.gnu.org/                           */
/**************************************************************/
#include "common.h"
#include "network.h"

void
bad_request(nunetwork_socket client)
{
        char buf[128];

        snprintf(buf, sizeof (buf)-1, "HTTP/1.1 400 BAD REQUEST\r\n");
        nunetwork_send(client, buf, sizeof (buf), 0);
        snprintf(buf, sizeof (buf)-1, "Content-type: text/html\r\n");
        nunetwork_send(client, buf, sizeof (buf), 0);
        snprintf(buf, sizeof (buf)-1, "\r\n");
        nunetwork_send(client, buf, sizeof (buf), 0);
        snprintf(buf, sizeof (buf)-1, "<P>Your browser sent a bad request, ");
        nunetwork_send(client, buf, sizeof (buf), 0);
        snprintf(buf, sizeof (buf)-1, "Such as a request without Host: header.\r\n");
        nunetwork_send(client, buf, sizeof (buf), 0);
}

/****************************************************/
/* Send client, contents of a file as binary format */
/****************************************************/
void
binarycat(nunetwork_socket client, FILE * resource)
{
        char buf;

        while (!feof(resource)) {
                fread(&buf, sizeof (buf), 1, resource);
                nunetwork_send(client, &buf, 1, 0);
        }
}

/**************************************************************/
/* Inform the client that a CGI script could not be executed. */
/**************************************************************/
void
cannot_execute(nunetwork_socket client)
{
        char buf[128];

        snprintf(buf, sizeof (buf)-1, "HTTP/1.1 500 Internal Server Error\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "Content-type: text/html\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "<P>Error prohibited CGI execution.\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
}


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
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/


int
get_line(nunetwork_socket client, char *buf, int size)
{
        int i = 0;
        char c = '\0';
        int n;

        while ((i < size - 1) && (c != '\n')) {
                n = nunetwork_recv(client, &c, 1, 0);
                if (n > 0) {
                        if (c == '\r') {
                                n = nunetwork_recv(client, &c, 1, MSG_PEEK);
                                if ((n > 0) && (c == '\n')) {
					;
					#ifdef WITHOUT_SSL
                                       		nunetwork_recv(client, &c, 1, 0);
					#endif
                                } else
                                        c = '\n';
                        }
                        buf[i] = c;
                        i++;
		} else 
			c='\n';
        }
        buf[i] = '\0';

        return (i);

}

/*************************/
/* End of Derieved Code  */
/*************************/


/******************************************/
/* Send client HTTP headers about a file. */
/******************************************/
void
headers(nunetwork_socket client, char *filename)
{
        char buf[1024];
        struct stat filestat;
        time_t zaman;
	struct tm zamanstr;
	char *cmonths[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	char *cdays[] = { "Sun","Mon","Tue","Wed","Thu", "Fri", "Sat" };

	strncpy(buf, "HTTP/1.1 200 OK\r\n", sizeof (buf)-1);
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof(buf) -1,  "%s\r\n", SERVER_STRING);
        nunetwork_send(client, buf, strlen(buf), 0);
	time(&zaman);	
	gmtime_r(&zaman, &zamanstr);
 	/* Fri, 31 Dec 1999 23:59:59 GMT 	*/
	snprintf(buf, sizeof(buf)-1, "Date: %s, %02d %s %02d:%02d:%02d %4d GMT\r\n", 
			cdays[zamanstr.tm_wday],
			zamanstr.tm_mday,
			cmonths[zamanstr.tm_mon],
			zamanstr.tm_hour,
			zamanstr.tm_min,
			zamanstr.tm_sec, 
			zamanstr.tm_year + 1900 );

	nunetwork_send(client, buf, strlen(buf), 0);
	stat(filename, &filestat);
	if (strstr(filename, ".png")!=NULL || strstr(filename, ".css")!=NULL) {
		gmtime_r(&filestat.st_mtime, &zamanstr);
		snprintf(buf, sizeof(buf)-1, "Last-Modified: %s, %02d %s %02d:%02d:%02d %4d GMT\r\n",
                        cdays[zamanstr.tm_wday],
                        zamanstr.tm_mday,
                        cmonths[zamanstr.tm_mon],
                        zamanstr.tm_hour,
                        zamanstr.tm_min,
                        zamanstr.tm_sec,
                        zamanstr.tm_year + 1900 );
		nunetwork_send(client, buf, strlen(buf), 0);	
	}

        if (strcmp(mimetype(filename), "text/html") != 0 &&
            strcmp(mimetype(filename), "text/css") != 0) {
                        snprintf(buf, sizeof (buf)-1, "Accept-Ranges: bytes\r\n");
                        nunetwork_send(client, buf, strlen(buf), 0);
                        snprintf(buf, sizeof (buf)-1, "Content-Length: %d\r\n",
                                 (int) filestat.st_size);
                        nunetwork_send(client, buf, strlen(buf), 0);
        }
        snprintf(buf, sizeof (buf)-1, "Connection: close\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "Content-Type: %s\r\n\r\n",
                 mimetype(filename));
        nunetwork_send(client, buf, strlen(buf), 0);
}

/******************************************/
/* Send a client a 404 not found message. */
/******************************************/
void
not_found(nunetwork_socket client)
{
        char buf[1024];

        snprintf(buf, sizeof (buf)-1, "HTTP/1.1 404 NOT FOUND\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "%s", SERVER_STRING);
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "Content-Type: text/html\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "<HTML><TITLE>Not Found</TITLE>\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "<BODY>\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "%s", gettext("Your request is unavailable or nonexistant\r\n"));
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "</BODY></HTML>\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
}


/**********************************************************************/
/* Inform the client that the requested web method is not implemented */
/**********************************************************************/
void
unimplemented(nunetwork_socket client)
{
        char buf[1024];

        snprintf(buf, sizeof (buf)-1, "HTTP/1.1 501 Method Not Implemented\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "%s", SERVER_STRING);
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "Content-Type: text/html\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1,
                 "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "</TITLE></HEAD>\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1,
                 "<BODY>HTTP request method not supported.\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
        snprintf(buf, sizeof (buf)-1, "</BODY></HTML>\r\n");
        nunetwork_send(client, buf, strlen(buf), 0);
}

/**********************************************************************/



/******************************/
/* netustad network functions */
/* prefix -- nunetwork_       */
/******************************/


/*****************************/
/* Wrapper Functions         */
/* onur.yalazi@gmail.com     */
/*****************************/

void nunetwork_close(nunetwork_socket client) {

                #ifdef WITHOUT_SSL
                        close(client);
                #else
                        close(SSL_get_fd(client));
                        SSL_shutdown(client);
                #endif
}

int nunetwork_recv(nunetwork_socket s, char *buf, int num, int flags ) {
#ifdef WITHOUT_SSL
        return recv(s, buf, num, flags);
#else
        flags=0;
        return SSL_read(s, buf, num);
#endif
}

int nunetwork_send(nunetwork_socket s, char *buf, int num, int flags ) {

#ifdef WITHOUT_SSL
        return send(s, buf, num, flags);
#else
        flags=0;
        return SSL_write(s, buf, num);
#endif

}

#ifndef WITHOUT_SSL
/* 
	SSL_CTX *nunetwork_init_ctx()
	This function inits SSL Context object ctx with the supplied
	key & cert pair and return a 

*/
SSL_CTX *nunetwork_init_ctx( char *certfile, char *keyfile) {

                SSL_METHOD *meth;

                SSL_library_init();
                SSL_load_error_strings();
                SSLeay_add_ssl_algorithms();

                /* Create our context*/
                meth=SSLv23_server_method();
                ctx=SSL_CTX_new(meth);
                SSL_CTX_set_quiet_shutdown(ctx, 0);
                /* Load our keys and certificates*/
                if(SSL_CTX_use_certificate_chain_file(ctx,  certfile) <= 0) 
                        error_die(gettext("SSL Certificate file could not be found!\n"));
                
                if (!(SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM))) 
                        error_die(gettext("SSL Private Key file could not be found!\n"));
                
                if (!SSL_CTX_check_private_key(ctx)) 
                        error_die(gettext("SSL Private Key does not match certificate public key!\n"));

                #if (OPENSSL_VERSION_NUMBER < 0x00905100L)
                        SSL_CTX_set_verify_depth(ctx,1);
                #endif

                return ctx;
}

SSL *nunetwork_init_ssl( SSL_CTX* ctx) {
                return SSL_new(ctx);
}
#endif
/*
void nunetwork_discardheaders( nunetwork_socket client) {
         int numchars=1;
	 char buf[1024]="A";
         while ((numchars > 0) && strcmp("\n", buf) )  / read & discard headers * /
                numchars = get_line(client, (char *) buf, sizeof(buf));

}
*/

/* 
	This function will search for accepted headers
	of the clients, any un-acceptable header will be dropped 
	RETURN VALUE: 
		on succesful completion returns the the number of headers accepted 
		on getting no header (just getting CRLF) returns 0
		on error returns -1
		on request without a host header -2 or more than one method  (GET, HEAD, POST)
		
	onuryalazi@gmail.com
*/

int nunetwork_getheaders( nunetwork_socket client, struct nunetwork_headerstruct *headerstr, char **acceptedheaders) {

	int numchars=1, i, t;
	char tmpbuf[512];
	char *tmpstr;
	char *start;
	char header[256], value[256];
	int numheaders=0;
	int hostheader=0;
	int metcount=0;
	int clienth11=0;

	/* getting any headers sent */

	while (numheaders < MAX_HEADERS) {
	
		numchars = get_line( client, tmpbuf, sizeof(tmpbuf) );
		/* if no */
		if (  tmpbuf[0]=='\r' || tmpbuf[0]=='\n' )  {
				/* This is the end of headers */
				headerstr->number_of_headers = numheaders;
				return numheaders;
		} else if (!numchars)

                                /* read no char? something is strange! Return error */
                                return -1;

		else {

			/* Getting the header's type without : */
			/* header cannot be longer than the supplied header buffer */
			i=0;
			while (tmpbuf[i]!=' ' && tmpbuf[i]!='\t' && i < strlen(tmpbuf) && i++ < sizeof(header) ) ;
			

			strncpy( header, tmpbuf, i);
			header[i]='\0';
			t=0;
			/*checking if the header is acceptable */
			while ( acceptedheaders[t]!=NULL ) 
				if (!strcasecmp(acceptedheaders[t++], header))  {
					if (!strcasecmp(header, "Host:")) 
							hostheader=1;
					else if ( !strcasecmp(header, "Get") || 
					 	!strcasecmp(header, "Head") || 
					 	!strcasecmp(header, "Post")) {
							metcount++;	
							if (strstr(tmpbuf+i+1, "HTTP/1.1")!=NULL)  {
								clienth11 = 1;
								nunetwork_send(client, "HTTP/1.1 100 Continue\r\n\r\n", 25, 0);
							}

					}

					strncpy(headerstr->headers[numheaders].header_id, 
						header, 
						sizeof(headerstr->headers[numheaders].header_id));
					/* Some pointer arithmetic 		*/
					/* Just to remove headers identifier 	*/
					/* and to remove unwanted full url part	*/
					if (!strcasecmp(header, "GET") || 
						!strcasecmp(header, "HOST") ||
						!strcasecmp(header, "HEAD") ) {

						tmpstr = strdup(tmpbuf+i+1);
						start = tmpstr;
						while (*tmpstr!='\0') {
								*tmpstr=toupper(*tmpstr); 
								tmpstr++; 
						};
						if (strstr(start, "HTTP://") && 
				  			(!strcasecmp(header, "GET") || 
							 !strcasecmp(header, "HEAD") ||
							 !strcasecmp(header, "POST")) ) {
								tmpstr = strchr( tmpbuf+i+9, '/');
						} else tmpstr = tmpbuf + i + 1;

					} else tmpstr = tmpbuf + i + 1;
					strncpy(value, tmpstr, sizeof(value)-1  );
					strncpy(headerstr->headers[numheaders].header_value, 
						value, 
						sizeof(headerstr->headers[numheaders].header_value) );
					headerstr->number_of_headers=++numheaders;
					break;
				}  
		}
	}
	
	/*  It is a bad request if its HTTP/1.1 and doesn't have HOST: header 	*/
	/*  also it is bad if it has more than one of HEAD, GET, POST		*/
	if ( (!hostheader && clienth11 )|| metcount != 1) return -2;
	
	if (clienth11) nunetwork_send(client, "HTTP/1.1 100 Continue\r\n\r\n", 25, 0);
	return numheaders; 
}
