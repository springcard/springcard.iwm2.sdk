/**** FkgTCP.dll/callbacks.c
 *
 * NAME
 *   FkgTCP.dll -- Callbacks
 *
 * DESCRIPTION
 *   Manage the DLL's callbacks
 *
 **/
#include "fkgtcp_dll_i.h"

FKG_CALLBACKS_ST FKG_Callbacks;

/**f* FkgTCP.dll/FKGTCP_SetStatusChangeCallback
 *
 * NAME
 *   FKGTCP_SetStatusChangeCallback
 *
 * DESCRIPTION
 *  Declare a callback function that will be called whenever the communication channel
 *  with a device (FunkyGate reader or HandyDrummer I/O module) is opened or closed.
 *
 * SYNOPSIS
 *   FKGTCP_SetStatusChangeCallback(BOOL (*cb) (const char *device_name, WORD device_status));
 *
 * NOTES
 *
 *   The callback receives 2 parameters:
 *
 *   - const char *device_name : the device's "friendly-name"
 *   - WORD device_status : the device's status; could be FKG_STATUS_CREATED, FKG_STATUS_STARTING,
 *                          FKG_STATUS_STARTED, FKG_STATUS_DISCONNECTED, FKG_STATUS_CONNECTION_FAILED,
 *                          FKG_STATUS_CONNECTED, FKG_STATUS_TERMINATED, FKG_STATUS_DISPOSED
 *
 *   The callback SHALL return TRUE. Returning FALSE will shutdown the communication channel and
 *   terminate the thread.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 **/
FKGTCP_LIB void FKGTCP_API FKGTCP_SetStatusChangeCallback(BOOL (*cb) (const char *device_name, WORD device_status))
{
  FKG_Callbacks.OnStatusChange = cb;
}

/**f* FkgTCP.dll/FKGTCP_SetStatusChangeCallback
 *
 * NAME
 *   FKGTCP_SetTamperChangeCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when the device's tampers are broken (or restored)
 *
 * SYNOPSIS
 *   FKGTCP_SetTamperChangeCallback(BOOL (*cb) (const char *device_name, BYTE tampers_state));
 *
 * NOTES
 *
 *   The callback receives 2 parameters:
 *
 *   - const char *device_name : the device's "friendly-name"
 *   - BYTE tampers_state : the status of the device's tampers
 *
 *   The callback SHALL return TRUE. If it returns FALSE, the default TLV callback (registered
 *   using FKGTCP_SetTlvReceiveCallback) will be called afterwards.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 **/
FKGTCP_LIB void FKGTCP_API FKGTCP_SetTamperChangeCallback(BOOL (*cb) (const char *device_name, BYTE tampers_state))
{
  FKG_Callbacks.OnTamperChange = cb;
}

/**f* FkgTCP.dll/FKGTCP_Rdr_SetCardReadCallback
 *
 * NAME
 *   FKGTCP_Rdr_SetCardReadCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when a reader device (FunkyGate) has read a card
 *
 * SYNOPSIS
 *   FKGTCP_Rdr_SetCardReadCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz));
 *
 * NOTES
 *
 *   The callback receives 4 parameters:
 *
 *   - const char *device_name : the device's "friendly-name"
 *   - BYTE head_idx : the internal index of the reader. Always 0 for current version
 *   - const BYTE card_id[] : the card's ID
 *   - BYTE card_id_sz : the size of the ID
 *
 *   The callback SHALL return TRUE. If it returns FALSE, the default TLV callback (registered
 *   using FKGTCP_SetTlvReceiveCallback) will be called afterwards.
 *
 *   If the reader is configured for insert/remove mode, this callback will never be invoked.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 * SEE ALSO
 *   FKGTCP_Rdr_SetCardInsertCallback
 *   FKGTCP_Rdr_SetCardRemoveCallback
 *
 **/
FKGTCP_LIB void FKGTCP_API FKGTCP_Rdr_SetCardReadCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz))
{
  FKG_Callbacks.OnCardRead = cb;
}

/**f* FkgTCP.dll/FKGTCP_Rdr_SetCardInsertCallback
 *
 * NAME
 *   FKGTCP_Rdr_SetCardReadCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when a reader device (FunkyGate) has read a card
 *  and the reader is configured to monitor insert/remove moves.
 *
 * SYNOPSIS
 *   FKGTCP_Rdr_SetCardInsertCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz));
 *
 * NOTES
 *
 *   The callback receives 4 parameters:
 *
 *   - const char *device_name : the device's "friendly-name"
 *   - BYTE head_idx : the internal index of the reader. Always 0 for current version
 *   - const BYTE card_id[] : the card's ID
 *   - BYTE card_id_sz : the size of the ID
 *
 *   The callback SHALL return TRUE. If it returns FALSE, the default TLV callback (registered
 *   using FKGTCP_SetTlvReceiveCallback) will be called afterwards.
 *
 *   If the reader is not configured for insert/remove mode, this callback will never be invoked.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 * SEE ALSO
 *   FKGTCP_Rdr_SetCardRemoveCallback
 *   FKGTCP_Rdr_SetCardReadCallback
 *
 **/
FKGTCP_LIB void FKGTCP_API FKGTCP_Rdr_SetCardInsertCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz))
{
  FKG_Callbacks.OnCardInsert = cb;
}

/**f* FkgTCP.dll/FKGTCP_Rdr_SetCardRemoveCallback
 *
 * NAME
 *   FKGTCP_Rdr_SetCardRemoveCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when a reader device (FunkyGate) detects the
 *  card removal (only available when the reader is configured to monitor insert/remove moves).
 *
 * SYNOPSIS
 *   FKGTCP_Rdr_SetCardRemoveCallback(BOOL (*cb) (const char *device_name, BYTE head_idx));
 *
 * NOTES
 *
 *   The callback receives 4 parameters:
 *
 *   - const char *device_name : the device's "friendly-name"
 *   - BYTE head_idx : the internal index of the reader. Always 0 for current version
 *
 *   The callback SHALL return TRUE. If it returns FALSE, the default TLV callback (registered
 *   using FKGTCP_SetTlvReceiveCallback) will be called afterwards.
 *
 *   If the reader is not configured for insert/remove mode, this callback will never be invoked.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 * SEE ALSO
 *   FKGTCP_Rdr_SetCardInsertCallback
 *
 **/
FKGTCP_LIB void FKGTCP_API FKGTCP_Rdr_SetCardRemoveCallback(BOOL (*cb) (const char *device_name, BYTE head_idx))
{
  FKG_Callbacks.OnCardRemove = cb;
}

/**f* FkgTCP.dll/FKGTCP_Rdr_SetPinEnterCallback
 *
 * NAME
 *   FKGTCP_Rdr_SetPinEnterCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when a PIN is entered on a reader with a PIN-pad (FunkyCode).
 *
 * SYNOPSIS
 *   FKGTCP_Rdr_SetPinEnterCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE pin[], BYTE pin_sz));
 *
 * NOTES
 *
 *   The callback receives 4 parameters:
 *
 *   - const char *device_name : the device's "friendly-name"
 *   - BYTE head_idx : the internal index of the reader. Always 0 for current version
 *   - const BYTE pin_id[] : the entered PIN
 *   - BYTE pin_id_sz : the size of the PIN
 *
 *   The callback SHALL return TRUE. If it returns FALSE, the default TLV callback (registered
 *   using FKGTCP_SetTlvReceiveCallback) will be called afterwards.
 *
 *   If the reader has no PIN-pad, this callback will never be invoked.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 **/
FKGTCP_LIB void FKGTCP_API FKGTCP_Rdr_SetPinEnterCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE pin[], BYTE pin_sz))
{
  FKG_Callbacks.OnPinEnter = cb;
}

/**f* FkgTCP.dll/FKGTCP_Mio_SetInputChangeCallback
 *
 * NAME
 *   FKGTCP_Mio_SetInputChangeCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when any input of an I/O module (HandyDrummer) changes.
 *
 * SYNOPSIS
 *   FKGTCP_Mio_SetInputChangeCallback(BOOL (*cb) (const char *device_name, BYTE input_idx, BYTE input_state));
 *
 * NOTES
 *
 *   The callback receives 3 parameters:
 *
 *   - const char *device_name : the device's "friendly-name"
 *   - BYTE input_idx : the internal index of the Input that has changed
 *   - BYTE input_state : the new state of the Input
 *
 *   The callback SHALL return TRUE. If it returns FALSE, the default TLV callback (registered
 *   using FKGTCP_SetTlvReceiveCallback) will be called afterwards.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 **/
FKGTCP_LIB void FKGTCP_API FKGTCP_Mio_SetInputChangeCallback(BOOL (*cb) (const char *device_name, BYTE input_idx, BYTE input_state))
{
  FKG_Callbacks.OnInputChange = cb;
}

/**f* FkgTCP.dll/FKGTCP_SetTlvReceiveCallback
 *
 * NAME
 *   FKGTCP_SetTlvReceiveCallback
 *
 * DESCRIPTION
 *  Declare a callback function that will be called whenever the computer receives a packet from the device
 *  (FunkyGate reader or HandyDrummer I/O module) and this packet has not been handled by another internal routine
 *  or callback.
 *
 * SYNOPSIS
 *   void FKGTCP_SetTlvReceiveCallback(BOOL (*cb) (const char *device_name, BYTE tag, BYTE tag_idx, const BYTE payload[], BYTE payload_sz));
 *
 * NOTES
 *
 *   The callback receives 4 parameters:
 *
 *   const char *device_name : the device's "friendly-name"
 *   BYTE tag : the high-order byte of the Tag (T in T,L,V)
 *   BYTE tag_idx : the low-order byte of the Tag (T in T,L,V)
 *   const BYTE payload[] : the Value (V in T,L,V)
 *   BYTE payload_sz : the Length of the Value (L in T,L,V)
 *
 *   The callback SHALL return TRUE. Returning FALSE will shutdown the communication channel and
 *   terminate the thread.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 **/
FKGTCP_LIB void FKGTCP_API FKGTCP_SetTlvReceiveCallback(BOOL (*cb) (const char *device_name, BYTE tag, BYTE tag_idx, const BYTE payload[], BYTE payload_sz))
{
  FKG_Callbacks.OnTlvReceive = cb;
}

