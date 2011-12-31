#ifndef NETWORK_H
#define NETWORK_H

#include "common.h"

#ifndef WITHOUT_SSL
	#include <openssl/ssl.h>
	#include <openssl/err.h>
        int sslerror;
        typedef SSL* nunetwork_socket;
        SSL *ssl;
        SSL_CTX *ctx;
        SSL_CTX *nunetwork_init_ctx( char *, char *);
        SSL *nunetwork_init_ssl();
#else
        typedef int nunetwork_socket;
#endif

#define MAX_HEADERS 5
struct nunetwork_header {
	char header_id[256];
	char header_value[256];
};

struct nunetwork_headerstruct {

	int number_of_headers;
	struct nunetwork_header headers[MAX_HEADERS];

};

void bad_request( nunetwork_socket );
void binarycat(nunetwork_socket , FILE *);
void cannot_execute(nunetwork_socket );
void headers(nunetwork_socket , char *);
void not_found(nunetwork_socket );
void unimplemented(nunetwork_socket );

int get_line(nunetwork_socket , char *, int );
int nunetwork_recv(nunetwork_socket , char *, int , int );
int nunetwork_send(nunetwork_socket , char *, int , int );
void nunetwork_close(nunetwork_socket );
/* void nunetwork_discardheaders( nunetwork_socket ); */
int nunetwork_getheaders( nunetwork_socket , struct nunetwork_headerstruct *, char ** );


#endif
