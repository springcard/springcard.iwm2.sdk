#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdarg.h>
#ifndef WIN32
#include "events.h"
#endif
#include "../../fkg485_dll/inc/fkg485_dll.h"

#ifdef WIN32
extern HANDLE hPrintMutex;
extern HANDLE hLoopsEnded;
#else
extern pthread_mutex_t hPrintMutex;
extern void           *hLoopsEnded;
#endif
DWORD dwLastPrintOn = 0;

static void print_begin(void)
{
#ifdef WIN32
  WaitForSingleObject(hPrintMutex, INFINITE);
#else
  if (pthread_mutex_lock(&hPrintMutex) != 0)
  {
    printf("Error:Can't lock hPrintMutex\n");
    return ;
  }
#endif
}

static void print_end(void)
{
#ifdef WIN32
  ReleaseMutex(hPrintMutex);
#else 
 if (pthread_mutex_unlock(&hPrintMutex) != 0)
  {
    printf("Error:Can't unlock hPrintMutex\n");
    return;    
  }
#endif
}

static void print_protect(const char *fmt, ...)
{
  va_list args;

  print_begin();
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  print_end();
}

extern BOOL reader_found;
extern int addr;
extern BOOL print_enum;
BOOL OnEnumerate(const char *comm_name, BYTE device_addr)
{
  
  if (print_enum)
		print_protect("Found: reader %03d\n", device_addr);
 
	if (device_addr == addr)
		reader_found = TRUE;

  return TRUE;
}

/* OnLoopeEnded callback is called each time an S-Enum or a I-block loop (depending on	*/
/* configuration) has ended. It enables us to count the number of loops, and set the		*/
/* hLoopsEnded event, to tell the main program that all the commands have been sent.		*/
extern int awaited_loops;
BOOL OnLoopEnded(const char *comm_name)
{
	static int i=0;
	i++;
	if (i >= awaited_loops)
	{
		i=0;	/* reset the counter */
#ifdef WIN32
		SetEvent(hLoopsEnded);
#else
    signal_event(hLoopsEnded);
#endif
	}

	return TRUE;
}