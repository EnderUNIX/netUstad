/* ******** netUstad: Network Ustadi (Network Master) *********/
/* This software is Copyright (C) 2004 by Ozkan KIRIK.        */
/* Permission is granted to redistribute and modify this      */
/* software under the terms of the GNU General Public License */
/* available at http://www.gnu.org/                           */
/**************************************************************/


#define PID_FILE	"/var/run/netustad.pid"

#include "common.h"
#include "loadconfig.h"
#include "mimetype.h"
#include "acceptrequest.h"

/* Headers that are handled  */
char *nu_acceptedheaders[]={"GET", "POST", /*"If-Modified-Since",*/ "HOST", "HEAD", NULL };

int server_sock = -1;
char server_sid[] = "1234567890123456789012";


/**********************************************************************/
/* Write message to log file					      */
/**********************************************************************/
void
log_msg(char *filename, char *msg, char booltime)
{
	FILE *log_file;
	char chartime[50]="";
	char logmsg[1024]="";
	time_t timenow;
	struct tm *timestr;
	
	log_file = fopen(filename, "a");
	if (!log_file)
		return;
	
	if (booltime==1) {
		timenow = time(NULL);
		timestr = localtime(&timenow);
		strftime( chartime, sizeof(chartime)-1, "%c --", timestr);	
		
	}
	snprintf(logmsg, sizeof(logmsg)-1, "%s %s", chartime, msg);
	fprintf(log_file, "%s", logmsg);
	fclose(log_file);
	return;
}

/**********************************************************************/
/* Catch and parse signals                                            */
/**********************************************************************/
void
signal_handler(int sig)
{
	switch (sig) {
	case SIGXFSZ:
		log_msg(mainlogfile, gettext("File size limit exceeded"), 1);
		log_msg(mainlogfile, gettext("\nProgram Terminated.\n"), 1);
		log_msg(mainlogfile,
			"<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>\n\n", 0);
		if (client_sock != -1)
				close(client_sock);

		#ifndef WITHOUT_SSL
		        SSL_shutdown(ssl);
    			SSL_free(ssl);
    			SSL_CTX_free(ctx);
		#else
        		close(server_sock);
		#endif

		exit(1);

		break;
	case SIGPIPE:
		
		#ifndef WITHOUT_SSL		
			nunetwork_close(ssl);
		#else
			nunetwork_close(client_sock);
		#endif
		
		break;
	case SIGHUP:
		log_msg(mainlogfile, gettext("Hangup signal catched"), 1);
		log_msg(mainlogfile, gettext("\nReloading configuration...\n\n"), 1);
		readconfig(conffile);
		setlocale (LC_ALL, (const char*)lc_all);
		bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
		textdomain ( PACKAGE );
		log_msg(mainlogfile,
			gettext("Configuration reloaded successfully.\n\n"), 1);

		break;
	case SIGTERM:
		log_msg(mainlogfile, gettext("Terminate signal catched"), 1);
		log_msg(mainlogfile, gettext("\nClosing active sessions...\n"), 1);
		if (client_sock != -1)
			close(client_sock);
		
		#ifndef WITHOUT_SSL
        		SSL_shutdown(ssl);
    			SSL_free(ssl);
    			SSL_CTX_free(ctx);
		#else
        		close(server_sock);
		#endif
		

		log_msg(mainlogfile, gettext("Bye bye\n"), 1);
		log_msg(mainlogfile,
			"<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>\n\n", 0);
		log_msg(logfile,
			"<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>\n\n", 0);
		exit(0);
		break;
	}
}

/**************************************/
/* Get the program to become a daemon */
/**************************************/
void
daemonize()
{
	int i, lfp;
	char str[10];
	if (getppid() == 1)
		return;		/* already a daemon */
	i = fork();
	if (i < 0)
		exit(1);	/* fork error */
	if (i > 0)
		exit(0);	/* parent exits */
	/* child (daemon) continues */
	setsid();		/* obtain a new process group */
	for (i = getdtablesize(); i >= 0; --i)
		close(i);	/* close all descriptors */

	i = open("/dev/null", O_RDWR);
	dup(i);
	dup(i);			/* handle standart I/O */
	umask(027);		/* set newly created file permissions */
	chdir(workdir);		/* change running directory */

	lfp = open(PID_FILE, O_RDWR | O_CREAT, 0640);
	if (lfp < 0)
		exit(1);	/* can not open */

	if (lockf(lfp, F_TLOCK, 0) < 0) {
		log_msg(mainlogfile, gettext("netUstad is already running!\n"), 1);
		log_msg(nameoftty, gettext("netUstad is already running!\n"), 1);
		syslog(LOG_WARNING, gettext("netUstad is already running!\n"));
		exit(0);
	}

	/* can not lock */
	/* first instance continues */
	snprintf(str, sizeof (str)-1, "%d\n", getpid());
	write(lfp, str, strlen(str));	/* record pid to lockfile */
	/*signal(SIGCHLD,SIG_IGN); *//* ignore child */
	signal(SIGTSTP, SIG_IGN);	/* ignore tty signals */
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGXFSZ, signal_handler);	/* catch file size limit exceeded sgn */
	signal(SIGPIPE, signal_handler);	/* catch broken pipe sgn */
	signal(SIGHUP, signal_handler);	/* catch hangup signal */
	signal(SIGTERM, signal_handler);	/* catch kill signal */
}

/**************************************************/
/* Log the Critical error, and send it to SysLog! */
/**************************************************/
void
error_die(char *errormsg)
{
	log_msg(mainlogfile, errormsg, 1);
	syslog(LOG_EMERG, errormsg);
	exit(1);
}


/********************************************/
/* Open Socket and check errors then listen */
/********************************************/
int
startup(u_short * port)
{
	int httpd = 0;
	struct sockaddr_in name;
	int tmp;

	httpd = socket(PF_INET, SOCK_STREAM, 0);
	if (httpd == -1)
		error_die(gettext("Socket error\n"));
	memset(&name, 0, sizeof (name));
	tmp = 1;
	setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, (char *) &tmp,
		   sizeof (tmp));
	name.sin_family = AF_INET;
	name.sin_port = htons(*port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(httpd, (struct sockaddr *) &name, sizeof (name)) < 0)
		error_die(gettext("Bind error\n"));
	if (*port == 0) {	/* if dynamically allocating a port */
		int namelen = sizeof (name);
		if (getsockname(httpd, (struct sockaddr *) &name, (socklen_t *)&namelen) ==
		    -1)
			error_die(gettext("Getsockname error\n"));
		*port = ntohs(name.sin_port);
	}
	if (listen(httpd, 1) < 0)
		error_die(gettext("Listen error\n"));
	return (httpd);
}


/**********************************************************************/

int
main(int argc, char *argv[])
{
	
	int client_sock = -1;
	int client_name_len = sizeof (client_name);
	int request=0;
	
	struct nunetwork_headerstruct *header_buf;
	header_buf = (struct nunetwork_headerstruct*)malloc( sizeof(*header_buf) );
	/********************
         * Don't forget to make pointer to variable cast (*pointer)
         * When using sizeof(). Otherwise it may cost you a week of
	 * debug time.
         *******************/
	

	if (geteuid()) {
		printf("Netustad must be run as root.\n");
		exit(1);
	}
	
	conffile	= (char *)strdup(default_conf_file);
	
	
	/*setlocale (LC_ALL, "");*/
	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain ( PACKAGE );
	

	/*******************/
	/* Check Arguments */
	/*******************/
	if (argc==2) {	
			
			if (strcmp(argv[1], "-h") == 0 ||
				strcmp(argv[1], "--help") == 0) {
				printf( gettext("\nnetUstad: Network Ustadi (Network Master)\n"));
				printf( gettext("Copyright (C) 2004   by Ozkan KIRIK\n"));
				printf( gettext("Usage: netustad [options]\n"));
				printf( gettext("\n"));
				printf( "%s:\n", gettext("Options"));
				printf( gettext("\t-h\tShow this help screen\n"));
				printf( gettext("\t-v\tShow version\n"));
				printf
					(gettext("\t-c\tUse following parameter as configuration file"));
				printf("\n");
				exit(0);
			} else if (strcmp(argv[1], "-v") == 0 ||
				   strcmp(argv[1], "--version") == 0) {
				printf(gettext("\nnetUstad: Network Ustadi (Network Master)\n"));
				printf(gettext("Copyright (C) 2004  by Ozkan KIRIK\n\n"));
				printf(gettext("Version: %s\n\n"), NUVERSION);
				exit(0);
			}
				
	} else if (argc == 3 && strcmp(argv[1], "-c") == 0) {
			conffile = strdup(argv[2]);

	}  else if (argc!=1) {
			if (strcmp(argv[1], "-c") == 0) {
				printf (gettext("\nnetUstad: Invalid Number Of Arguments\n\n"));
			} else {
				printf(gettext("\nnetUstad: Invalid Argument\n\n"));
			}
			exit(1);

	}


	/**********************/
	/* Start Main Program */
	/**********************/

	readconfig(conffile);
	if (setlocale (LC_ALL, (const char*) lc_all) == NULL) {
		log_msg(mainlogfile, gettext("setlocale failed\n"), 1);
		printf(gettext("setlocale failed\n"));
	}
	bindtextdomain (PACKAGE, PACKAGE_LOCALE_DIR);
	textdomain ( PACKAGE );
	nameoftty = ttyname(0);
	daemonize();

	#ifndef WITHOUT_SSL
        	ssl = nunetwork_init_ssl( nunetwork_init_ctx( cert_file, key_file) );
	#endif

	server_sock = startup(&port);	/* Open Socket & Listen */

	log_msg(mainlogfile, "<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>\n", 0);
	snprintf(log_msg_text, sizeof(log_msg_text)-1, "netUstad-%s\n", NUVERSION);
        log_msg(mainlogfile, log_msg_text, 0);
	snprintf(log_msg_text, sizeof (log_msg_text)-1, "%s",
		 gettext("netUstad is started\n"));
	log_msg(mainlogfile, log_msg_text, 1);
	snprintf(log_msg_text, sizeof (log_msg_text)-1, gettext("\nListening port %d\n"), port);
	log_msg(mainlogfile, log_msg_text, 0);
	log_msg(mainlogfile, gettext("Ready for requests...\n\n"), 0);
	snprintf(log_msg_text, sizeof(log_msg_text)-1, "netUstad-%s\n", NUVERSION);
	log_msg(nameoftty, log_msg_text, 0);
	snprintf(log_msg_text, sizeof (log_msg_text)-1,
		 gettext("\nnetUstad is started\nListening port %d\n"), port);
	log_msg(nameoftty, log_msg_text, 0);

	while (1) {
		client_sock = accept(server_sock,
				     (struct sockaddr *) &client_name,
				     (socklen_t *)&client_name_len);

		if (client_sock == -1)
			continue;
	
		 #ifndef WITHOUT_SSL
		        SSL_set_fd(ssl, client_sock);
		        sslerror = SSL_accept(ssl);
		
		        if ( sslerror <= 0 ) {
		                sslerror= SSL_get_error(ssl, sslerror);
		                ERR_error_string(sslerror, log_msg_text);
		
		                log_msg(mainlogfile, log_msg_text, 1);
				log_msg(mainlogfile, "\n",0);
				SSL_shutdown(ssl);
				SSL_free(ssl);
				close(client_sock);
				ssl = nunetwork_init_ssl(ctx);
				continue;
		        }
			
			request =  nunetwork_getheaders(ssl, header_buf, nu_acceptedheaders); 
			if (request > 0) 
				accept_request(ssl, header_buf);
			else if (request==-2 || request==0)
				bad_request(ssl);
			nunetwork_close(ssl);
		 #else
			request = nunetwork_getheaders(client_sock, header_buf, nu_acceptedheaders);
		        if (request > 0)
				accept_request(client_sock, header_buf);
			else if (request==-2 || request==0 )
				bad_request(ssl);
		 	nunetwork_close(client_sock);
		 #endif
		
	
	
	}
	#ifndef WITHOUT_SSL
		SSL_shutdown(ssl);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
	#else
		close(server_sock);
	#endif


	return (0);
}
