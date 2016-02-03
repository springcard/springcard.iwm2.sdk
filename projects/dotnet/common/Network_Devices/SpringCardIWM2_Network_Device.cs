/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 04/12/2015
 * Time: 13:59
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Net;
using System.Threading;
using System.IO;
using System.Text;
using System.Net.Sockets;
using System.Collections.Concurrent;
using System.Security.Cryptography;
using SpringCard.LibCs;

namespace SpringCard.IWM2
{

#region Static cryptographic class
	/* This static class contains all necessary cryptographic functions,	*/
	/* to communicate in a secure way with a SpringCard TCP device				*/
	public static class Crypto
	{
		/* this method performs AES Ciphering */
		public static byte[] AESCipher(byte[] PlainText, byte[] key, byte[] iv)
		{
			AesCryptoServiceProvider aesCSP = new AesCryptoServiceProvider();
			aesCSP.BlockSize = 128;
			aesCSP.Key = key;
			aesCSP.IV = iv;
			aesCSP.Padding = PaddingMode.Zeros;
			aesCSP.Mode = CipherMode.CBC;
			
			ICryptoTransform xfrm = aesCSP.CreateEncryptor(key, iv);
			byte[] result = xfrm.TransformFinalBlock(PlainText, 0, PlainText.Length);
			
			return result;
		}
		
		/* this method performs AES Deciphering */
		public static byte[] AESDecipher(byte[] CypherText, byte[] key, byte[] iv)
		{
			AesCryptoServiceProvider aesCSP = new AesCryptoServiceProvider();
			aesCSP.BlockSize = 128;
			aesCSP.Key = key;
			aesCSP.IV = iv;
			aesCSP.Padding = PaddingMode.Zeros;
			aesCSP.Mode = CipherMode.CBC;
			
			ICryptoTransform xfrm = aesCSP.CreateDecryptor(key, iv);
			byte[] result = xfrm.TransformFinalBlock(CypherText, 0, CypherText.Length);
			
			return result;

		}
		
		/* This method performs a 1 bit left rotation */
		public static byte[] RotateLeftBit(byte[] entry)
		{
			if (entry == null)
				return null;
			
			byte[] result = new byte[entry.Length];	
			
			for (int i=0; i<entry.Length; i++)
			{        
				result[i] = (byte) ((entry[i] << 1) & 0x00FF);
				if (i == (entry.Length-1))
				{
					if ( (entry[0] & 0x80) == 0x80)
						result[i] |= 0x01;    
				} else
				{
					if ( (entry[i+1] & 0x80) == 0x80)
						result[i] |= 0x01;
				}
			}
			
			return result;
			
		}
	
		/* This method increments the Sequence number */
		public static byte[] IncrementSeq(byte[] seq)
		{	
			Int64 val=0;
			byte[] res;
			
			if (seq == null)
				return null;
			
			if (seq.Length != 4)
				return null;	
			
			res = new byte[4];
			
			/* Particular case FFFFFFFF --> 00000000 */
			if((seq[0] == 0xFF) && (seq[1] == 0xFF) && (seq[2] == 0xFF) && (seq[3] == 0xFF)  )
			{
				res[0] = 0;
				res[1] = 0;
				res[2] = 0;
				res[3] = 0;
				return res;
			}
			
			val  = seq[0] & 0x00FF;
			val <<= 8;
			val += seq[1] & 0x00FF;
			val <<= 8;
			val += seq[2] & 0x00FF;
			val <<= 8;
			val += seq[3] & 0x00FF;
			
			val++;
			
			res[0] = (byte)((val >> 24) & 0x000000FF);
			res[1] = (byte)((val >> 16) & 0x000000FF);
			res[2] = (byte)((val >> 8) & 0x000000FF);
			res[3] = (byte)((val >> 0) & 0x000000FF);
	
			return res;
	
		}
		
		/* This method calculates the CMAC over the type + payload */
		public static byte[] CalculateCMAC(byte type, byte[] payload, byte[] key, byte[] seq)
		{
			if (key == null)
				return null;
				
			if (key.Length != 16)
				return null;
				
			if (seq == null)
				return null;
				
			if (seq.Length != 4)
				return null;
			
			byte payloadLen;
			if (payload == null)
				payloadLen=0;
			else
				payloadLen = (byte) (payload.Length);
			
			byte length= (byte) (payloadLen );
			byte[] H = new byte[8];
			byte[] T;
			int TLength;
			
			/* Construct H	*/
			for (int i=0; i<4; i++)
				H[i] = seq[i];
				
			H[4] = type;
			H[5] = length;
			H[6] = (byte) (0xFF ^ type);
			H[7] = (byte) (0xFF ^ length);
			
			/* Construct T	*/
			if ((payloadLen + H.Length)%16 > 0)
				TLength = ((payloadLen + H.Length)/16 + 1) * 16;
			else
				TLength = (payloadLen + H.Length);
			
			T = new byte[TLength];
			int offset = 0;
			for (int i=0; i<H.Length; i++)		
				T[offset++] = H[i];
			for (int i=0; i<payloadLen; i++)		
				T[offset++] = payload[i];
			
			/* Padd T if necessary	*/
			if ((payloadLen + H.Length)%16 > 0)
			{
				T[offset++] = (byte) 0x80;
				for (int i=offset; i<T.Length; i++)
					T[i] = 0x00;
			}
						
			/* Cipher T	*/
			byte[] IV = new byte[16] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			byte[] C = AESCipher(T, key, IV);
			
			/* Get CLast	*/
			byte[] CLast = new byte[16];
			for (int i=0; i<16; i++)
				CLast[i] = C[C.Length-16+i];
			
			/* Extract CMAC	*/
			byte[] CMAC = new byte[8];
			CMAC[0] = CLast[0];
			CMAC[1] = CLast[2];
			CMAC[2] = CLast[4];
			CMAC[3] = CLast[6];
			CMAC[4] = CLast[8];
			CMAC[5] = CLast[10];
			CMAC[6] = CLast[12];
			CMAC[7] = CLast[14];
			
			return CMAC;
			
		}
		
	}
		
#endregion

	/* This class represents a SpringCard IWM2 TCP Device: a HandyDrummer or a FunkyGate	*/
	/* TCP reader. It implements the ISpringCardIWM2_Device: therefore it must implement 	*/
 	/* all the methods defined in the interface. It also inherits from the 								*/
 	/* SpringCardIWM2_Device class: it must override the abstract methods									*/
	
	public class SpringCardIWM2_Network_Device : SpringCardIWM2_Device, ISpringCardIWM2_Device
	{

		
#region Communication mode constants
		public const byte COM_TYPE_PLAIN 									= 0x00;
		public const byte COM_TYPE_SECURED_OPERATION 			= 0x01;
		public const byte COM_TYPE_SECURED_ADMINISTRATION = 0x02;
#endregion
	
	
#region Protocol constants			
		private const int	FREQUENCY_KEEP_ALIVE								 = 45;

		private const byte SECURE_MODE_OPERATION							 = 0x01;
		private const byte SECURE_MODE_ADMINISTRATION					 = 0x02;

		private const byte IWM2_MK2_PROTO_TYPE_MASK 					 = 0x70;
		
		private const byte IWM2_MK2_PROTO_VERSION							 = 0x00;
		
		private const byte IWM2_MK2_PROTO_I_BLOCK 					 	 = 0x00;
		private const byte IWM2_MK2_PROTO_I_BLOCK_CHAINING 		 = 0x10;
		
		private const byte IWM2_MK2_PROTO_I_S_BLOCK					 	 = 0x20;
		private const byte IWM2_MK2_PROTO_I_S_BLOCK_CHAINING 	 = 0x30;
		
		private const byte IWM2_MK2_PROTO_HELO								 = 0x40;
		private const byte IWM2_MK2_PROTO_HELO_OK    					 = 0x50;
		private const byte IWM2_MK2_PROTO_HELO_AUTH						 = 0x70;
		private const byte IWM2_MK2_PROTO_HELO_AUTH_RCVD			 = 0xF0;
		
		private const byte IWM2_MK2_CMD_PUT_TAMPERS       		 = 0x2F;
				
#endregion


#region Private members
		
		/* Full address and communication objects */
		private IPAddress ip;
		private int port;
		private TcpClient client  = null;
		private Stream stream 		= null;
		
		/* Communication type (plain, secured) and the deice's key */
		private byte com_type;
		private byte[] DeviceKey;
		
		/* Two Initilization Vectors: one for the host, one for the device */ 
		private byte[] IVHost;
		private byte[] IVDevice;
		
		/* The session and CMAC keys */
		private byte[] CipherKey;
		private byte[] CMACKey;		
		
		/* The Sequence numbers */
		private byte[] SeqH;
		private byte[] SeqR;
		
		/* Thread-safe blocking FIFO queue, to send messages between the different threads */
		private BlockingCollection<IntraThreadMsg> queue;		
		
		/* This is the main thread, that tries to connect to the object, and	*/
 		/* then listens on the queue, for specific commands from the user 		*/
		private Thread mainThread			 = null;
		
		/* This thread listens on the TCP link for everything that is */
		/* sent by the device, and then analyses each received frame	*/
		private Thread TCPListenThread = null;
					
		/* This is a mutex preventing a Keep Alive and a command	*/
 		/* from being written on the TCP link at the same time		*/
		private Mutex sendingMutex		 = null;
		
		/* This is a timer, to keep track of time */
		private System.Timers.Timer KeepAlivesTimer = null;
		
		/* This boolean indicates whether all cryptographic details must be printed */
		private bool ShowCrypto = false;

		/* This boolean indicates whether the host shall stop trying to connect to the 	*/
 		/* device. This is useful when the main thread has never been able to connect, 	*/
 		/* due to security reasons for example, and so doesn't listen to the queue yet 	*/
		private bool StopReconnect = false;

#endregion


#region Devices's current state
		
		protected enum STATE
		{
			UNKNOWN,
			ACTIVE, 		/* active, in a plain communication mode 		*/
			ACTIVE_S,		/* active, with a secure communication mode */
			ERROR
		}
		protected STATE curState;
		
#endregion


#region Inter-thread messages
		
		/* This constant indicates that the user wants to terminate communication with the device */
		private const byte INT_CMD_TERMINATE = 0x01;
		
		/* This constant indicates that there has been a communication error: main thread must try to reconnect */
		private const byte INT_CMD_RECONNECT = 0x02;
		
		/* This class only contains a byte array: if its length is exactely 1, it must 	 */
		/* be one of the constants defined just above - if its length is greater than 1, */
		/* then it must be a message to send to the device															 */
		protected class IntraThreadMsg
		{
			public byte[] data;
		}
		
#endregion		
		

#region TCP Send

		
		/* This method writes a byte array on the TCP link */
		protected bool TCPSend(byte[] array, int len)
		{
			string s = "Snd: ";
			for (int i=0; i<len; i++)
				s += String.Format("{0:X02}", array[i]);
			log(s);
					
	    /* Use a mutex, to avoid having a Keep Alive and a command sent at the same time */
			sendingMutex.WaitOne(Timeout.Infinite);

			try
			{
				stream.Write(array, 0, len);		
			} catch (Exception)
			{
				/* Don't forget to release the mutex, after writing */
				sendingMutex.ReleaseMutex();	      
				return false;
			}
	
	    /* Don't forget to release the mutex, after writing */
			sendingMutex.ReleaseMutex();
			
			/* Adjust the time until next keep alive*/
			ResetTimer();
		
			return true;
			
		}
	  			
		/* This method sends a message (type + payload) on the TCP link */
		protected bool GenerateAndSendBlock(byte type, byte[] payload, int payload_len)
		{
			/* Reconstruct the frame */
	    /* --------------------- */
	    byte[] buffer = new byte[payload_len+2];
			
	    /* First the length ... */
	    buffer[0] = (byte) (payload_len+2);
	    
	    /* ... then the type ... */
			buffer[1] = type;
	    
	    /* ...and finally, the payload !*/
			for (int i=0; i<payload_len; i++)
				buffer[i+2] = payload[i];
	
	    /* Send it to the device ! */
	    /* ----------------------- */
			return TCPSend(buffer, payload_len+2);
	
		}
		
#endregion
	
	
#region TCP listen and analyse
		
		/* This method analyses a block, part of a frame received from the device on the TCP link */
		private void AnalyseBock(byte[] buffer, int len)
		{   
	    /* The block must be valid: the first byte indicates the length */
	    if (buffer == null)
			{
				log("Empty buffer received");
				return;
			}
	    
			if (buffer[0] != len)
			{
				log("Error: length received");
				OnCommunicationError();
				return;
			}
	
	    /* Second byte indicates the type */
			switch ( buffer[1] & IWM2_MK2_PROTO_TYPE_MASK)
			{
				case IWM2_MK2_PROTO_I_BLOCK :
	        /* We've received an I block ... */
	        if (curState == STATE.ACTIVE)
					{
	          /* ... analyse it ! */
	          if (len > 2)
	          {
	          	byte[] buf = new byte[len-2];
	          	Array.ConstrainedCopy(buffer, 2, buf, 0, len-2);
	          	AnalyseTLVPayload(buf, buf.Length);
	          }
					} else
					{
						/* STATE is not ACTIVE: no I-Block is allowed at this step -> Drop the connection */
						log("No I-Block allowed at this step");
						OnCommunicationError();
						return;
					}
	        
					break;
	
				case IWM2_MK2_PROTO_I_S_BLOCK	:
					/* We've received an Is block ... */
	        if (curState == STATE.ACTIVE_S)
					{
						if (len > 2)
						{	
							/* ... first retrieve the payload (decipher it and check CMAC) ... */
		          byte[] cryptoPayload = new byte[len-2];
		          Array.ConstrainedCopy(buffer, 2, cryptoPayload, 0, len-2);
		          byte[] payload = RetrieveCryptoPayload(buffer[1], cryptoPayload);
						
	            /* ... then analyse it ! */
	            if (payload != null)
								if (payload.Length > 0)
	                AnalyseTLVPayload(payload, payload.Length);
						}
					} else
					{
						/* STATE is not ACTIVE_S: no Is Block is allowed at this step -> Drop the connection */
						log("No Is-Block allowed at this step");
						OnCommunicationError();
						return;
					}
	        
					break;
					
				default :		
					log("Invalid I-Block received");
					OnCommunicationError();
					break;
				
			}
	
		}
		
		/* This method analyses a frame received from the device on the TCP link */
		/* A frame may contain several blocks																		 */
		private void AnalyseFrame(byte[] buffer, int len)
		{   
	    /* The first byte should indicate the length */
	    if (buffer == null)
			{
				log("Empty buffer received");
				return;
			}
	    
			if (buffer[0] == len)
			{
				/* This is a valid frame --> Analyse its content ! */
	      AnalyseBock(buffer, len);
			} else
			if (buffer[0] > len)
			{
				/* Frame too short */
				log("Error: length received");
				OnCommunicationError();
				return;
				
			} else
			{
				/* There must be several blocks within this frame: analyse them one after another	*/
				int offset=0;
				byte[] tmp;
								
				while(offset < len)
				{
					tmp = new byte[buffer[offset]];
					Array.ConstrainedCopy(buffer, offset, tmp, 0, buffer[offset]);
					offset += buffer[offset];
					
					AnalyseBock(tmp, tmp.Length);
				
				}
				
				
			}
	
		}
		
		/* This method runs in a thread as long as the device's status is active (in a plain or secured communication 	*/
 		/* mode): it performs a blocking read on the TCP link, and then analyses what has been received									*/
		private void ListenTCPDevice()
		{
			int count = 0;
			string s;
			byte[] buffer = new byte[100];
			
			while ((curState == STATE.ACTIVE) || (curState == STATE.ACTIVE_S))
			{
				/* Blocking read until something arrives ... */		
				try
				{
					stream.ReadTimeout = Timeout.Infinite;
					count = stream.Read(buffer, 0, buffer.Length);
				} catch (Exception)
				{
					
					if ((curState == STATE.ACTIVE) || (curState == STATE.ACTIVE_S))
					{
						/* Main thread doesn't know there is an error yet */
						OnCommunicationError();
					} else
					{
						/* Main thread already knows there is an error,	*/ 
 						/* and has closed the connexion	--> do nothing	*/
					}
					return;
				}
				
				s = "Rcv: ";
				for (int i=0; i<count; i++) 
					s += String.Format("{0:X02}", buffer[i]);
				log(s);

				if (count <= 0)
				{
					OnCommunicationError();
					return;
				}

				/* ... then, analyse the received frame       */
				AnalyseFrame(buffer, count);
				
			}
			
		}		
		
		/* This method creates the background thread, that listens */
		/* on the TCP link, and analyses what has been received		 */
		private void LaunchTCPListenThread()
		{
			TCPListenThread = new Thread(() => ListenTCPDevice());
			TCPListenThread.Start();
		}
		
#endregion
	
		
#region Authentication and ciphering

		/* This method adds the CMAC, and then ciphers the payload */ 
		private byte[] GenerateCryptoPayload(byte type, byte[] buffer)
		{
			byte bufferLen;
			if (buffer == null)
				bufferLen = 0;
			else
				bufferLen = (byte) (buffer.Length);
			
			/* Compute CMAC and increment the Host Sequence number */
			byte[] CMAC = Crypto.CalculateCMAC(type, buffer, CMACKey, SeqH);
			SeqH = Crypto.IncrementSeq(SeqH);
			
			/* Compute p	*/
			int p = 16 - (bufferLen + CMAC.Length)%16;
	
			/* Compute T	*/
			byte[] T = new byte[p];
			for (int i=0; i<p; i++)
				T[i] = (byte) p;
			
			/* Compute P'' */
			byte[] Ppp = new byte[bufferLen + CMAC.Length + p];
			int offset = 0;
			for (int i=0; i<bufferLen; i++)
				Ppp[offset++] = buffer[i];
			for (int i=0; i<CMAC.Length; i++)
				Ppp[offset++] = CMAC[i];		
			for (int i=0; i<T.Length; i++)
				Ppp[offset++] = T[i];		
		
			/* Cipher P''	*/
			byte[] res = Crypto.AESCipher(Ppp, CipherKey, IVHost);
		
			/* Remember IVHost */
			Array.ConstrainedCopy(res, res.Length - 16, IVHost, 0, 16);
			
			if (ShowCrypto)
	    	logArray("Ciphered", res);
			
	    return res;
	    
		}
		
		/* This method deciphers the received payload, then checks CMAC */ 
		private byte[] RetrieveCryptoPayload(byte type, byte[] buffer)
		{
	    if (buffer == null)
				return null;
				
			if (buffer.Length % 16 != 0)
				return null;
			
			/* Decipher	*/
			byte[] deciphered = Crypto.AESDecipher(buffer, CipherKey, IVDevice);
	    if (deciphered == null)
	    {
	 			log("Can't decipher received frame");
	 			OnCommunicationError();
				return null;   
	    }
	    
	    if (ShowCrypto)
	    	logArray("Deciphered", deciphered);
	    
	    /* Remember IVDevice */
			Array.ConstrainedCopy(buffer, buffer.Length - 16, IVDevice, 0, 16);
	    
			/* Check padding	*/
			byte nbBytesAdded = deciphered[deciphered.Length - 1];
			if ((nbBytesAdded+8) > deciphered.Length)
			{
				log("Ciphered frame - overflow");
				OnCommunicationError();
				return null;
			}
			
			for (int i=0; i<nbBytesAdded; i++)
			{
				if (deciphered[deciphered.Length - nbBytesAdded + i] != nbBytesAdded)
				{
					log("Wrong padding");
					OnCommunicationError();
	        return null;
				}
			}
				
			/* Retrieve CMAC */
			byte[] receivedCMAC = new byte[8];
			try
	    {
	      for (int i=0; i<8; i++)
	        receivedCMAC[i] = deciphered[deciphered.Length - nbBytesAdded - 8 + i];
	    } catch (Exception e)
	    {
	      log("Error retrieving CMAC: " + e);
	      OnCommunicationError();
	      return null;   
	    }
			
			/* Retrieve original Payload	*/
			byte[] originalPayload = new byte[deciphered.Length - nbBytesAdded - 8];
			for (int i=0; i<deciphered.Length - nbBytesAdded - 8; i++)
				originalPayload[i] = deciphered[i];
			if (ShowCrypto)
				logArray("Plain payload received", originalPayload);
				
			/* Calculate CMAC on received payload	*/
			byte[] CMAC = Crypto.CalculateCMAC(type, originalPayload, CMACKey, SeqR);
			SeqR = Crypto.IncrementSeq(SeqR);
			
			/* CMAC comparison	*/
			for (int i=0; i<CMAC.Length; i++)
			{
				if (CMAC[i] != receivedCMAC[i])
				{
					log("Wrong CMAC");
					OnCommunicationError();
	        return null;
				}
			}
			
			return originalPayload;			
		}
		
#endregion


#region Keep alives

		/* This method resets the timer. It is called		*/ 
 		/* each	time something is sent on the TCP link	*/
		private void ResetTimer()
		{
			if (KeepAlivesTimer != null)
			{
				KeepAlivesTimer.Stop();
				KeepAlivesTimer.Start();			
			}
			
		}
		
		/* This method creates the timer that sends Keep Alives */
		private void PlanifyKeepAlives()
		{				
			KeepAlivesTimer = new System.Timers.Timer(1000 * FREQUENCY_KEEP_ALIVE);
			KeepAlivesTimer.AutoReset = true;
			KeepAlivesTimer.Enabled = true;
			KeepAlivesTimer.Elapsed += (object sender, System.Timers.ElapsedEventArgs e) => SendKeepAlive();
			KeepAlivesTimer.Start();
		}
		
		/* This method sends the Keep Alives */
		private void SendKeepAlive()
		{
			if (curState == STATE.ACTIVE)
			{
				if (!GenerateAndSendBlock(IWM2_MK2_PROTO_I_BLOCK, null, 0))
					OnCommunicationError();
			
			} else
			if (curState == STATE.ACTIVE_S)
			{
				/* Secured mode: add CMAC + cipher the payload, then send it */
				if (ShowCrypto)
					log("Plain payload to send: (Keep Alive)");
				byte[] cryptPL = GenerateCryptoPayload(IWM2_MK2_PROTO_I_S_BLOCK, null);
				if (!GenerateAndSendBlock(IWM2_MK2_PROTO_I_S_BLOCK, cryptPL, cryptPL.Length))
					OnCommunicationError();
			}
			
		}

		
#endregion


#region Communication through queue

		/* This method runs in the main thread: it performs	*/
 		/* a blocking read on the thread-safe FIFO queue		*/
		private void AnalyseQueue()
		{
			IntraThreadMsg Msg;
			while ((curState == STATE.ACTIVE) || (curState == STATE.ACTIVE_S))
			{
				/* Blocking read: the method waits until there is something in the queue */
				queue.TryTake(out Msg, Timeout.Infinite);
				if (Msg.data.Length == 1)
				{
					if (Msg.data[0] == INT_CMD_TERMINATE)
					{
						/* Join the threads and terminate connection	 */
						DropConnection(false);
						
					} else
					if (Msg.data[0] == INT_CMD_RECONNECT)
					{
						/* A problem has been detected: try to reconnect */
						DropConnection(true);
					}
				} else
				{
					/* This is a command to send to the device */
					if (curState == STATE.ACTIVE)
					{
						/* Send in non-secured mode */
						if (!GenerateAndSendBlock(IWM2_MK2_PROTO_I_BLOCK, Msg.data, Msg.data.Length))
			      	DropConnection(true);
					}
					else
					if (curState == STATE.ACTIVE_S)
					{
						/* Secured mode: add CMAC + cipher the payload, then send it */ 
						if (ShowCrypto)
							logArray("Plain payload to send", Msg.data);
			      byte[] cryptPL = GenerateCryptoPayload(IWM2_MK2_PROTO_I_S_BLOCK, Msg.data);
			      if (!GenerateAndSendBlock(IWM2_MK2_PROTO_I_S_BLOCK, cryptPL, cryptPL.Length))
			      	DropConnection(true);
					
					}

				}
				
			}
			
		}
		
		/* This method is called when the user wants to terminate the connection */
		public void Terminate()
		{
			/* Enqueue the one byte message in the FIFO queue */
			StopReconnect = true;
			byte[] cmd = new byte[1] { INT_CMD_TERMINATE }; 

			IntraThreadMsg msg = new IntraThreadMsg
			{
				data = cmd
			};
						
			queue.Add(msg);
			
		}
		
		/* This method is called a communication error has been encountered */
		protected override void OnCommunicationError()
		{
			/* Enqueue the one byte message in the FIFO queue */
			
			byte[] cmd = new byte[1] { INT_CMD_RECONNECT };

			IntraThreadMsg msg = new IntraThreadMsg
			{
				data = cmd
			};
						
			queue.Add(msg);			
		}
		
		/* This method adds the message in the FIFO queue */
		protected override void EnqueueCommand(byte[] cmd)
		{
			IntraThreadMsg msg = new IntraThreadMsg
			{
				data = cmd
			};
						
			queue.Add(msg);					
			
		}
		
#endregion
	
	
#region Connect to device
		
		/* This method checks whether the protocol version is ok */
		private bool IsProtocolVersionOK(byte b)
		{
			if ((b & 0x0F) != 0x00)
				return false;
	
			return true;
		}
		
		/* This method waits for "sec" seconds, and then tries to reconnect */
		/* It is called when a connection or protocol error is encountered  */
		private void WaitAndTryToReconnect(int sec)
		{
			Thread.Sleep(sec * 1000);
			EstablishConnection();
		}	
		
		/* This method checks whether a valid HELO block has been received */
		private bool IsHelloBlockRcv()
		{
			int count = 0;
			byte[] buffer = new byte[100];
			
			if (stream == null)
				return false;
			
			try 
			{
				stream.ReadTimeout = 3000;
				count = stream.Read(buffer, 0, buffer.Length);
			} catch (Exception)
			{
				return false;
			}
			
			string s = "Rcv: ";
			for (int i=0; i<count; i++) 
				s += String.Format("{0:X02}", buffer[i]);
			log(s);
			
			/* First byte should be the length */  
			if (buffer[0] != count)
				return false;
		

  	  /* Second byte should be the type */
			if ( (buffer[1] & IWM2_MK2_PROTO_TYPE_MASK) != IWM2_MK2_PROTO_HELO )
			{
				log("Error: wrong block received");
				return false;
			}

  	  /* Check protocol version */
			if (!IsProtocolVersionOK(buffer[1]))
				return false;
			
			return true;
		
		}

		/* This method sends a HELO-OK Block */
		private bool SendHeloOK(byte[] buffer)
		{
			if (buffer == null)
			{
				/* Corresponds to the non-secured mode */
	      return GenerateAndSendBlock((byte) (IWM2_MK2_PROTO_HELO_OK | IWM2_MK2_PROTO_VERSION), null, 0);
			} else
			{
				/* Correspondes to the secured mode */
	      return GenerateAndSendBlock((byte) (IWM2_MK2_PROTO_HELO_OK | IWM2_MK2_PROTO_VERSION), buffer, buffer.Length);
			}
		}

		/* This method sends the first HELO-Auth block*/
		private bool SendHelloAuth(byte type)
		{
	    /* Type can be 'Operation' or 'Administration' */
	    return GenerateAndSendBlock((byte) (IWM2_MK2_PROTO_HELO_AUTH | type), null, 0);			
		}
		
		/* This method sends the second HELO-Auth block */
		private bool SendHeloAuth2(byte[] E_concat)
		{
			if (E_concat != null)
			{
				return GenerateAndSendBlock((byte) (IWM2_MK2_PROTO_HELO_AUTH), E_concat, E_concat.Length);
			} else
			{
				return false;
			}
		}
		
		/* This method performs the last part of a "plain" connection establishment */
		private void CompletePlainConnection()
		{
			if (!SendHeloOK(null))
			{
				if (_device_status_changed_callback != null)
					_device_status_changed_callback(IWM2_DEVICE_STATUS_DISCONNECTED);		

				WaitAndTryToReconnect(5);
			
			} else
			{
				/* The current state becomes ACTIVE */
				curState = STATE.ACTIVE;
				
				/* Tell that we are connected to the device */
				if (_device_status_changed_callback != null)
					_device_status_changed_callback(IWM2_DEVICE_STATUS_CONNECTED);		
				
				/* Ask for the device's global status */				
				GetGlobalStatus();
				
				/* Launch the timer that sends the Keep Alives ... */
				PlanifyKeepAlives();
				
				/* ... and the thread that listens for incoming messages from the device ... */ 
				LaunchTCPListenThread();
				
				/* ... and then, analyse what is in the thread-safe FIFO queue */
				AnalyseQueue();					
			}

		}
		
		/* This method checks whether a valid HELO-Auth block has been received */
		private bool IsValidAuthBlock(byte[] buffer, int len)
		{
			/* First byte should be the length */
	    if (buffer[0] != len)
			{
				log("Error: length received");
				return false;
			}
	
			if (len != 0x12)
			{
				log("Error: incorrect length received");
				return false;
			}
			
	    /* Second byte should be the type */
			if ( buffer[1] != IWM2_MK2_PROTO_HELO_AUTH_RCVD )
			{
				log("Error: wrong block received");
				return false;
			}
			
			return true;			
		}
		
		/* This method waits for a HELO-Auth block to be received, 			*/
		/* and then check whether it is valid (see "IsValidAuthBlock")	*/
		private byte[] WaitAndDecipherAuth()
		{
			int count = 0;
			byte[] buffer = new byte[100];
			
			if (stream == null)
				return null;
			
			try 
			{
				stream.ReadTimeout = 3000;
				count = stream.Read(buffer, 0, buffer.Length);
			} catch (Exception)
			{
				return null;
			}
					
			string s = "Rcv: ";
			for (int i=0; i<count; i++) 
				s += String.Format("{0:X02}", buffer[i]);
			log(s);
			
			if (!IsValidAuthBlock(buffer, count))
				return null;
			
			/* Decipher the received buffer */
			byte[] ciph = new byte[count-2];
			Array.ConstrainedCopy(buffer, 2, ciph, 0, count-2);
			
			return Crypto.AESDecipher(ciph, DeviceKey, IVDevice);
		}
		
		/* This method performs the last part of a "secured" connection establishment */
		private void CompleteSecureConnection(byte type)
		{
			bool success = true;
			if (!SendHelloAuth(type))
			{
				if (_device_status_changed_callback != null)
					_device_status_changed_callback(IWM2_DEVICE_STATUS_DISCONNECTED);		
				success = false;
			}
			
			IVHost = new byte[16] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			IVDevice = new byte[16] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			
			/* Get Cr from first AUTH block	*/
			byte[] Cr = null;
			if (success)
			{
				Cr = WaitAndDecipherAuth();
				if (Cr == null)
				{
					if (_device_status_changed_callback != null)
						_device_status_changed_callback(IWM2_DEVICE_STATUS_DISCONNECTED);							
					success = false;
				} else
				{
					if (ShowCrypto)
						logArray("Cr", Cr);
				}
			}

			/* Compute Cr' */
			byte[] Crp = Crypto.RotateLeftBit(Cr);
			if (Crp != null)
				if (ShowCrypto)
					logArray("Crp", Crp);

			
			/* Compute Ch */
			byte[] Ch;
			Ch = new byte[16];
			Random rand = new Random();
			for (int i=0; i<Ch.Length; i++)
				Ch[i] = (byte) rand.Next(0x00, 0xFF);
			
			if (success)			
				if (ShowCrypto)
					logArray("Ch", Ch);

			
			/* Concatene Ch and Cr' */
			byte[] ChCrp;
			byte[] E_ChCrp = null;
			if (success)
			{
				ChCrp = new byte[32];
				Array.ConstrainedCopy(Ch, 0, ChCrp, 0, 16);
				Array.ConstrainedCopy(Crp, 0, ChCrp, 16, 16);
				
				if (ShowCrypto)
					logArray("Ch||Cr'", ChCrp);
				
				E_ChCrp = Crypto.AESCipher(ChCrp, DeviceKey, IVHost);
				if (ShowCrypto)
					logArray("E(K, Ch||Cr')", E_ChCrp);
				
			}
			
			/* Send AUTH block, step 2 */
			if (success)
			{
				if (!SendHeloAuth2(E_ChCrp))
				{
					success = false;
					if (_device_status_changed_callback != null)
						_device_status_changed_callback(IWM2_DEVICE_STATUS_DISCONNECTED);			
				}
			}
					

			/* Wait for AUTH block, step 3	*/
			byte[] Chp_r = null;
			if (success)
			{
				Chp_r = WaitAndDecipherAuth();
				if (Chp_r == null)
				{
					success = false;
					if (_device_status_changed_callback != null)
						_device_status_changed_callback(IWM2_DEVICE_STATUS_DISCONNECTED);			
					
				} else
				{
					if (ShowCrypto)
						logArray("Ch' received", Chp_r);
				}
			}
			
			/* Calculate Ch' */
			byte[] Chp = null;
			if (success)
			{
				Chp = Crypto.RotateLeftBit(Ch);
				if (ShowCrypto)
					logArray("Ch'", Chp);
			}
		
			/* Check Ch' received */
			if (success)
			{
				for (int i=0; i<Chp.Length; i++)
				{
					if (Chp[i] != Chp_r[i])
					{
						success = false;
						if (_device_status_changed_callback != null)
							_device_status_changed_callback(IWM2_DEVICE_STATUS_DISCONNECTED);			
					}	
				}
			}
			
			/* Generate Cipher Key */
			byte[] T = new byte[16];
			if (success)
			{
				T[0] = Ch[11];
				T[1] = Ch[12];
				T[2] = Ch[13];
				T[3] = Ch[14];
				T[4] = Ch[15];
				T[5] = Cr[11];
				T[6] = Cr[12];
				T[7] = Cr[13];
				T[8] = Cr[14];
				T[9] = Cr[15];
				T[10] = (byte) ((Ch[4] ^ Cr[4]) & 0x00FF);
				T[11] = (byte) ((Ch[5] ^ Cr[5]) & 0x00FF);
				T[12] = (byte) ((Ch[6] ^ Cr[6]) & 0x00FF);
				T[13] = (byte) ((Ch[7] ^ Cr[7]) & 0x00FF);
				T[14] = (byte) ((Ch[8] ^ Cr[8]) & 0x00FF);
				T[15] = 0x11;	
				CipherKey = Crypto.AESCipher(T, DeviceKey, IVHost);
				if (ShowCrypto)
					logArray("Cipher Key", CipherKey);
			}
			
			/* Generate CMAC Key */
			if (success)
			{
				T = new byte[16];
				T[0] = Ch[7];
				T[1] = Ch[8];
				T[2] = Ch[9];
				T[3] = Ch[10];
				T[4] = Ch[11];
				T[5] = Cr[7];
				T[6] = Cr[8];
				T[7] = Cr[9];
				T[8] = Cr[10];
				T[9] = Cr[11];
				T[10] = (byte) ((Ch[0] ^ Cr[0]) & 0x00FF);
				T[11] = (byte) ((Ch[1] ^ Cr[1]) & 0x00FF);
				T[12] = (byte) ((Ch[2] ^ Cr[2]) & 0x00FF);
				T[13] = (byte) ((Ch[3] ^ Cr[3]) & 0x00FF);
				T[14] = (byte) ((Ch[4] ^ Cr[4]) & 0x00FF);
				T[15] = 0x22;
				CMACKey = Crypto.AESCipher(T, DeviceKey, IVHost);
				if (ShowCrypto)
					logArray("CMAC Key", CMACKey);
			}
			

			/* Generate Nh */
			byte[] Nh = new byte[16];
			for (int i=0; i<Nh.Length; i++)
				Nh[i] = (byte) rand.Next(0x00, 0xFF);			
			
			if (success)
				if (ShowCrypto)
					logArray("Nh", Nh);
			
			/* Generate the Payload */
			SeqH = new byte[4] { 0, 0, 0, 0 };
			SeqR = new byte[4] { 0, 0, 0, 0 };
			byte[] payload = null;
			if (success)
			{
				payload = GenerateCryptoPayload((byte) (IWM2_MK2_PROTO_HELO_OK | IWM2_MK2_PROTO_VERSION), Nh);
				if (payload == null)
				{
					success = false;
					if (_device_status_changed_callback != null)
						_device_status_changed_callback(IWM2_DEVICE_STATUS_DISCONNECTED);							
				}
			}
			
			
			/* Copy the Initialization Vectors */
			if (success)
				Array.ConstrainedCopy(IVHost, 0, IVDevice, 0, 16);
			
			/* Send final HELO-ok block */
			if (success)
			{
				if (!SendHeloOK(payload))
				{
					success = false;
					if (_device_status_changed_callback != null)
						_device_status_changed_callback(IWM2_DEVICE_STATUS_DISCONNECTED);							
				}
			}
			
			if (!success)
			{
				WaitAndTryToReconnect(5);
			} else
			{
				/* The current state becomes ACTIVE_S */
				curState = STATE.ACTIVE_S;
				
				/* Tell that we are connected to the device */
				if (_device_status_changed_callback != null)
					_device_status_changed_callback(IWM2_DEVICE_STATUS_CONNECTED);		
				
				/* Ask for the device's global status */				
				GetGlobalStatus();
				
				/* Launch the timer that manages the Keep Alives ... */
				PlanifyKeepAlives();
				
				/* ... and the thread that listens for incoming messages from the device ... */ 
				LaunchTCPListenThread();
				
				/* ... and then, analyse what is in the thread safe FIFO queue */
				AnalyseQueue();		
			}
			
		}
		
		/* This method is run in the main thread. It tries to connect and */
 		/* then analyse the FIFO queue for internal messages or commands  */
		private void EstablishConnection()
		{
			bool success = false;
			var delay = 5; /* seconds */
			
			if (StopReconnect)
			{
				/* Terminate connection	 */
				DropConnection(false);
				return;
			}
						
			
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_STARTED);
			
			
			/* Asynchonous connection, with a limit of 5 seconds */
			try 
			{
				client = new TcpClient();
				IAsyncResult result = client.BeginConnect(ip, port, null, null);
				success = result.AsyncWaitHandle.WaitOne(TimeSpan.FromSeconds(delay));
				
			}	catch (Exception e) 
			{
				log(e.StackTrace);
				if (_device_status_changed_callback != null)
					_device_status_changed_callback(IWM2_DEVICE_STATUS_CONNECTION_FAILED);
				
				return;
			}		

			if (!success)
			{
				log("Error connecting to the device");
				if (_device_status_changed_callback != null)
					_device_status_changed_callback(IWM2_DEVICE_STATUS_CONNECTION_FAILED);
			}
					
			if (success)
			{
				try
				{
					stream = client.GetStream();
				} catch (Exception)
				{
					success = false;
					log("Error connecting to the device");
					if (_device_status_changed_callback != null)
						_device_status_changed_callback(IWM2_DEVICE_STATUS_CONNECTION_FAILED);
				}
			}
			
			/* Check if a Hello block is received */
			if (success)
			{
				if (!IsHelloBlockRcv())
				{
					if (_device_status_changed_callback != null)
						_device_status_changed_callback(IWM2_DEVICE_STATUS_CONNECTION_FAILED);		
					
					success = false;
				
				}
				
			}
			
			/* Complete the last part of the connection, depending	*/
			/* on the desired mode (plain, or secured)							*/
			if (success)
			{
				switch (com_type)
				{
					case COM_TYPE_SECURED_OPERATION :								
					case COM_TYPE_SECURED_ADMINISTRATION :
						CompleteSecureConnection(com_type);
						break;
						
					case COM_TYPE_PLAIN:
					default:
						CompletePlainConnection();
						break;
				}
				

			} else
			{
				/* Something is wrong: wait for 5 seconds, and try to reconnect */
				WaitAndTryToReconnect(5);
			}
			
		}

		/* This method is called when the user wants to start the connection */
		public void Start()
		{
			
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_CREATED);
			
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_STARTING);
						
			/* Connection is done in a background thread (the "main" thread), to avoid freezing main form */
			mainThread = new Thread(() => EstablishConnection());
			mainThread.Start();
		
		}
		
#endregion	
	
	
#region Drop Connection
		
		/* This method drops the connection with the device. The startAgain */
 		/* boolean indicates whether connection must be re-established			*/
		private void DropConnection(bool startAgain)
		{

			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_DISCONNECTED);
						
			/* A communication error occured ! */
	    curState = STATE.ERROR;
			
 	    /* Interrupt the sending of 'Keep Alive' messages */			
			if (KeepAlivesTimer != null)
			{
				KeepAlivesTimer.Dispose();
				KeepAlivesTimer.Close();
				KeepAlivesTimer = null;
			}
 			
	    /* Close the socket -> this will terminate the socket-listening thread */		
	    if (stream != null)
	    {
	    	stream.Dispose();
				stream = null;
	    }
	    
	    if (client != null)
	    {
	    	client.Close();
	    	client = null;
	    }			
	    
			/* Wait for the socket-listening thread to return */
			if (TCPListenThread != null)
			{
				TCPListenThread.Join();
				TCPListenThread = null;
			}
	

	    if (startAgain)
	    {
	    	WaitAndTryToReconnect(5);
	    } else
	    {
				if (_device_status_changed_callback != null)
					_device_status_changed_callback(IWM2_DEVICE_STATUS_TERMINATED);	    	
	    }
	    	
			
		}
		
#endregion


#region Log
		
		public void SetShowCrypto(bool val)
		{
			ShowCrypto = val;
		}
	
		
		private void logArray(string title, byte[] array)
		{
			string s = title+ ": ";
			for (int i=0; i<array.Length; i++)
				s += String.Format("{0:X02}", array[i]);
			log(s);
		}

		protected override void log(string msg)
		{
			SystemConsole.Verbose(ip.ToString()+":"+port+": " + msg);
		}		
	
#endregion
		
		
#region Constructor
		
		public SpringCardIWM2_Network_Device(IPAddress _ip, int _port, byte _com_type, byte[] _key)
		{
			ip = _ip;
			port = _port;
			com_type = _com_type;
			DeviceKey = _key;
			curState = STATE.UNKNOWN;
			sendingMutex = new Mutex();
			queue = new BlockingCollection<IntraThreadMsg>();
		}
		
#endregion
		
	}
}
