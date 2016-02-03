#include "fkg485_i.h"
#include <assert.h>
#include "lib-c/utils/timeval.h"
#include "lib-c/utils/gettimeofday.h"

#define FKG485_KEEP_COOL_MS  260

#define FKG485_KIPA_INTERVAL 10
#define FKG485_RESP_INTERVAL 3

static void FKG485_SetReaderDelay(FKG485_DEVICE_ST *_device)
{
	struct timeval now;

	if (_device == NULL)
		return;
  
  gettimeofday(&now, NULL);
  add_ms_to_timeval(&now, FKG485_KEEP_COOL_MS, &_device->_keep_cool_until);
}

static BOOL FKG485_IsReaderDelayed(FKG485_DEVICE_ST *_device, unsigned long *wait_ms)
{
  struct timeval now;
  
	if (_device == NULL)
		return FALSE;
  
  gettimeofday(&now, NULL);
  if (compare_timeval(&now, &_device->_keep_cool_until) < 0)
  {
    if (wait_ms != NULL)
    {
      unsigned long diff_us;
      diff_us  = _device->_keep_cool_until.tv_sec - now.tv_sec;
      diff_us *= 1000000;
      diff_us += _device->_keep_cool_until.tv_usec - now.tv_usec;
      
      diff_us /= 1000;
    
      /* Only decrease wait_ms, never increase it (to avoid sleeping too much) */
      if ((*wait_ms == 0) || (diff_us < *wait_ms))
        *wait_ms = diff_us;
    }
    
    return TRUE;
  }
    
  return FALSE;
}

static BOOL FKG485_Enumerate(FKG485_CHANNEL_ST *channel, BYTE *count)
{
  BYTE addr;
  BYTE c = 0;
  
  for (addr=0; addr<channel->max_devices; addr++)
  {
    if (channel->terminated)
      return FALSE;

    if (FKG485_Xfer_S_EnumEx(channel, addr))
    {
      c++;
      channel->_devices[addr].enumerated = TRUE;
      if (FKG485_Callbacks.OnEnumerate != NULL)  
        if (!FKG485_Callbacks.OnEnumerate(channel->comm_name, addr))
          return FALSE;
    }
  }

	if (FKG485_Callbacks.OnEnumLoopEnded != NULL)
    if (!FKG485_Callbacks.OnEnumLoopEnded(channel->comm_name))
      return FALSE;
  

  if (count != NULL)
  {
    *count = c;
  } else
  {
    if (!c)
      return FALSE;
  }

  return TRUE;
}

static BOOL FKG485_WorkDevice(FKG485_DEVICE_ST *_device)
{
  IWM_PACKET_ST packet;
  BOOL packet_to_send;
  BYTE buffer[FKG485_BUFFER_SZ];
  WORD length;

  FKG_DEVICE_CTX_ST *device_ctx = _device->_device_ctx;

  assert(device_ctx != NULL);

  /* Open connexion with FKG485 server */
  if (!_device->connected)
  {            
    if (_device->was_connected)
    {
      /* We were connected, but we no longer are */
      if (fkg485_verbose || fkg485_debug)
        printf("%s:connexion lost\n", device_ctx->Name);

      _device->was_connected = FALSE;

      /* Callback */
      if (FKG_Callbacks.OnStatusChange != NULL)
        if (!FKG_Callbacks.OnStatusChange(device_ctx->Name, FKG_STATUS_DISCONNECTED))
          return FALSE;

      /* We'll try to reconnect later */
      return TRUE;
    }
    _device->connected = FKG485_Xfer_S_Enum(_device);

    if (!_device->connected)
    {
      /* Callback */
      if (FKG_Callbacks.OnStatusChange != NULL)
        if (!FKG_Callbacks.OnStatusChange(device_ctx->Name, FKG_STATUS_CONNECTION_FAILED))
          return FALSE;

      return TRUE;
    }

    /* Success */
    if (fkg485_verbose || fkg485_debug)
      printf("%s:connected\n", device_ctx->Name);

    if (FKG_Callbacks.OnStatusChange != NULL)
      if (!FKG_Callbacks.OnStatusChange(device_ctx->Name, FKG_STATUS_CONNECTED))
        return FALSE;

    /* We've found it */
    _device->state = FKG485_STATE_FOUND;
  
  }

  packet_to_send = FALSE;
#ifdef WIN32
  WaitForSingleObject(_device->_channel->send_mutex, INFINITE);
#else
  if (pthread_mutex_lock(&_device->_channel->send_mutex) != 0)
  {
    printf("Error:Can't lock send_mutex\n");
    return FALSE;
  }
#endif
  if (_device->send_queue != NULL)
  {
    FKG485_QUEUE_ST *p;
    p = _device->send_queue;

    memcpy(&packet, &p->packet, sizeof(IWM_PACKET_ST));
    _device->send_queue = p->next;
    free(p);
    packet_to_send = TRUE;
  }
#ifdef WIN32
  ReleaseMutex(_device->_channel->send_mutex);
#else
  if (pthread_mutex_unlock(&_device->_channel->send_mutex) != 0)
  {
    printf("Error:Can't unlock send_mutex\n");
    return FALSE;    
  }  
#endif
  if (packet_to_send)
  {
    if (!FKG485_Xfer_I(_device, packet.payload, packet.length, buffer, sizeof(buffer), &length))
    {
      if (fkg485_verbose || fkg485_debug)
        printf("%s:no answer to I\n", device_ctx->Name);

      _device->connected = FALSE;
      return TRUE;
    }
  } else
  {
    if (!FKG485_Xfer_I_Poll(_device, buffer, sizeof(buffer), &length))
    {
      if (fkg485_verbose || fkg485_debug)
        printf("%s:no answer to I (poll)\n", device_ctx->Name);

      _device->connected = FALSE;
      return TRUE;
    }
  }

  /* Communication is OK */
  _device->was_connected = TRUE;

  /* Is there something to process ? */
  if (length)
  {
    if (fkg485_debug)
      printf("%s:received %dB\n", device_ctx->Name, length);

    if (!FKG485_RecvFromDeviceEx(device_ctx, buffer, length))
    {
      if (fkg485_verbose || fkg485_debug)
        printf("%s:receive failed (1)\n", device_ctx->Name);

      _device->connected = FALSE;
    }
  }

  return TRUE;
}

#ifdef WIN32
DWORD WINAPI FKG485_CommThread(void *param)
#else
void FKG485_CommThread(void *param)
#endif
{
  FKG485_CHANNEL_ST *channel;
  BYTE count=0;
  BYTE addr;

  if (fkg485_verbose)
    printf("Starting thread...\n");

  channel = (FKG485_CHANNEL_ST *) param;	
  if (channel == NULL)
  {
    if (fkg485_verbose || fkg485_debug)
      printf("Internal error\n");
#ifdef WIN32
    return 0;
#else
    return;
#endif
  }

  /* Step 1 : enumeration of all the available devices */
  /* ------------------------------------------------- */

  if (fkg485_verbose)
    printf("Starting enumeration on %s\n", channel->comm_name);

  if (!FKG485_Enumerate(channel, &count))
#ifdef WIN32
    return 0;
#else
    return;
#endif

  
  if (fkg485_verbose)
    printf("Enumeration terminated, %d devices found on %s\n", count, channel->comm_name);

  /* Step 2 : infinite loop */
  /* ---------------------- */

  for (;;)
  {
    BOOL released = FALSE;
    unsigned long wait_ms = 0; 
    for (addr=0; addr<channel->max_devices; addr++)
    {
      FKG485_DEVICE_ST *_device;
#ifdef WIN32      
      WaitForSingleObject(channel->list_mutex, INFINITE);
#else
      if (pthread_mutex_lock(&channel->list_mutex) != 0)
      {
        printf("Error:Can't lock list_mutex\n");
        return;
      }
#endif   
      _device = &channel->_devices[addr];
      if (_device->enumerated)
      {
        if (!channel->terminated)
        {
          if (_device->_device_ctx != NULL)
          {
            /* Is it time to send a command ? */
            if (FKG485_IsReaderDelayed(_device, &wait_ms))
            {
              /*  No ---> continue ... */
#ifdef WIN32
              ReleaseMutex(channel->list_mutex);
#else
              if (pthread_mutex_unlock(&channel->list_mutex) != 0)
              {
                printf("Error:Can't unlock list_mutex\n");
                return;
              }
#endif          
              continue;
            }
            if (!FKG485_WorkDevice(_device))
            {
              released = TRUE;          
#ifdef WIN32
              ReleaseMutex(channel->list_mutex);
#else
              if (pthread_mutex_unlock(&channel->list_mutex) != 0)
              {
                printf("Error:Can't unlock list_mutex\n");
                return;
              }
#endif
            }
            /* Set a delay, in order not to talk to fast to the reader */
            FKG485_SetReaderDelay(_device);
            
          } else
          {
            FKG485_Xfer_S_Enum(_device);
          }
          
        } else
        {
          if (FKG_Callbacks.OnStatusChange != NULL)
          {
            released = TRUE;
#ifdef WIN32
            ReleaseMutex(channel->list_mutex);
#else
            if (pthread_mutex_unlock(&channel->list_mutex) != 0)
            {
              printf("Error:Can't unlock list_mutex\n");
              return; 
            }
#endif
            if (_device->_device_ctx != NULL)
              if (FKG_Callbacks.OnStatusChange != NULL)
                FKG_Callbacks.OnStatusChange(_device->_device_ctx->Name, FKG_STATUS_TERMINATED);
          }
        }
      }
      if (!released)
      {
#ifdef WIN32        
        ReleaseMutex(channel->list_mutex);
#else
        if (pthread_mutex_unlock(&channel->list_mutex) != 0)
        {
          printf("Error:Can't unlock list_mutex\n");
          return; 
        } 
#endif
      }
    }
    /* 'wait_ms' is the smallest required sleeping time found while looping */
#ifndef WIN32
    usleep((wait_ms+1)*1000 ) ; /* add '+1' for micro-seconds adjustment	*/
#else
    Sleep(wait_ms+1);
#endif

    /* Only if we have not waited (ie: a command was sent): call callback */
    if (wait_ms == 0) 
      if (FKG485_Callbacks.OnILoopEnded != NULL)
        if (!FKG485_Callbacks.OnILoopEnded(channel->comm_name))
#ifdef WIN32
          return 0;
#else
          return;
#endif
    if (channel->terminated)
      break;

  }

  FKG485_CloseCommEx(channel->comm_handle);

  channel->thrd_running = FALSE;
#ifdef WIN32  
  return 0; 
#endif  
}


