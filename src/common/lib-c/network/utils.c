/*
 *  common/lib-c/network/utils.c
 *  ---------------------------------------
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
#include "../utils/types.h"
#include "network.h"

#ifdef WIN32
  #pragma warning( disable : 4996 )
#endif

char *NETWORK_AddrToStr(BYTE *addr)
{
  static char buf[18];
  sprintf(buf, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
  return buf;
}

BOOL NETWORK_GetSockName(unsigned int sk, BYTE *addr, WORD *port)
{
	struct sockaddr_in sa;
  unsigned int len = sizeof(sa);
	if (getsockname(sk, (struct sockaddr *)&sa, &len) < 0)
  {
		return FALSE;
	}
  memcpy(addr, &sa.sin_addr, 4);
  *port   = ntohs(sa.sin_port);
  return TRUE;
}

BOOL NETWORK_GetMyName(char *name, int nameLen)
{
#ifdef WIN32
  if (!NETWORK_WinsockStartup()) return FALSE;
#endif

  return (gethostname(name, nameLen) == 0);
}

BOOL NETWORK_GetMyAddr(BYTE *addr)
{
#ifdef __linux__

  int fd = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr;

	strcpy(ifr.ifr_name, "eth0");
	ioctl(fd, SIOCGIFADDR, (int)&ifr);
	close(fd);

	memcpy(addr, &(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), 4);

  return TRUE;
#else
  char name[64];

#ifdef WIN32
  if (!NETWORK_WinsockStartup()) return FALSE;
#endif

  if (NETWORK_GetMyName(name, sizeof(name)))
    return NETWORK_GetHostByName(addr, name);
  return FALSE;
#endif
}

BOOL NETWORK_GetHostByName(BYTE *addr, const char *name)
{
  struct hostent *hp;

#ifdef WIN32
  if (!NETWORK_WinsockStartup()) return FALSE;
#endif

  hp = gethostbyname(name);
  if (!hp)
    return FALSE;

  memcpy(addr, hp->h_addr, 4);
  return TRUE;
}

BOOL NETWORK_GetHostByAddr(BYTE *addr, const char *str)
{
  int iaddr[4];
  if (sscanf(str, "%d.%d.%d.%d", &(iaddr[0]), &(iaddr[1]), &(iaddr[2]), &(iaddr[3])) != 4)
    return FALSE;

  addr[0] = (BYTE) iaddr[0];
  addr[1] = (BYTE) iaddr[1];
  addr[2] = (BYTE) iaddr[2];
  addr[3] = (BYTE) iaddr[3];
  return TRUE;
}

#ifdef WIN32
static BOOL NETWORK_WinsockStarted = FALSE;
BOOL NETWORK_WinsockStartup(void)
{
  int err;
  WORD wVersionRequested;
  WSADATA wsaData;


  if (NETWORK_WinsockStarted)
    return TRUE;

  wVersionRequested = MAKEWORD( 2, 0 );
  err = WSAStartup( wVersionRequested, &wsaData );
  if ( err != 0 )
	  return FALSE;

  if ( wsaData.wVersion < wVersionRequested )
  { 
	  WSACleanup();
	  return FALSE;
  }

  NETWORK_WinsockStarted = TRUE;
  return TRUE;
}

void NETWORK_WinsockCleanup(void)
{
  NETWORK_WinsockStarted = FALSE;
  WSACleanup();
}
#endif
