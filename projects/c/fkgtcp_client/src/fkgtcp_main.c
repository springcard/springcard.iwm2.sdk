#include "../../fkgtcp_dll/inc/fkgtcp_dll.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <conio.h>

HANDLE hPrintMutex;
DWORD dwLastPrintOn = 0;

static void print_begin(void)
{
  DWORD t;

  WaitForSingleObject(hPrintMutex, INFINITE);

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
  ReleaseMutex(hPrintMutex);
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

BOOL OnStatusChange(const char *device_name, WORD device_status)
{
  print_begin();
  printf("%s:OnStatusChanged(%04X)\n", device_name, device_status);
  print_end();
  return TRUE;
}

BOOL OnTlvReceive(const char *device_name, BYTE tag, BYTE tag_idx, const BYTE payload[], BYTE payload_sz)
{
  char buffer[2*256+1];
  int i;

  if (payload != NULL)
  {
    for (i=0; i<payload_sz; i++)
      sprintf(&buffer[2*i], "%02X", payload[i]);
  } else
    buffer[0] = '\0';

  print_begin();
  printf("%s:OnTlvReceive(%02X.%02d:%s)\n", device_name, tag, tag_idx, buffer);
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
  print_begin();
  printf("%s:OnCardInsert\n", device_name);
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
  print_begin();
  printf("%s:OnPinEnter\n", device_name);
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

typedef struct _reader_st
{
  char *ip_addr;
  struct _reader_st *next;
} READER_ST;


void usage(void)
{
  printf("************************************************************************\n");
  printf("** The reference client to communicate with SpringCard FunkyGate IP & **\n");
  printf("** SpringCard HandyDrummer IP                                         **\n");
  printf("************************************************************************\n\n\n");
  printf("Usage: fkgtcp_client [flags] <address1>[:<port1>] ... <addressN>[:<portN>]\n");
  printf("       where <addressX> is the IPv4 address or the DNS host name of a device\n");
  printf("       and <portX> is the TCP port number. If no port is specified, the\n");
  printf("       default is 3999.\n");
  printf("Possible flags are:\n");
  printf("       --s : use secure connexion\n");
  printf("       --v : be verbose\n");
}

void commands(void)
{
  printf("The program is now waiting for events coming from the device(s)\n");
  printf("You may also send commands (to all active devices):\n");
  printf("  Y : accept sequence (Green LED is ON, short sound)\n");
  printf("  N : deny sequence (Red LED blinks, long sound)\n");
  printf("  R : switch ON the Red LED\n");
  printf("  G : switch ON the Green LED\n");
  printf("  Z : buzzer sounds\n");
  printf("  D : set the LEDs back to default mode\n");
  printf("  I : read the current value of the inputs\n");
  printf("  X : exit the application\n");
}

int main(int argc, char **argv)
{
  int i;
  BOOL use_auth = FALSE;
  READER_ST *p, *readers = NULL;
  const BYTE auth_key[16] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

  printf("fkgtcp_client (%s %s)\n", __DATE__, __TIME__);
  printf("(c) PRO ACTIVE SAS, FRANCE 2014-2015 - www.springcard.com\n\n");

  if (argc < 2)
  {
    usage();
    exit(EXIT_FAILURE);
  }

  for (i=1; i<argc; i++)
  {
    if (!strcmp(argv[i], "--h"))
    {
      usage();
      return EXIT_FAILURE;
    } else
    if (!strncmp(argv[i], "--v", 3))
    {
      int level = atoi(&argv[i][3]);
      if (level == 0) level = 3;
      if (level > 6) level = 6;
      FKGTCP_SetVerbose((BYTE) level, NULL);
    } else
    if (!strncmp(argv[i], "--s", 3))
    {
      use_auth = TRUE;
    }
  }

  hPrintMutex = CreateMutex(NULL, FALSE, NULL);

  FKGTCP_SetStatusChangeCallback(OnStatusChange);
  FKGTCP_SetTlvReceiveCallback(OnTlvReceive);
  FKGTCP_SetTamperChangeCallback(OnTamperChange);
  FKGTCP_Rdr_SetCardReadCallback(OnCardRead);
  FKGTCP_Rdr_SetCardInsertCallback(OnCardInsert);
  FKGTCP_Rdr_SetCardRemoveCallback(OnCardRemove);
  FKGTCP_Rdr_SetPinEnterCallback(OnPinEnter);

  for (i=1; i<argc; i++)
  {
    READER_ST *r;

    if (!strncmp(argv[i], "--", 2))
      continue;

    r = malloc(sizeof(READER_ST));

    r->ip_addr = argv[i];
    r->next = NULL;

    if (readers == NULL)
    {
      readers = r;
    } else
    {
      p = readers;
      while (p->next != NULL)
        p = p->next;
      p->next = r;
    }
  }

  if (readers == NULL)
  {
    printf("FKGTCP: no reader specified\n");
    printf("(use fkgctp_client --h for help)\n");
    return EXIT_FAILURE;
  }

  p = readers;
  while (p != NULL)
  {
    if (!FKGTCP_Create(p->ip_addr))
    {
      printf("FKGTCP_Create(%s) failed\n", p->ip_addr);
      return EXIT_FAILURE;
    }
    p = p->next;
  }

  commands();

  p = readers;
  while (p != NULL)
  {
    if (!FKGTCP_Start(p->ip_addr, p->ip_addr, use_auth ? auth_key : NULL))
    {
      printf("FKGTCP_Start(%s) failed\n", p->ip_addr);
      return EXIT_FAILURE;
    }
    p = p->next;
  }

  for (;;)
  {
    int ch = getch();
    switch (ch)
    {
      case 'X' :
      case 'x' :
        goto done;

      case 'D' :
      case 'd' :
        p = readers;
        while (p != NULL)
        {
          FKGTCP_Rdr_ClearLeds(p->ip_addr);
          p = p->next;
        }
        break;

      case 'R' :
      case 'r' :
        p = readers;
        while (p != NULL)
        {
          FKGTCP_Rdr_SetLeds(p->ip_addr, 1, 0);
          p = p->next;
        }
        break;

      case 'G' :
      case 'g' :
        p = readers;
        while (p != NULL)
        {
          FKGTCP_Rdr_SetLeds(p->ip_addr, 0, 1);
          p = p->next;
        }
        break;

      case 'Y' :
      case 'y' :
        p = readers;
        while (p != NULL)
        {
          FKGTCP_Rdr_SetLedsT(p->ip_addr, 0, 1, 5);
          FKGTCP_Rdr_Buzzer(p->ip_addr, 2);
          p = p->next;
        }
        break;

      case 'N' :
      case 'n' :
        p = readers;
        while (p != NULL)
        {
          FKGTCP_Rdr_SetLedsT(p->ip_addr, 2, 0, 5);
          FKGTCP_Rdr_Buzzer(p->ip_addr, 3);
          p = p->next;
        }
        break;

      case 'Z' :
      case 'z' :
        p = readers;
        while (p != NULL)
        {
          FKGTCP_Rdr_Buzzer(p->ip_addr, 3);
          p = p->next;
        }
        break;

      case 'I' :
      case 'i' :
        p = readers;
        while (p != NULL)
        {
          printf("READ INPUT (%s)\n", p->ip_addr);
          FKGTCP_Mio_ReadInputs(p->ip_addr);
          p = p->next;
        }
        break;

      case '0' :
        p = readers;
        while (p != NULL)
        {
          int i;
          for (i=0; i<8; i++)
            FKGTCP_Mio_SetOutput(p->ip_addr, i);
          p = p->next;
        }
        break;

      case '1' :
        p = readers;
        while (p != NULL)
        {
          int i;
          for (i=0; i<8; i++)
            FKGTCP_Mio_ClearOutput(p->ip_addr, i);
          p = p->next;
        }
        break;

    }
  }

done:
  printf("Exiting...\n");

  p = readers;
  while (p != NULL)
  {
    READER_ST *r = p;
    FKGTCP_Destroy(p->ip_addr);
    p = p->next;
    free(r);
  }

  return EXIT_SUCCESS;
}
