/*
 *  common/lib-c/network.h
 *  ---------------------------------
 *  Network lib entry points
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */

#ifndef _NETWORK_H
#define _NETWORK_H

#ifdef WIN32
  #include <winsock2.h>
  #include <io.h>
  #include <stdio.h>
  #include <string.h>
  BOOL NETWORK_WinsockStartup(void);
  void NETWORK_WinsockCleanup(void);
#else
  #include <stdio.h>
  #include <string.h>
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/time.h>
  #include <sys/socket.h>
  #include <sys/ioctl.h>
  #include <net/if.h>
  #include <netinet/in.h>
  #include <netinet/ip.h>
  #include <netinet/ip_icmp.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <fcntl.h>
  
  typedef int SOCKET;
  #define INVALID_SOCKET 0
  #define SOCKET_ERROR 0
#endif

/*
 * From udp.c
 */
BOOL NETWORK_UDPOpen(SOCKET *sk, WORD port);
BOOL NETWORK_UDPRecv(SOCKET sk, BYTE *rcvAddr, WORD *rcvPort, void *msg, WORD *len);
BOOL NETWORK_UDPRecvWait(SOCKET sk, BYTE *rcvAddr, WORD *rcvPort, void *msg, WORD *len, DWORD wait_sec);
BOOL NETWORK_UDPSend(SOCKET sk, const BYTE *dstAddr, WORD dstPort, const void *msg, WORD len);

/*
 * From tcp.c
 */

/* Server functions */
BOOL NETWORK_TCPSrvOpen(SOCKET *sk, WORD port);

typedef BOOL (*TCP_SERV_PROC) (SOCKET client_sk, const BYTE *cliAddr, WORD cliPort);
BOOL NETWORK_TCPSrvLoopSingle(SOCKET server_sk, TCP_SERV_PROC proc);
BOOL NETWORK_TCPSrvLoopMulti(SOCKET server_sk, TCP_SERV_PROC proc);

typedef BOOL (*TCP_SERV_PROC_EX) (SOCKET client_sk, const BYTE *cliAddr, WORD cliPort, void *serverParam);
BOOL NETWORK_TCPSrvLoopSingleEx(SOCKET server_sk, TCP_SERV_PROC_EX proc, void *serverParam);
BOOL NETWORK_TCPSrvLoopMultiEx(SOCKET server_sk, TCP_SERV_PROC_EX proc, void *serverParam);

BOOL NETWORK_TCPSend(SOCKET sk, const void *msg, WORD len);
BOOL NETWORK_TCPRecv(SOCKET sk, void *msg, WORD *len);
BOOL NETWORK_TCPWaitEx(SOCKET sk, DWORD wait_sec, BOOL *is_timeout);
BOOL NETWORK_TCPWait(SOCKET sk, DWORD wait_sec);
BOOL NETWORK_TCPWaitRecv(SOCKET sk, void *msg, WORD *len, DWORD wait_sec);

BOOL NETWORK_TCPSrvRecv(SOCKET sk, BYTE *rcvAddr, WORD *rcvPort, void *msg, WORD *len);
BOOL NETWORK_TCPSrvSend(SOCKET sk, BYTE *dstAddr, WORD dstPort, const void *msg, WORD len);


/* Client functions */

BOOL NETWORK_TCPCliOpen(SOCKET *sk, const BYTE *dstAddr, WORD port);
//BOOL NETWORK_TCPCliOpen(SOCKET *sk, BYTE dstAddr[4], WORD port);
BOOL NETWORK_TCPCliRecv(SOCKET sk, void *msg, WORD *len);
BOOL NETWORK_TCPCliSend(SOCKET sk, const void *msg, WORD len);

/*
 * From utils.c
 */
char *NETWORK_AddrToStr(BYTE *addr);
BOOL NETWORK_GetMyAddr(BYTE *addr);
BOOL NETWORK_GetMyName(char *name, int nameLen);
BOOL NETWORK_GetSockName(unsigned int sk, BYTE *addr, WORD *port);
BOOL NETWORK_GetHostByName(BYTE *addr, const char *name);
BOOL NETWORK_GetHostByAddr(BYTE *addr, const char *str);

#ifdef WIN32
  BOOL NETWORK_WinsockStartup(void);
  void NETWORK_WinsockCleanup(void);
#endif

/*
 * From network.c
 */
BOOL NETWORK_CreateClient(SOCKET *sk, const char *host, const char *port, const char *proto);
BOOL NETWORK_CreateServer(SOCKET *sk, const char *port, const char *proto);
int NETWORK_WriteBuffer(SOCKET sk, char *buf, int count);
int NETWORK_ReadBuffer(SOCKET sk, char *buf, int count);
int NETWORK_WriteString(SOCKET sk, char *str);
int NETWORK_ReadString(SOCKET sk, char *buf, int maxlen);
int NETWORK_ReadDelimString(SOCKET sk, char *buf, int maxlen, char delim);
int NETWORK_ReadNewLineString(SOCKET sk, char *buf, int maxlen);
void NETWORK_Close(SOCKET sk);

#define NETWORK_PERROR(x) perror(x)

#endif
