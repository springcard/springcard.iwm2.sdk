/**h* IWM2-SDK/fkgtcp_i.h
 *
 * NAME
 *   fkgtcp_i.h
 *
 * DESCRIPTION
 *   SpringCard SDK for FunkyGate & Readers - TCP client - internal header
 *
 * COPYRIGHT
 *   Copyright (c) 2013-2014 PRO ACTIVE SAS, FRANCE - www.springcard.com
 *
 * NOTES
 *   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
 *   ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
 *   TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 *   PARTICULAR PURPOSE.
 *
 * AUTHOR
 *   Johann.D / SpringCard
 *
 **/
#ifndef __FKGTCP_I_H__
#define __FKGTCP_I_H__

#define FKGTCP_DEBUG

#include "../fkg_i.h"

#include "lib-c/crypt/block/blowfish.h"
#include "lib-c/crypt/block/des.h"
#include "lib-c/crypt/block/aes.h"
#include "lib-c/crypt/utils/random.h"

/**d* IWM2-SDK/FKGTCP_DEFAULT_PORT
 *
 * NAME
 *   FKGTCP_DEFAULT_PORT
 *
 * DESCRIPTION
 *   The default TCP port the readers are listening on
 *
 **/
#define FKGTCP_DEFAULT_PORT 3999

/**d* IWM2-SDK/FKGTCP_SECURE
 *
 * NAME
 *   FKGTCP_SECURE
 *
 * DESCRIPTION
 *   This define must be set to enable AES-protected authentication and ciphering.
 *   Undef if you don't want to use AES ciphering while communicating with the readers.
 *
 **/
#define FKGTCP_SECURE

/**v* IWM2-SDK/fkgtcp_verbose
 *
 * NAME
 *   fkgtcp_verbose
 *
 * DESCRIPTION
 *   Set this global variable to TRUE to show processing information on the
 *   console.
 *
 * SEE ALSO
 *   FKGTCP_DEBUG
 *
 **/
extern BOOL fkgtcp_verbose;

/**d* IWM2-SDK/FKGTCP_DEBUG
 *
 * NAME
 *   FKGTCP_DEBUG
 *
 * DESCRIPTION
 *   Use this define to provide detailed debug information.
 *
 * NOTE
 *   Authentication keys may appear in the output when this define is set.
 *   Do not use in production, with sensitive keys!!!
 *
 * SEE ALSO
 *   fkgtcp_debug
 *
 **/
#undef FKGTCP_DEBUG


/**v* IWM2-SDK/fkgtcp_debug
 *
 * NAME
 *   fkgtcp_debug
 *
 * DESCRIPTION
 *   Set this global variable to TRUE to show detailed debug information on the
 *   console.
 *
 * SEE ALSO
 *   FKGTCP_DEBUG
 *
 **/
extern BOOL fkgtcp_debug;

/**t* IWM2-SDK/FKGTCP_SECURE_MODE_E
 *
 * NAME
 *   FKGTCP_SECURE_MODE_E
 *
 * DESCRIPTION
 *   This enumeration chooses the communication mode between the Host and the Readers.
 *   Valid values are
 *   - FKGTCP_SECURE_NONE : no authentication, plain communication mode
 *   - FKGTCP_SECURE_USER_AES : using AES, "user" key
 *   - FKGTCP_SECURE_ADMIN_AES : using AES, "admin" key
 *
 **/
typedef enum
{
  FKGTCP_SECURE_NONE,
  FKGTCP_SECURE_USER_AES,
  FKGTCP_SECURE_ADMIN_AES,
  FKGTCP_SECURE_MAX
} FKGTCP_SECURE_MODE_E;

/**t* IWM2-SDK/FKGTCP_SECURE_MODE_E
 *
 * NAME
 *   FKGTCP_SECURE_MODE_E
 *
 * DESCRIPTION
 *   This enumeration stores the internal status of the Host/Reader's state machine.
 *   Valid values are
 *   - FKGTCP_STATE_UNKNOWN : not initialized
 *   - FKGTCP_STATE_WAIT_HELO : the Host is waiting for Reader's HELO
 *   - FKGTCP_STATE_WAIT_AUTH_1 : the Host has just sent HELO-AUTH, and is now waiting for the 1st Auth frame from the Reader
 *   - FKGTCP_STATE_WAIT_AUTH_3 : the Host has just sent the 2nd Auth frame, and is now waiting for the 3rd Auth frame from the Reader
 *   - FKGTCP_STATE_ACTIVE : Host/Reader channel is open, no authentication performed (plain communication mode)
 *   - FKGTCP_STATE_ACTIVE_S : Host/Reader channel is open, communication is secured using AES
 *   - FKGTCP_STATE_ERROR : error in the channel, the Host must drop the link
 *
 **/
typedef enum
{
  FKGTCP_STATE_UNKNOWN,
  FKGTCP_STATE_WAIT_HELO,
  FKGTCP_STATE_WAIT_AUTH_1,
  FKGTCP_STATE_WAIT_AUTH_3,
  FKGTCP_STATE_ACTIVE,
  FKGTCP_STATE_ACTIVE_S,
  FKGTCP_STATE_ERROR,
} FKGTCP_STATE_E;

/**s* IWM2-SDK/FKGTCP_THRD_CTX_ST
 *
 * NAME
 *   FKGTCP_THRD_CTX_ST
 *
 * DESCRIPTION
 *   This structure is the "context" where every Reader's thread stores
 *   both is parameters and its state variables.
 *
 **/
typedef struct
{
  HANDLE         thrd_hnd;
  DWORD          thrd_id;
  BOOL           thrd_running;
  BOOL           terminated;

	BYTE           addr[4];
	WORD           port;
	SOCKET         sock;
	BOOL           connected;
	BOOL           was_connected;

	FKGTCP_STATE_E state;

#ifdef FKGTCP_SECURE

  struct
  {
    FKGTCP_SECURE_MODE_E mode;
    BYTE auth_key[16];

    struct
    {
      DWORD reader;
      DWORD host;
    } sequence;

    union
    {
      struct
      {
        BYTE  reader[16];
        BYTE  host[16];
      } chal;
      struct
      {
        BYTE  send[16];
        BYTE  receive[16];
      } IV;
    } chal_or_IV;

    AES_CTX_ST    aes_encipher_ctx;
    AES_CTX_ST    aes_decipher_ctx;
    AES_CTX_ST    aes_cmac_ctx;

  } secure;

#endif

} FKGTCP_CLIENT_CTX_ST;


BOOL FKGTCP_RecvFromDeviceEx(FKG_DEVICE_CTX_ST *device, BYTE frame[], WORD length);

typedef struct
{
	BYTE type;
	BYTE length;
	BYTE payload[4 + IWM_PACKET_PAYLOAD_SIZE + 4 + ((4 + IWM_PACKET_PAYLOAD_SIZE + 4) % 16)]; 
} IWM_PACKET_ST;

BOOL FKGTCP_StartEx_Auth(FKG_DEVICE_CTX_ST *device, const char *connection_string, const BYTE auth_key[]);
BOOL FKGTCP_StartEx_Plain(FKG_DEVICE_CTX_ST *device, const char *connection_string);

BOOL FKGTCP_RecvFromDevice(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet);
BOOL FKGTCP_SendToDevice(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet);

BOOL FKGTCP_ComputeSessionCMACKey(FKG_DEVICE_CTX_ST *device);
BOOL FKGTCP_ComputeSessionCipherKey(FKG_DEVICE_CTX_ST *device);

BOOL FKGTCP_DecipherPacket(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet, BYTE IV[AES_BLOCK_SIZE]);
BOOL FKGTCP_EncipherPacket(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet, BYTE IV[AES_BLOCK_SIZE]);

BOOL FKGTCP_PaddPacket(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet);
BOOL FKGTCP_UnPaddPacket(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet);

BOOL FKGTCP_AddPacketCMAC(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet);
BOOL FKGTCP_VerifyPacketCMAC(FKG_DEVICE_CTX_ST *device, IWM_PACKET_ST *packet);

void compute_prime(BYTE dst[], BYTE src[], BYTE length);

BOOL FKGTCP_InitNewCipher(FKG_DEVICE_CTX_ST *device);
BOOL FKGTCP_InitSessionCipher(FKG_DEVICE_CTX_ST *device);

BOOL FKGTCP_CliOpen(SOCKET *sock, BYTE fkg_host[4], WORD fkg_port);

#define FKGTCP_BUFFER_SZ 128

DWORD WINAPI FKGTCP_ClientThread(void *param);

#endif
