/**** Fkg485.dll/cmd.c
 *
 * NAME
 *   Fkg485.dll -- Commands
 *
 * DESCRIPTION
 *   Commands to send to the reader or I/O device
 *
 **/
#include "fkg485_dll_i.h"
#include <assert.h>


/**f* Fkg485.dll/FKG485_AskForStatus
 *
 * NAME
 *   FKG485_AskForStatus
 *
 * DESCRIPTION
 *  Asks for reader status
 *
 * SYNOPSIS
 *   BOOL FKG485_LIB BOOL FKG485_API FKG485_AskForStatus(const char *device_name);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_AskForStatus(const char *device_name)
{
  FKG_DEVICE_CTX_ST *device_ctx;
  
  assert(device_name != NULL);

  if (!strcmp(device_name, "*"))
  {
    device_ctx = FKG_DeviceFirst();

    while (device_ctx != NULL)
    {
      if (!FKG_AskForStatus(device_ctx))
        return FALSE;

      device_ctx = FKG_DeviceNext(device_ctx);
    }

    return TRUE;
  } else
  {
    device_ctx = FKG_FindDevice(device_name);

    if (device_ctx == NULL)
      return FALSE;

    return FKG_AskForStatus(device_ctx);
  }

}

/**f* Fkg485.dll/FKG485_ReadConfig
 *
 * NAME
 *   FKG485_ReadConfig
 *
 * DESCRIPTION
 *  Reads a configuration register
 *
 * SYNOPSIS
 *   BOOL FKG485_LIB BOOL FKG485_API FKG485_ReadConfig(const char *device_name, BYTE reg);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *   BYTE reg : the register to be written
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_ReadConfig(const char *device_name, BYTE reg)
{
  FKG_DEVICE_CTX_ST *device_ctx;
  
  assert(device_name != NULL);

  if (!strcmp(device_name, "*"))
  {
    device_ctx = FKG_DeviceFirst();

    while (device_ctx != NULL)
    {
      if (!FKG_ReadConfig(device_ctx, reg))
        return FALSE;

      device_ctx = FKG_DeviceNext(device_ctx);
    }

    return TRUE;
  } else
  {
    device_ctx = FKG_FindDevice(device_name);

    if (device_ctx == NULL)
      return FALSE;

    return FKG_ReadConfig(device_ctx, reg);
  }

}

/**f* Fkg485.dll/FKG485_WriteConfig
 *
 * NAME
 *   FKG485_WriteConfig
 *
 * DESCRIPTION
 *  Writes a configuration register
 *
 * SYNOPSIS
 *   BOOL FKG485_WriteConfig(const char *device_name, BYTE reg, const BYTE data[], BYTE size);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *   BYTE reg : the register to be written
 *   const BYTE data[] : new register value
 *   BYTE size : number of bytes in data
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_WriteConfig(const char *device_name, BYTE reg, const BYTE data[], BYTE size)
{
  FKG_DEVICE_CTX_ST *device_ctx;
  
  assert(device_name != NULL);
  
  if (!strcmp(device_name, "*"))
  {
		device_ctx = FKG_DeviceFirst();

    while (device_ctx != NULL)
    {
      if (!FKG_WriteConfig(device_ctx, reg, data, size))
        return FALSE;

      device_ctx = FKG_DeviceNext(device_ctx);
    }

    return TRUE;
  } else
  {
		device_ctx = FKG_FindDevice(device_name);
    
    if (device_ctx == NULL)
			return FALSE;
    
    return FKG_WriteConfig(device_ctx, reg, data, size);
  }
}

/**f* Fkg485.dll/FKG485_EraseConfig
 *
 * NAME
 *   FKG485_EraseConfig
 *
 * DESCRIPTION
 *  Erases a configuration register - NOT IMPLEMENTED IN READER YET
 *
 * SYNOPSIS
 *   BOOL FKG485_EraseConfig(const char *device_name, BYTE reg);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *   BYTE reg : the register to be erased
 *
 * RETURNS
 *   TRUE  : success, command sent - NOT IMPLEMENTED IN READER YET
 *   FALSE : error
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_EraseConfig(const char *device_name, BYTE reg)
{
  /* NOT IMPLEMENTED IN READER YET */
	return FALSE;
}


/**f* Fkg485.dll/FKG485_DoReset
 *
 * NAME
 *   FKG485_DoReset
 *
 * DESCRIPTION
 *  Resets a KunkyGate Reader
 *
 * SYNOPSIS
 *   BOOL FKG485_DoReset(const char *device_name);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_DoReset(const char *device_name)
{
  FKG_DEVICE_CTX_ST *device_ctx;
  
  assert(device_name != NULL);
  
  if (!strcmp(device_name, "*"))
  {
		device_ctx = FKG_DeviceFirst();

    while (device_ctx != NULL)
    {
      if (!FKG_DoReset(device_ctx))
        return FALSE;

      device_ctx = FKG_DeviceNext(device_ctx);
    }

    return TRUE;
  } else
  {
		device_ctx = FKG_FindDevice(device_name);
    
    if (device_ctx == NULL)
			return FALSE;
    return FKG_DoReset(device_ctx);
	}
}


/**f* Fkg485.dll/FKG485_Rdr_SetLeds
 *
 * NAME
 *   FKG485_Rdr_SetLeds
 *
 * DESCRIPTION
 *  Drive the FunkyGate's LEDs
 *
 * SYNOPSIS
 *   BOOL FKG485_Rdr_SetLeds(const char *device_name, BYTE red, BYTE green);
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
FKG485_LIB BOOL FKG485_API FKG485_Rdr_SetLeds(const char *device_name, BYTE red, BYTE green)
{
  FKG_DEVICE_CTX_ST *device_ctx;
  
  assert(device_name != NULL);

  if (!strcmp(device_name, "*"))
  {
    device_ctx = FKG_DeviceFirst();

    while (device_ctx != NULL)
    {
      if (!FKG_LedPermanent(device_ctx, red, green))
        return FALSE;

      device_ctx = FKG_DeviceNext(device_ctx);
    }

    return TRUE;
  } else
  {
    device_ctx = FKG_FindDevice(device_name);

    if (device_ctx == NULL)
      return FALSE;

    return FKG_LedPermanent(device_ctx, red, green);
  }
}

/**f* Fkg485.dll/FKG485_Rdr_SetLedsT
 *
 * NAME
 *   FKG485_Rdr_SetLedsT
 *
 * DESCRIPTION
 *  Drive the FunkyGate's LEDs
 *
 * SYNOPSIS
 *   BOOL FKG485_Rdr_SetLedsT(const char *device_name, BYTE red, BYTE green, WORD timeout_s);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *   BYTE red : status of the Red LED
 *   BYTE green : status of the Green LED
 *   WORD timeout_s : duration in seconds
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_Rdr_SetLedsT(const char *device_name, BYTE red, BYTE green, WORD timeout_s)
{
  FKG_DEVICE_CTX_ST *device_ctx;
  
  assert(device_name != NULL);

  if (!strcmp(device_name, "*"))
  {
    device_ctx = FKG_DeviceFirst();

    while (device_ctx != NULL)
    {
      if (!FKG_LedSequence(device_ctx, red, green, timeout_s))
        return FALSE;

      device_ctx = FKG_DeviceNext(device_ctx);
    }

    return TRUE;
  } else
  {
    device_ctx = FKG_FindDevice(device_name);

    if (device_ctx == NULL)
      return FALSE;

    return FKG_LedSequence(device_ctx, red, green, timeout_s);
  }
}

/**f* Fkg485.dll/FKG485_Rdr_ClearLeds
 *
 * NAME
 *   FKG485_Rdr_ClearLeds
 *
 * DESCRIPTION
 *  Put the FunkyGate's LEDs back in default mode
 *
 * SYNOPSIS
 *   BOOL FKG485_Rdr_ClearLeds(const char *device_name);
 *
 * INPUTS
 *   const char *device_name : the device's "friendly-name"
 *
 * RETURNS
 *   TRUE  : success, command sent
 *   FALSE : error
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_Rdr_ClearLeds(const char *device_name)
{
  FKG_DEVICE_CTX_ST *device_ctx;
  
  assert(device_name != NULL);

  if (!strcmp(device_name, "*"))
  {
    device_ctx = FKG_DeviceFirst();

    while (device_ctx != NULL)
    {
      if (!FKG_LedDefault(device_ctx))
        return FALSE;

      device_ctx = FKG_DeviceNext(device_ctx);
    }

    return TRUE;
  } else
  {
    device_ctx = FKG_FindDevice(device_name);

    if (device_ctx == NULL)
      return FALSE;

    return FKG_LedDefault(device_ctx);
  }
}

/**f* Fkg485.dll/FKG485_Rdr_Buzzer
 *
 * NAME
 *   FKG485_Rdr_Buzzer
 *
 * DESCRIPTION
 *  Drive the FunkyGate's buzzer
 *
 * SYNOPSIS
 *   BOOL FKG485_Rdr_Buzzer(const char *device_name, BYTE sequence);
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
FKG485_LIB BOOL FKG485_API FKG485_Rdr_Buzzer(const char *device_name, BYTE sequence)
{
  FKG_DEVICE_CTX_ST *device_ctx;
  
  assert(device_name != NULL);

  if (!strcmp(device_name, "*"))
  {
    device_ctx = FKG_DeviceFirst();

    while (device_ctx != NULL)
    {
      if (!FKG_Buzzer(device_ctx, sequence))
        return FALSE;

      device_ctx = FKG_DeviceNext(device_ctx);
    }

    return TRUE;
  } else
  {
    device_ctx = FKG_FindDevice(device_name);

    if (device_ctx == NULL)
      return FALSE;

    return FKG_Buzzer(device_ctx, sequence);
  }
}

/**f* Fkg485.dll/FKG485_Mio_SetOutput
 *
 * NAME
 *   FKG485_Mio_SetOutput
 *
 * DESCRIPTION
 *  Set one of the HandyDrummer's output
 *
 * SYNOPSIS
 *   BOOL FKG485_Mio_SetOutput(const char *device_name, BYTE which);
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
 *   FKG485_Mio_ClearOutput
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_Mio_SetOutput(const char *device_name, BYTE which)
{
  FKG_DEVICE_CTX_ST *device_ctx;
  
  assert(device_name != NULL);

  if (!strcmp(device_name, "*"))
  {
    device_ctx = FKG_DeviceFirst();

    while (device_ctx != NULL)
    {
      if (!FKG_Mio_SetOutput(device_ctx, which))
        return FALSE;

      device_ctx = FKG_DeviceNext(device_ctx);
    }

    return TRUE;
  } else
  {
    device_ctx = FKG_FindDevice(device_name);

    if (device_ctx == NULL)
      return FALSE;

    return FKG_Mio_SetOutput(device_ctx, which);
  }
}

/**f* Fkg485.dll/FKG485_Mio_ClearOutput
 *
 * NAME
 *   FKG485_Mio_ClearOutput
 *
 * DESCRIPTION
 *  Clear (reset) one of the HandyDrummer's output
 *
 * SYNOPSIS
 *   BOOL FKG485_Mio_ClearOutput(const char *device_name, BYTE which);
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
 *   FKG485_Mio_SetOutput
 *
 **/
FKG485_LIB BOOL FKG485_API FKG485_Mio_ClearOutput(const char *device_name, BYTE which)
{
  FKG_DEVICE_CTX_ST *device_ctx;
  
  assert(device_name != NULL);

  if (!strcmp(device_name, "*"))
  {
    device_ctx = FKG_DeviceFirst();

    while (device_ctx != NULL)
    {
      if (!FKG_Mio_ClearOutput(device_ctx, which))
        return FALSE;

      device_ctx = FKG_DeviceNext(device_ctx);
    }

    return TRUE;
  } else
  {
    device_ctx = FKG_FindDevice(device_name);

    if (device_ctx == NULL)
      return FALSE;

    return FKG_Mio_ClearOutput(device_ctx, which);
  }
}
