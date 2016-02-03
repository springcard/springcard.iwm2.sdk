/**** Fkg485.dll/ctx.c
 *
 * NAME
 *   Fkg485.dll -- Context
 *
 * DESCRIPTION
 *   Constructor/destructor, start/stop the link
 *
 **/
#include "fkg485_dll_i.h"
#include <assert.h>

BOOL fkg485_verbose = FALSE;
BOOL fkg485_debug = FALSE;

/**f* Fkg485.dll/FKG485_Create
 *
 * NAME
 *   FKG485_Create
 *
 * DESCRIPTION
 *  Instanciate a new context to communicate with a device (either a
 *  FunkyGate or a HandyDrummer)
 *
 * SYNOPSIS
 *   BOOL FKG485_Create(const char *comm_name, BYTE device_addr, const char *device_name);
 *
 * INPUTS
 *   const char *comm_name : the communication port
 *   BYTE        device_addr   : the device's address
 *   const char *device_name   : the device's "friendly-name"
 *
 * RETURNS
 *   TRUE  : success, context created
 *   FALSE : can't create the context (out of memory or device_name invalid)
 *
 * SEE ALSO
 *   FKG485_Start
 *   FKG485_Destroy
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_Create(const char *comm_name, BYTE device_addr, const char *device_name)
{
  FKG_DEVICE_CTX_ST *device_ctx  = NULL;
  FKG485_DEVICE_ST  *_device = NULL;

  if (device_name != NULL)
  {
    /* Check the name isn't already in use */
    device_ctx = FKG_FindDevice(device_name);
    if (device_ctx != NULL)
      return FALSE;
  }

  device_ctx = (FKG_DEVICE_CTX_ST *) calloc(1, sizeof(FKG_DEVICE_CTX_ST));
  if (device_ctx == NULL)
    goto failed;

  if (device_name != NULL)
  {
    device_ctx->Name = sc_strdup(device_name);
  } else
  {
    char buffer[64];
    DWORD i = 1 + FKG_GetDeviceCount();

    do
    {
      sprintf(buffer, "Device %d", i);
    } while (FKG_FindDevice(buffer) != NULL);

    device_ctx->Name = sc_strdup(buffer);
  }

  if (device_ctx->Name == NULL)
    goto failed;

  _device = FKG485_AttachDevice(comm_name, device_addr);
  if (_device == NULL)
		goto failed;

	 _device->_device_ctx = device_ctx;
  device_ctx->CtxData = _device;

  if (!FKG_InsertDevice(device_ctx))
    goto failed;

  if (FKG_Callbacks.OnStatusChange != NULL)
    if (!FKG_Callbacks.OnStatusChange(device_ctx->Name, FKG_STATUS_CREATED))
      goto failed;

  return TRUE;

failed:
  if (_device != NULL)
  {
    FKG485_DetachDevice(_device);
    free(_device);
  }
  if (device_ctx != NULL)
  {
    if (device_ctx->Name != NULL)
      free(device_ctx->Name);
    free(device_ctx);
  }

  return FALSE;
}

/**f* Fkg485.dll/FKG485_Destroy
 *
 * NAME
 *   FKG485_Destroy
 *
 * DESCRIPTION
 *  Free a context previously allocated by FKG485_Create
 *
 * SYNOPSIS
 *   void FKG485_Create(const char *device_name);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   Nothing
 *
 * SEE ALSO
 *   FKG485_Stop
 *   FKG485_Create
 *
 **/
FKG485_LIB void FKG485_API FKG485_Destroy(const char *device_name)
{
  FKG_DEVICE_CTX_ST *device_ctx = FKG_FindDevice(device_name);

  if (device_ctx != NULL)
  {
    FKG485_DEVICE_ST *_device = (FKG485_DEVICE_ST *) device_ctx->CtxData;
    if (_device != NULL)
    {
      FKG485_DetachDevice(_device);
      free(_device);
    }

    FKG_RemoveDevice(device_ctx);
    if (device_ctx->Name != NULL)
      free(device_ctx->Name);
    free(device_ctx);
  }
}

/**f* Fkg485.dll/FKG485_Start
 *
 * NAME
 *   FKG485_Start
 *
 * DESCRIPTION
 *  Start a thread that will open the communication channel with the device, and
 *  handle it until FKG485_Stop is called
 *
 * SYNOPSIS
 *   BOOL FKG485_Start(const char *comm_name)
 *
 * INPUTS
 *   const char *comm_name : the name of the communication port
 *
 * RETURNS
 *   TRUE  : success, thread is starting
 *   FALSE : error (device_name doesn't point to a valid context?)
 *
 * NOTES
 *   This function is asynchronous: it starts a thread that does the job in background.
 *   Therefore, a TRUE return value doesn't imply that the connection_string is valid
 *   and points truly to a running device.
 *   Use FKG485_SetStatusChangeCallback to register a function that will be notified
 *   when the communication channel is actually opened (and closed later on)
 *
 * SEE ALSO
 *   FKG485_Stop
 *   FKG485_SetStatusChangeCallback
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_Start(const char *comm_name, BYTE max_devices)
{
  FKG485_CHANNEL_ST *channel = FKG485_FindChannel(comm_name);

  if (fkg485_verbose)
    printf("FKG485_Start(%s)\n", comm_name);

  if (channel != NULL)
  {
    if (fkg485_verbose)
      printf("Error: %s is already active\n", comm_name);
    return FALSE;
  }

  return FKG485_StartEx(comm_name, max_devices);
}

/**f* Fkg485.dll/FKG485_Stop
 *
 * NAME
 *   FKG485_Stop
 *
 * DESCRIPTION
 *  Ask the thread that handles the communication with a device to close its
 *  communication channel.
 *
 * SYNOPSIS
 *   BOOL FKG485_Stop(const char *device_name)
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
 *   Use FKG485_SetStatusChangeCallback to register a function that will be notified
 *   when the communication channel is actually closed, or call FKG485_Join to wait
 *   until the thread terminates.
 *
 * SEE ALSO
 *   FKG485_Start
 *   FKG485_Join
 *   FKG485_SetStatusChangeCallback
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_Stop(const char *comm_name)
{
  FKG485_CHANNEL_ST *channel = FKG485_FindChannel(comm_name);

  if (channel == NULL)
    return FALSE;

  channel->terminated = TRUE;
  return TRUE;
}

/**f* Fkg485.dll/FKG485_Join
 *
 * NAME
 *   FKG485_Join
 *
 * DESCRIPTION
 *  Wait until the thread that communicates with the device terminates.
 *
 * SYNOPSIS
 *   BOOL FKG485_Join(const char *device_name)
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   TRUE  : success, thread has exited
 *   FALSE : error (device_name doesn't point to a valid context, FKG485_Stop not
 *                  called earlier?)
 *
 * SEE ALSO
 *   FKG485_Stop
 *   FKG485_Destroy
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_Join(const char *comm_name)
{
  FKG485_CHANNEL_ST *channel = FKG485_FindChannel(comm_name);
  
	if (channel == NULL)
    return FALSE;
	
	if (!channel->terminated)
    return FALSE;
  
	while (channel->thrd_running)
#ifdef WIN32
  Sleep(200);
#else
  usleep(200000);
#endif

  return TRUE;
}
