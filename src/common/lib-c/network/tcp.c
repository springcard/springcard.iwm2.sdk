/*
 *  common/lib-c/network/tcp.c
 *  -------------------------------------
 *  Copyright (c) 2000-2008 SpringCard - www.springcard.com
 */


/*
 * SpringCard includes
 */
#include "../utils/types.h"
#include "network.h"

/*
 * Open new TCP server socket
 */
BOOL NETWORK_TCPSrvOpen(SOCKET *sk, WORD port)
{
  SOCKET new_sk;
  struct sockaddr_in sa;
  char trueval = 1;
  
  if (sk == NULL) return FALSE;

#ifdef WIN32
  if (!NETWORK_WinsockStartup()) return FALSE;
#endif

  new_sk = socket(AF_INET, SOCK_STREAM, 0);
  if ((new_sk < 0) || (new_sk == INVALID_SOCKET))
  {
    return FALSE;
  }

  setsockopt(new_sk, SOL_SOCKET, SO_REUSEADDR, &trueval, sizeof(trueval));

  memset((char*)&sa, 0, sizeof(sa));
  sa.sin_family      = AF_INET;
  sa.sin_addr.s_addr = htonl(INADDR_ANY);
  sa.sin_port        = htons(port);

  if (bind(new_sk, (struct sockaddr *)&sa, sizeof(sa)) < 0)
  {
    NETWORK_Close(new_sk);
    return FALSE;
  }

  if (listen(new_sk, SOMAXCONN) < 0)
  {
    NETWORK_Close(new_sk);
    return FALSE;
  }

  *sk = new_sk;
  return TRUE;
}

BOOL NETWORK_TCPSrvLoop(SOCKET sk, TCP_SERV_PROC proc, BOOL keep_client_open)
{
  SOCKET new_sk;
	struct sockaddr_in sa;
	fd_set rdfdset;
	int selret;
	unsigned int salen;
	
	if (proc == NULL) return FALSE;

  for(;;)
  {
		FD_ZERO(&rdfdset);
		FD_SET(sk, &rdfdset);

		/* Wait for any connection.  */
		selret = select(sk+1, &rdfdset, 0, 0, NULL);
    if (selret == SOCKET_ERROR)
    {
      return FALSE;
    }
		if (!selret)
    {
      continue;
    }

		if(FD_ISSET(sk, &rdfdset))
    {
			/* Accept the incoming connection and handle it in a child.  */
			salen = sizeof(sa);
			new_sk = accept(sk, (struct sockaddr *)&sa, &salen);
      if (new_sk < 0)
      {
        return FALSE;
      } else
      if (new_sk == INVALID_SOCKET)
      {
        return FALSE;
      } else
      {
				/* We got a new connection */
        if (!proc(new_sk, (BYTE *) &sa.sin_addr, ntohs(sa.sin_port)))
          return TRUE;
        if (!keep_client_open)
          NETWORK_Close(new_sk);
      }
		}
	}

	return TRUE;
}

BOOL NETWORK_TCPSrvLoopSingle(SOCKET sk, TCP_SERV_PROC proc)
{
  return NETWORK_TCPSrvLoop(sk, proc, FALSE);
}

BOOL NETWORK_TCPSrvLoopMulti(SOCKET sk, TCP_SERV_PROC proc)
{
  return NETWORK_TCPSrvLoop(sk, proc, TRUE);
}

BOOL NETWORK_TCPSrvLoopEx(SOCKET sk, TCP_SERV_PROC_EX proc, void *param, BOOL keep_client_open)
{
  SOCKET new_sk;
	struct sockaddr_in sa;
	fd_set rdfdset;
	int selret;
	unsigned int salen;
	
	if (proc == NULL) return FALSE;

  for(;;)
  {
		FD_ZERO(&rdfdset);
		FD_SET(sk, &rdfdset);

		/* Wait for any connection.  */
		selret = select(sk+1, &rdfdset, 0, 0, NULL);
    if (selret == SOCKET_ERROR)
    {
      return FALSE;
    }
		if (!selret)
    {
      continue;
    }

		if(FD_ISSET(sk, &rdfdset))
    {
			/* Accept the incoming connection and handle it in a child.  */
			salen = sizeof(sa);
			new_sk = accept(sk, (struct sockaddr *)&sa, &salen);
      if (new_sk < 0)
      {
        return FALSE;
      } else
      if (new_sk == INVALID_SOCKET)
      {
        return FALSE;
      } else
      {
				/* We got a new connection */
        if (!proc(new_sk, (BYTE *) &sa.sin_addr, ntohs(sa.sin_port), param))
          return TRUE;
        if (!keep_client_open)
          NETWORK_Close(new_sk);
      }
		}
	}

	return TRUE;
}

BOOL NETWORK_TCPSrvLoopSingleEx(SOCKET sk, TCP_SERV_PROC_EX proc, void *param)
{
  return NETWORK_TCPSrvLoopEx(sk, proc, param, FALSE);
}

BOOL NETWORK_TCPSrvLoopMultiEx(SOCKET sk, TCP_SERV_PROC_EX proc, void *param)
{
  return NETWORK_TCPSrvLoopEx(sk, proc, param, TRUE);
}

BOOL NETWORK_TCPWaitRecv(SOCKET sk, void *msg, WORD *len, DWORD wait_sec)
{
  BOOL is_timeout = FALSE;

  if (!NETWORK_TCPWaitEx(sk, wait_sec, &is_timeout))
    return FALSE;

  if (is_timeout)
  {
    if (len != NULL)
      *len = 0;
    return TRUE;
  }

  if (!NETWORK_TCPRecv(sk, msg, len))
  {
    return FALSE;
  }

  return TRUE;
}

BOOL NETWORK_TCPRecv(SOCKET sk, void *msg, WORD *len)
{
  int done;

  if (len == NULL) return FALSE;
  if (msg == NULL) return FALSE;

  done = recv(sk, msg, *len, 0);
  if (done < 0)
  {
    return FALSE;
  }

  *len = done;
  return TRUE;
}

BOOL NETWORK_TCPWaitEx(SOCKET sk, DWORD wait_sec, BOOL *is_timeout)
{
	fd_set rdfdset;
	int selret;
  struct timeval timeout;

  if (is_timeout != NULL)
    *is_timeout = FALSE;
  
  timeout.tv_sec  = wait_sec;
  timeout.tv_usec = 0;

	FD_ZERO(&rdfdset);
	FD_SET(sk, &rdfdset);

	selret = select(sk+1, &rdfdset, 0, 0, &timeout);
  if (selret < 0)
  {
    return FALSE;
  }
	if (selret == 0)
  {
		/* timeout */
    if (is_timeout != NULL)
    {
      *is_timeout = TRUE;
      return TRUE;
    } else
    {
      return FALSE;
    }
  }

  if (!FD_ISSET(sk, &rdfdset))
  {
    return FALSE;
  }

  return TRUE;
}

BOOL NETWORK_TCPWait(SOCKET sk, DWORD wait_sec)
{
  return NETWORK_TCPWaitEx(sk, wait_sec, NULL);
}


BOOL NETWORK_TCPSend(SOCKET sk, const void *msg, WORD len)
{
  int done, remaining;
  const BYTE *p = msg;

  if ((msg != NULL) && (len == 0))
    len = (WORD) (1 + strlen(msg));

  remaining = len;

  while (remaining > 0)
  {
    done = send(sk, p, remaining, 0);
    if (done < 0)
    {
      return FALSE;
    }
    if (done == 0)
    {
      return FALSE;
    }
    remaining -= done;
    p += done;
  }
  return TRUE;
}

/*
 * Wait for incoming TCP data
 */
BOOL NETWORK_TCPRecvFrom(SOCKET sk, BYTE *rcvAddr, WORD *rcvPort, void *msg, WORD *len)
{
  struct sockaddr_in sa;
  unsigned int saLen;
  int done;
  
  if (msg == NULL) return FALSE;
  if (len == NULL) return FALSE;

  memset((char*)msg, 0, *len);

  if ((rcvAddr != NULL) && (rcvPort != NULL))
  {
    saLen = sizeof(sa);

    done = recvfrom(sk, msg, *len, 0, (struct sockaddr *)&sa, &saLen);
    if (done < 0)
    {
      return FALSE;
    }
    *len = done;

    if (rcvAddr != NULL)
	    memcpy(rcvAddr, &sa.sin_addr, 4); /* IPv4 */
	  
	  if (rcvPort != NULL)
	    *rcvPort = ntohs(sa.sin_port);
  } else
  {
    done = recv(sk, msg, *len, 0);
    if (done < 0)
    {
      return FALSE;
    }
    *len = done;
  }

  return TRUE;
}

/*
 * Send data via TCP
 * Modified by MAR the 05 April 2002
 * ToDo :
 *  verify one by one that the bytes have been sent
 */
BOOL NETWORK_TCPSendTo(SOCKET sk, const BYTE *dstAddr, WORD dstPort, const void *msg, WORD len)
{
  struct sockaddr_in sa;
  int done, remaining;
  const BYTE *p = msg;
  
  if ((msg != NULL) && (len == 0))
    len = (WORD) (1 + strlen(msg));

  if ((dstAddr != NULL) && (dstPort != 0))
  {
    memset((char*)&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    memcpy(&sa.sin_addr, dstAddr, 4);
    sa.sin_port   = htons(dstPort);

    remaining = len;

    while (remaining > 0)
    {
      done = sendto(sk, p, remaining, 0, (const struct sockaddr *)&sa, sizeof(sa));
      if (done < 0)
      {
        return FALSE;
      }
      if (done == 0)
      {
        return FALSE;
      }
      remaining -= done;
      p += done;
    }
  } else
  {
    remaining = len;

    while (remaining > 0)
    {
      done = send(sk, p, remaining, 0);
      if (done < 0)
      {
        return FALSE;
      }
      if (done == 0)
      {
        return FALSE;
      }
      remaining -= done;
      p += done;
    }
  }
  return TRUE;
}

BOOL NETWORK_TCPCliOpen(SOCKET *sk, const BYTE *dstAddr, WORD port)
{
  SOCKET new_sk;
	struct sockaddr_in sa;

#ifdef WIN32
  if (!NETWORK_WinsockStartup()) return FALSE;
#endif

	new_sk = socket(AF_INET, SOCK_STREAM, 0);
	if (new_sk < 0)
	{
		return FALSE;
	}

  memset(&sa, 0, sizeof(sa));

  memcpy(& ( sa.sin_addr), dstAddr, 4);
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
	if (connect(new_sk, (struct sockaddr *) & sa, sizeof(sa)) < 0)
	{
		NETWORK_Close(new_sk); /* Oups ca manquait ! */
		return FALSE;
  }
  
  if (sk != NULL)
  	*sk = new_sk;

	return TRUE;
}

/*
 * Safe send a message to a server.
 * Return TRUE if success, FALSE otherwise.
 *
*/
BOOL NETWORK_TCPCliSend(SOCKET sk, const void *msg, WORD len)
{
  int done, remaining;
  const BYTE *p = msg;

  if ((msg != NULL) && (len == 0))
    len = (WORD) (1 + strlen(msg));

  remaining = len;

  while (remaining > 0)
  {
    done = send(sk, p, remaining, 0);
    if (done < 0)
    {
      return FALSE;
    }
    if (done == 0)
    {
      return FALSE;
    }
    remaining -= done;
    p += done;
  }
  return TRUE;
}

/*
 * Safe receives a message from a server.
 * Return TRUE if success, FALSE otherwise.
 *
*/
BOOL NETWORK_TCPCliRecv(SOCKET sk, void *msg, WORD *len)
{
	int done;
	
	if (msg == NULL) return FALSE;
	if (len == NULL) return FALSE;

  done = recv(sk, msg, *len, 0);
  if (done < 0)
  {
    return FALSE;
  }
  *len = done;

	return TRUE;
}


