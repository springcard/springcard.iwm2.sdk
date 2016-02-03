#ifndef __FKGMK2_I_H__
#define __FKGMK2_I_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib-c/utils/types.h"
#include "lib-c/utils/stri.h"
#include "lib-c/utils/strl.h"
#include "lib-c/utils/binconvert.h"
#include "lib-c/network/network.h"
#include "products/iwm2/iwm2_proto_mk2.h"

#define FKG_STATUS_CREATED            1
#define FKG_STATUS_STARTING           2
#define FKG_STATUS_STARTED            3
#define FKG_STATUS_DISCONNECTED       4
#define FKG_STATUS_CONNECTION_FAILED  5
#define FKG_STATUS_CONNECTED          6
#define FKG_STATUS_TERMINATED         7
#define FKG_STATUS_DISPOSED           8

#ifndef WIN32
#define MAX_PATH          260
#endif

typedef enum
{
  FKG_QUERY_NONE,
  FKG_QUERY_STATUS,
  FKG_QUERY_INFOS,
  FKG_QUERY_CAPACITIES,
  FKG_QUERY_SERNO,
  FKG_QUERY_READ,
  FKG_QUERY_WRITE,
  FKG_QUERY_RESET
} FKG_QUERY_E;

typedef struct
{
  char *Name;
  void *CtxData;
  FKG_QUERY_E LastQuery;
} FKG_DEVICE_CTX_ST;

typedef struct
{
  BOOL (*OnStatusChange) (const char *device_name, WORD device_status);
  BOOL (*OnTlvReceive) (const char *device_name, BYTE tag, BYTE tag_idx, const BYTE payload[], BYTE payload_sz);
  BOOL (*OnTamperChange) (const char *device_name, BYTE tampers_state);
  BOOL (*OnCardRead) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz);
  BOOL (*OnCardInsert) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz);
  BOOL (*OnCardRemove) (const char *device_name, BYTE head_idx);
  BOOL (*OnPinEnter) (const char *device_name, BYTE head_idx, const BYTE pin[], BYTE pin_sz);
  BOOL (*OnInputChange) (const char *device_name, BYTE input_idx, BYTE input_state);
} FKG_CALLBACKS_ST;

extern FKG_CALLBACKS_ST FKG_Callbacks;

FKG_DEVICE_CTX_ST *FKG_FindDevice(const char *device_name);
FKG_DEVICE_CTX_ST *FKG_DeviceFirst(void);
FKG_DEVICE_CTX_ST *FKG_DeviceNext(FKG_DEVICE_CTX_ST *current);
BOOL FKG_RemoveDevice(FKG_DEVICE_CTX_ST *device_ctx);
BOOL FKG_InsertDevice(FKG_DEVICE_CTX_ST *device_ctx);
DWORD FKG_GetDeviceCount(void);

void FKG_SetLastQuery(FKG_DEVICE_CTX_ST *device_ctx, FKG_QUERY_E query);

BOOL FKG_Mio_SetOutput(FKG_DEVICE_CTX_ST *device_ctx, BYTE which);
BOOL FKG_Mio_SetOutputT(FKG_DEVICE_CTX_ST *device_ctx, BYTE which, WORD timeout_s);
BOOL FKG_Mio_ClearOutput(FKG_DEVICE_CTX_ST *device_ctx, BYTE which);
BOOL FKG_Mio_ReadInputs(FKG_DEVICE_CTX_ST *device_ctx);

BOOL FKG_LedSequence(FKG_DEVICE_CTX_ST *device_ctx, BYTE led_red, BYTE led_green, WORD duration_s);
BOOL FKG_LedPermanent(FKG_DEVICE_CTX_ST *device_ctx, BYTE led_red, BYTE led_green);
BOOL FKG_LedDefault(FKG_DEVICE_CTX_ST *device_ctx);
BOOL FKG_Buzzer(FKG_DEVICE_CTX_ST *device_ctx, BYTE sequence);

BOOL FKG_SetRuntimeMode(FKG_DEVICE_CTX_ST *device_ctx, BYTE runtime_mode, WORD duration_s);

BOOL FKG_ReadConfig(FKG_DEVICE_CTX_ST *device_ctx, BYTE reg);

BOOL FKG_WriteConfig(FKG_DEVICE_CTX_ST *device_ctx, BYTE reg, const BYTE data[], BYTE size);
BOOL FKG_DoReset(FKG_DEVICE_CTX_ST *device_ctx);

BOOL FKG_Ping(FKG_DEVICE_CTX_ST *device_ctx);
BOOL FKG_AskForInfo(FKG_DEVICE_CTX_ST *device_ctx);
BOOL FKG_AskForCapacities(FKG_DEVICE_CTX_ST *device_ctx);
BOOL FKG_AskForSerno(FKG_DEVICE_CTX_ST *device_ctx);
BOOL FKG_AskForStatus(FKG_DEVICE_CTX_ST *device_ctx);
BOOL FKG_Keepalive(FKG_DEVICE_CTX_ST *device_ctx);

BYTE FKG_GetState(FKG_DEVICE_CTX_ST *device_ctx);
BOOL FKG_SetState(FKG_DEVICE_CTX_ST *device_ctx, BYTE new_state);

BOOL FKG_SendToDeviceEx(FKG_DEVICE_CTX_ST *device_ctx, BYTE buffer[], WORD length);
BOOL FKG_RecvFromDevice(FKG_DEVICE_CTX_ST *device_ctx, BYTE buffer[], WORD length);

#define TRACE_OFF   0
#define TRACE_MAJOR 1
#define TRACE_MINOR 2
#define TRACE_INFO  3
#define TRACE_DEEP  4
#define TRACE_DEBUG 5

void FKG_Trace(BYTE level, const char *fmt, ...);
void FKG_SetVerbose(BYTE level, const char *filename);

#define LED_OFF  0
#define LED_ON   1
#define LED_SLOW 2
#define LED_FAST 3

#define LED_G_ON       					0x03
#define LED_G_SLOW     					0x02
#define LED_G_FAST     					0x01
#define LED_G_OFF      					0x00

#define LED_G_SLOW_INV 					0x08
#define LED_G_FAST_INV 					0x04

#define LED_R_ON       					0x30
#define LED_R_SLOW     					0x20
#define LED_R_FAST     					0x10
#define LED_R_OFF      					0x00

#define LED_R_SLOW_INV 					0x80
#define LED_R_FAST_INV 					0x40

#define RUNTIME_MODE_READER_OFF       0x00
#define RUNTIME_MODE_READER_ON        0x01
#define RUNTIME_MODE_READER_WITH_PINS 0x03

/* Liaison au routeur */
extern void *queue_to_router;
extern volatile BOOL queue_connected;

extern BOOL fkg_use_old_protocol;

#endif
