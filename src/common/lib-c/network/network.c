/*
 *  common/lib-c/network/network.c
 *  -----------------------------------------
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "../utils/types.h"
#include "network.h"

#ifdef WIN32
  #pragma comment(lib, "WS2_32")
  #pragma warning( disable : 4996 )
#endif

static int checkstring(const char *string);

/* checkstring() is a private function used only by this library.  It checks
    the passed string.  It returns false if there are no nonnumeric
    characters  in the string, or true if there are such characters. */

static int checkstring(const char *string)
{
  unsigned int counter;
  for (counter = 0; counter < strlen(string); counter++)
    if (!(isdigit(string[counter])))
      return 1;
  return 0;
}

/*
 * NETWORK_WriteString
 * -------------------
 * Send a string, including terminating null.  readdelimstring() could be
 * perfect for reading it on the other end.  And in fact, readstring()
 * uses just that
 */
int NETWORK_WriteString(SOCKET sockid, char *str)
{
  return NETWORK_WriteBuffer(sockid, str, strlen(str) + 1);
}

/*
 * NETWORK_ReadString
 * ------------------
 * Reads a string from the network, terminated by a null
 */
int NETWORK_ReadString(SOCKET sockid, char *buf, int maxlen)
{
  return NETWORK_ReadDelimString(sockid, buf, maxlen, 0);
}

/*
 * NETWORK_ReadNewLineString
 * -------------------------
 * Reads a string terminated by a newline
 */
int NETWORK_ReadNewLineString(SOCKET sockid, char *buf, int maxlen)
{
  return NETWORK_ReadDelimString(sockid, buf, maxlen, '\n');
}

/*
 * NETWORK_ReadDelimString
 * -----------------------
 * Reads a string with an arbitrary ending delimiter
 */
int NETWORK_ReadDelimString(SOCKET sockid, char *buf, int maxlen, char delim)
{
  int count = 0, status;

  while (count <= maxlen)
  {
    status = read(sockid, buf+count, 1);
    if (status < 0) return status;
    if (status < 1)
    {
      return status;
    }
    if (buf[count] == delim)
    {            /* Found the delimiter */
      buf[count] = 0;
      return 0;
    }
    count++;
  }
  return 0;
}

#if 0
/* Copies data from the in to the out file descriptor.  If numsize
   is nonzero, specifies the maximum number of bytes to copy.  If
   it is 0, data will continue being copied until in returns EOF. */

int copy(int in, int out, unsigned long maxbytes) {
  char buffer[COPY_BUFSIZE];
  int indata, remaining;

  remaining = maxbytes;

  while (remaining || !maxbytes) {
    indata = saferead(in, buffer,
             (!remaining || COPY_BUFSIZE < remaining) ? COPY_BUFSIZE
              : remaining);
    if (indata < 1) return indata;
    write_buffer(out, buffer, indata);
    if (maxbytes) remaining -= indata;
  }
  return (0);
}
#endif

/*
 * NETWORK_WriteBuffer
 * -------------------
 */

int NETWORK_WriteBuffer(SOCKET sk, char *buf, int count)
{
  int  status = 0, result;

  if (count < 0) return (-1);

  while (status != count)
  {
    result = write(sk, buf + status, count - status);
    if (result < 0) return result;
    status += result;
  }
  return (status);
}

/*
 * NETWORK_ReadBuffer
 * ------------------
 * This function will read a number of bytes from the descriptor sk.  The
 * number of bytes read are returned.  In the event of an error, the
 * error handler is returned.  In the event of an EOF at the first read
 * attempt, 0 is returned.  In the event of an EOF after some data has
 * been received, the count of the already-received data is returned
 */
int NETWORK_ReadBuffer(SOCKET sk, char *buf, int count)
{
  char *pts = buf;
  int  status = 0, n;

  if (count < 0) return (-1);

  while (status != count)
  {
    n = read(sk, pts+status, count-status);
    if (n < 0) return n;
    if (n == 0) return status;
    status += n;
  }
  return (status);
}

/* Reads a uint32 from the network in network byte order.

   A note on the implementation: because some architectures cannot
   write to the memory of the integer except all at once, a character
   buffer is used that is then copied into place all at once. */

int read_netulong(int sk, DWORD *value)
{
  char buffer[sizeof(DWORD)];
  int status;

  status = NETWORK_ReadBuffer(sk, buffer, sizeof(DWORD));
  if (status != sizeof(DWORD)) {
    return -1;
  }
  memcpy(buffer, (char *)value, sizeof(DWORD));
  *value = ntohl(*value);
  return (0);
}

/* Write an unsigned long in network byte order */

int write_netulong(int sk, const unsigned long int value) {
  char buffer[sizeof(DWORD)];
  DWORD temp;
  int status;

   temp = htonl(value);
   memcpy(buffer, (char *)&temp, sizeof(temp));
   status = NETWORK_WriteBuffer(sk, buffer, sizeof(temp));
   if (status != sizeof(temp)) return -1;
   return (0);
}


void socketaddr_init(struct sockaddr_in *socketaddr)
{
  memset((char *) socketaddr, 0, sizeof(*socketaddr));
  socketaddr->sin_family = AF_INET;
}

int socketaddr_service(struct sockaddr_in *socketaddr,
                       const char *service, const char *proto) {
  struct servent *serviceaddr;

  /* Need to allow numeric as well as textual data. */

  /* 0: pass right through. */

  if (strcmp(service, "0") == 0)
    socketaddr->sin_port = 0;
  else {                           /* nonzero port */
    serviceaddr = getservbyname(service, proto);
    if (serviceaddr) {
      socketaddr->sin_port = serviceaddr->s_port;
    } else {                      /* name did not resolve, try number */
      if (checkstring(service)) { /* and it's a text name, fail. */
        return -1;
      }
      if ((socketaddr->sin_port = htons((u_short)atoi(service))) == 0) {
        return -1;
      }
    }
  }

  return 0;
}

int socketaddr_host(struct sockaddr_in *socketaddr,
                    const char *host) {
  struct hostent *hostaddr;
  hostaddr = gethostbyname(host);
  if (!hostaddr) {
    return -1;
  }

  memcpy(&socketaddr->sin_addr, hostaddr->h_addr, hostaddr->h_length);
  return 0;
}

int resolveproto(const char *proto) {
  struct protoent *protocol;
  protocol = getprotobyname(proto);
  if (!protocol) {
    return -1;
  }

  return protocol->p_proto;
}

int prototype(const char *proto)
{
  if (strcmp(proto, "tcp") == 0) return SOCK_STREAM;
  if (strcmp(proto, "udp") == 0) return SOCK_DGRAM;
  return -1;
}

/*
 * Create a TCP or UDP client connected to a specific server
 * ---------------------------------------------------------
 */
BOOL NETWORK_CreateClient(SOCKET *sk, const char *host, const char *port, const char *proto)
{
  struct sockaddr_in socketaddr;
  int sockid, iproto;

#ifdef WIN32
  if (!NETWORK_WinsockStartup())
    return FALSE;
#endif

  iproto = prototype(proto);
  if (iproto == -1)
    return FALSE;

  sockid = socket(PF_INET, iproto, resolveproto(proto));
  if (sockid < 0)
    return FALSE;

  if (iproto == SOCK_STREAM)
  {
    socketaddr_init(&socketaddr);
    socketaddr_service(&socketaddr, port, proto);
    socketaddr_host(&socketaddr, host);

    if (connect(sockid, (struct sockaddr *) &socketaddr, sizeof(socketaddr)) < 0)
      return FALSE;
  }

  *sk = sockid;
  return TRUE;
}

/*
 * Create a TCP or UDP server listening to a specific port
 * -------------------------------------------------------
 */
BOOL NETWORK_CreateServer(SOCKET *sk, const char *port, const char *proto)
{
  struct sockaddr_in socketaddr;
  int mastersock;
  char trueval = 1;

#ifdef WIN32
  if (!NETWORK_WinsockStartup())
    return FALSE;
#endif

  socketaddr_init(&socketaddr);
  socketaddr.sin_addr.s_addr = INADDR_ANY;
  socketaddr_service(&socketaddr, port, proto);

  mastersock = socket(PF_INET, prototype(proto), resolveproto(proto));
  if (mastersock < 0)
    return FALSE;

  if (bind(mastersock, (struct sockaddr *) &socketaddr, sizeof(socketaddr)) < 0)
    return FALSE;

  setsockopt(mastersock, SOL_SOCKET, SO_REUSEADDR, &trueval, sizeof(trueval));

  if (prototype(proto) == SOCK_STREAM)
  {
	/* modifié par mario le 03/06/02                                 */
	/* motif: je n'autorise aucune autre connexion en attente durant */
	/*         la connexion principale ouverte                       */
	/* avant : if (listen(mastersock, 5) < 0) */
    if (listen(mastersock, 5) < 0)
	{
      return FALSE;
	}
  }

  *sk = mastersock;
  return TRUE;
}

/*
 * Removes CR and LF from the end of a string
 * ------------------------------------------
 */
void NETWORK_StripCRLF(char *temp)
{
  while (strlen(temp)
    && ((temp[strlen(temp)-1] == 13) || (temp[strlen(temp)-1] == 10)))
  {
    temp[strlen(temp)-1] = 0;
  }
}

/*
 * Close a socket
 * --------------
 */
void NETWORK_Close(SOCKET sk)
{
#ifdef WIN32
  closesocket(sk);
#else
  if (close(sk))
  {
  	//perror("close");
  }
#endif
}


#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#endif
