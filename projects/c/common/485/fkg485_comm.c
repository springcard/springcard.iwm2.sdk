#include "fkg485_i.h"

#ifndef WIN32
#include <termios.h>
#include <unistd.h>
#include <errno.h>  
#endif

#ifdef WIN32

static BOOL FKG485_ResetUart(HANDLE h)
{
  DWORD   comerrors;
  COMSTAT comstat;

  if (!ClearCommError(h, &comerrors, &comstat))
  {
    if (fkg485_debug)
      printf("ClearCommError(%p) -> %ld\n", h, GetLastError());
    return FALSE;
  }

  PurgeComm(h, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
  FlushFileBuffers(h);

  return TRUE;
}

static BOOL FKG485_SetupComm(HANDLE h)
{
  DCB dcb;

  if (!SetupComm(h, 512, 512))
  {
    if (fkg485_debug)
      printf("SetupComm(%p) -> %ld\n", h, GetLastError());
    return FALSE;
  }

  if (!FKG485_ResetUart(h))
  {
    if (fkg485_debug)
      printf("ResetUart(%p) -> KO\n", h);
    return FALSE;
  }

  if (!GetCommState(h, &dcb))
  {
    if (fkg485_debug)
      printf("GetCommState(%p) -> %ld\n", h, GetLastError());
    return FALSE;
  }

  dcb.BaudRate = CBR_38400;

  dcb.fBinary = TRUE;
  dcb.fParity = FALSE;
  dcb.fOutxCtsFlow = FALSE;
  dcb.fOutxDsrFlow = FALSE;
  dcb.fDsrSensitivity = FALSE;
  dcb.fOutX = FALSE;
  dcb.fInX = FALSE;
  dcb.fNull = FALSE;
  dcb.ByteSize = 8;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  dcb.fRtsControl = RTS_CONTROL_ENABLE; /* New 1.73 : for compatibility with RL78 flash board */
  dcb.fDtrControl = DTR_CONTROL_ENABLE; /* New 1.73 : for compatibility with RL78 flash board */

  dcb.fAbortOnError = TRUE;
  dcb.fTXContinueOnXoff = TRUE;

  if (!SetCommState(h, &dcb))
  {
    if (fkg485_debug)
      printf("SetCommState(%p) -> %ld\n", h, GetLastError());
    return FALSE;
  }

  if (!FKG485_ResetUart(h))
  {
    if (fkg485_debug)
      printf("ResetUart(%p) -> KO\n", h);
    return FALSE;
  }

  return TRUE;
}

static BOOL FKG485_SetTimeouts(HANDLE h, DWORD constant, DWORD interval)
{
  COMMTIMEOUTS stTimeNew;

  stTimeNew.ReadIntervalTimeout         = interval;
  stTimeNew.ReadTotalTimeoutConstant    = constant;
  stTimeNew.ReadTotalTimeoutMultiplier  = interval;
  stTimeNew.WriteTotalTimeoutConstant   = 250;
  stTimeNew.WriteTotalTimeoutMultiplier = 100;
    
  if (!SetCommTimeouts(h, &stTimeNew))
  {
    if (fkg485_debug)
      printf("SetCommTimeouts(%p) -> %ld\n", h, GetLastError());
    return FALSE;
  }

  return TRUE;
}

BOOL FKG485_OpenCommEx(HANDLE *comm_handle, const char *comm_name)
{
  HANDLE h;

  if (comm_handle == NULL)
  {
    if (fkg485_debug)
      printf("NULL pointer\n");
    return FALSE;
  }
  
  if (fkg485_debug)
    printf("CreateFile(%s)...\n", comm_name);

  h = CreateFileA(comm_name,
                  GENERIC_READ | GENERIC_WRITE,
                  0,							/* comm devices must be opened w/exclusive-  */
                  NULL,						/* no security attributes										 */
                  OPEN_EXISTING,	/* comm devices must use OPEN_EXISTING			 */
                  0,							/* not overlapped I/O												 */
                  NULL						/* hTemplate must be NULL for comm devices	 */
    );

  if (h == INVALID_HANDLE_VALUE)
  {
    if (fkg485_debug)
      printf("CreateFile(%s) -> %ld\n", comm_name, GetLastError());
    return FALSE;
  }

  if (fkg485_debug)
    printf("CreateFile(%s) -> OK (handle=%p)\n", comm_name, h);

  if (!FKG485_SetupComm(h))
  {
    if (fkg485_debug)
      printf("SetupComm(%s) -> KO\n", comm_name);
    CloseHandle(h);
    return FALSE;
  }

  if (!FKG485_SetTimeouts(h, 100, 20))
  {
    if (fkg485_debug)
      printf("SetTimeouts(%s) -> KO\n", comm_name);
    CloseHandle(h);
    return FALSE;
  }

  if (fkg485_debug)
    printf("OpenComm(%s) -> OK\n", comm_name);

  *comm_handle = h;
  return TRUE;
}

#endif

#ifdef WIN32
BOOL FKG485_SendEx(HANDLE comm_handle, const BYTE send_buffer[], WORD send_len)
#else
BOOL FKG485_SendEx(int fd, const BYTE send_buffer[], WORD send_len)
#endif
{
  DWORD i;
  DWORD l = 0;

  if (fkg485_debug)
  {
    printf("< ");
    for (i=0; i<send_len; i++)
      printf("%02X", send_buffer[i]);
    printf("\n");
  }

#ifdef WIN32
  if (!WriteFile(comm_handle, send_buffer, send_len, &l, NULL))
  {
    if (fkg485_debug)
      printf("WriteFile -> %ld\n", GetLastError());
    return FALSE;   
  }
#else
  l = write(fd, send_buffer, send_len);
  if ( l < 0)
  {
    if (fkg485_debug)
       printf("write -> %s\n", strerror(errno));
    return FALSE;
  }
#endif  
  
  if (l < send_len)
  {
    if (fkg485_debug)
#ifdef WIN32
      printf("WriteFile -> too short\n");
#else
      printf("write -> too short\n");
#endif
    return FALSE;
  }

  return TRUE;
}

#ifdef WIN32
BOOL FKG485_RecvEx(HANDLE comm_handle, BYTE recv_buffer[], WORD max_recv_len, WORD *actual_recv_len)
#else
BOOL FKG485_RecvEx(int fd, BYTE recv_buffer[], WORD max_recv_len, WORD *actual_recv_len)
#endif
{
  DWORD i;
  DWORD l = 0;
#ifndef WIN32  
	int res, offset;
	fd_set set;
	struct timeval delay;
	WORD remaining = max_recv_len;
  FD_ZERO(&set); 							/* 	initialize fd_set to "set"								*/
	FD_SET(fd, &set); 					/*	Add file descriptor "fd" to fd_set "set"  */
#endif
   
#ifdef WIN32
  if (!ReadFile(comm_handle, recv_buffer, max_recv_len, &l, NULL))
  {
    if (fkg485_debug)
      printf("ReadFile -> %ld\n", GetLastError());
    return FALSE;
  }
#else
  delay.tv_sec = 0;
	delay.tv_usec = 200 * 1000;
  offset = 0;
  
  while(remaining > 0)
  {
    res = select(FD_SETSIZE, &set, NULL, NULL, &delay);
    if (res < 0)
    {
      printf("select -> %s\n", strerror(errno));
      return FALSE;
    } else
    if (res == 0)
    {
      /* timeout */
      break;
    } else
    {
      int recv;
      recv = read(fd, &recv_buffer[offset], remaining);

      if (recv < 0)
      {
        if (fkg485_debug)
          printf("read -> %s\n", strerror(errno));
        return FALSE;
      }
      remaining -= recv;
      offset += recv;
    }
  }
  l=offset;
#endif
  if (fkg485_debug)
  {
    printf("> ");
    for (i=0; i<l; i++)
      printf("%02X", recv_buffer[i]);
    printf("\n");
  }

  if (actual_recv_len != NULL)
    *actual_recv_len = (WORD) l;
  
  return TRUE;
}

#ifdef WIN32
void FKG485_CloseCommEx(HANDLE comm_handle)
{
  FKG485_ResetUart(comm_handle);
  CloseHandle(comm_handle);
}
#else

BOOL FKG485_OpenCommEx(int *fd, const char *comm_name)
{
  struct termios oldtio;
  struct termios newtio;
  int f_des = open(comm_name, O_RDWR | O_NOCTTY ); 

	if (f_des < 0)
	{
    if (fkg485_debug)
      printf("open -> %s\n", strerror(errno));
    return FALSE; 
  }

	/* Open modem device for reading and writing and not as controlling tty			*/
	/* because we don't want to get killed if linenoise sends CTRL-C.						*/

	tcgetattr(f_des,&oldtio); 					/* save current serial port settings 				*/
	bzero(&newtio, sizeof(newtio));     /* clear struct for new port settings 			*/

	/* B38400: Set bps rate. You could also use cfsetispeed and cfsetospeed.	*/
	/* CRTSCTS : output hardware flow control (only used if the cable has				*/
	/*					all necessary lines. See sect. 7 of Serial-HOWTO)								*/
	/* CS8     : 8n1 (8bit,no parity,1 stopbit)																	*/
	/* CLOCAL  : local connection, no modem contol															*/
	/* CREAD   : enable receiving characters																		*/	
	newtio.c_cflag = B38400 | CLOCAL | CREAD;
	 

	/* IGNPAR  : ignore bytes with parity errors																*/
	/* ICRNL   : map CR to NL (otherwise a CR input on the other computer				*/
	/*					will not terminate input)																				*/
	/* otherwise make device raw (no other input processing)										*/
	newtio.c_iflag = IGNPAR | ICRNL;
	 
	
	/* Raw output. */
	newtio.c_oflag = 0;
	 
	/* ICANON  : enable canonical input																						*/
	/* disable all echo functionality, and don't send signals to calling program	*/
	newtio.c_lflag = ~ICANON;
	 
  /*	initialize all control characters 																		*/
  /*	default values can be found in /usr/include/termios.h, and are given	*/
  /*	in the comments, but we don't need them here													*/
  newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
  newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
  newtio.c_cc[VERASE]   = 0;     /* del */
  newtio.c_cc[VKILL]    = 0;     /* @ */
  newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */

  newtio.c_cc[VTIME]    = 1;     /* inter-character timer : 1/10th  (0: unused) */
  newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives     */
  
  newtio.c_cc[VSWTC]    = 0;     /* '\0' */
  newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
  newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
  newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
  newtio.c_cc[VEOL]     = 0;     /* '\0' */
  newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
  newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
  newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
  newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
  newtio.c_cc[VEOL2]    = 0;     /* '\0' */

  /*	now clean the modem line and activate the settings for the port	*/
  tcflush(f_des, TCIFLUSH);
  tcsetattr(f_des,TCSANOW,&newtio);
  
  *fd = f_des;
  return TRUE;
  
}

void FKG485_CloseCommEx(int fd)
{
  close(fd);
  return;
}
#endif