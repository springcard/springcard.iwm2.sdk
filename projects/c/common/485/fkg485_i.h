/**h* IWM2-SDK/fkg485_i.h
 *
 * NAME
 *   fkg485_i.h
 *
 * DESCRIPTION
 *   SpringCard SDK for FunkyGate & Readers - 485 client - internal header
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
#ifndef __FKG485_I_H__
#define __FKG485_I_H__

#include "../fkg_i.h"

#ifndef WIN32
#include <pthread.h>
#endif

/**v* IWM2-SDK/fkg485_verbose
 *
 * NAME
 *   fkg485_verbose
 *
 * DESCRIPTION
 *   Set this global variable to TRUE to show processing information on the
 *   console.
 *
 * SEE ALSO
 *   FKG485_DEBUG
 *
 **/
extern BOOL fkg485_verbose;

/**d* IWM2-SDK/FKG485_DEBUG
 *
 * NAME
 *   FKG485_DEBUG
 *
 * DESCRIPTION
 *   Use this define to provide detailed debug information.
 *
 * NOTE
 *   Authentication keys may appear in the output when this define is set.
 *   Do not use in production, with sensitive keys!!!
 *
 * SEE ALSO
 *   fkg485_debug
 *
 **/
#undef FKG485_DEBUG

/**v* IWM2-SDK/fkg485_debug
 *
 * NAME
 *   fkg485_debug
 *
 * DESCRIPTION
 *   Set this global variable to TRUE to show detailed debug information on the
 *   console.
 *
 * SEE ALSO
 *   FKG485_DEBUG
 *
 **/
extern BOOL fkg485_debug;

typedef enum
{
  FKG485_STATE_UNKNOWN,
  FKG485_STATE_FOUND,
  FKG485_STATE_ACTIVE,
  FKG485_STATE_ERROR,
} FKG485_STATE_E;

typedef struct
{
	BYTE type;
	BYTE length;
	BYTE payload[IWM_PACKET_PAYLOAD_SIZE]; 
} IWM_PACKET_ST;

/**s* IWM2-SDK/FKG485_THRD_CTX_ST
 *
 * NAME
 *   FKG485_THRD_CTX_ST
 *
 * DESCRIPTION
 *   This structure is the "context" where every Reader's thread stores
 *   both is parameters and its state variables.
 *
 **/

typedef struct _FKG485_QUEUE_ST
{
  IWM_PACKET_ST packet;
  struct _FKG485_QUEUE_ST *next;
} FKG485_QUEUE_ST;

typedef struct
{
  BOOL            enumerated;
  BOOL            connected;
	BOOL            was_connected;
  BYTE            block_number;
  FKG485_QUEUE_ST *send_queue;
	FKG485_STATE_E  state;
  
  struct _FKG485_CHANNEL_ST *_channel;
  BYTE                      _addr;
  FKG_DEVICE_CTX_ST         *_device_ctx;

  struct timeval _keep_cool_until;
  
} FKG485_DEVICE_ST;

#define FKG485_MAX_DEVICES_PER_CHANNEL 32

typedef struct _FKG485_CHANNEL_ST
{
#ifdef WIN32
  HANDLE           thrd_hnd;
  DWORD            thrd_id;
#else
  pthread_t        thrd_hnd;
#endif
  
  BOOL             thrd_running;
  BOOL             terminated;

#ifdef WIN32
  HANDLE           comm_handle;
#else
  int              comm_handle;
#endif
  char             *comm_name;

#ifdef WIN32
  HANDLE           send_mutex;
  HANDLE           list_mutex;
#else
  pthread_mutex_t  send_mutex;
  pthread_mutex_t  list_mutex;
#endif

  BYTE             max_devices;
  FKG485_DEVICE_ST _devices[FKG485_MAX_DEVICES_PER_CHANNEL];

  struct _FKG485_CHANNEL_ST *next;

} FKG485_CHANNEL_ST;

typedef struct
{
  FKG485_CHANNEL_ST *channel;
  BYTE              addr;
} FKG485_DEVICE_CTX_ST;

BOOL FKG485_RecvFromDeviceEx(FKG_DEVICE_CTX_ST *device_ctx, BYTE frame[], WORD length);

BOOL FKG485_StartEx(const char *comm_name, BYTE max_devices);

#define FKG485_BUFFER_SZ 128

#ifdef WIN32
DWORD WINAPI FKG485_CommThread(void *param);
#else
void FKG485_CommThread(void *param);
#endif

FKG485_CHANNEL_ST *FKG485_FindChannel(const char *comm_name);
FKG485_DEVICE_ST *FKG485_AttachDevice(const char *comm_name, BYTE device_addr);
void FKG485_DetachDevice(FKG485_DEVICE_ST *device);

BOOL FKG485_Xfer_S_EnumEx(FKG485_CHANNEL_ST *channel, BYTE addr);
BOOL FKG485_Xfer_S_Enum(FKG485_DEVICE_ST *device);
BOOL FKG485_Xfer_I(FKG485_DEVICE_ST *device, const BYTE send_payload[], WORD send_payload_length, BYTE recv_frame[], WORD recv_frame_size, WORD *recv_frame_length);
BOOL FKG485_Xfer_I_Poll(FKG485_DEVICE_ST *device, BYTE recv_payload[], WORD recv_payload_size, WORD *recv_frame_length);

#ifdef WIN32
BOOL FKG485_OpenCommEx(HANDLE *comm_handle, const char *comm_name);
BOOL FKG485_SendEx(HANDLE comm_handle, const BYTE send_buffer[], WORD send_len);
BOOL FKG485_RecvEx(HANDLE comm_handle, BYTE recv_buffer[], WORD max_recv_len, WORD *actual_recv_len);
void FKG485_CloseCommEx(HANDLE comm_handle);

BOOL FKG485_MK1_Probe(HANDLE comm_handle);
BOOL FKG485_MK1_Command(HANDLE comm_handle, const char *command);
BOOL FKG485_MK1_Response(HANDLE comm_handle, char *response, WORD response_size);

BOOL FKG485_MK2_Probe(HANDLE comm_handle);
BOOL FKG485_MK2_Command(HANDLE comm_handle, const char *command);
BOOL FKG485_MK2_Response(HANDLE comm_handle, char *response, WORD response_size);
#else
BOOL FKG485_OpenCommEx(int *comm_handle, const char *comm_name);
BOOL FKG485_SendEx(int comm_handle, const BYTE send_buffer[], WORD send_len);
BOOL FKG485_RecvEx(int comm_handle, BYTE recv_buffer[], WORD max_recv_len, WORD *actual_recv_len);
void FKG485_CloseCommEx(int comm_handle);

BOOL FKG485_MK1_Probe(int comm_handle);
BOOL FKG485_MK1_Command(int comm_handle, const char *command);
BOOL FKG485_MK1_Response(int comm_handle, char *response, WORD response_size);

BOOL FKG485_MK2_Probe(int comm_handle);
BOOL FKG485_MK2_Command(int comm_handle, const char *command);
BOOL FKG485_MK2_Response(int comm_handle, char *response, WORD response_size);
#endif

typedef struct
{
  BOOL (*OnEnumerate) (const char *com_port_name, BYTE device_addr);
	BOOL (*OnEnumLoopEnded) (const char *com_port_name);
	BOOL (*OnILoopEnded) (const char *com_port_name);
} FKG485_CALLBACKS_ST;

extern FKG485_CALLBACKS_ST FKG485_Callbacks;

#endif
