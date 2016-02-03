#include "fkg485_i.h"
#include <assert.h>

#define ASCII_STX 0x02
#define ASCII_ETX 0x03
#define ASCII_DLE 0x10
#define ASCII_ESC 0x1B

#define FKG485_DEBUG

BOOL FKG485_RecvFromDevice(FKG_DEVICE_CTX_ST *device_ctx, IWM_PACKET_ST *packet)
{
  FKG485_DEVICE_ST *_device;
  
  assert(device_ctx != NULL);
  _device = (FKG485_DEVICE_ST *) device_ctx->CtxData;	
  assert(_device != NULL);

  switch (packet->type & IWM2_MK2_PROTO_TYPE_MASK)
  { 
    case IWM2_MK2_PROTO_I_BLOCK :
      /* Proces the I-Block */
      if (!FKG_RecvFromDevice(device_ctx, packet->payload, packet->length))
      {
        if (fkg485_verbose || fkg485_debug)
          printf("%s:recv:I-Block:processing failed\n", device_ctx->Name);
        return FALSE;
      }      
      break;
    
    default :
      /* Unsupported frame type */
      if (fkg485_verbose || fkg485_debug)
        printf("%s:recv:bad frame type\n", device_ctx->Name);
      return FALSE;
  }    

  return TRUE;
}

BOOL FKG485_RecvFromDeviceEx(FKG_DEVICE_CTX_ST *device_ctx, BYTE frame[], WORD length)
{
  FKG485_DEVICE_ST *_device;
  IWM_PACKET_ST packet;

  assert(device_ctx != NULL);
  _device = (FKG485_DEVICE_ST *) device_ctx->CtxData;
  assert(_device != NULL);

#ifdef FKG485_DEBUG
  if (fkg485_debug)
  {
    int i;
    printf("%s:R>H ", device_ctx->Name);
    for (i=0; i<length; i++)
      printf("%02X", frame[i]);
    printf("\n");
  }
#endif

  if ((length < 2) || (length > sizeof(packet.payload) + 2))
  {
    if (fkg485_verbose || fkg485_debug)
      printf("%s:recv:wrong length\n", device_ctx->Name);
    return FALSE;
  }

  if (!(frame[0] & IWM2_MK2_PROTO_TYPE_SLAVE_TO_MASTER))
  {
    if (fkg485_verbose || fkg485_debug)
      printf("%s:recv:wrong header\n", device_ctx->Name);
    return FALSE;
  }

  if (frame[1] != _device->_addr)
  {
    if (fkg485_verbose || fkg485_debug)
      printf("%s:recv:wrong address\n", device_ctx->Name);
    return FALSE;
  }

  packet.type   = frame[0];
  packet.length = (BYTE) (length - 2);
  memcpy(packet.payload, &frame[2], packet.length);

  return FKG485_RecvFromDevice(device_ctx, &packet);
}

BOOL FKG485_SendToDeviceEx(FKG_DEVICE_CTX_ST *device_ctx, BYTE frame[], WORD length)
{
  FKG485_DEVICE_ST *_device;
  
  assert(device_ctx != NULL);
  _device = (FKG485_DEVICE_ST *) device_ctx->CtxData;	
  assert(_device != NULL);

  if (frame == NULL)
    length = 0;

  if (!_device->connected)
  {
    if (fkg485_verbose || fkg485_debug)
      printf("%s:can't send (not connected)\n", device_ctx->Name);
    return FALSE;
  }

#ifdef FKG485_DEBUG
  if (fkg485_debug)
  {
    int i;
    printf("%s:R<H ", device_ctx->Name);
    for (i=0; i<length; i++)
      printf("%02X", frame[i]);
    printf("\n");
  }
#endif
  if (!FKG485_SendEx(_device->_channel->comm_handle, frame, length))
  {
    if (fkg485_verbose || fkg485_debug)
      printf("%s:failed to send to reader\n", device_ctx->Name);
    return FALSE;
  }

  return TRUE;
}

BOOL FKG485_SendToDevice(FKG_DEVICE_CTX_ST *device_ctx, IWM_PACKET_ST *packet)
{
  FKG485_DEVICE_ST *_device;
  FKG485_QUEUE_ST *send;
  
  assert(device_ctx != NULL);
  _device = (FKG485_DEVICE_ST *) device_ctx->CtxData;	
  assert(_device != NULL);

  send = malloc(sizeof(FKG485_QUEUE_ST));
  if (send == NULL)
    return FALSE;

  memcpy(&send->packet, packet, sizeof(IWM_PACKET_ST));
  send->next = NULL;

#ifdef WIN32
  WaitForSingleObject(_device->_channel->send_mutex, INFINITE);
#else
  if (pthread_mutex_lock(&_device->_channel->send_mutex) != 0)
  {
    printf("Error:Can't lock send_mutex\n");
    return FALSE;
  }
#endif
  if (_device->send_queue == NULL)
  {
    _device->send_queue = send;
  } else
  {
    FKG485_QUEUE_ST *queue = _device->send_queue;
    while (queue->next != NULL)
      queue = queue->next;
    queue->next = send;
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
  return TRUE;
}

BOOL FKG_SendToDeviceEx(FKG_DEVICE_CTX_ST *device_ctx, BYTE buffer[], WORD length)
{
  IWM_PACKET_ST packet;

  assert(device_ctx != NULL);

  if (buffer == NULL)
    length = 0;

  packet.type = IWM2_MK2_PROTO_I_BLOCK;
  packet.length = (BYTE) length;
  if (packet.length)
    memcpy(packet.payload, buffer, packet.length);

  return FKG485_SendToDevice(device_ctx, &packet);
}

static BOOL buffer_get(BYTE buffer[], WORD size, WORD *offset, BYTE *value, BYTE *lrc)
{
  assert(buffer != NULL);
  assert(offset != NULL);

  if (buffer[*offset] == ASCII_DLE)
    *offset = *offset + 1;

  *value = buffer[*offset];
  if (lrc != NULL)
    *lrc = *lrc ^ buffer[*offset];

  *offset = *offset + 1;

  if (*offset >= size)
    return FALSE;
  if (buffer[*offset] == ASCII_ETX)
    return FALSE;

  return TRUE;
}

static void buffer_put(BYTE buffer[], WORD size, WORD *offset, BYTE value, BYTE *lrc)
{
  assert(buffer != NULL);
  assert(offset != NULL);

  switch (value)
  {
    case ASCII_STX :
    case ASCII_ETX :
    case ASCII_DLE :
    case ASCII_ESC :
      buffer[*offset] = ASCII_DLE;
      *offset = *offset + 1;
      break;
    default :
      break;
  }

  buffer[*offset] = value;
  *offset = *offset + 1;

  if (lrc != NULL)
    *lrc = *lrc ^ value;
}

BOOL FKG485_Xfer_S_EnumEx(FKG485_CHANNEL_ST *channel, BYTE addr)
{
  BYTE buffer[2 + 2 * 3];
  WORD length = 0;
  BYTE lrc = 0;

  assert(channel != NULL);

  /* STX */
  buffer[length++] = ASCII_STX;
  /* TYPE */
  buffer_put(buffer, sizeof(buffer), &length, IWM2_MK2_PROTO_S_ENUM, &lrc);
  /* ADDR */
  buffer_put(buffer, sizeof(buffer), &length, addr, &lrc);
  /* LRC */
  buffer_put(buffer, sizeof(buffer), &length, lrc, &lrc);
  /* ETX */
  buffer[length++] = ASCII_ETX;

  if (!FKG485_SendEx(channel->comm_handle, buffer, length))
  {
    if (fkg485_debug)
      printf("SENUM(%s:%d): failed to send\n", channel->comm_name, addr);
    return FALSE;
  }
  
  if (!FKG485_RecvEx(channel->comm_handle, buffer, sizeof(buffer), &length))
  {
    if (fkg485_debug)
      printf("SENUM(%s:%d): failed to recv\n", channel->comm_name, addr);
    return FALSE;
  }
  if (!length)
    return FALSE;

  if (fkg485_debug)
    printf("SENUM(%s:%d): answer received\n", channel->comm_name, addr);
  return TRUE;
}

BOOL FKG485_Xfer_S_Enum(FKG485_DEVICE_ST *_device)
{
  assert(_device != NULL);

  return FKG485_Xfer_S_EnumEx(_device->_channel, _device->_addr);
}

BOOL FKG485_Send_R_OK(FKG485_DEVICE_ST *_device)
{
  BYTE buffer[2 + 2 * 3];
  WORD length = 0;
  BYTE lrc = 0;

  assert(_device != NULL);
  assert(_device->_channel != NULL);

  /* STX */
  buffer[length++] = ASCII_STX;
  /* TYPE */
  buffer_put(buffer, sizeof(buffer), &length, IWM2_MK2_PROTO_R_OK | (_device->block_number & 0x0F), &lrc);
  /* ADDR */
  buffer_put(buffer, sizeof(buffer), &length, _device->_addr, &lrc);
  /* LRC */
  buffer_put(buffer, sizeof(buffer), &length, lrc, &lrc);
  /* ETX */
  buffer[length++] = ASCII_ETX;

  /* Send */
  if (!FKG485_SendEx(_device->_channel->comm_handle, buffer, length))
  {
    if (fkg485_debug)
      printf("R_OK(%s:%d): failed to send\n", _device->_channel->comm_name, _device->_addr);
    return FALSE;
  }

  return TRUE;
}

BOOL FKG485_Xfer_I(FKG485_DEVICE_ST *_device, const BYTE send_payload[], WORD send_payload_length, BYTE recv_frame[], WORD recv_frame_size, WORD *recv_frame_length)
{
  BYTE buffer[2 + 2 * (IWM_PACKET_PAYLOAD_SIZE + 3)];
  WORD i, length = 0, offset = 0;
  BYTE lrc = 0;
  BYTE recv_byte;

  assert(_device != NULL);
  assert(_device->_channel != NULL);

  if ((send_payload == NULL) && (send_payload_length != 0))
    return FALSE;
  if (send_payload_length > IWM_PACKET_PAYLOAD_SIZE)
    return FALSE;
  if (recv_frame_length != NULL)
    *recv_frame_length = 0;

  /* STX */
  buffer[length++] = ASCII_STX;
  /* TYPE */
  buffer_put(buffer, sizeof(buffer), &length, IWM2_MK2_PROTO_I_BLOCK | (_device->block_number & 0x0F), &lrc);
  /* ADDR */
  buffer_put(buffer, sizeof(buffer), &length, _device->_addr, &lrc);
  /* PAYLOAD */
  if (send_payload != NULL)
    for (i=0; i<send_payload_length; i++)
      buffer_put(buffer, sizeof(buffer), &length, send_payload[i], &lrc);
  /* LRC */
  buffer_put(buffer, sizeof(buffer), &length, lrc, &lrc);
  /* ETX */
  buffer[length++] = ASCII_ETX;

  /* Send */
  if (!FKG485_SendEx(_device->_channel->comm_handle, buffer, length))
  {
    if (fkg485_debug)
      printf("I(%s:%d): failed to send\n", _device->_channel->comm_name, _device->_addr);
    return FALSE;
  }

  /* Recv */
  if (!FKG485_RecvEx(_device->_channel->comm_handle, buffer, sizeof(buffer), &length))
  {
    if (fkg485_debug)
      printf("I(%s:%d): failed to recv\n", _device->_channel->comm_name, _device->_addr);
    return FALSE;
  }

  if (length < 5)
  {
    if (fkg485_debug)
      printf("I(%s:%d): recv too short\n", _device->_channel->comm_name, _device->_addr);
    return FALSE;
  }

  /* Check answer */
  if (buffer[offset++] != ASCII_STX)
  {
    if (fkg485_debug)
      printf("I(%s:%d): !STX\n", _device->_channel->comm_name, _device->_addr);
    return FALSE;
  }
  lrc = 0;
  while (buffer_get(buffer, length, &offset, &recv_byte, &lrc))
  {
    if (recv_frame_size == 0)
    {
      if (fkg485_debug)
        printf("I(%s:%d): buffer overflow\n", _device->_channel->comm_name, _device->_addr);
      return FALSE;
    }
    *recv_frame = recv_byte;
    recv_frame++;
    *recv_frame_length = *recv_frame_length + 1;
    recv_frame_size--;
  }
  if (offset >= length)
  {
    if (fkg485_debug)
      printf("I(%s:%d): no ETX\n", _device->_channel->comm_name, _device->_addr);
  }
  if (buffer[offset++] != ASCII_ETX)
  {
    if (fkg485_debug)
      printf("I(%s:%d): invalid frame\n", _device->_channel->comm_name, _device->_addr);
    return FALSE;
  }

  if (*recv_frame_length > 2)
  {
    if (!FKG485_Send_R_OK(_device))
    {
      return FALSE;
    }
   
  }

  _device->block_number++;
  _device->block_number &= 0x0F;

  return TRUE;
}

BOOL FKG485_Xfer_I_Poll(FKG485_DEVICE_ST *_device, BYTE recv_payload[], WORD recv_payload_size, WORD *recv_payload_length)
{
  return FKG485_Xfer_I(_device, NULL, 0, recv_payload, recv_payload_size, recv_payload_length);
}

