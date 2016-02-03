#include "fkg485_i.h"
#include <assert.h>


static FKG485_CHANNEL_ST *channels = NULL;

BOOL FKG485_StartEx(const char *comm_name, BYTE max_devices)
{
  FKG485_CHANNEL_ST *channel;
  BYTE addr;

  if (max_devices > FKG485_MAX_DEVICES_PER_CHANNEL)
    max_devices = FKG485_MAX_DEVICES_PER_CHANNEL;

  assert(comm_name != NULL);

  channel = calloc(1, sizeof(FKG485_CHANNEL_ST));
  if (channel == NULL)
    return FALSE;

  for (addr=0; addr<FKG485_MAX_DEVICES_PER_CHANNEL; addr++)
  {
    channel->_devices[addr]._channel = channel;
    channel->_devices[addr]._addr    = addr;
  }

  if (fkg485_verbose || fkg485_debug)
    printf("Creating thread for port %s\n", comm_name);

  channel->comm_name = sc_strdup(comm_name);  
  if (channel->comm_name == NULL)
    goto failed;
  
  if (!FKG485_OpenCommEx(&channel->comm_handle, channel->comm_name))
    goto failed;
  
#ifdef WIN32
  channel->send_mutex = CreateMutex(NULL, FALSE, NULL);
  if (channel->send_mutex == INVALID_HANDLE_VALUE)
    goto failed;  
#else
  if (pthread_mutex_init(&channel->send_mutex, NULL) != 0)
    goto failed;
#endif
  channel->max_devices = max_devices;

  channel->terminated = FALSE;
  channel->thrd_running = TRUE;
#ifdef WIN32
  channel->thrd_hnd = CreateThread(
    NULL,
    0,
    FKG485_CommThread,
    channel,
    0,
    &channel->thrd_id);
    
    if (channel->thrd_hnd == INVALID_HANDLE_VALUE)
      goto failed;
#else
  if (pthread_create(&channel->thrd_hnd, NULL, (void *) FKG485_CommThread, channel) != 0)
    goto failed;
#endif

  if (channels == NULL)
  {
    channels = channel;
  } else
  {
    FKG485_CHANNEL_ST *p = channels;
    while (p->next != NULL)
      p = p->next;
    p->next = channel;
  }

  return TRUE;

failed:
#ifdef WIN32
  if (channel->thrd_hnd) TerminateThread(channel->thrd_hnd, 0);
  if (channel->comm_handle) CloseHandle(channel->comm_handle);
  if (channel->send_mutex) CloseHandle(channel->send_mutex);
  if (channel->list_mutex) CloseHandle(channel->list_mutex);
#else
  if (channel->thrd_hnd) pthread_join(channel->thrd_hnd, NULL);
  pthread_mutex_destroy(&channel->send_mutex);
  pthread_mutex_destroy(&channel->list_mutex);
#endif
  if (channel->comm_name) free(channel->comm_name);
  free(channel);
  return FALSE;
}

FKG485_CHANNEL_ST *FKG485_FindChannel(const char *comm_name)
{
  FKG485_CHANNEL_ST *p = channels;

  assert(comm_name != NULL);

  while (p != NULL)
  {
    if (!strcmp(p->comm_name, comm_name))
      return p;
    p = p->next;
  }

  return p;
}

FKG485_DEVICE_ST *FKG485_AttachDevice(const char *comm_name, BYTE device_addr)
{
  FKG485_CHANNEL_ST *channel = FKG485_FindChannel(comm_name);

  if (channel == NULL)
		return NULL;

  if (device_addr >= FKG485_MAX_DEVICES_PER_CHANNEL)
    return NULL;

  return &channel->_devices[device_addr];
}

void FKG485_DetachDevice(FKG485_DEVICE_ST *device)
{
  FKG485_CHANNEL_ST *channel = channels;

  while (channel != NULL)
  {
    BYTE addr;
    for (addr=0; addr<channel->max_devices; addr++)
    {
      if (&channel->_devices[addr] == device)
      {
        memset(&channel->_devices[addr], 0, sizeof(FKG485_DEVICE_ST));
        return;
      }
    }
    channel = channel->next;
  } 
}


BYTE FKG_GetState(FKG_DEVICE_CTX_ST *device_ctx)
{
 FKG485_DEVICE_ST *_device;

  if (device_ctx == NULL)
    return FKG485_STATE_UNKNOWN;
  _device = (FKG485_DEVICE_ST *) device_ctx->CtxData;	
  if (_device == NULL)
    return FKG485_STATE_UNKNOWN;

  return _device->state;
}

BOOL FKG_SetState(FKG_DEVICE_CTX_ST *device_ctx, BYTE new_state) 
{
  FKG485_DEVICE_ST *_device;

  if (device_ctx == NULL)
    return FALSE;
  _device = (FKG485_DEVICE_ST *) device_ctx->CtxData;	
  if (_device == NULL)
    return FALSE;

  _device->state = new_state;
  return TRUE;
}

void FKG_SetLastQuery(FKG_DEVICE_CTX_ST *device_ctx, FKG_QUERY_E query) 
{
  if (device_ctx == NULL)
    return;

  device_ctx->LastQuery = query;
}
