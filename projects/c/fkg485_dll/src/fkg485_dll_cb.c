/**** Fkg485.dll/callbacks.c
 *
 * NAME
 *   Fkg485.dll -- Callbacks
 *
 * DESCRIPTION
 *   Manage the DLL's callbacks
 *
 **/
#include "fkg485_dll_i.h"

FKG_CALLBACKS_ST FKG_Callbacks;
FKG485_CALLBACKS_ST FKG485_Callbacks;

/**f* Fkg485.dll/FKG485_Rdr_SetCardInsertCallback
 *
 * NAME
 *   FKG485_SetEnumerateCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when a reader device (FunkyGate) has responded to 
 *  a S-ENUM request
 *
 * SYNOPSIS
 *   FKG485_SetEnumerateCallback(BOOL (*cb) (const char *comm_name, BYTE device_idx));
 *
 * NOTES
 *
 *   The callback receives 2 parameters:
 *
 *   - const char *comm_name : the communication port
 *   - BYTE device_idx : the adress of the responding reader.
 *
 *   The callback SHALL return TRUE.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 **/
FKG485_LIB void FKG485_API FKG485_SetEnumerateCallback(BOOL (*cb) (const char *comm_name, BYTE device_idx))
{
  FKG485_Callbacks.OnEnumerate = cb;
}

/**f* Fkg485.dll/FKG485_SetEnumLoopEndedCallback
 *
 * NAME
 *   FKG485_SetEnumLoopEndedCallback
 *
 * DESCRIPTION
 *  Declares a callback function that will be called whenever the first S-ENUM
 *  loop (to detect readers on the bus) has ended
 *
 * SYNOPSIS
 *   FKG485_LIB void FKG485_API FKG485_SetEnumLoopEndedCallback(BOOL (*cb) (const char *comm_name))
 *
 * NOTES
 *
 *   The callback receives 1 parameter:
 *   - const char *device_name : the device's "friendly-name"
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
FKG485_LIB void FKG485_API FKG485_SetEnumLoopEndedCallback(BOOL (*cb) (const char *comm_name))
{
  FKG485_Callbacks.OnEnumLoopEnded = cb;
}



/**f* Fkg485.dll/FKG485_SetILoopEndedCallback
 *
 * NAME
 *   FKG485_SetILoopEndedCallback
 *
 * DESCRIPTION
 *  Declares a callback function that will be called each time an I-BLOCK
 *  loop (to communicate with readers on the bus) has ended. This is especially
 *  useful when several commands need to be sent to the same reader (registry
 *  configuration for example), in order to know when the actual commands 
 *  have all been sent. When communicating with several readers, this callback
 *  may be of limited use.
 *
 * SYNOPSIS
 *   FKG485_LIB void FKG485_API FKG485_SetILoopEndedCallback(BOOL (*cb) (const char *comm_name))
 *
 * NOTES
 *
 *   The callback receives 1 parameter:
 *   - const char *device_name : the device's "friendly-name"
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
FKG485_LIB void FKG485_API FKG485_SetILoopEndedCallback(BOOL (*cb) (const char *comm_name))
{
  FKG485_Callbacks.OnILoopEnded = cb;
}



/**f* Fkg485.dll/FKG485_SetStatusChangeCallback
 *
 * NAME
 *   FKG485_SetStatusChangeCallback
 *
 * DESCRIPTION
 *  Declare a callback function that will be called whenever the communication channel
 *  with a device (FunkyGate reader or HandyDrummer I/O module) is opened or closed.
 *
 * SYNOPSIS
 *   FKG485_SetStatusChangeCallback(BOOL (*cb) (const char *device_name, WORD device_status));
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
FKG485_LIB void FKG485_API FKG485_SetStatusChangeCallback(BOOL (*cb) (const char *device_name, WORD device_status))
{
  FKG_Callbacks.OnStatusChange = cb;
}

/**f* Fkg485.dll/FKG485_SetStatusChangeCallback
 *
 * NAME
 *   FKG485_SetTamperChangeCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when the device's tampers are broken (or restored)
 *
 * SYNOPSIS
 *   FKG485_SetTamperChangeCallback(BOOL (*cb) (const char *device_name, BYTE tampers_state));
 *
 * NOTES
 *
 *   The callback receives 2 parameters:
 *
 *   - const char *device_name : the device's "friendly-name"
 *   - BYTE tampers_state : the status of the device's tampers
 *
 *   The callback SHALL return TRUE. If it returns FALSE, the default TLV callback (registered
 *   using FKG485_SetTlvReceiveCallback) will be called afterwards.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 **/
FKG485_LIB void FKG485_API FKG485_SetTamperChangeCallback(BOOL (*cb) (const char *device_name, BYTE tampers_state))
{
  FKG_Callbacks.OnTamperChange = cb;
}

/**f* Fkg485.dll/FKG485_Rdr_SetCardReadCallback
 *
 * NAME
 *   FKG485_Rdr_SetCardReadCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when a reader device (FunkyGate) has read a card
 *
 * SYNOPSIS
 *   FKG485_Rdr_SetCardReadCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz));
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
 *   using FKG485_SetTlvReceiveCallback) will be called afterwards.
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
 *   FKG485_Rdr_SetCardInsertCallback
 *   FKG485_Rdr_SetCardRemoveCallback
 *
 **/
FKG485_LIB void FKG485_API FKG485_Rdr_SetCardReadCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz))
{
  FKG_Callbacks.OnCardRead = cb;
}

/**f* Fkg485.dll/FKG485_Rdr_SetCardInsertCallback
 *
 * NAME
 *   FKG485_Rdr_SetCardReadCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when a reader device (FunkyGate) has read a card
 *  and the reader is configured to monitor insert/remove moves.
 *
 * SYNOPSIS
 *   FKG485_Rdr_SetCardInsertCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz));
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
 *   using FKG485_SetTlvReceiveCallback) will be called afterwards.
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
 *   FKG485_Rdr_SetCardRemoveCallback
 *   FKG485_Rdr_SetCardReadCallback
 *
 **/
FKG485_LIB void FKG485_API FKG485_Rdr_SetCardInsertCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE card_id[], BYTE card_id_sz))
{
  FKG_Callbacks.OnCardInsert = cb;
}

/**f* Fkg485.dll/FKG485_Rdr_SetCardRemoveCallback
 *
 * NAME
 *   FKG485_Rdr_SetCardRemoveCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when a reader device (FunkyGate) detects the
 *  card removal (only available when the reader is configured to monitor insert/remove moves).
 *
 * SYNOPSIS
 *   FKG485_Rdr_SetCardRemoveCallback(BOOL (*cb) (const char *device_name, BYTE head_idx));
 *
 * NOTES
 *
 *   The callback receives 4 parameters:
 *
 *   - const char *device_name : the device's "friendly-name"
 *   - BYTE head_idx : the internal index of the reader. Always 0 for current version
 *
 *   The callback SHALL return TRUE. If it returns FALSE, the default TLV callback (registered
 *   using FKG485_SetTlvReceiveCallback) will be called afterwards.
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
 *   FKG485_Rdr_SetCardInsertCallback
 *
 **/
FKG485_LIB void FKG485_API FKG485_Rdr_SetCardRemoveCallback(BOOL (*cb) (const char *device_name, BYTE head_idx))
{
  FKG_Callbacks.OnCardRemove = cb;
}

/**f* Fkg485.dll/FKG485_Rdr_SetPinEnterCallback
 *
 * NAME
 *   FKG485_Rdr_SetPinEnterCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when a PIN is entered on a reader with a PIN-pad (FunkyCode).
 *
 * SYNOPSIS
 *   FKG485_Rdr_SetPinEnterCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE pin[], BYTE pin_sz));
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
 *   using FKG485_SetTlvReceiveCallback) will be called afterwards.
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
FKG485_LIB void FKG485_API FKG485_Rdr_SetPinEnterCallback(BOOL (*cb) (const char *device_name, BYTE head_idx, const BYTE pin[], BYTE pin_sz))
{
  FKG_Callbacks.OnPinEnter = cb;
}

/**f* Fkg485.dll/FKG485_Mio_SetInputChangeCallback
 *
 * NAME
 *   FKG485_Mio_SetInputChangeCallback
 *
 * DESCRIPTION
 *  Declare a callback function to be called when any input of an I/O module (HandyDrummer) changes.
 *
 * SYNOPSIS
 *   FKG485_Mio_SetInputChangeCallback(BOOL (*cb) (const char *device_name, BYTE input_idx, BYTE input_state));
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
 *   using FKG485_SetTlvReceiveCallback) will be called afterwards.
 *
 * WARNING
 *   The callback is called in the context of a background thread. Care must be taken
 *   - to return as soon as possible, so the communication could go on quickly
 *   - to prevent any interaction with the windowed components that belong to the
 *     application's main loop
 *
 **/
FKG485_LIB void FKG485_API FKG485_Mio_SetInputChangeCallback(BOOL (*cb) (const char *device_name, BYTE input_idx, BYTE input_state))
{
  FKG_Callbacks.OnInputChange = cb;
}

/**f* Fkg485.dll/FKG485_SetTlvReceiveCallback
 *
 * NAME
 *   FKG485_SetTlvReceiveCallback
 *
 * DESCRIPTION
 *  Declare a callback function that will be called whenever the computer receive a packet from the device
 *  (FunkyGate reader or HandyDrummer I/O module) and this packet has not been handled by another internal routine
 *  or callback.
 *
 * SYNOPSIS
 *   void FKG485_SetTlvReceiveCallback(BOOL (*cb) (const char *device_name, BYTE tag, BYTE tag_idx, const BYTE payload[], BYTE payload_sz));
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
FKG485_LIB void FKG485_API FKG485_SetTlvReceiveCallback(BOOL (*cb) (const char *device_name, BYTE tag, BYTE tag_idx, const BYTE payload[], BYTE payload_sz))
{
  FKG_Callbacks.OnTlvReceive = cb;
}
