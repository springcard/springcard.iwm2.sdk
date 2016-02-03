#include "../../fkg485_dll/inc/fkg485_dll.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef WIN32
#include <conio.h>
#else
#include <sys/time.h>
#endif

#ifdef WIN32
HANDLE hPrintMutex;
#else
pthread_mutex_t  hPrintMutex;
#endif
DWORD dwLastPrintOn = 0;

#ifndef WIN32
static unsigned long GetTickCount()
{
  struct timeval tv;
  if( gettimeofday(&tv, NULL) != 0 )
    return 0;
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
#endif

static void print_begin(void)
{
  DWORD t;

#ifdef WIN32
  WaitForSingleObject(hPrintMutex, INFINITE);
#else
 
if (pthread_mutex_lock(&hPrintMutex) != 0)
  {
    printf("Error:Can't lock hPrintMutex\n");
    return ;
  }
 
#endif

  t = GetTickCount();
  if (dwLastPrintOn != 0)
  {
    printf("%04d\t", t - dwLastPrintOn);
  } else
    printf("\t");
  dwLastPrintOn = t;
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

void print_protect(const char *fmt, ...)
{
  va_list args;

  print_begin();
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  print_end();
}

BOOL OnEnumerate(const char *comm_name, BYTE device_addr)
{

  char device_name[64];
  print_begin();
  printf("OnEnumerateCallback(%s,%03d)\n", comm_name, device_addr);
  print_end();

  sprintf(device_name, "FunkyGate%03d", device_addr);

  if (!FKG485_Create(comm_name, device_addr, device_name))
  {
    printf("FKG485_Create(%s) failed\n", device_name);
    return FALSE;
  }
  
  return TRUE;
}

BOOL OnStatusChange(const char *device_name, WORD device_status)
{
  print_begin();
  printf("%s:OnStatusChanged(%04X)\n", device_name, device_status);
  print_end();
  return TRUE;
}

BOOL OnTlvReceive(const char *device_name, BYTE tag, BYTE tag_idx, const BYTE payload[], BYTE payload_sz)
{
  print_begin();
  printf("%s:OnTlvReceive\n", device_name);
  print_end();
  return TRUE;
}

BOOL OnTamperChange(const char *device_name, BYTE tampers_state)
{
  print_begin();
  printf("%s:OnTamperChange(%02X)\n", device_name, tampers_state);
  print_end();
  return TRUE;
}

BOOL OnCardRead(const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz)
{
  int i;

  print_begin();
  printf("%s:OnCardRead\n", device_name);
  printf("\tCardID: ");
  for (i=0; i<card_id_sz; i++)
    printf("%02X", card_id[i]);
  printf("\n");
  print_end();
  return TRUE;
}

BOOL OnCardInsert(const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz)
{
  int i;

  print_begin();
  printf("%s:OnCardInsert\n", device_name);
  printf("\tCardID: ");
  for (i=0; i<card_id_sz; i++)
    printf("%02X", card_id[i]);
  printf("\n");
  print_end();
  return TRUE;
}

BOOL OnCardRemove(const char *device_name, BYTE head_idx)
{
  print_begin();
  printf("%s:OnCardRemove\n", device_name);
  print_end();
  return TRUE;
}

BOOL OnPinEnter(const char *device_name, BYTE head_idx, const BYTE pin[], BYTE pin_sz)
{
  int i;

  print_begin();
  printf("%s:OnPinEnter\n", device_name);
  printf("\tPIN: ");
  for (i=0; i<pin_sz; i++)
    printf("%X", pin[i]);
  printf("\n");
  print_end();
  return TRUE;
}

BOOL OnInputChange(const char *device_name, BYTE input_idx, BYTE input_state)
{
  print_begin();
  printf("%s:OnInputChange(%d,%d)\n", device_name, input_idx, input_state);
  print_end();
  return TRUE;
}

int main(int argc, char **argv)
{
  const char *comm_device = "COM1";

  if (argc >= 2)
  {
#ifdef WIN32		
		comm_device = argv[1];
#else
		char dev[30];
    memset(dev, 0, sizeof(dev));
    if ((strlen("/dev/") + strlen(argv[1])) < sizeof(dev))
    {
      sprintf(dev, "/dev/%s", argv[1]);
      comm_device = dev;
    } else
    {
      comm_device = argv[1];
    }
#endif
	}

  printf("FKG485:%s:%d\n", __FILE__, __LINE__);

#ifdef WIN32
  hPrintMutex = CreateMutex(NULL, FALSE, NULL);
#else
  if (pthread_mutex_init(&hPrintMutex, NULL) != 0)
  {
    printf("Error pthread_mutex_init hPrintMutex\n");
    return EXIT_FAILURE;
  }
#endif
	/* Set all callbacks */
  FKG485_SetEnumerateCallback(OnEnumerate);
  FKG485_SetStatusChangeCallback(OnStatusChange);
  FKG485_SetTlvReceiveCallback(OnTlvReceive);
  FKG485_SetTamperChangeCallback(OnTamperChange);
  FKG485_Rdr_SetCardReadCallback(OnCardRead);
  FKG485_Rdr_SetCardInsertCallback(OnCardInsert);
  FKG485_Rdr_SetCardRemoveCallback(OnCardRemove);
  FKG485_Rdr_SetPinEnterCallback(OnPinEnter);
  printf("FKG485:%s:%d\n", __FILE__, __LINE__);

  
	/* Start communication */
  if (!FKG485_Start(comm_device, 4))
  {
    printf("FKG485_Start failed\n");
    return EXIT_FAILURE;
  }


  printf("FKG485:%s:%d\n", __FILE__, __LINE__);

	/* Enter the following keys to play with the reader	*/
	/* Enter x to exit the infinite loop								*/
  for (;;)
  {
#ifdef WIN32  
    int ch = _getch();
#else
    char ch = getc(stdin);
#endif
		switch (ch)
    {
      case 'X' :
      case 'x' :
        goto done;

      case 'D' :
      case 'd' :
        FKG485_Rdr_ClearLeds("*");
        break;

      case 'R' :
      case 'r' :
        FKG485_Rdr_SetLeds("*", 1, 0);
        break;

      case 'G' :
      case 'g' :
        FKG485_Rdr_SetLeds("*", 0, 1);
        break;

      case 'Z' :
      case 'z' :
        FKG485_Rdr_Buzzer("*", 3);
        break;
    }
   
  }

done:
  printf("Exiting...\n");
  FKG485_Stop(comm_device);
  FKG485_Join(comm_device);
  return EXIT_SUCCESS;
}
