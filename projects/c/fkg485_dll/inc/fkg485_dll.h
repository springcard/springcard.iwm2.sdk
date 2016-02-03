/**h* Fkg485.dll
 *
 * NAME
 *   Fkg485.dll -- a simple DLL to drive SpringCard FunkyGate-IP NFC readers,
 *                 as well as SpringCard HandyDrummer-IP I/O modules.
 *
 * COPYRIGHT
 *   Copyright (c) 2014 PRO ACTIVE SAS, FRANCE - www.springcard.com
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
 * PORTABILITY
 *   Win32
 *   Linux
 *
 **/
#ifndef __FKG485_H__
#define __FKG485_H__

#ifdef WIN32
  #include <winsock2.h>
  #include <windows.h>
  #ifndef FKG485_LIB
    #define FKG485_LIB __declspec( dllimport )
  #endif
	#ifndef FKG485_API
	  #define FKG485_API __cdecl
	#endif
#else
  #include "lib-c/utils/types.h"	
  #include <pthread.h>
  #ifndef FKG485_LIB
    #define FKG485_LIB
  #endif
  #ifndef FKG485_API
    #define FKG485_API
  #endif
#endif


#ifdef __cplusplus
extern  "C"
{
#endif

#define FKG_STATUS_CREATED            1
#define FKG_STATUS_STARTING           2
#define FKG_STATUS_STARTED            3
#define FKG_STATUS_DISCONNECTED       4
#define FKG_STATUS_CONNECTION_FAILED  5
#define FKG_STATUS_CONNECTED          6
#define FKG_STATUS_TERMINATED         7
#define FKG_STATUS_DISPOSED           8

FKG485_LIB void FKG485_API FKG485_SetEnumerateCallback(BOOL (*OnEnumerate) (const char *comm_name, BYTE device_idx));
FKG485_LIB void FKG485_API FKG485_SetEnumLoopEndedCallback(BOOL (*OnEnumLoopEnded) (const char *comm_name));
FKG485_LIB void FKG485_API FKG485_SetILoopEndedCallback(BOOL (*OnILoopEnded) (const char *comm_name));
FKG485_LIB void FKG485_API FKG485_SetLoopEndedCallback(BOOL (*OnEnumerate) (const char *comm_name));
FKG485_LIB void FKG485_API FKG485_SetStatusChangeCallback(BOOL (*OnStatusChange) (const char *device_name, WORD device_status));
FKG485_LIB void FKG485_API FKG485_SetTlvReceiveCallback(BOOL (*OnTlvReceive) (const char *device_name, BYTE tag, BYTE tag_idx, const BYTE payload[], BYTE payload_sz));
FKG485_LIB void FKG485_API FKG485_SetTamperChangeCallback(BOOL (*OnTamperChange) (const char *device_name, BYTE tampers_state));
FKG485_LIB void FKG485_API FKG485_Rdr_SetCardReadCallback(BOOL (*OnCardRead) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz));
FKG485_LIB void FKG485_API FKG485_Rdr_SetCardInsertCallback(BOOL (*OnCardInsert) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz));
FKG485_LIB void FKG485_API FKG485_Rdr_SetCardRemoveCallback(BOOL (*OnCardRemove) (const char *device_name, BYTE head_idx));
FKG485_LIB void FKG485_API FKG485_Rdr_SetPinEnterCallback(BOOL (*OnPinEnter) (const char *device_name, BYTE head_idx, const BYTE pin[], BYTE pin_sz));
FKG485_LIB void FKG485_API FKG485_Mio_SetInputChangeCallback(BOOL (*OnInputChange) (const char *device_name, BYTE input_idx, BYTE input_state));

FKG485_LIB BOOL FKG485_API FKG485_Create(const char *comm_name, BYTE device_addr, const char *device_name);
FKG485_LIB void FKG485_API FKG485_Destroy(const char *device_name);

FKG485_LIB BOOL FKG485_API FKG485_Start(const char *comm_name, BYTE max_devices);
FKG485_LIB BOOL FKG485_API FKG485_Stop(const char *comm_name);
FKG485_LIB BOOL FKG485_API FKG485_Join(const char *comm_name);

FKG485_LIB BOOL FKG485_API FKG485_AskForStatus(const char *device_name);
FKG485_LIB BOOL FKG485_API FKG485_ReadConfig(const char *device_name, BYTE reg);
FKG485_LIB BOOL FKG485_API FKG485_WriteConfig(const char *device_name, BYTE reg, const BYTE data[], BYTE size);
FKG485_LIB BOOL FKG485_API FKG485_EraseConfig(const char *device_name, BYTE reg);
FKG485_LIB BOOL FKG485_API FKG485_DoReset(const char *device_name);

FKG485_LIB BOOL FKG485_API FKG485_Rdr_SetMode(const char *device_name, BYTE mode);

FKG485_LIB BOOL FKG485_API FKG485_Rdr_SetLeds(const char *device_name, BYTE red, BYTE green);
FKG485_LIB BOOL FKG485_API FKG485_Rdr_SetLedsT(const char *device_name, BYTE red, BYTE green, WORD timeout_s);
FKG485_LIB BOOL FKG485_API FKG485_Rdr_ClearLeds(const char *device_name);

FKG485_LIB BOOL FKG485_API FKG485_Rdr_Buzzer(const char *device_name, BYTE sequence);

FKG485_LIB BOOL FKG485_API FKG485_Mio_ReadInputs(const char *device_name);
FKG485_LIB BOOL FKG485_API FKG485_Mio_SetOutput(const char *device_name, BYTE which);
FKG485_LIB BOOL FKG485_API FKG485_Mio_SetOutputT(const char *device_name, BYTE which, WORD timeout_s);
FKG485_LIB BOOL FKG485_API FKG485_Mio_ClearOutput(const char *device_name, BYTE which);

#ifdef __cplusplus
}
#endif

#endif
