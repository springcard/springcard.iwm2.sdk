/**** FkgTCP.dll/cmd.c
 *
 * NAME
 *   FkgTCP.dll -- Commands
 *
 * DESCRIPTION
 *   Commands to send to the reader or I/O device
 *
 **/
#include "fkgtcp_dll_i.h"
#include <assert.h>

/**f* FkgTCP.dll/FKGTCP_Rdr_SetLeds
 *
 * NAME
 *   FKGTCP_Rdr_SetLeds
 *
 * DESCRIPTION
 *  Drive the FunkyGate's LEDs
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Rdr_SetLeds(const char *device_name, BYTE red, BYTE green);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *   BYTE red : status of the Red LED
 *   BYTE green : status of the Green LED
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Rdr_SetLeds(const char *device_name, BYTE red, BYTE green)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);

  if (device == NULL)
    return FALSE;

  return FKG_LedPermanent(device, red, green);
}

FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Rdr_SetLedsT(const char *device_name, BYTE red, BYTE green, WORD timeout_s)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);

  if (device == NULL)
    return FALSE;

  return FKG_LedSequence(device, red, green, timeout_s);
}

/**f* FkgTCP.dll/FKGTCP_Rdr_ClearLeds
 *
 * NAME
 *   FKGTCP_Rdr_ClearLeds
 *
 * DESCRIPTION
 *  Put the FunkyGate's LEDs back in default mode
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Rdr_ClearLeds(const char *device_name);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Rdr_ClearLeds(const char *device_name)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);

  if (device == NULL)
    return FALSE;

  return FKG_LedDefault(device);
}

/**f* FkgTCP.dll/FKGTCP_Rdr_Buzzer
 *
 * NAME
 *   FKGTCP_Rdr_Buzzer
 *
 * DESCRIPTION
 *  Drive the FunkyGate's buzzer
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Rdr_Buzzer(const char *device_name, BYTE sequence);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *   BYTE sequence : the state of the buzzer
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Rdr_Buzzer(const char *device_name, BYTE sequence)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);

  if (device == NULL)
    return FALSE;

  return FKG_Buzzer(device, sequence);
}

/**f* FkgTCP.dll/FKGTCP_Mio_SetOutput
 *
 * NAME
 *   FKGTCP_Mio_SetOutput
 *
 * DESCRIPTION
 *  Set one of the HandyDrummer's output
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Mio_SetOutput(const char *device_name, BYTE which);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *   BYTE which : the index of the Output to be set
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 * SEE ALSO
 *   FKGTCP_Mio_ClearOutput
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Mio_SetOutput(const char *device_name, BYTE which)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);

  if (device == NULL)
    return FALSE;

  return FKG_Mio_SetOutput(device, which);
}

/**f* FkgTCP.dll/FKGTCP_Mio_SetOutputT
 *
 * NAME
 *   FKGTCP_Mio_SetOutputT
 *
 * DESCRIPTION
 *  Set one of the HandyDrummer's output, for a specified time period
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Mio_SetOutputT(const char *device_name, BYTE which, WORD timeout_s);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *   BYTE which : the index of the Output to be set
 *	 WORD timeout_s : time in seconds
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Mio_SetOutputT(const char *device_name, BYTE which, WORD timeout_s)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);

  if (device == NULL)
    return FALSE;

  return FKG_Mio_SetOutputT(device, which, timeout_s);
}

/**f* FkgTCP.dll/FKGTCP_Mio_ClearOutput
 *
 * NAME
 *   FKGTCP_Mio_ClearOutput
 *
 * DESCRIPTION
 *  Clear (reset) one of the HandyDrummer's output
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Mio_ClearOutput(const char *device_name, BYTE which);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *   BYTE which : the index of the Output to be reset
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 * SEE ALSO
 *   FKGTCP_Mio_SetOutput
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Mio_ClearOutput(const char *device_name, BYTE which)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);

  if (device == NULL)
    return FALSE;

  return FKG_Mio_ClearOutput(device, which);
}

/**f* FkgTCP.dll/FKGTCP_Mio_ReadInputs
 *
 * NAME
 *   FKGTCP_Mio_ReadInputs
 *
 * DESCRIPTION
 *  Read the HandyDrummer's current input state
 *
 * SYNOPSIS
 *   BOOL FKGTCP_Mio_ReadInputs(const char *device_name);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 * SEE ALSO
 *   
 *
 **/
FKGTCP_LIB BOOL FKGTCP_API FKGTCP_Mio_ReadInputs(const char *device_name)
{
  FKG_DEVICE_CTX_ST *device = FKG_FindDevice(device_name);

  if (device == NULL)
    return FALSE;

  return FKG_Mio_ReadInputs(device);
}
