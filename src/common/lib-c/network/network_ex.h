/* Don't include this file twice... */
#ifndef __NETWORK_EX_H__
#define __NETWORK_EX_H__

#include "../types.h"
#include "ports.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <malloc.h>
#include <errno.h>           /* errno global variable */
//#include "../utils/safecalls.h"

#ifndef  INADDR_NONE
#define  INADDR_NONE 0xffffffff
#endif

#ifndef COPY_BUFSIZE
#define COPY_BUFSIZE 10*1024       /* Buffer size for copies is 10K */
#endif

/* Basic reading and writing */
int read_buffer(int fd, char *buf, int count);
int write_buffer(int fd, char *buf, int count);

/* String/delimited reading and writing */

int writestring(int sockid, char *str);
int readstring(int sockid, char *buf, int maxlen);
int readnlstring(int sockid, char *buf, int maxlen);
int readdelimstring(int sockid, char *buf, int maxlen, char delim);

/* Integer reading and writing */

int read_netulong(int fd, DWORD *value);
int write_netulong(int fd, const unsigned long int value);

/* Data copy */

int copy(int in, int out, unsigned long maxbytes);

/* Reverse DNS lookups and friends */

char *getmyfqdn(void);
char *getfqdn(const char *host);

/* Network initialization */

void socketaddr_init(struct sockaddr_in *socketaddr);
int socketaddr_service(struct sockaddr_in *socketaddr,
               const char *service, const char *proto);
int socketaddr_host(struct sockaddr_in *socketaddr,
               const char *host);
int resolveproto(const char *proto);
int prototype(const char *proto);
int clientconnect(const char *host, const char *port, const char *proto);
int serverinit(const char *port, const char *proto);

/* Miscellaneous */

void stripcrlf(char *temp);

#endif
