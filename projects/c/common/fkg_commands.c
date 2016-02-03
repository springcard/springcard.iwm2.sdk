#include "fkg_i.h"

BOOL FKG_Ping(FKG_DEVICE_CTX_ST *device_ctx)
{
  return FKG_SendToDeviceEx(device_ctx, NULL, 0);
}

BOOL FKG_AskForStatus(FKG_DEVICE_CTX_ST *device_ctx)
{
  BYTE buffer[2];

  buffer[0] = 0x00;
  buffer[1] = 0x00;

  FKG_SetLastQuery(device_ctx, FKG_QUERY_STATUS);
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_AskForInfo(FKG_DEVICE_CTX_ST *device_ctx)
{
  BYTE buffer[2];

  buffer[0] = 0x01;
  buffer[1] = 0x00;

  FKG_SetLastQuery(device_ctx, FKG_QUERY_INFOS);
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_AskForCapacities(FKG_DEVICE_CTX_ST *device_ctx)
{
  BYTE buffer[2];

  buffer[0] = 0x02;
  buffer[1] = 0x00;

  FKG_SetLastQuery(device_ctx, FKG_QUERY_CAPACITIES);
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_AskForSerno(FKG_DEVICE_CTX_ST *device_ctx)
{
  BYTE buffer[2];

  buffer[0] = 0x03;
  buffer[1] = 0x00;

  FKG_SetLastQuery(device_ctx, FKG_QUERY_SERNO);
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_ReadConfig(FKG_DEVICE_CTX_ST *device_ctx, BYTE reg)
{
  /* NOT IMPLEMENTED IN READER */
  return FALSE;
}

BOOL FKG_WriteConfig(FKG_DEVICE_CTX_ST *device_ctx, BYTE reg, const BYTE data[], BYTE size)
{
  BYTE buffer[30];
  BYTE i;
    
  if ((size+1) > 30)
    return FALSE;
  
  buffer[0] = 0x0C;
  buffer[1] = 1+size;
  buffer[2] = reg;
  for (i=0; i<size; i++)
    buffer[3+i] = data[i];

  FKG_SetLastQuery(device_ctx, FKG_QUERY_WRITE);
  return FKG_SendToDeviceEx(device_ctx, buffer, size+3);
  
}

BOOL FKG_DoReset(FKG_DEVICE_CTX_ST *device_ctx)
{
  BYTE buffer[4];
  
  buffer[0] = 0x0B;
  buffer[1] = 0x02;
  buffer[2] = 0xDE;
  buffer[3] = 0xAD;
  
  FKG_SetLastQuery(device_ctx, FKG_QUERY_RESET);
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));  
  
}

BOOL FKG_LedDefault(FKG_DEVICE_CTX_ST *device_ctx)
{
  BYTE buffer[3];

  buffer[0] = 0xD0;
  buffer[1] = 0x00;
  buffer[2] = 0x00;
  
  FKG_Trace(TRACE_INFO, "%s:led->default\n", device_ctx->Name);
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_LedPermanent(FKG_DEVICE_CTX_ST *device_ctx, BYTE led_red, BYTE led_green)
{
  BYTE buffer[5];

  buffer[0] = 0xD0;
  buffer[1] = 0x00;
  buffer[2] = 0x02;
  buffer[3] = led_red;
  buffer[4] = led_green;
  
  FKG_Trace(TRACE_INFO, "%s:led permanent (%02X, %02X)\n", device_ctx->Name, led_red, led_green);

  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_LedSequence(FKG_DEVICE_CTX_ST *device_ctx, BYTE led_red, BYTE led_green, WORD duration_s)
{
  BYTE buffer[7];

  buffer[0] = 0xD0;
  buffer[1] = 0x00;
  buffer[2] = 0x04;
  buffer[3] = led_red;
  buffer[4] = led_green;
  buffer[5] = (BYTE) (duration_s / 0x0100);
  buffer[6] = (BYTE) (duration_s % 0x0100);
  
  FKG_Trace(TRACE_INFO, "%s:led sequence (%02X, %02X, %d)\n", device_ctx->Name, led_red, led_green, duration_s);
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_Buzzer(FKG_DEVICE_CTX_ST *device_ctx, BYTE sequence)
{
  BYTE buffer[4];

  buffer[0] = 0xD1;
  buffer[1] = 0x00;
  buffer[2] = 0x01;
  buffer[3] = sequence;
  
  FKG_Trace(TRACE_INFO, "%s:buzzer (%02X)\n", device_ctx->Name, sequence);
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_SetRuntimeMode(FKG_DEVICE_CTX_ST *device_ctx, BYTE runtime_mode, WORD duration_s)
{
  BYTE buffer[5];

  buffer[0] = 0x0A;
  buffer[1] = 0x03;
  buffer[2] = runtime_mode;
  buffer[3] = (BYTE) (duration_s / 0x0100);
  buffer[4] = (BYTE) (duration_s % 0x0100);
  
  FKG_Trace(TRACE_INFO, "%s:set runtime mode (%02X, %d)\n", device_ctx->Name, runtime_mode, duration_s);
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_Mio_SetOutput(FKG_DEVICE_CTX_ST *device_ctx, BYTE which)
{
  BYTE buffer[3];

  buffer[0] = 0x90;
  buffer[1] = which;
  buffer[2] = 0x00;
  
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_Mio_SetOutputT(FKG_DEVICE_CTX_ST *device_ctx, BYTE which, WORD timeout_s)
{
  BYTE buffer[5];

  buffer[0] = 0x90;
  buffer[1] = which;
  buffer[2] = 0x02;
	buffer[3]	=	(BYTE) (timeout_s >> 8);
	buffer[4]	=	(BYTE) (timeout_s & 0x00FF);
  
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_Mio_ClearOutput(FKG_DEVICE_CTX_ST *device_ctx, BYTE which)
{
  BYTE buffer[3];

  buffer[0] = 0xA0;
  buffer[1] = which;
  buffer[2] = 0x00;
  
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

BOOL FKG_Mio_ReadInputs(FKG_DEVICE_CTX_ST *device_ctx)
{
  BYTE buffer[2];

  buffer[0] = 0x04;
  buffer[1] = 0x00;
  
  return FKG_SendToDeviceEx(device_ctx, buffer, sizeof(buffer));
}

