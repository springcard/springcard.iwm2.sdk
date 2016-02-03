#include "fkg_i.h"

BOOL FKG_OnDeviceTLV(FKG_DEVICE_CTX_ST *device_ctx, BYTE tag, BYTE tag_idx, BYTE *value, BYTE length)
{
  BOOL handled = FALSE;
  FKG_Trace(TRACE_DEBUG, "%s:on device TLV (T=%02X %02X, L=%d)\n", device_ctx->Name, tag, tag_idx, length);

  switch (tag)
  {
    case 0x01 :
      /* Global information */
      FKG_Trace(TRACE_INFO, "%s:firmware='%.*s'\n", device_ctx->Name, length, value);
      
      // if (FKG_GetState(device) == FKG_STATE_WAIT_INFO)
      {
        FKG_AskForCapacities(device_ctx);
      }
      break;

    case 0x02 :
      /* Total capacity */
      if (length >= 3)
      {
        FKG_Trace(TRACE_INFO, "%s:%d head(s), %d in, %d out\n", device_ctx->Name, value[0], value[1], value[2]);
      }

      // if (FKG_GetState(device) == FKG_STATE_WAIT_CAPACITIES)
      {
        FKG_AskForSerno(device_ctx);
      }
      break;

    case 0x03 :
      /* Serial number */
      FKG_Trace(TRACE_INFO, "%s:serial number='%.*s'\n", device_ctx->Name, length, value);

      // if (FKG_GetState(device) == FKG_STATE_WAIT_SERNO)
      {
        FKG_AskForStatus(device_ctx);
      }
      break;

    case 0x2F :
      /* Tamper status change notification */
      if (length >= 1)
      {
        if (FKG_Callbacks.OnTamperChange != NULL)
          handled = FKG_Callbacks.OnTamperChange(device_ctx->Name, value[0]);
      } else
      {
        if (FKG_Callbacks.OnTamperChange != NULL)
          handled = FKG_Callbacks.OnTamperChange(device_ctx->Name, 0);
      }
      break;

    case 0x81 :
      /* Reader information */
      FKG_Trace(TRACE_INFO, "%s:head%02d='%.*s'\n", device_ctx->Name, tag_idx, length, value);
      break;

    case 0xB0 :
      /* Badge presentation */
      if (FKG_Callbacks.OnCardRead != NULL)
        handled = FKG_Callbacks.OnCardRead(device_ctx->Name, tag_idx, value, length);
      break;

    case 0xB1 :
      /* Badge inserted or removed */
      if (length == 0)
      {
        if (FKG_Callbacks.OnCardRemove != NULL)
          handled = FKG_Callbacks.OnCardRemove(device_ctx->Name, tag_idx);
      } else
      {
        if (FKG_Callbacks.OnCardInsert != NULL)
          handled = FKG_Callbacks.OnCardInsert(device_ctx->Name, tag_idx, value, length);
      }
      break;

    case 0xA0 :
      /* Pin */
      if (FKG_Callbacks.OnPinEnter != NULL)
        handled = FKG_Callbacks.OnPinEnter(device_ctx->Name, tag_idx, value, length);
      break;

		case 0xC0	:
			/* Handydrummer: Input notification */
			if (FKG_Callbacks.OnInputChange != NULL)
				if (length == 1)
					handled = FKG_Callbacks.OnInputChange(device_ctx->Name, tag_idx, value[0]);

			break;

    default :
      /* ? */
      break;
  }

  if (!handled)
  {
    if (FKG_Callbacks.OnTlvReceive != NULL)
      handled = FKG_Callbacks.OnTlvReceive(device_ctx->Name, tag, tag_idx, value, length);
  }

  return handled;
}

BOOL FKG_RecvFromDevice(FKG_DEVICE_CTX_ST *device_ctx, BYTE buffer[], WORD length)
{
  WORD offset = 0;
  BOOL valid = TRUE;
  BOOL handled = FALSE;

  for (;;)
  {
    BYTE tag, tag_idx = 0;
    BYTE len;

    if (offset >= length) 
			break;
			
    tag = buffer[offset++];

    if (tag & 0x80)
    {
      if (offset >= length) 
				break;
				
      tag_idx = buffer[offset++];
    }

    if (offset >= length) 
			break;
			
    len = buffer[offset++];

    if (offset + len > length) 
			break;
			
    if (FKG_OnDeviceTLV(device_ctx, tag, tag_idx, &buffer[offset], len))
      handled = TRUE;

    offset += len;
  }

  return valid;
}
