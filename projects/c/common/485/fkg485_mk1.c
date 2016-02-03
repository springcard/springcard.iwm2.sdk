#include "fkg485_i.h"

static const char ESCAPE_STR[]     = { 0x1B, '\0' };
static const char CRLF_STR[]       = "\r\n";
static const char SPRINGCARD_STR[] = "SpringCard";
static const char RDR_STR[]        = "/RDR";
static const char MIO_STR[]        = "/MIO";
static const char SERNO_STR[]      = "Serno: ";

#ifdef WIN32
BOOL FKG485_MK1_Command(HANDLE comm_handle, const char *command)
#else
BOOL FKG485_MK1_Command(int comm_handle, const char *command)  
#endif
{
  if (!FKG485_SendEx(comm_handle, (const BYTE *) ESCAPE_STR, strlen(ESCAPE_STR)))
    return FALSE;

  if (!FKG485_SendEx(comm_handle, (const BYTE *) command, strlen(command)))
    return FALSE;

  if (!FKG485_SendEx(comm_handle, (const BYTE *) CRLF_STR, strlen(CRLF_STR)))
    return FALSE;

  return TRUE;
}

#ifdef WIN32
BOOL FKG485_MK1_Response(HANDLE comm_handle, char *response, WORD response_size)
#else
BOOL FKG485_MK1_Response(int comm_handle, char *response, WORD response_size)
#endif
{
  WORD total_length = 0;

  memset(response, '\0', response_size);

  for (;;)
  {
    WORD recv_length = 0;

    if (!FKG485_RecvEx(comm_handle,(BYTE *) &response[total_length], 8, &recv_length))
      return FALSE;

    if (recv_length == 0)
      break;

    total_length += recv_length;
    if (total_length+8 >= response_size)
      break;
  }

  if (total_length == 0)
    return FALSE;

  return TRUE;
}

#ifdef WIN32
BOOL FKG485_MK1_Probe(HANDLE comm_handle)
#else
BOOL FKG485_MK1_Probe(int comm_handle) 
#endif
{
  BYTE wink_command[]  = { 'Z', '2', 0x0D, 0x0A };
  BYTE response[512];
  WORD total_length = 0;
  BOOL got_ack = FALSE;
  char *ident = NULL;
  char *serno = NULL;
  WORD i, j;

  if (!FKG485_SendEx(comm_handle, wink_command, sizeof(wink_command)))
    return FALSE;

#ifdef WIN32
  Sleep(100);
#else
  usleep(100000);
#endif

  if (!FKG485_MK1_Command(comm_handle, "info"))
    return FALSE;

  for (;;)
  {
    WORD recv_length = 0;
    if (!FKG485_RecvEx(comm_handle, &response[total_length], 8, &recv_length))
      return FALSE;
    if (recv_length == 0)
      break;
    total_length += recv_length;
    if (total_length+8 >= sizeof(response))
      break;
  }
  
  for (i=0; i<total_length; i++)
  {
    if ((response[i] == 0x06) || (response[i] == 0x15))
    {
      got_ack = TRUE;
      i++;
      break;
    }
  }

  if (!got_ack)
    return FALSE;

  ident = (char *) &response[i];

  for (j=i; j<total_length; j++)
  {
    if ((response[j] == 0x0D) || (response[j] == 0x0A))
    {
      response[j] = '\0';
      i = j;
      break;
    }
  }

  if (ident == NULL)
    return FALSE;

  for (   ; i<total_length - strlen(SERNO_STR); i++)
  {
    if (!sc_strnicmp((const char *) &response[i], SERNO_STR, strlen(SERNO_STR)))
    {
			serno = (char *) &response[i + strlen(SERNO_STR)];
      for (j=i; j<total_length; j++)
      {
        if ((response[j] == 0x0D) || (response[j] == 0x0A))
        {
          response[j] = '\0';
          i = j;
          break;
        }
      }
      break;
    }
  }

  if (serno == NULL)
    return FALSE;

  printf("Got an answer:\n");
  printf("\t%s\n", ident);
  printf("\tSerno: %s\n", serno);

  if (strstr(ident, SPRINGCARD_STR) == ident)
  {
    if (strstr(ident, RDR_STR) != NULL)
    {
      printf("This is a Reader\n");
      return TRUE;
    } else
    if (strstr(ident, MIO_STR) != NULL)
    {
      printf("This is an I/O Module\n");
      return TRUE;
    } else
      printf("This is not a Reader nor an I/O Module\n");
  } else
    printf("This is not a %s device\n", SPRINGCARD_STR);

  return FALSE;
}
