#include "fkgtcp_i.h"
#include <assert.h>

BOOL FKGTCP_RecvFromDeviceEx(FKG_DEVICE_CTX_ST *device, BYTE frame[], WORD length)
{
  IWM_PACKET_ST packet;

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    int i;
    printf("%s:R>H ", device->Name);
    for (i=0; i<length; i++)
      printf("%02X", frame[i]);
    printf("\n");
  }
#endif

  if ((length < 2) || (length > sizeof(packet.payload) + 2))
  {
    FKG_Trace(TRACE_DEEP, "%s:recv:wrong length\n", device->Name);
    return FALSE;
  }

  if (frame[0] > length)
  {
    FKG_Trace(TRACE_DEEP, "%s:recv:wrong format\n", device->Name);
    return FALSE;
  } else
  if (frame[0] < length)
  {
    WORD i;
    
    for (i=0; i<length; i+=frame[i])
    {
      if (!FKGTCP_RecvFromDeviceEx(device, &frame[i], frame[i]))
        return FALSE;
    }

    return TRUE;
  }

  if (!(frame[1] & IWM2_MK2_PROTO_TYPE_SLAVE_TO_MASTER))
  {
    FKG_Trace(TRACE_DEEP, "%s:recv:wrong header\n", device->Name);
    return FALSE;
  }

  packet.type   = frame[1];
  packet.length = (BYTE) (length - 2);
  memcpy(packet.payload, &frame[2], packet.length);

  return FKGTCP_RecvFromDevice(device, &packet);
}

BOOL FKGTCP_RecvFromDevice(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet)
{
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);

  switch (packet->type & IWM2_MK2_PROTO_TYPE_MASK)
  {
#ifdef FKGTCP_SECURE
    case IWM2_MK2_PROTO_I_S_BLOCK :
      /* Secure communication - Block holding an application level payload */
      /* ----------------------------------------------------------------- */

      /* Check the state machine */
      if (client->state != FKGTCP_STATE_ACTIVE_S)
      {
        FKG_Trace(TRACE_DEEP, "%s:recv:Is-Block !Active (%d)\n", device->Name, client->state);
        return FALSE;
      }      

      /* Transform the Is-Block into an I-Block */
      if (!FKGTCP_DecipherPacket(device, packet, client->secure.chal_or_IV.IV.receive))
      {
        FKG_Trace(TRACE_DEEP, "%s:recv:Is-Block !Dk\n", device->Name);
        return FALSE;        
      }
      if (!FKGTCP_UnPaddPacket(device, packet))
      {
        FKG_Trace(TRACE_DEEP, "%s:recv:Is-Block !Padd\n", device->Name);
        return FALSE;        
      }
      if (!FKGTCP_VerifyPacketCMAC(device, packet))
      {
        FKG_Trace(TRACE_DEEP, "%s:recv:Is-Block !CMAC\n", device->Name);
        return FALSE;        
      }

      /* Process as an I-Block */
      if (!FKG_RecvFromDevice(device, packet->payload, packet->length))
      {
        FKG_Trace(TRACE_DEEP, "%s:recv:Is-Block:processing error\n", device->Name);
        return FALSE;
      }      
      break;
#endif
  
    case IWM2_MK2_PROTO_I_BLOCK :
      /* Plain communication - Block holding an application level payload */
      /* ---------------------------------------------------------------- */

      /* Check the state machine */
      if (client->state != FKGTCP_STATE_ACTIVE)
      {
        FKG_Trace(TRACE_DEEP, "%s:recv:I-Block !Active (%d)\n", device->Name, client->state);
        return FALSE;
      }

      /* Proces the I-Block */
      if (!FKG_RecvFromDevice(device, packet->payload, packet->length))
      {
        FKG_Trace(TRACE_DEEP, "%s:recv:I-Block:processing failed\n", device->Name);
        return FALSE;
      }      
      break;
    
    case IWM2_MK2_PROTO_HELO :
      /* HELO */
			if (client->state == FKGTCP_STATE_WAIT_HELO)
      {
#ifdef FKGTCP_SECURE
				if (client->secure.mode != FKGTCP_SECURE_NONE)
        {
					/* We are to perform an AES authentication */
          /* --------------------------------------- */
          
          /* Load the AES authentication key */
          if (!FKGTCP_InitNewCipher(device))
          {
            FKG_Trace(TRACE_DEEP, "%s:recv:Failed to init cipher\n", device->Name);
            return FALSE;
          }

          /* Send HELO-AUTH to the Reader */
          packet->type = IWM2_MK2_PROTO_HELO_AUTH | (client->secure.mode & 0x0F);
          packet->length = 0;                   
          client->state = FKGTCP_STATE_WAIT_AUTH_1;          
          FKGTCP_SendToDevice(device, packet);
        } else
#endif
        {
					/* Plain communication only */
          /* ------------------------ */
          
          /* Send HELO-OK to the Reader */
          packet->type = IWM2_MK2_PROTO_HELO_OK | IWM2_MK2_PROTO_VERSION;
          packet->length = 0;
          client->state = FKGTCP_STATE_ACTIVE;
          FKGTCP_SendToDevice(device, packet);

          /* Remember we are connected to a (assumed) valid Reader */
          client->was_connected = TRUE;

          /* Ask for Reader's status right after */
          FKG_AskForStatus(device);
        }
      } else
      {
        /* No HELO frame allowed at this step */
        FKG_Trace(TRACE_DEEP, "%s:recv:HELO !Expected\n", device->Name);
        return FALSE;
      }
      break;
      
#ifdef FKGTCP_SECURE      
    case IWM2_MK2_PROTO_HELO_AUTH :
      /* HELO-AUTH */
      if (client->state == FKGTCP_STATE_WAIT_AUTH_1)
      {
        /* Process Reader's "Auth 1st" frame */
        /* --------------------------------- */

        /* Check the size (must be 16) */
        if (packet->length != AES_BLOCK_SIZE)
        {
          FKG_Trace(TRACE_DEEP, "%s:recv:HELO-AUTH-1 L=%d!=%d\n", device->Name, packet->length, AES_BLOCK_SIZE);
          return FALSE;                
        }
        /* Decipher the packet */
        if (!FKGTCP_DecipherPacket(device, packet, NULL))
        {
          FKG_Trace(TRACE_DEEP, "%s:recv:HELO-AUTH-1 !Dk\n", device->Name);
          return FALSE;        
        }
        /* We've got the Reader's challenge */
        memcpy(client->secure.chal_or_IV.chal.reader, packet->payload, AES_BLOCK_SIZE);

        /* Process Host's "Auth 2nd" frame */
        /* ------------------------------- */

        packet->length = 2 * AES_BLOCK_SIZE;

        /* Create Host's challenge */
        RAND_GetBytes(client->secure.chal_or_IV.chal.host, AES_BLOCK_SIZE);
        memcpy(&packet->payload[0], client->secure.chal_or_IV.chal.host, AES_BLOCK_SIZE);

        /* Put rotated Reader's challenge */
        compute_prime(&packet->payload[AES_BLOCK_SIZE], client->secure.chal_or_IV.chal.reader, AES_BLOCK_SIZE);

        /* Cipher the 2 challenges */
        if (!FKGTCP_EncipherPacket(device, packet, NULL))
        {
          FKG_Trace(TRACE_DEEP, "%s:send:HELO-AUTH-1 !Ek\n", device->Name);
          return FALSE;        
        }
        
        /* Send "Auth 2nd" frame to the Reader, and wait for "Auth 3rd" */
        packet->type = IWM2_MK2_PROTO_HELO_AUTH;
        client->state = FKGTCP_STATE_WAIT_AUTH_3;
        FKGTCP_SendToDevice(device, packet);
              
      } else
      if (client->state == FKGTCP_STATE_WAIT_AUTH_3)      
      {
        /* Process Reader's "Auth 3rd" frame */
        /* --------------------------------- */

        BYTE buffer[AES_BLOCK_SIZE];

        /* Check the size (must be 16) */
        if (packet->length != AES_BLOCK_SIZE)
        {
          FKG_Trace(TRACE_DEEP, "%s:recv:HELO-AUTH-3 L=%d!=%d\n", device->Name, packet->length, AES_BLOCK_SIZE);
          return FALSE;                
        }
        /* Decipher the packet */
        if (!FKGTCP_DecipherPacket(device, packet, NULL))
        {
          FKG_Trace(TRACE_DEEP, "%s:recv:HELO-AUTH-3 !Dk\n", device->Name);
          return FALSE;        
        }
        /* Check we've got the rotated Host's challenge */
        compute_prime(buffer, client->secure.chal_or_IV.chal.host, AES_BLOCK_SIZE);
        if (memcmp(buffer, packet->payload, AES_BLOCK_SIZE))
        {
          FKG_Trace(TRACE_DEEP, "%s:recv:HELO-AUTH-3 Bad Dk\n", device->Name);
          return FALSE;        
        }

        /* Compute the two session keys (one for the CMAC, one for ciphering) */
        FKGTCP_ComputeSessionCMACKey(device);
        FKGTCP_ComputeSessionCipherKey(device);

        /* Init the sequence numbers and clear both Reader's and Host's IV */
        client->secure.sequence.reader = client->secure.sequence.host = 0;
        memset(client->secure.chal_or_IV.IV.receive, 0, AES_BLOCK_SIZE);
        memset(client->secure.chal_or_IV.IV.send, 0, AES_BLOCK_SIZE);

        /* Remember we are connected to a (assumed) valid Reader */
        client->was_connected = TRUE;

        /* Send HELO-OK to the Reader */
        packet->type = IWM2_MK2_PROTO_HELO_OK | IWM2_MK2_PROTO_VERSION;
        packet->length = AES_BLOCK_SIZE;

        /* The HELO-OK contains a random payload only to init the IVs with an unpredictable value */
        RAND_GetBytes(packet->payload, AES_BLOCK_SIZE);
        client->state = FKGTCP_STATE_ACTIVE_S;
        FKGTCP_SendToDevice(device, packet);

        /* Copy new Host's IV into Reader's IV */
        memcpy(client->secure.chal_or_IV.IV.receive, client->secure.chal_or_IV.IV.send, AES_BLOCK_SIZE);

        /* Ask for Reader's status right after */
        FKG_AskForStatus(device);
        
      } else
      {
        /* No HELO-AUTH frame allowed at this step */
        FKG_Trace(TRACE_DEEP, "%s:recv:HELO-AUTH !Expected", device->Name);
        return FALSE;
      }
      break;
#endif

    default :
      /* Unsupported frame type */
      FKG_Trace(TRACE_DEEP, "%s:recv:bad frame type\n", device->Name);
      return FALSE;
  }    

  return TRUE;
}

static BOOL FKGTCP_SendToDeviceEx(FKG_DEVICE_CTX_ST *device, BYTE frame[], WORD length)
{
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);

  if (frame == NULL)
    length = 0;

  if (!client->connected)
  {
    FKG_Trace(TRACE_DEEP, "%s:can't send (not connected)\n", device->Name);
    return FALSE;
  }

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    int i;
    printf("%s:R<H ", device->Name);
    for (i=0; i<length; i++)
      printf("%02X", frame[i]);
    printf("\n");
  }
#endif

  if (!NETWORK_TCPSend(client->sock, frame, length))
  {
    FKG_Trace(TRACE_DEEP, "%s:failed to send to reader\n", device->Name);
    return FALSE;
  }

  return TRUE;
}

BOOL FKGTCP_SendToDevice(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet)
{
  BYTE frame[128];
  WORD length = 0;
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);

#ifdef FKGTCP_SECURE
  if ( (client->state == FKGTCP_STATE_ACTIVE_S) &&
       ( ((packet->type & IWM2_MK2_PROTO_TYPE_MASK) == IWM2_MK2_PROTO_I_BLOCK) || ((packet->type & IWM2_MK2_PROTO_TYPE_MASK) == IWM2_MK2_PROTO_HELO_OK) ) )
  {
    /* Prepare a crypted packet */
    /* ------------------------ */

    if ((packet->type & IWM2_MK2_PROTO_TYPE_MASK) == IWM2_MK2_PROTO_I_BLOCK)
    {
      /* Is-Block instead of I-Block */
      packet->type = IWM2_MK2_PROTO_I_S_BLOCK;
    }

    /* Add CMAC */
    if (!FKGTCP_AddPacketCMAC(device, packet))
    {
      FKG_Trace(TRACE_DEEP, "%s:send:Is-Block !DoCMAC\n", device->Name);
      return FALSE;        
    }

    /* Add padding */
    if (!FKGTCP_PaddPacket(device, packet))
    {
      FKG_Trace(TRACE_DEEP, "%s:send:Is-Block !DoPadd\n", device->Name);
      return FALSE;        
    }

    /* Cipher */
    if (!FKGTCP_EncipherPacket(device, packet, client->secure.chal_or_IV.IV.send))
    {
      FKG_Trace(TRACE_DEEP, "%s:send:Is-Block !DoCipher\n", device->Name);
      return FALSE;        
    }
  }
#endif

  frame[length++] = packet->length + 2;
  frame[length++] = packet->type;
  memcpy(&frame[length], packet->payload, packet->length);
  length += packet->length;

  return FKGTCP_SendToDeviceEx(device, frame, length);
}

BOOL FKG_SendToDeviceEx(FKG_DEVICE_CTX_ST *device, BYTE buffer[], WORD length)
{
  IWM_PACKET_ST packet;

  assert(device != NULL);

  if (buffer == NULL)
    length = 0;

  packet.type = IWM2_MK2_PROTO_I_BLOCK;
  packet.length = (BYTE) length;
  if (packet.length)
    memcpy(packet.payload, buffer, packet.length);

  return FKGTCP_SendToDevice(device, &packet);
}

