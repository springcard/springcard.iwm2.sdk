#include "fkgtcp_i.h"
#include <assert.h>

BOOL fkgtcp_verbose = TRUE;


#ifdef FKGTCP_SECURE
static BOOL FKGTCP_StartThread(FKG_DEVICE_CTX_ST *device, BYTE fkg_addr[4], WORD fkg_port, FKGTCP_SECURE_MODE_E secure_mode, const BYTE *auth_key)
#else
static BOOL FKGTCP_StartThread(FKG_DEVICE_CTX_ST *device, BYTE fkg_addr[4], WORD fkg_port)
#endif
{
  FKGTCP_CLIENT_CTX_ST *client;

	assert(device != NULL);
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  assert(client != NULL);

#ifdef FKGTCP_SECURE
  if (secure_mode != FKGTCP_SECURE_NONE)
	  assert(auth_key != NULL);
#endif

  if (fkg_port == 0) 
    fkg_port = FKGTCP_DEFAULT_PORT;

  FKG_Trace(TRACE_INFO, "%s:creating client (addr=%d.%d.%d.%d, port=%d)\n", device->Name, fkg_addr[0], fkg_addr[1], fkg_addr[2], fkg_addr[3], fkg_port);
   
  client->sock = INVALID_SOCKET;  
  memcpy(client->addr, fkg_addr, 4);
  client->port = fkg_port;

#ifdef FKGTCP_SECURE
  client->secure.mode = secure_mode;
  if (auth_key != NULL)
    memcpy(client->secure.auth_key, auth_key, 16);
  else
    memset(client->secure.auth_key, 0, 16);
#endif

	client->terminated = FALSE;
  client->thrd_running = TRUE;
  client->thrd_hnd = CreateThread(
    NULL,
    0,
    FKGTCP_ClientThread,
    device,
    0,
    &client->thrd_id);
  
  if (client->thrd_hnd == INVALID_HANDLE_VALUE)
  {
    client->thrd_running = FALSE;
    return FALSE;
  }
  
  return TRUE;
}

#ifdef FKGTCP_SECURE
BOOL FKGTCP_StartEx_Auth(FKG_DEVICE_CTX_ST *device, const char *connection_string, const BYTE *auth_key)
{
  BYTE addr[4];
  WORD port = 0;
	int secure_mode = FKGTCP_SECURE_USER_AES;

  assert(device != NULL);
  assert(connection_string != NULL);

  assert(auth_key != NULL);

	if (sscanf(connection_string, "%d.%d.%d.%d:%d", &(addr[0]), &(addr[1]), &(addr[2]), &(addr[3]), &port) != 5)
	{
		if (!NETWORK_GetHostByName(addr, connection_string))
		{
			if (!NETWORK_GetHostByAddr(addr, connection_string))
			{
				FKG_Trace(TRACE_INFO, "%s:Host '%s' incorrect or not found on the network\n", device->Name, connection_string);
				return FALSE;
			}
		}
	}
	 
  FKG_Trace(TRACE_INFO, "%s:Host '%s' resolved in %d.%d.%d.%d\n", device->Name, connection_string, addr[0], addr[1], addr[2], addr[3]);
	return FKGTCP_StartThread(device, addr, port, (FKGTCP_SECURE_MODE_E) secure_mode, auth_key);
}
#endif

BOOL FKGTCP_StartEx_Plain(FKG_DEVICE_CTX_ST *device, const char *connection_string)
{
  BYTE addr[4];
  WORD port = 0;

  assert(device != NULL);
  assert(connection_string != NULL);

	if (sscanf(connection_string, "%d.%d.%d.%d:%d", &(addr[0]), &(addr[1]), &(addr[2]), &(addr[3]), &port) != 5)
	{
		if (!NETWORK_GetHostByName(addr, connection_string))
		{
			if (!NETWORK_GetHostByAddr(addr, connection_string))
			{
				FKG_Trace(TRACE_INFO, "%s:Host '%s' incorrect or not found on the network\n", device->Name, connection_string);
				return FALSE;
			}
		}
	}
	 
  FKG_Trace(TRACE_INFO, "%s:Host '%s' resolved in %d.%d.%d.%d\n", device->Name, connection_string, addr[0], addr[1], addr[2], addr[3]);

#ifdef FKGTCP_SECURE
	return FKGTCP_StartThread(device, addr, port, FKGTCP_SECURE_NONE, NULL);
#else
  return FKGTCP_StartThread(device, addr, port);
#endif
}

BYTE FKG_GetState(FKG_DEVICE_CTX_ST *device)
{
  FKGTCP_CLIENT_CTX_ST *client;

  if (device == NULL)
    return FKGTCP_STATE_UNKNOWN;
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  if (client == NULL)
    return FKGTCP_STATE_UNKNOWN;

  return client->state;
}

BOOL FKG_SetState(FKG_DEVICE_CTX_ST *device, BYTE new_state) 
{
  FKGTCP_CLIENT_CTX_ST *client;

  if (device == NULL)
    return FALSE;
  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  if (client == NULL)
    return FALSE;

  client->state = new_state;
  return TRUE;
}

void FKG_SetLastQuery(FKG_DEVICE_CTX_ST *device, FKG_QUERY_E query) 
{
  if (device == NULL)
    return;

  device->LastQuery = query;
}
