#include "fkgtcp_i.h"

#define FKGTCP_KIPA_INTERVAL 10
#define FKGTCP_RESP_INTERVAL 3

DWORD WINAPI FKGTCP_ClientThread(void *param)
{
	BYTE buffer[FKGTCP_BUFFER_SZ];
	WORD length;
  WORD wait_sec;
  time_t send_kipa_after = 0;
  BOOL expect_answer = FALSE;

	FKG_DEVICE_CTX_ST *device;
  FKGTCP_CLIENT_CTX_ST *client;

#ifndef WIN32
	struct sigaction act;
	memset (&act, 0, sizeof(act));
 
	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &on_SIGUSER_sigaction;
 
	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;
		
	sigaction(SIGUSER, &act, NULL);
#endif	
	
  device = (FKG_DEVICE_CTX_ST *) param;	
  if (device == NULL)
  {
    FKG_Trace(TRACE_MAJOR, "Internal error\n");
    return 0;
  }

  client = (FKGTCP_CLIENT_CTX_ST *) device->CtxData;	
  if (client == NULL)
  {
    FKG_Trace(TRACE_MAJOR, "Internal error\n");
    return 0;
  }

  if (FKG_Callbacks.OnStatusChange != NULL)
    if (!FKG_Callbacks.OnStatusChange(device->Name, FKG_STATUS_STARTED))
      return 0;

  FKG_Trace(TRACE_INFO, "%s:client is ready\n", device->Name);
	
  /* Boucle infinie, on attend les evenements sur la socket */
  while (!client->terminated)
  { 
    /* Ouverture de la connexion avec le serveur FKGTCP */
    if (!client->connected)
    {            
      if (client->sock != INVALID_SOCKET)
      {
      	/* On etait connecte, on ne l'est plus */
      	/* ----------------------------------- */      	
      	
        /* Annule l'attente sur cette socket */
        FKG_Trace(TRACE_INFO, "%s:connexion lost\n", device->Name);
        NETWORK_Close(client->sock);
        client->sock = INVALID_SOCKET;

        /* Callback */
        if (FKG_Callbacks.OnStatusChange != NULL)
          if (!FKG_Callbacks.OnStatusChange(device->Name, FKG_STATUS_DISCONNECTED))
            break;
                        
        /* On va reessayer de nous reconnecter dans 5s */
#ifdef WIN32
        Sleep(5000);
#else
        sleep(5);
#endif
        continue;
      }
      
      /* Tentative de connection */
      FKG_Trace(TRACE_DEEP, "%s:trying to connect to reader (%d.%d.%d.%d:%d)...\n",
        device->Name, client->addr[0], client->addr[1], client->addr[2], client->addr[3], client->port); 

	    client->connected = NETWORK_TCPCliOpen(&client->sock, client->addr, client->port);

      if (!client->connected)
      {
        /* Echec */
        FKG_Trace(TRACE_INFO, "%s:failed to connect\n", device->Name);
        client->sock = INVALID_SOCKET;

        /* Callback */
        if (FKG_Callbacks.OnStatusChange != NULL)
          if (!FKG_Callbacks.OnStatusChange(device->Name, FKG_STATUS_CONNECTION_FAILED))
            break;

        /* On va reessayer de nous connecter dans 5s */
#ifdef WIN32
        Sleep(5000);
#else
        sleep(5);
#endif
        continue;
      }

      /* Succes */
      FKG_Trace(TRACE_INFO, "%s:connected\n", device->Name);

      if (FKG_Callbacks.OnStatusChange != NULL)
        if (!FKG_Callbacks.OnStatusChange(device->Name, FKG_STATUS_CONNECTED))
          break;

      /* Attente du HELO */
      client->state = FKGTCP_STATE_WAIT_HELO;

      /* Preparation des heures des prochains traitements */
      expect_answer = TRUE;
    }

    /* Combien de temps on attend? */
    /* --------------------------- */
    if (expect_answer)
    {
      wait_sec = FKGTCP_RESP_INTERVAL;
    } else
    {
      wait_sec = FKGTCP_KIPA_INTERVAL;
    }
    
    /* Attente bloquante */
    FKG_Trace(TRACE_DEEP, "%s:waiting during %ds\n", device->Name, wait_sec);
    length = sizeof(buffer);
    if (!NETWORK_TCPWaitRecv(client->sock, buffer, &length, wait_sec))
    {
      FKG_Trace(TRACE_DEEP, "%s:connexion lost\n", device->Name);
      client->connected = FALSE;
    } else  
    if (length)
    {
      FKG_Trace(TRACE_DEEP, "%s:received %dB\n", device->Name, length);          
      if (FKGTCP_RecvFromDeviceEx(device, buffer, length))
      {
        /* On n'attend plus rien de particulier */
        expect_answer = FALSE;

      } else
      {
        FKG_Trace(TRACE_DEEP, "%s:receive failed (1)\n", device->Name);
        client->connected = FALSE;
      }
    } else
    {
      /* Timeout */
      if (expect_answer)
      {
        /* Oups, on a perdu le lecteur */
        FKG_Trace(TRACE_DEEP, "%s:timeout\n", device->Name);
        client->connected = FALSE;
      } else
      if (FKG_Ping(device)) /* Il est temps d'envoyer un keep-alive */
      {
        /* On attend une reponse a ce keep-alive */
        expect_answer = TRUE;
      } else
      {
        /* Erreur, le TCPCliSend n'a pas fonctionne*/
        FKG_Trace(TRACE_DEEP, "%s:send ping failed\n", device->Name);
        client->connected = FALSE;
      }
    }
  } /* Boucle infinie */

  /* Si on est sorti de la boucle, c'est qu'il y a une erreur fatale... */
  NETWORK_Close(client->sock);
  FKG_Trace(TRACE_INFO, "%s:exiting\n", device->Name);

  if (FKG_Callbacks.OnStatusChange != NULL)
    FKG_Callbacks.OnStatusChange(device->Name, FKG_STATUS_TERMINATED);

  client->thrd_running = FALSE;
  return 0;  
}


