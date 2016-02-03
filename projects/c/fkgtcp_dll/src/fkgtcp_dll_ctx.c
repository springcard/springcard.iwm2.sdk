/**** FkgTCP.dll/ctx.c
 *
 * NAME
 *   FkgTCP.dll -- Context
 *
 * DESCRIPTION
 *   Constructor/destructor, start/stop the link
 *
 **/
#include "fkgtcp_dll_i.h"
#include <assert.h>

FKGTCP_LIB void FKGTCP_API FKGTCP_SetVerbose(BYTE level, const char *filename)
{
  FKG_SetVerbose(level, filename);
}

/**f* FkgTCP.dll/FKGTCP_Create
 *
 * NAME
 *   FKGTCP_Create
 *
 * DESCRIPTION
 *  Instanciate a new context to communicate with a device (either a
 *  FunkyGate or a HandyDrummer)
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Create(const char *device_name);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   TRUE  : success, context created
 *   FALSE : can't create the context (out of memory or device_name invalid)
 *
 * SEE ALSO
 *   FKGTCP_Start
 *   FKGTCP_Destroy
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Create(const char *device_name)
{
  FKG_DEVICE_CTX_ST *device;

  if (device_name != NULL)
  {
    /* Check the name isn't already in use */
    device = FKG_FindDevice(device_name);
    if (device != NULL)
      return FALSE;
  }
  
  device = calloc(1, sizeof(FKG_DEVICE_CTX_ST));
  if (device == NULL)
    goto failed;

  if (device_name != NULL)
  {
    device->Name = sc_strdup(device_name);
  } else
  {
    char buffer[64];
    DWORD i = 1 + FKG_GetDeviceCount();

    do
    {
      sprintf(buffer, "Device %d", i);
    } while (FKG_FindDevice(buffer) != NULL);

    device->Name = sc_strdup(buffer);
  }

  if (device->Name == NULL)
    goto failed;

  device->CtxData = calloc(1, sizeof(FKGTCP_CLIENT_CTX_ST));
  if (device->CtxData == NULL)
    goto failed;

  if (!FKG_InsertDevice(device))
    goto failed;

  if (FKG_Callbacks.OnStatusChange != NULL)
    if (!FKG_Callbacks.OnStatusChange(device->Name, FKG_STATUS_CREATED))
      goto failed;

  return TRUE;

failed:
  if (device != NULL)
  {
    if (device->CtxData != NULL)
      free(device->CtxData);
    if (device->Name != NULL)
      free(device->Name);
    free(device);
  }

  return FALSE;
}

/**f* FkgTCP.dll/FKGTCP_Destroy
 *
 * NAME
 *   FKGTCP_Destroy
 *
 * DESCRIPTION
 *  Free a context previously allocated by FKGTCP_Create
 *
 * SYNOPSIS
 *   void FKGTCP_Create(const char *device_name);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   Nothing
 *
 * SEE ALSO
 *   FKGTCP_Stop
 *   FKGTCP_Create
 *
 **/
FKGTCP_LIB void FKGTCP_API FKGTCP_Destroy(const char *device_name)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);

  if (device != NULL)
  {
    FKGTCP_Stop(device_name);
    FKGTCP_Join(device_name);

    FKG_RemoveDevice(device);

    if (device->CtxData != NULL)
      free(device->CtxData);
    if (device->Name != NULL)
      free(device->Name);
    free(device);
  }
}

/**f* FkgTCP.dll/FKGTCP_Start
 *
 * NAME
 *   FKGTCP_Start
 *
 * DESCRIPTION
 *  Start a thread that will open the communication channel with the device, and
 *  handle it until FKGTCP_Stop is called
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Start(const char *device_name, const char *connection_string, const BYTE auth_key[16])
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *   const char *connection_string : the device's address on the network (coul'd be
 *                                   a DNS name or IPv4 address in dotted notation)
 *	 const BYTE auth_key[16] : the device's AES Operation Key
 *
 * RETURNS
 *   TRUE  : success, thread is starting
 *   FALSE : error (device_name doesn't point to a valid context?)
 *
 * NOTES
 *   This function is asynchronous: it starts a thread that does the job in background.
 *   Therefore, a TRUE return value doesn't imply that the connection_string is valid
 *   and points truely to a running device.
 *   Use FKGTCP_SetStatusChangeCallback to register a function that will be notified
 *   when the communication channel is actually opened (and closed later on)
 *
 * SEE ALSO
 *   FKGTCP_Stop
 *   FKGTCP_SetStatusChangeCallback
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Start(const char *device_name, const char *connection_string, const BYTE auth_key[])
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);

  if (device == NULL)
    return FALSE;

#ifdef FKGTCP_SECURE
  if (auth_key != NULL)
	  return FKGTCP_StartEx_Auth(device, connection_string, auth_key);
#endif

  return FKGTCP_StartEx_Plain(device, connection_string);
}

/**f* FkgTCP.dll/FKGTCP_Stop
 *
 * NAME
 *   FKGTCP_Stop
 *
 * DESCRIPTION
 *  Ask the thread that handles the communication with a device to close its
 *  communication channel.
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Stop(const char *device_name)
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   TRUE  : success, thread is stopping
 *   FALSE : error (device_name doesn't point to a valid context?)
 *
 * NOTES
 *   This function is asynchronous.
 *   Use FKGTCP_SetStatusChangeCallback to register a function that will be notified
 *   when the communication channel is actually closed, or call FKGTCP_Join to wait
 *   until the thread terminates.
 *
 * SEE ALSO
 *   FKGTCP_Start
 *   FKGTCP_Join
 *   FKGTCP_SetStatusChangeCallback
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Stop(const char *device_name)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);
  FKGTCP_CLIENT_CTX_ST *client;

  if (device == NULL)
    return FALSE;

  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);

  client->terminated = TRUE;
  return TRUE;
}

/**f* FkgTCP.dll/FKGTCP_Join
 *
 * NAME
 *   FKGTCP_Join
 *
 * DESCRIPTION
 *  Wait until the thread that communicates with the device terminates.
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Join(const char *device_name)
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   TRUE  : success, thread has exited
 *   FALSE : error (device_name doesn't point to a valid context, FKGTCP_Stop not
 *                  called earlier?)
 *
 * SEE ALSO
 *   FKGTCP_Stop
 *   FKGTCP_Destroy
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Join(const char *device_name)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);
  FKGTCP_CLIENT_CTX_ST *client;

  if (device == NULL)
    return FALSE;

  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);

  if (!client->terminated)
    return FALSE;

  while (client->thrd_running)
    Sleep(200);

  if (FKG_Callbacks.OnStatusChange != NULL)
    FKG_Callbacks.OnStatusChange(device->Name, FKG_STATUS_DISPOSED);

  return TRUE;
}
