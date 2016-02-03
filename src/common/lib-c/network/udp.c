/*
 *  common/lib-c/network/udp.c
 *  -------------------------------------
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

/*
 * C lib
 */
#include <stdio.h>
#include <string.h>

/*
 * SpringCard includes
 */
#include "../types.h"
#include "network.h"

/*
 * Open new UDP server socket
 */
BOOL NETWORK_UDPOpen(SOCKET *sk, WORD port)
{
	SOCKET new_sk;
  struct sockaddr_in sa; /* Local address */
  
  if (sk == NULL) return FALSE;

#ifdef WIN32
  if (!NETWORK_WinsockStartup()) return FALSE;
#endif

  /* Open socket */
  /* ----------- */
  new_sk = socket(AF_INET, SOCK_DGRAM, 0);
  if ((new_sk < 0) || (new_sk == INVALID_SOCKET))
  {
    //perror("UDPOpen:socket");
    return FALSE;
  }

  if (port)
  {
    /* Bind socket to specified port */
    /* ----------------------------- */
    memset(&sa, 0, sizeof(sa));
    sa.sin_family      = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    sa.sin_port        = htons(port);
  
    if (bind(new_sk, (struct sockaddr *)&sa, sizeof(sa)) < 0)
	  {
	    //perror("UDPOpen:bind");
	    NETWORK_Close(new_sk);
	    return FALSE;
	  }
  }

  /* Everything seems to work fine */
  /* ----------------------------- */
  *sk = new_sk;
  return TRUE;
}

/*
 * Wait for incoming UDP data
 */
BOOL NETWORK_UDPRecv(SOCKET sk, BYTE *rcvAddr, WORD *rcvPort, void *msg, WORD *len)
{
  struct sockaddr_in sa;
  unsigned int saLen;
  int rc;

  if (len == NULL) return FALSE;
  if (msg == NULL) return FALSE;

  saLen = sizeof(sa);
  memset(msg, 0, *len);

  rc = recvfrom(sk, msg, *len, 0, (struct sockaddr *)&sa, &saLen);
  if (rc < 0)
  {
    //perror("UDPRecv:recvfrom");
    return FALSE;
  }
  *len = (WORD) rc;

  if (rcvAddr != NULL)
    memcpy(rcvAddr, &sa.sin_addr, 4); /* IPv4 */
  if (rcvPort != NULL)
    *rcvPort = ntohs(sa.sin_port);

  return TRUE;
}

/*
 * Wait for incoming UDP data (with max duration)
 */
/*
int isReadable(int sd,int * error,int timeOut) { // milliseconds
  fd_set socketReadSet;
  FD_ZERO(&socketReadSet);
  FD_SET(sd,&socketReadSet);
  struct timeval tv;
  if (timeOut) {
    tv.tv_sec  = timeOut / 1000;
    tv.tv_usec = (timeOut % 1000) * 1000;
  } else {
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
  } // if
  if (select(sd+1,&socketReadSet,0,0,&tv) == SOCKET_ERROR) {
    *error = 1;
    return 0;
  } // if
  *error = 0;
  return FD_ISSET(sd,&socketReadSet) != 0;
}
*/


BOOL NETWORK_UDPRecvWait(SOCKET sk, BYTE *rcvAddr, WORD *rcvPort, void *msg, WORD *len, DWORD wait_sec)
{
	fd_set rdfdset;
	int selret;
  struct timeval timeout;
  struct sockaddr_in sa;
  unsigned int saLen;
  int rc;

  if (len == NULL) return FALSE;
  if (msg == NULL) return FALSE;

  timeout.tv_sec  = wait_sec;
  timeout.tv_usec = 0;

	FD_ZERO(&rdfdset);
	FD_SET(sk, &rdfdset);

	selret = select(sk+1, &rdfdset, 0, 0, &timeout);
  if (selret == SOCKET_ERROR)
  {
    //perror("UDPRecvWait:select");
    return FALSE;
  }
	if (!selret)
  {
    *len = 0;
    return FALSE;
  }


  saLen = sizeof(sa);
  memset(msg, 0, *len);

  rc = recvfrom(sk, msg, *len, 0, (struct sockaddr *)&sa, &saLen);
  if (rc < 0)
  {
    //perror("UDPRecvWait:recvfrom");
    return FALSE;
  }
  *len = (WORD) rc;

  if (rcvAddr != NULL)
    memcpy(rcvAddr, &sa.sin_addr, 4); /* IPv4 */
  if (rcvPort != NULL)
    *rcvPort = ntohs(sa.sin_port);

  return TRUE;
}

/*
 * Send data via UDP
 */
BOOL NETWORK_UDPSend(SOCKET sk, const BYTE *dstAddr, WORD dstPort, const void *msg, WORD len)
{
  struct sockaddr_in sa;

  /* Init destination address */
  /* ------------------------ */
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  if (dstAddr != NULL)
  {
    memcpy(&sa.sin_addr, dstAddr, 4); /* IPv4 */
  } else
  {
    BYTE tmp[4] = {127,0,0,1}; 
    memcpy(&sa.sin_addr, tmp, 4); /* IPv4 */
  }
  sa.sin_port   = htons(dstPort);

  /* Perform sendto */
  /* -------------- */
  if (sendto(sk, msg, len, 0, (const struct sockaddr *)&sa, sizeof(sa)) != len)
  {
    //perror("UDPSend:sendto");
    return FALSE;
  }

  return TRUE;
}

