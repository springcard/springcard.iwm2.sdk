#include "fkgmk2_i.h"

void *queue_to_router = NULL;
volatile BOOL queue_connected = FALSE;
extern void *event_config_to_be_reloaded;
volatile BYTE global_nb_reload = 0;
volatile BOOL poll_rs485 = FALSE;

BOOL rs485_to_be_polled(void)
{
	BOOL res = FALSE;
	if (!UTL_AcquireMutex(mutex_poll_rs485))
	{
		UTL_Trace(NULL, VRB_MAJOR, "Can't acquire Mutex (poll rs485)");
		exit(EXIT_FAILURE);
	}
	
	res = poll_rs485;
	
	if (!UTL_ReleaseMutex(mutex_poll_rs485))
	{
		UTL_Trace(NULL, VRB_MAJOR, "Can't realease Mutex (poll rs485)");
		exit(EXIT_FAILURE);
	}
	
	return res;
	
}

void set_poll_rs485(BOOL b)
{
	if (!UTL_AcquireMutex(mutex_poll_rs485))
	{
		UTL_Trace(NULL, VRB_MAJOR, "Can't acquire Mutex (poll rs485)");
		exit(EXIT_FAILURE);
	}
	
	poll_rs485 = b;
	
	if (!UTL_ReleaseMutex(mutex_poll_rs485))
	{
		UTL_Trace(NULL, VRB_MAJOR, "Can't realease Mutex (poll rs485)");
		exit(EXIT_FAILURE);
	}
}

void increment_reload_config(void)
{
	if (!UTL_AcquireMutex(mutex_nb_reload_acces))
	{
		UTL_Trace(NULL, VRB_MAJOR, "Can't acquire Mutex (nb reload config)");
		exit(EXIT_FAILURE);
	}
	
	global_nb_reload++;
	
	if (!UTL_ReleaseMutex(mutex_nb_reload_acces))
	{
		UTL_Trace(NULL, VRB_MAJOR, "Can't realease Mutex (nb reload config)");
		exit(EXIT_FAILURE);
	}
	
}

void decrement_reload_config(BYTE *remaining)
{
	if (!UTL_AcquireMutex(mutex_nb_reload_acces))
	{
		UTL_Trace(NULL, VRB_MAJOR, "Can't acquire Mutex (nb reload config)");
		exit(EXIT_FAILURE);
	}
	
	if (global_nb_reload > 0)
		global_nb_reload--;
	
	*remaining = global_nb_reload;
	
	if (!UTL_ReleaseMutex(mutex_nb_reload_acces))
	{
		UTL_Trace(NULL, VRB_MAJOR, "Can't realease Mutex (nb reload config)");
		exit(EXIT_FAILURE);
	}

}

BYTE get_reload_config(void)
{
	BYTE res;
	
	if (!UTL_AcquireMutex(mutex_nb_reload_acces))
	{
		UTL_Trace(NULL, VRB_MAJOR, "Can't acquire Mutex (nb reload config)");
		exit(EXIT_FAILURE);
	}
	
	res = global_nb_reload;
	
	if (!UTL_ReleaseMutex(mutex_nb_reload_acces))
	{
		UTL_Trace(NULL, VRB_MAJOR, "Can't realease Mutex (nb reload config)");
		exit(EXIT_FAILURE);
	}
	
	return res;
}

void FKG_OnMessageToRDR(FKG_THRD_CTX_ST *thrd_ctx, BYTE command, BYTE buffer[], DWORD length)
{

	UTL_Trace(NULL, VRB_DEBUG_APP, "P%02d:R%02d:to RDR %02X", thrd_ctx->por_idx, thrd_ctx->rdr_idx, command);
  
#ifdef NOT_DEFINED
	DWORD i =0;
	printf("FKG_OnMessageToRDR : ");
	for (i=0; i<length; i++)
		printf("0x%2.2x-", buffer[i]);
	printf("\n");
#endif	
	
	switch (command)
  {
    case MCA_RDR_STATUS_AND_ALARMS :
      break;

    case MCA_RDR_LED_COMMAND :
      if (length == 2)
      {
				BYTE red=0, green=0;
				switch (buffer[0])
				{
				
					case LED_G_ON :
						red=0;
						green=1;
						UTL_Trace(NULL, VRB_TRACE, "P%02d:Led Green On (%d sec)", thrd_ctx->por_idx, buffer[1]);
						/*
						gettimeofday(&now, NULL);
						add_ms_to_timeval((struct timeval *) &now, 1000 * second, (struct timeval *) &fkg_com[i].time_to_send_normal_command);
						fkg_com[i].normal_command_to_perform = TRUE;
						fkg_serial_send(fd, "M101\r\n", sizeof("M101\r\n"));
						*/
						break;
						
					case LED_R_ON :
						red=1;
						green=0;
						UTL_Trace(NULL, VRB_TRACE, "P%02d:Led Red On (%d sec)", thrd_ctx->por_idx, buffer[1]);
						
						//gettimeofday(&now, NULL);
						//add_ms_to_timeval((struct timeval *) &now, 1000 * second, (struct timeval *) &fkg_com[i].time_to_send_normal_command);
						//fkg_com[i].normal_command_to_perform = TRUE;
						//fkg_serial_send(fd, "M110\r\n", sizeof("M110\r\n"));
						break;
						
					case (LED_R_FAST | LED_G_FAST_INV)	:
						UTL_Trace(NULL, VRB_TRACE, "P%02d:Anomaly", thrd_ctx->por_idx);
						red=3;
						green=7;
						/*
						gettimeofday(&now, NULL);
						add_ms_to_timeval((struct timeval *) &now, 1000 * second, (struct timeval *) &fkg_com[i].time_to_send_normal_command);
						fkg_com[i].normal_command_to_perform = TRUE;
						fkg_serial_send(fd, "M137\r\n", sizeof("M137\r\n"));
						*/
						break;		
					
					default	:
						break;
				}		
				
				FKG_LedSequence(thrd_ctx, red, green, buffer[1]);
				
			}
      break;

    default:
      UTL_Trace(NULL, VRB_DEBUG_APP, "Message to RDR %02X non gere", command);  
  }
}

void FKG_OnJessAcsDn(FKG_THRD_CTX_ST *thrd_ctx, JESS_ACS_DN_ST *msg)
{
  UTL_Trace(NULL, VRB_DEBUG_APP, "P%02d:R%02d:on ACS DN %02X", thrd_ctx->por_idx, thrd_ctx->rdr_idx, msg->action);
  switch (msg->action & ACS_DN_ACT_MASK)
  {
    case ACS_DN_ACT_ACCES_OK :
      if (thrd_ctx->tag_is_inserted)
        FKG_LedSequence(thrd_ctx, LED_OFF, LED_SLOW, 0);
      else
        FKG_LedSequence(thrd_ctx, LED_OFF, LED_ON, 3);
      FKG_OnAccessOK(thrd_ctx);
      break;
    case ACS_DN_ACT_ACCES_KO :
      FKG_LedSequence(thrd_ctx, LED_ON, LED_OFF, 3);
      FKG_OnAccessKO(thrd_ctx);
      break;
    case ACS_DN_ACT_ACCES_EX :
      FKG_LedSequence(thrd_ctx, LED_OFF, LED_FAST, 3);
      break;
    case ACS_DN_ACT_BLOQUE   :
      FKG_LedSequence(thrd_ctx, LED_ON, LED_OFF, 0);
      break;
    case ACS_DN_ACT_LIBRE    :
      FKG_LedSequence(thrd_ctx, LED_OFF, LED_ON, 0);
      break;
    case ACS_DN_ACT_NORMAL   :
      FKG_SetRuntimeMode(thrd_ctx, RUNTIME_MODE_READER_ON, 0);
      if (thrd_ctx->tag_is_inserted)
        FKG_LedSequence(thrd_ctx, LED_OFF, LED_SLOW, 0);
      else
        FKG_LedSequence(thrd_ctx, LED_OFF, LED_OFF, 0);
      break;
    case ACS_DN_ACT_PINS_ON  :
      FKG_SetRuntimeMode(thrd_ctx, RUNTIME_MODE_READER_WITH_PINS, 0);
      if (thrd_ctx->tag_is_inserted)
        FKG_LedSequence(thrd_ctx, LED_OFF, LED_SLOW, 0);
      else
        FKG_LedSequence(thrd_ctx, LED_OFF, LED_OFF, 0);
      break;
    default :
      UTL_Trace(NULL, VRB_DEBUG_APP, "Message ACS DN %02X non gere", msg->action & ACS_DN_ACT_MASK);  
      break;
  }

}

void FKG_OnMessageReceived(DWORD tag, BYTE buffer[], DWORD length)
{ 
	
	#ifdef NOT_DEFINED
	int i;
	printf("tag=0x%4.4x\n", tag);
	for (i=0; i<length; i++)
		printf("0x%2.2x-", buffer[i]);
	printf("\n");
	#endif
	
	if (tag & MSG_TAG_BASETIME)
  {
    /* Ignore */
		set_poll_rs485(TRUE);
  } else
  switch (tag)
  {
    case MSG_TAG_APPL_SPEC | ROUTE_MSG_ACS_RDR_TO_MCA :
      /* C'est un echo de mes propres messages */
      if (FKG_ThreadsRunning())
				UTL_Trace(NULL, VRB_DEBUG_LIB, "(echo)");
      break;

    case MSG_TAG_APPL_SPEC | ROUTE_MSG_ACS_MCA_TO_RDR :
      /* Message pour les lecteurs */
			if (FKG_ThreadsRunning())
			{
				if (length >= 2)
				{
					BYTE por_idx, rdr_idx;
					FKG_THRD_CTX_ST *thrd_ctx;

					por_idx = buffer[0];
					for (rdr_idx=0; rdr_idx<MAX_READERS_PER_DOOR; rdr_idx++)
					{
						thrd_ctx = FKG_FindContext(por_idx, rdr_idx);
						if (thrd_ctx != NULL)
							FKG_OnMessageToRDR(thrd_ctx, buffer[1], &buffer[2], length-2);
					}
				}
			}
      break;

    case MSG_TAG_APPL_SPEC | JESS_MSG_ACS_DN :
      /* Message pour les MCAs 	*/
			if (FKG_ThreadsRunning())
			{
				if (length >= sizeof(JESS_ACS_DN_ST))
				{
					BYTE rdr_idx;
					JESS_ACS_DN_ST msg;
					FKG_THRD_CTX_ST *thrd_ctx;
					
					memcpy(&msg, buffer, sizeof(JESS_ACS_DN_ST));
					UTL_Trace(NULL, VRB_DEBUG_APP, "ACS:P%02d:Action %02X", msg.jess_id, msg.action);  

					for (rdr_idx=0; rdr_idx<MAX_READERS_PER_DOOR; rdr_idx++)
					{
						thrd_ctx = FKG_FindContext(msg.jess_id, rdr_idx);

						if (thrd_ctx != NULL)
						{
							/* Message pour un de nos lecteurs	*/
							if (!thrd_ctx->has_mca)
								FKG_OnJessAcsDn(thrd_ctx, &msg);
						}
					}
				}
			}
      break;

		case MSG_TAG_CFG_UPDATED :
			UTL_Trace(NULL, VRB_DEBUG_LIB, "Config updated");
			/* On doit recharger la configuration	*/
			CFG_ReloadConfig(NULL);
			CFG_FileUpdated();
			increment_reload_config();
			UTL_SignalEvent(event_config_to_be_reloaded);
			break;
			
    default :
      UTL_Trace(NULL, VRB_DEBUG_APP, "Message %08lX non gere", tag);  
  }
}

/*
 * RDR_DownlinkProc
 * ----------------
 * Thread en charge de la communication venant du routeur
 * Assure:
 * - la connexion en mode client
 * - la reception
 * - le traitement des messages recus
 */
void FKG485_OnAlarm();
void FKG_DownlinkProc(void *unused)
{  
  BYTE  buffer[128];
  DWORD length;
  DWORD tag;

	//signal(SIGALRM, FKG485_OnAlarm);
	
	/*
	struct sigaction jiz;
	memset (&jiz, 0, sizeof(jiz));
	jiz.sa_sigaction = &FKG485_OnAlarm;
	jiz.sa_flags = SA_SIGINFO;
	sigaction(SIGALRM, &jiz, NULL);
	*/
	
  for (;;)
  {
    void *q;
    
    /* Il faut etre connecte pour exister ! */
    /* ------------------------------------ */
    
    UTL_Trace(NULL, VRB_DEBUG_APP, "Tentative de connexion au routeur...");
    
    if (!UTL_ConnectedQueueOpen(&q, NULL, MLINS_ROUTE_PORT))
    {
      UTL_Trace(NULL, VRB_MINOR, "Echec de la connexion au routeur!");
      sleep(5);
      continue;
    }
    
    UTL_Trace(NULL, VRB_TRACE, "Connecte au routeur...");
       
    /* Tout va bien, autorisons l'autre thread a exploiter notre connexion au routeur */
    /* ------------------------------------------------------------------------------ */
    
    queue_to_router = q;
    queue_connected = TRUE;
    
    /* Et maintenant mettons nous en attente de messages venant du routeur */
    /* ------------------------------------------------------------------- */
    
    for (;;)
    {    
      length = sizeof(buffer);
      if (!UTL_ConnectedQueueRecv(queue_to_router, &tag, buffer, &length))
      {
        /* Connexion perdue!!! */
        queue_connected = FALSE;
              
        UTL_Trace(NULL, VRB_MINOR, "La connexion avec le routeur a ete perdue");
        sleep(1);
        UTL_ConnectedQueueClose(queue_to_router);
        queue_to_router = NULL;
        break;
      }
      
      /* On a recu un message, traitons le !!! */
      /* ------------------------------------- */
      
      FKG_OnMessageReceived(tag, buffer, length);    
    }
  }
}
