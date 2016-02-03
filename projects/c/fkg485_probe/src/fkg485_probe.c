#define _CRT_SECURE_NO_WARNINGS

#include "../../common/485/fkg485_i.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib-c/utils/stri.h"
#include "../../fkg485_dll/inc/fkg485_dll.h"

#ifndef WIN32
#include "events.h"
#include <stdarg.h>
#endif

#define T_GLOBAL_STATUS		0x81
#define MAX_READERS				10
static const char *PROGNAME = "flkg485_probe";

#ifdef WIN32
BOOL fkg485_debug;
#endif

void usage(void)
{
  printf("Usage: %s comm_name\n", PROGNAME);
}

#ifdef WIN32
HANDLE hPrintMutex;
HANDLE hLoopsEnded;
#else
pthread_mutex_t hPrintMutex;
void *hLoopsEnded;
#endif

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

BOOL reader_present[MAX_READERS];
BOOL OnEnumerate(const char *comm_name, BYTE device_addr)
{
	if (device_addr < MAX_READERS)
		reader_present[device_addr] = TRUE;

  return TRUE;
}

int awaited_loops;
BOOL OnLoopEnded(const char *comm_name)
{
	static int i=0;
	i++;

	if (i>=awaited_loops)
#ifdef WIN32
		SetEvent(hLoopsEnded);
#else
    signal_event(hLoopsEnded);
#endif

	return TRUE;
}

BOOL OnTlvReceive(const char *device_name, BYTE tag, BYTE tag_idx, const BYTE payload[], BYTE payload_sz)
{
	char resp[50];
	char dummy1[50];
	char dummy2[50];
	int v1=0, v2=0;

	memset(dummy1, 0, sizeof(dummy1));
	memset(dummy2, 0, sizeof(dummy2));

	if (tag == T_GLOBAL_STATUS)
	{
		memset(resp, 0, sizeof(resp));
		if (payload_sz < sizeof(resp))
		{
			memcpy(resp, payload, payload_sz);
			if (sscanf(resp, "%s %s %01d.%02d", dummy1, dummy2, &v1, &v2) == 4)
			{
				print_protect("\tFound a reader at adress %s - Version: %01d.%02d\n", device_name, v1, v2);
			} else
			{
				print_protect("\tFound a reader at adress %s - %s\n", device_name, "Version<=1.64");
			}

		}
	}
  return TRUE;
}


int main(int argc, char **argv)
{
  char *comm_name = NULL;
#ifdef WIN32
  HANDLE comm_handle;
#else
  int comm_handle;
#endif
	int i;

  if (argc > 1)
  {
#ifdef WIN32
    comm_name = argv[1];
#else
    char dev[30];
    memset(dev, 0, sizeof(dev));
    if ((strlen("/dev/") + strlen(argv[1])) < sizeof(dev))
    {
      sprintf(dev, "/dev/%s", argv[1]);
      comm_name = dev;
    } else
    {
      comm_name = argv[1];
    }
#endif
  }
  
  if (comm_name == NULL)
  {
    usage();
    return EXIT_FAILURE;
  }

	/* Check for a reader in MK1 protocol */
	/* ---------------------------------- */
  printf("Opening communication device '%s'\n", comm_name);

  if (!FKG485_OpenCommEx(&comm_handle, comm_name))
  {
    printf("ERROR\n");
    return EXIT_FAILURE;
  }

  printf("Probing for a MK1 reader...\n");

  if (FKG485_MK1_Probe(comm_handle))
	{
		printf("OK - Found a MK1 reader!\n");
	}	else
	{
		
		/* Check for readers in MK2 protocol */
		/* --------------------------------- */		
		char device_name[64];
		printf("\tNo MK1 reader found\n");

		FKG485_CloseCommEx(comm_handle);

		printf("Probing for MK2 reader(s)...\n");
#ifdef WIN32
		hPrintMutex = CreateMutex(NULL, FALSE, NULL);
#else
    if (pthread_mutex_init(&hPrintMutex, NULL) != 0)
    {
      printf("Error creating mutex hPrintMutex\n");
      return FALSE;
    }
#endif

		/* This callback is declared to get the adresses of the readers */
		FKG485_SetEnumerateCallback(OnEnumerate);
		
		/* This second callback is declared in order to wait for the whole	*/
		/* S-Enum loop to finish: hLoopsEnded is set in the callback				*/
		awaited_loops=1;
		FKG485_SetEnumLoopEndedCallback(OnLoopEnded);
#ifdef WIN32
    hLoopsEnded = CreateEvent(NULL, TRUE, FALSE, TEXT("LoopEnded") ); 
#else
  if (!create_event(&hLoopsEnded))
  {
    printf("Error creating event hLoopsEnded\n");
    return FALSE;
  }  
#endif
		if (!FKG485_Start(comm_name, MAX_READERS))
		{
			printf("FKG485_Start failed\n");
			return EXIT_FAILURE;  
		}
#ifdef WIN32
		WaitForSingleObject(hLoopsEnded, INFINITE);
#else
    if (!wait_event(hLoopsEnded))
    {
      printf("Error waiting for event hLoopsEnded\n");
      return EXIT_FAILURE; 
    }  
#endif
		/* At this stage, we know how many readers are on the bus: */
		/* - declare as many readers as found on the bus					 */
		/* - ask them for their global status											 */
		/* - get the readers' answers in "OnTlvReceive"						 */
#ifdef WIN32
    ResetEvent(hLoopsEnded);
#endif
		FKG485_SetTlvReceiveCallback(OnTlvReceive);
		awaited_loops++;
		FKG485_SetILoopEndedCallback(OnLoopEnded);

		for (i=0; i<MAX_READERS; i++)
		{
			if (reader_present[i])
			{
				sprintf(device_name, "%02d", i);
				if (!FKG485_Create(comm_name, i, device_name))
				{
					printf("FKG485_Create(%s) failed\n", device_name);
					return FALSE;
				}

				awaited_loops += 2;				/* Add time for the reader(s) to answer */
				
				if (!FKG485_AskForStatus(device_name))
				{
					printf("FKG485_AskForStatus(%s) failed\n", device_name);
					return FALSE;
				}

			}

		}
#ifdef WIN32
		WaitForSingleObject(hLoopsEnded, INFINITE);
#else
    if (!wait_event(hLoopsEnded))
    {
      printf("Error waiting for event hLoopsEnded\n");
      return EXIT_FAILURE; 
    }  
#endif
		FKG485_Stop(comm_name);
		FKG485_Join(comm_name);
	
	}
  return EXIT_SUCCESS;

}
