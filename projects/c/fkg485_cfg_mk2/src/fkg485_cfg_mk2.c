#include "../../common/fkg_cfg_cli.h"
#include "../../common/485/fkg485_i.h"
#include "../../fkg485_dll/inc/fkg485_dll.h"

#ifndef WIN32
#include "events.h"
#endif

static const char *PROGNAME = "fkg485_cfg_mk2";
static char  *comm_name = NULL;

#ifdef WIN32
HANDLE hPrintMutex;
HANDLE hLoopsEnded;
#else
pthread_mutex_t hPrintMutex;
void *hLoopsEnded;
#endif

#ifdef WIN32
BOOL fkg485_debug;
#endif

int addr;
BOOL reader_found;
int awaited_loops;
BOOL print_enum;

BOOL OnEnumerate(const char *comm_name, BYTE device_addr);
BOOL OnLoopEnded(const char *comm_name);

int fkg_cfg_list(void)
{
	/* This first callback is declared to get the adresses of the readers */
	FKG485_SetEnumerateCallback(OnEnumerate);
  
	/* This second callback is declared in order to prevent exiting the	 */
	/* application before the whole S-Enum loop is finished (so that we  */
	/* don't miss an address): hLoopsEnded is set in the callback				 */
	awaited_loops = 1;
	print_enum = TRUE;
#ifdef WIN32
	hLoopsEnded = CreateEvent(NULL, TRUE, FALSE, TEXT("LoopEnded") ); 
#else
  if (!create_event(&hLoopsEnded))
  {
    printf("Error creating event hLoopsEnded\n");
    return FALSE;
  }
#endif
	FKG485_SetEnumLoopEndedCallback(OnLoopEnded);
	if (!FKG485_Start(comm_name, 10))
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

	return TRUE;
}

int main(int argc, char **argv)
{
  int new_argc = 0;
  char **new_argv = NULL;

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
    new_argc = argc - 1;
    new_argv = &argv[1];
  }

  if (comm_name == NULL)
  {
    fkg_cfg_usage();
    return EXIT_FAILURE;
  }

#ifdef WIN32
	hPrintMutex = CreateMutex(NULL, FALSE, NULL);
#else
  if (pthread_mutex_init(&hPrintMutex, NULL) != 0)
  {
    printf("Error creating mutex hPrintMutex\n");
    return FALSE;
  }
#endif
	if (new_argc > 1)
		if (!sc_stricmp("-l", new_argv[1]))
			return fkg_cfg_list();

	return fkg_cfg_main(new_argc, new_argv);

}

void fkg_cfg_usage(void)
{
  printf("Usage: %s comm_name < -l | -a XX ACTION >\n", PROGNAME);
	printf("\t-l            list readers on bus\n");
	printf("\t-a XX         reader address\n");
  fkg_cfg_usage_common();
}

BOOL fkg_cfg_connect(void)
{
	char device_name[64];
	sprintf(device_name, "FunkyGate%03d", addr);

	/* First, check that the desired reader is connected */
  reader_found = FALSE;
	awaited_loops = 1;
	print_enum = FALSE;
    
#ifdef WIN32  
	hLoopsEnded = CreateEvent(NULL, TRUE, FALSE, TEXT("LoopEnded") ); 
#else
  if (!create_event(&hLoopsEnded))
  {
    printf("Error creating event hLoopsEnded\n");
    return FALSE;
  }  
#endif
	FKG485_SetEnumerateCallback(OnEnumerate);
	FKG485_SetEnumLoopEndedCallback(OnLoopEnded); 
  
	if (!FKG485_Start(comm_name, 10))
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
	FKG485_SetEnumLoopEndedCallback(NULL);

	/* We now know, that the SEnum loop has ended : check if	*/
	/* the desired reader has responded												*/
	if (!reader_found)
	{
		printf("Reader with address %d is not responding\n", addr);
		return FALSE;
	}

	/* The reader is connected: launch the thread. At this step we don't  */
	/* know how many commands will be sent: the awaited_loops variable		*/
	/* will be incremented each time a command is issued									*/
	awaited_loops++;
#ifdef WIN32
	ResetEvent(hLoopsEnded);
#else
  
#endif
	FKG485_SetILoopEndedCallback(OnLoopEnded); 

	if (!FKG485_Create(comm_name, addr, device_name))
  {
    printf("FKG485_Create(%s) failed\n", device_name);
    return FALSE;
  }	

	return TRUE;
}

void fkg_cfg_disconnect(void)
{
  /* The hLoopEnded object is set in the declared callback */
	/* That way, we don't exit too soon										 	 */
#ifdef WIN32
	WaitForSingleObject(hLoopsEnded, INFINITE);
	CloseHandle(hLoopsEnded);
#else
  if (!wait_event(hLoopsEnded))
  {
    printf("Error waiting for event hLoopsEnded\n");
    return; 
  }  
  destroy_event(hLoopsEnded);
#endif
	FKG485_Stop(comm_name);
  FKG485_Join(comm_name);

}


BOOL fkg_cfg_reset(void)
{
  char device_name[64];
	sprintf(device_name, "FunkyGate%03d", addr);
	awaited_loops++;
	if (!FKG485_DoReset(device_name))
	{
		printf("FKG485_DoReset(%s) failed\n", device_name);
    return FALSE;
	}
	return TRUE;
}

BOOL fkg_cfg_write(BYTE ident, const BYTE data[], DWORD length)
{
	char device_name[64];
	sprintf(device_name, "FunkyGate%03d", addr);
	
	if (length > 0xFF)
		return FALSE; 
	
	/* Increment the minimum number of loops, before	*/
	/* killing the communication thread								*/
	awaited_loops++;
	
	if (!FKG485_WriteConfig(device_name, ident, data, (BYTE) length))
	{
		printf("FKG485_WriteConfig(%s) failed\n", device_name);
    return FALSE;
	}
	return TRUE;
}

BOOL fkg_cfg_read(BYTE ident, BYTE data[], DWORD maxlength, DWORD *curlength)
{
	char device_name[64];
	sprintf(device_name, "FunkyGate%03d", addr);
	awaited_loops += 2;
	if (!FKG485_ReadConfig(device_name, ident))
	{
		printf("FKG485_ReadConfig(%s) failed\n", device_name);
		*curlength=0;
    return FALSE;
	}


	return TRUE;
}

