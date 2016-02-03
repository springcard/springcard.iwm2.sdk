#include "fkgtcp_i.h"

#ifdef FKGTCP_SECURE

#include <assert.h>

/**f* IWM2-SDK/FKGTCP_InitNewCipher
 *
 * NAME
 *   FKGTCP_InitNewCipher
 *
 * DESCRIPTION
 *   Initialize both Cipher and Decipher contexts we the authentication key
 *
 * SEE ALSO
 *   FKGTCP_EncipherPacket
 *   FKGTCP_DecipherPacket
 *
 **/
BOOL FKGTCP_InitNewCipher(FKG_DEVICE_CTX_ST *device)
{
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    int i;
    printf("AUTH key=");
    for (i=0; i<16; i++)
      printf("%02X", client->secure.auth_key[i]);
    printf("\n");
  }
#endif

  switch (client->secure.mode)
  {
    case FKGTCP_SECURE_USER_AES :
    case FKGTCP_SECURE_ADMIN_AES :
      AES_Init(&client->secure.aes_encipher_ctx, client->secure.auth_key, 128, TRUE);
      AES_Init(&client->secure.aes_decipher_ctx, client->secure.auth_key, 128, FALSE);
      break;
    
    default :
      return FALSE;
  }
      
  return TRUE;
}

/**f* IWM2-SDK/FKGTCP_ComputeSessionCMACKey
 *
 * NAME
 *   FKGTCP_ComputeSessionCMACKey
 *
 * DESCRIPTION
 *   Compute the Session's CMAC Key (from both Reader's and Host's challenges)
 *   and init the AES context for CMAC
 *
 * SEE ALSO
 *   FKGTCP_ComputeSessionCipherKey
 *   FKGTCP_AddPacketCMAC
 *   FKGTCP_VerifyPacketCMAC
 *
 **/
BOOL FKGTCP_ComputeSessionCMACKey(FKG_DEVICE_CTX_ST *device)
{
  BYTE buffer[16];
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);

	buffer[0]  = client->secure.chal_or_IV.chal.host[7];
	buffer[1]  = client->secure.chal_or_IV.chal.host[8];
	buffer[2]  = client->secure.chal_or_IV.chal.host[9];
	buffer[3]  = client->secure.chal_or_IV.chal.host[10];
	buffer[4]  = client->secure.chal_or_IV.chal.host[11];

	buffer[5]  = client->secure.chal_or_IV.chal.reader[7];
	buffer[6]  = client->secure.chal_or_IV.chal.reader[8];
	buffer[7]  = client->secure.chal_or_IV.chal.reader[9];
	buffer[8]  = client->secure.chal_or_IV.chal.reader[10];
	buffer[9]  = client->secure.chal_or_IV.chal.reader[11];

	buffer[10] = client->secure.chal_or_IV.chal.host[0] ^ client->secure.chal_or_IV.chal.reader[0];
	buffer[11] = client->secure.chal_or_IV.chal.host[1] ^ client->secure.chal_or_IV.chal.reader[1];
	buffer[12] = client->secure.chal_or_IV.chal.host[2] ^ client->secure.chal_or_IV.chal.reader[2];
	buffer[13] = client->secure.chal_or_IV.chal.host[3] ^ client->secure.chal_or_IV.chal.reader[3];
	buffer[14] = client->secure.chal_or_IV.chal.host[4] ^ client->secure.chal_or_IV.chal.reader[4];

	buffer[15] = 0x22;

  AES_Encrypt(&client->secure.aes_encipher_ctx, buffer);

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    int i;
    printf("CMAC key=");
    for (i=0; i<16; i++)
      printf("%02X", buffer[i]);
    printf("\n");
  }
#endif

  AES_Init(&client->secure.aes_cmac_ctx, buffer, 128, TRUE);

  return TRUE;
}

/**f* IWM2-SDK/FKGTCP_ComputeSessionCipherKey
 *
 * NAME
 *   FKGTCP_ComputeSessionCipherKey
 *
 * DESCRIPTION
 *   Compute the Session's Cipher Key (from both Reader's and Host's challenges)
 *   and init the AES contexts for ciphering and deciphering.
 *
 * SEE ALSO
 *   FKGTCP_ComputeSessionCMACKey
 *   FKGTCP_EncipherPacket
 *   FKGTCP_DecipherPacket
 *
 **/
BOOL FKGTCP_ComputeSessionCipherKey(FKG_DEVICE_CTX_ST *device)
{
  BYTE buffer[16];
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);

	buffer[0]  = client->secure.chal_or_IV.chal.host[11];
	buffer[1]  = client->secure.chal_or_IV.chal.host[12];
	buffer[2]  = client->secure.chal_or_IV.chal.host[13];
	buffer[3]  = client->secure.chal_or_IV.chal.host[14];
	buffer[4]  = client->secure.chal_or_IV.chal.host[15];

	buffer[5]  = client->secure.chal_or_IV.chal.reader[11];
	buffer[6]  = client->secure.chal_or_IV.chal.reader[12];
	buffer[7]  = client->secure.chal_or_IV.chal.reader[13];
	buffer[8]  = client->secure.chal_or_IV.chal.reader[14];
	buffer[9]  = client->secure.chal_or_IV.chal.reader[15];

	buffer[10] = client->secure.chal_or_IV.chal.host[4] ^ client->secure.chal_or_IV.chal.reader[4];
	buffer[11] = client->secure.chal_or_IV.chal.host[5] ^ client->secure.chal_or_IV.chal.reader[5];
	buffer[12] = client->secure.chal_or_IV.chal.host[6] ^ client->secure.chal_or_IV.chal.reader[6];
	buffer[13] = client->secure.chal_or_IV.chal.host[7] ^ client->secure.chal_or_IV.chal.reader[7];
	buffer[14] = client->secure.chal_or_IV.chal.host[8] ^ client->secure.chal_or_IV.chal.reader[8];

  buffer[15] = 0x11;
  AES_Encrypt(&client->secure.aes_encipher_ctx, buffer);

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    int i;
    printf("Cipher key=");
    for (i=0; i<16; i++)
      printf("%02X", buffer[i]);
    printf("\n");
  }
#endif

  AES_Init(&client->secure.aes_encipher_ctx, buffer, 128, TRUE);
  AES_Init(&client->secure.aes_decipher_ctx, buffer, 128, FALSE);

  return TRUE;
}

/**f* IWM2-SDK/FKGTCP_DecipherPacket
 *
 * NAME
 *   FKGTCP_DecipherPacket
 *
 * DESCRIPTION
 *   Decipher a packet
 *
 * NOTE
 *   The packet's length must be a multiple of the AES block's length
 *
 * SEE ALSO
 *   FKGTCP_EncipherPacket
 *
 **/
BOOL FKGTCP_DecipherPacket(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet, BYTE IV[AES_BLOCK_SIZE])
{
  WORD i, j;
  BYTE buffer[AES_BLOCK_SIZE], dummy_iv[AES_BLOCK_SIZE];
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;
  assert(client != NULL);
  assert(packet != NULL);
  assert((packet->length % AES_BLOCK_SIZE) == 0);

  if (IV == NULL)
  {
    memset(dummy_iv, 0x00, AES_BLOCK_SIZE);      /* B  <- 00...00   */
    IV = dummy_iv;
  }
  memcpy(buffer, IV, AES_BLOCK_SIZE);

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    printf("Dk(");
    for (i=0; i<packet->length; i++)
      printf("%02X", packet->payload[i]);
    printf(", IV=");
    for (i=0; i<AES_BLOCK_SIZE; i++)
      printf("%02X", IV[i]);
    printf(") -> ");
  }
#endif

  for (i=0; i<packet->length; i+=AES_BLOCK_SIZE)
  {
    memcpy(IV, buffer, AES_BLOCK_SIZE);   /* IV <- B         */
    memcpy(buffer, &packet->payload[i], AES_BLOCK_SIZE);  /* B  <- P         */
    AES_Decrypt(&client->secure.aes_decipher_ctx, &packet->payload[i]);
    for (j=0; j<AES_BLOCK_SIZE; j++)
      packet->payload[i + j] ^= IV[j];  /* P  <- P XOR IV  */
  }

  memcpy(IV, buffer, AES_BLOCK_SIZE);   /* IV <- B         */

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    for (i=0; i<packet->length; i++)
      printf("%02X", packet->payload[i]);
    printf(", IV=");
    for (i=0; i<AES_BLOCK_SIZE; i++)
      printf("%02X", IV[i]);
    printf("\n");
  }
#endif

  return TRUE;
}

/**f* IWM2-SDK/FKGTCP_EncipherPacket
 *
 * NAME
 *   FKGTCP_EncipherPacket
 *
 * DESCRIPTION
 *   Decipher a packet
 *
 * NOTE
 *   The packet's length must be a multiple of the AES block's length
 *
 * SEE ALSO
 *   FKGTCP_DecipherPacket
 *
 **/
BOOL FKGTCP_EncipherPacket(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet, BYTE IV[AES_BLOCK_SIZE])
{
  WORD i, j;
  BYTE dummy_iv[AES_BLOCK_SIZE];
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);
  assert(packet != NULL);
  assert((packet->length % AES_BLOCK_SIZE) == 0);

  if (IV == NULL)
  {
    memset(dummy_iv, 0x00, AES_BLOCK_SIZE);      /* B  <- 00...00   */
    IV = dummy_iv;
  }

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    printf("Ek(");
    for (i=0; i<packet->length; i++)
      printf("%02X", packet->payload[i]);
    printf(", IV=");
    for (i=0; i<AES_BLOCK_SIZE; i++)
      printf("%02X", IV[i]);
    printf(") -> ");
  }
#endif

  for (i=0; i<packet->length; i+=AES_BLOCK_SIZE)
  {
    for (j=0; j<AES_BLOCK_SIZE; j++)
      packet->payload[i + j] ^= IV[j];  /* P  <- P XOR IV  */
    memcpy(IV, &packet->payload[i], AES_BLOCK_SIZE);
    AES_Encrypt(&client->secure.aes_encipher_ctx, IV);
    memcpy(&packet->payload[i], IV, AES_BLOCK_SIZE); /* P  <- IV        */
  }

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    for (i=0; i<packet->length; i++)
      printf("%02X", packet->payload[i]);
    printf(", IV=");
    for (i=0; i<AES_BLOCK_SIZE; i++)
      printf("%02X", IV[i]);
    printf("\n");
  }
#endif

  return TRUE;
}

/**f* IWM2-SDK/FKGTCP_PaddPacket
 *
 * NAME
 *   FKGTCP_PaddPacket
 *
 * DESCRIPTION
 *   Padd a packet
 *
 * NOTE
 *   The aim of this function is to ensure that the packet's length is a multiple of the AES block's length
 *
 * SEE ALSO
 *   FKGTCP_UnPaddPacket
 *   FKGTCP_EncipherPacket
 *
 **/
BOOL FKGTCP_PaddPacket(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet)
{
  BYTE i, p;
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);
  assert(packet != NULL);

  p = AES_BLOCK_SIZE - packet->length % AES_BLOCK_SIZE;
  if (p == 0)
    p = AES_BLOCK_SIZE;

  for (i=0; i<p; i++)
    packet->payload[packet->length++] = p;

  assert((packet->length % AES_BLOCK_SIZE) == 0);

  return TRUE;
}

/**f* IWM2-SDK/FKGTCP_UnPaddPacket
 *
 * NAME
 *   FKGTCP_UnPaddPacket
 *
 * DESCRIPTION
 *   Suppress the packet's padding (and check its validity)
 *
 * NOTE
 *   The aim of this function is to retrieve the initial packet's length after enciphering->deciphering
 *
 * SEE ALSO
 *   FKGTCP_PaddPacket
 *   FKGTCP_DecipherPacket
 *
 **/
BOOL FKGTCP_UnPaddPacket(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet)
{
  BYTE p, i;  
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);
  assert(packet != NULL);
  assert((packet->length % AES_BLOCK_SIZE) == 0);

  if (packet->length < AES_BLOCK_SIZE)
    return FALSE;
  
  p = packet->payload[packet->length - 1];
  if ((p == 0) || (p > AES_BLOCK_SIZE))
    return FALSE;
  
  for (i=1; i<=p; i++)
    if (packet->payload[packet->length - i] != p)
      return FALSE;
    
  packet->length -= p;
  return TRUE;
}

static BOOL compute_cmac(BYTE cmac[AES_BLOCK_SIZE / 2], IWM_PACKET_ST *packet, DWORD sequence, AES_CTX_ST *cmac_ctx)
{
  BYTE  vector[AES_BLOCK_SIZE];
  BYTE  block[AES_BLOCK_SIZE];
  DWORD dw, i;
  DWORD offset_block, offset_payload;

  assert(cmac != NULL);
  assert(packet != NULL);
  assert(cmac_ctx != NULL);

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    printf("CMAC(%08lX,%02X,%02X,", sequence, packet->type, packet->length);
    for (i=0; i<packet->length; i++)
      printf("%02X", packet->payload[i]);
    printf(") -> ");
  }
#endif

  memset(vector, 0, AES_BLOCK_SIZE);

  dw = sequence;

  offset_block = 0;

  for (i=0; i<4; i++)
  {
	  block[4-1-i] = (BYTE) (dw & 0x000000FF); 
		dw >>= 8;
	} 							/* 11/08/2014 - JIZ : add '{', '}' + reverse order: MSB	*/
	offset_block = 4;
	
  block[offset_block++] = packet->type;
  block[offset_block++] = packet->length;

  block[offset_block++] = 0xFF ^ packet->type;
  block[offset_block++] = 0xFF ^ packet->length;

  for (offset_payload=0; offset_payload<packet->length; offset_payload++)
  {
    block[offset_block++] = packet->payload[offset_payload];
    if (offset_block >= AES_BLOCK_SIZE)
    {
      /* We have a complete block to process */
      offset_block = 0;

      for (i=0; i<AES_BLOCK_SIZE; i++)
        block[i] ^= vector[i];  /* P  <- P XOR IV  */
      memcpy(vector, block, AES_BLOCK_SIZE);
      AES_Encrypt(cmac_ctx, vector);
      memcpy(block, vector, AES_BLOCK_SIZE); /* P  <- IV        */
    }
  }

  if ((offset_block != 0) && (offset_block < AES_BLOCK_SIZE)) /* 11/08/2014 - JIZ : change (AES_BLOCK_SIZE - 1) to AES_BLOCK_SIZE	*/
  {
    /* We have an incomplete block to process */
    block[offset_block++] = 0x80;
    while (offset_block < AES_BLOCK_SIZE)
      block[offset_block++] = 0x00;

    for (i=0; i<AES_BLOCK_SIZE; i++)
      block[i] ^= vector[i];  /* P  <- P XOR IV  */
    memcpy(vector, block, AES_BLOCK_SIZE);
    AES_Encrypt(cmac_ctx, vector);
    memcpy(block, vector, AES_BLOCK_SIZE); /* P  <- IV        */
  }

#ifdef FKGTCP_DEBUG
  if (fkgtcp_debug)
  {
    for (i=0; i<sizeof(block); i++)
      printf("%02X", block[i]);
    printf("\n");
  }
#endif

  cmac[0] = block[0];
  cmac[1] = block[2];
  cmac[2] = block[4];
  cmac[3] = block[6];
  cmac[4] = block[8];
  cmac[5] = block[10];
  cmac[6] = block[12];
  cmac[7] = block[14];

  return TRUE;
}

/**f* IWM2-SDK/FKGTCP_AddPacketCMAC
 *
 * NAME
 *   FKGTCP_AddPacketCMAC
 *
 * DESCRIPTION
 *   Compute the packet's CMAC, and append it at the end of the packet
 *
 * SEE ALSO
 *   FKGTCP_VerifyPacketCMAC
 *
 **/
BOOL FKGTCP_AddPacketCMAC(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet)
{
  BYTE cmac[AES_BLOCK_SIZE / 2];
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);
  assert(packet != NULL);

  if (!compute_cmac(cmac, packet, client->secure.sequence.host++, &client->secure.aes_cmac_ctx))
    return FALSE;

  memcpy(&packet->payload[packet->length], cmac, sizeof(cmac));
  packet->length += sizeof(cmac);

  return TRUE;
}

/**f* IWM2-SDK/FKGTCP_VerifyPacketCMAC
 *
 * NAME
 *   FKGTCP_VerifyPacketCMAC
 *
 * DESCRIPTION
 *   Compute the packet's CMAC, and verify a valid CMAC is present at the end of the packet
 *
 * SEE ALSO
 *   FKGTCP_AddPacketCMAC
 *
 **/
BOOL FKGTCP_VerifyPacketCMAC(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet)
{
  BYTE cmac[AES_BLOCK_SIZE / 2];
  FKGTCP_CLIENT_CTX_ST *client;
  
  assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);
  assert(packet != NULL);
  
  if (packet->length < sizeof(cmac))
    return FALSE;

  packet->length -= sizeof(cmac);

  if (!compute_cmac(cmac, packet, client->secure.sequence.reader++, &client->secure.aes_cmac_ctx))
    return FALSE;

  if (memcmp(&packet->payload[packet->length], cmac, sizeof(cmac)))
    return FALSE;

  return TRUE;
}




void compute_prime(BYTE dst[], BYTE src[], BYTE length)
{        
  BYTE i;

  assert(dst != NULL);
  assert(src != NULL);

  for (i=0; i<length; i++)
  {        
    dst[i] = src[i] << 1;
    if (i == (length-1))
    {
      if (src[0] & 0x80)
        dst[i] |= 0x01;    
    } else
    {
      if (src[i+1] & 0x80)
        dst[i] |= 0x01;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
//	this function calculates a 32 bit CRC value (Ethernet AAL5 standard )
//	The generator polynome is: x32 + x26 + x23 + x22 + x16 + x12 + x11 + x10 + x8 + x7 + x5 + x4 + x2 + x1 + 1
//INPUT:
//      unsigned char bInput.............the 8 bit value is added to the 32 bit CRC value (peReg)
//OUTPUT:
//      unsigned long *peReg............pointer to the 32 bit CRC value

static void update_crc32(BYTE ch, DWORD *pdwCrc)
{
  BYTE b;

  *pdwCrc ^= ch;

  //	bit wise calcualtion of the CRC value
  for (b=0; b<8; b++)
  {
    if (*pdwCrc & 0x00000001)
    {
      *pdwCrc >>= 1;
      *pdwCrc  ^= 0xEDB88320;
    } else
    {
      *pdwCrc >>= 1;
    }
  }
}

static DWORD compute_crc32(BYTE buffer[], WORD length)
{
  DWORD c = 0xFFFFFFFF;
  BYTE *p = buffer;

  if ((length != 0) && (buffer != NULL))
  {
    do
    {
      update_crc32(*p++, &c);
    } while (--length);
  }

  return c;
}

#endif
