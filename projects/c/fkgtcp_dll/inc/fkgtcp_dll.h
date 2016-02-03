/**h* FkgTCP.dll
 *
 * NAME
 *   FkgTCP.dll -- a simple DLL to drive SpringCard FunkyGate-IP NFC readers,
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
#ifndef __FKGTCP_H__
#define __FKGTCP_H__

#ifdef WIN32
  #include <winsock2.h>
  #include <windows.h>
  #ifndef FKGTCP_LIB
    #define FKGTCP_LIB __declspec( dllimport )
  #endif
	#ifndef FKGTCP_API
	  #define FKGTCP_API __cdecl
	#endif
#else
	#define FKGTCP_LIB
	#define FKGTCP_API
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

FKGTCP_LIB void FKGTCP_API FKGTCP_SetVerbose(BYTE level, const char *filename);

FKGTCP_LIB void FKGTCP_API FKGTCP_SetStatusChangeCallback(BOOL (*OnStatusChange) (const char *device_name, WORD device_status));
FKGTCP_LIB void FKGTCP_API FKGTCP_SetTlvReceiveCallback(BOOL (*OnTlvReceive) (const char *device_name, BYTE tag, BYTE tag_idx, const BYTE payload[], BYTE payload_sz));
FKGTCP_LIB void FKGTCP_API FKGTCP_SetTamperChangeCallback(BOOL (*OnTamperChange) (const char *device_name, BYTE tampers_state));
FKGTCP_LIB void FKGTCP_API FKGTCP_Rdr_SetCardReadCallback(BOOL (*OnCardRead) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz));
FKGTCP_LIB void FKGTCP_API FKGTCP_Rdr_SetCardInsertCallback(BOOL (*OnCardInsert) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz));
FKGTCP_LIB void FKGTCP_API FKGTCP_Rdr_SetCardRemoveCallback(BOOL (*OnCardRemove) (const char *device_name, BYTE head_idx));
FKGTCP_LIB void FKGTCP_API FKGTCP_Rdr_SetPinEnterCallback(BOOL (*OnPinEnter) (const char *device_name, BYTE head_idx, const BYTE pin[], BYTE pin_sz));
FKGTCP_LIB void FKGTCP_API FKGTCP_Mio_SetInputChangeCallback(BOOL (*OnInputChange) (const char *device_name, BYTE input_idx, BYTE input_state));

FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Create(const char *device_name);
FKGTCP_LIB void FKGTCP_API FKGTCP_Destroy(const char *device_name);

FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Start(const char *device_name, const char *connection_string, const BYTE auth_key[]);
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Stop(const char *device_name);
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Join(const char *device_name);

FKGTCP_LIB BOOL FKGTCP_API FKGTCP_WriteConfig(const char *device_name, BYTE addr, BYTE size, const BYTE data[]);
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_EraseConfig(const char *device_name, BYTE addr);
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_DoReset(const char *device_name);

FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Rdr_SetMode(const char *device_name, BYTE mode);

FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Rdr_SetLeds(const char *device_name, BYTE red, BYTE green);
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Rdr_SetLedsT(const char *device_name, BYTE red, BYTE green, WORD timeout_s);
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Rdr_ClearLeds(const char *device_name);

FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Rdr_Buzzer(const char *device_name, BYTE sequence);

FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Mio_ReadInputs(const char *device_name);
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Mio_SetOutput(const char *device_name, BYTE which);
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Mio_SetOutputT(const char *device_name, BYTE which, WORD timeout_s);
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Mio_ClearOutput(const char *device_name, BYTE which);

#ifdef __cplusplus
}
#endif

#endif
