package com.springcard.fkgtcp;

import java.io.*;
import java.net.*;

import java.util.Arrays;
import java.util.Timer;
import java.util.TimerTask;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import java.security.SecureRandom;

import java.nio.ByteBuffer;

abstract class Connector
{
	public enum STATE
	{
		UNKNOWN,
		ACTIVE,
		ACTIVE_S,
		ERROR
	}	
	
	
	class ReadBuffer
	{
		public int len;
		public byte[] buffer;
		
		public ReadBuffer(int l, byte[] b)
		{
			len=l;
			buffer = b;
		}	
	}
	
	
	protected String IP;
	protected int PORT;
	protected String MAC;
	protected STATE curState;
	protected Socket tcpsocket = null;
	protected Listener listener;
	
	protected Thread listen_thread = null;
	protected TimerTask KeepAlive = null;
	protected Timer timer_KeepAlive = null;
	
	private static final Lock send_mutex = new ReentrantLock();
	
	private final static int	FREQUENCY_KEEP_ALIVE								= 45;
	
	private final static byte SECURE_MODE_OPERATION								= 0x01;
	private final static byte SECURE_MODE_ADMINISTRATION					= 0x02;
	
	private final static byte IWM2_MK2_PROTO_TYPE_MASK 						= 0x70;
	
	private final static byte IWM2_MK2_PROTO_VERSION							= 0x00;
	
	protected final static byte IWM2_MK2_PROTO_I_BLOCK 						= 0x00;
	private final static byte IWM2_MK2_PROTO_I_BLOCK_CHAINING 		= 0x10;
	
	protected final static byte IWM2_MK2_PROTO_I_S_BLOCK					= 0x20;
	protected final static byte IWM2_MK2_PROTO_I_S_BLOCK_CHAINING = 0x30;
	
	private final static byte IWM2_MK2_PROTO_HELO									= 0x40;
	private final static byte IWM2_MK2_PROTO_HELO_OK    					= 0x50;
	private final static byte IWM2_MK2_PROTO_HELO_AUTH						= 0x70;
	private final static int  IWM2_MK2_PROTO_HELO_AUTH_RCVD				= 0xF0;
	
	private final static byte IWM2_MK2_CMD_PUT_TAMPERS       			= 0x2F;
	
	private final static int IWM2_MK2_DEVICE_ID								 		= 0x81;
	
	private final static int IWM2_MK2_CMD_PUT_TAG              		= 0xB0;
	private final static int IWM2_MK2_CMD_PUT_TAG_INSERT_EJECT 		= 0xB1;
	
	private final static int HANDY_DRUMMER_INPUT_CHANGED			 		= 0xC0;
	
	protected abstract void onBadgeRead(byte[] nb, int len);
	protected abstract void onInNotified(byte input, byte value);
	
	private byte[] KeyDevice;
	private byte[] IVHost;
	private byte[] IVDevice;
	
	private byte[] CipherKey;
	private byte[] CMACKey;
	
	private byte[] SeqH;
	private byte[] SeqR;
  
  /* Method that writes on the TCP Socket */ 	
	protected void TCPSend(byte[] array, int len)
	{
    PrintArray.out(4, tcpsocket.toString(), array, len);
		
    /* Use a mutex, to avoid having two processes writing on the socket at the same time */
		send_mutex.lock();
		try
		{
			OutputStream os = tcpsocket.getOutputStream();
			os.write(array);			
		} catch (IOException e)
		{
			System.out.println(tcpsocket.toString() + ": IOException: " + e.toString());
			
      /* On error: drop the connexion */
      dropConnexion();
		}

    /* Don't forget to release the mutex, after writing */
		send_mutex.unlock();
		
	}
  
	/* Method that tries to read on the TCP Socket,  */
  /* until something arrives, or a timeout occurs  */	
	private ReadBuffer TCPRead(int timeout)
	{

    InputStream is;
		int len;
		byte[] buffer = new byte[256];
		ReadBuffer rb;
		try
		{
			tcpsocket.setSoTimeout(timeout);
			is = tcpsocket.getInputStream();
			len = is.read(buffer);	
			PrintArray.in(4, tcpsocket.toString(), buffer, len);	
			rb = new ReadBuffer(len, buffer);
			return rb;

		} catch (IOException e)
		{
			System.out.println(tcpsocket.toString() + ": IOException: " + e.toString());
			
      /* On error, or if nothing can be read: drop the connexion */
      dropConnexion();
			return null;
		} 
		
	}	

	public String getIP()
	{
		return IP;
	}
	
	public int getPort()
	{
		return PORT;
	}
	
  public STATE getCurState()
  {
    return curState;
  }
  
  private boolean isLengthOk(byte[] buffer)
	{
		/* Length must be a multiple of 16  */
    if (buffer == null)
			return false;
			
		if (buffer.length%16 == 0)
			return true;
		
		return false;
	}
  
	/* Method that deciphers the received buffer, during authentication process */	
	private byte[] decipherBuffer(byte[] buffer)
	{
    if (buffer == null)
			return null;
			
		PrintArray.internal(6, tcpsocket.toString() + "-to decipher", buffer, buffer.length);

		byte[] res = SCrypto.AES_Decipher(buffer, KeyDevice, IVDevice);
		PrintArray.internal(6, tcpsocket.toString()+ "-deciphered", res, res.length);
		PrintArray.internal(6, tcpsocket.toString()+"-IV", IVDevice, IVDevice.length);

		return res;
	}
  
	/* Method that ciphers a buffer, during authentication process */
	private byte[] cipherBuffer(byte[] buffer)
	{  
    /* Check length */
    if (!isLengthOk(buffer))
      return null;
    
		PrintArray.internal(6, tcpsocket.toString() + "-to cipher", buffer, buffer.length);

    /* AES Cipher */
		byte[] res = SCrypto.AES_Cipher(buffer, KeyDevice, IVHost);
		
		PrintArray.internal(6, tcpsocket.toString()+ "-ciphered", res, res.length);
		PrintArray.internal(6, tcpsocket.toString()+"-IV", IVHost, IVHost.length);

		return res;
	}
	
  /* Method that adds the CMAC and ciphers the payload    */
  /* to send to the device, when current mode is secured. */
	protected byte[] generateCryptoPayload(byte type, byte[] buffer)
	{
    byte bufferLen;
		if (buffer == null)
			bufferLen = 0;
		else
			bufferLen = (byte) (buffer.length);
			
		/* Compute CMAC	*/
		byte[] CMAC = SCrypto.calculateCMAC(type, buffer, CMACKey, SeqH);
		SeqH = SCrypto.incrementSeq(SeqH);
		PrintArray.internal(6, tcpsocket.toString()+"-CMAC", CMAC, CMAC.length);
		
		/* Compte p	*/
		int p = 16 - (bufferLen + CMAC.length)%16;

		/* Compute T	*/
		byte[] T = new byte[p];
		for (int i=0; i<p; i++)
			T[i] = (byte) p;
		
		/* Compute P'' */
		byte[] Ppp = new byte[bufferLen + CMAC.length + p];
		int offset = 0;
		for (int i=0; i<bufferLen; i++)
			Ppp[offset++] = buffer[i];
		for (int i=0; i<CMAC.length; i++)
			Ppp[offset++] = CMAC[i];		
		for (int i=0; i<T.length; i++)
			Ppp[offset++] = T[i];		
		
		PrintArray.internal(6, tcpsocket.toString()+"-P''", Ppp, Ppp.length);
		
		/* Cipher P''	*/
		byte[] res = SCrypto.AES_Cipher(Ppp, CipherKey, IVHost);

		return res;
		
	}
	
  /* Method that uses cryptographic functions, to decipher the received */
  /* payload from the device, when current mode is secured.             */
	protected byte[] retrieveCryptoPayload(byte type, byte[] buffer)
	{		   
    if (buffer == null)
			return null;
			
		if (buffer.length%16 != 0)
			return null;
		
		/* Decipher	*/
		byte[] deciphered = SCrypto.AES_Decipher(buffer, CipherKey, IVDevice);
    if (deciphered == null)
    {
 			System.out.println(tcpsocket.toString() + ": Can't decipher received frame");
      dropConnexion();
			return null;   
    }
    
		PrintArray.internal(6, tcpsocket.toString()+"-deciphered", deciphered, deciphered.length);

		/* Check padding	*/
		byte nbBytesAdded = deciphered[deciphered.length - 1];
		if ((nbBytesAdded+8) > deciphered.length)
		{
			System.out.println(tcpsocket.toString() + ": Ciphered frame - overflow");
      dropConnexion();
			return null;
		}
		
		for (int i=0; i<nbBytesAdded; i++)
		{
			if (deciphered[deciphered.length - nbBytesAdded + i] != nbBytesAdded)
			{
				System.out.println(tcpsocket.toString() + ": Wrong padding");
				dropConnexion();
        return null;
			}
		}
			
		/* Retrieve CMAC */
		byte[] receivedCMAC = new byte[8];
		try
    {
      for (int i=0; i<8; i++)
        receivedCMAC[i] = deciphered[deciphered.length - nbBytesAdded - 8 + i];
    } catch (ArrayIndexOutOfBoundsException e)
    {
      System.out.println(tcpsocket.toString() + ": Error retrieving CMAC: " + e);
      dropConnexion();
      return null;   
    }
		
		/* Retrieve original Payload	*/
		byte[] originalPayload = new byte[deciphered.length - nbBytesAdded - 8];
		for (int i=0; i<deciphered.length - nbBytesAdded - 8; i++)
			originalPayload[i] = deciphered[i];
		PrintArray.internal(6, tcpsocket.toString()+"-received Payload", originalPayload, originalPayload.length);	
			
		/* Calculate CMAC on received payload	*/
		byte[] CMAC = SCrypto.calculateCMAC(type, originalPayload, CMACKey, SeqR);
		SeqR = SCrypto.incrementSeq(SeqR);
		
		/* CMAC comparison	*/
		for (int i=0; i<CMAC.length; i++)
		{
			if (CMAC[i] != receivedCMAC[i])
			{
				System.out.println(tcpsocket.toString() + ": Wrong CMAC");
				dropConnexion();
        return null;
			}
		}
		
		return originalPayload;
	
	}
	
	private boolean isProtocolVersionOK(byte b)	
	{
		if ((b & 0x0F) != 0x00)
			return false;

		return true;
		
	}
	
	private boolean isMACAddressOK(byte[] buffer)
	{
    if (MAC == null)
      return true;
		return StringBytes.byteArrayToHexString(buffer).toUpperCase().equals(MAC);
	}
	
	private final String CHARACTERS = "0123456789ABCDEFabcdef";
	private boolean ishex(char c)
	{
		return (CHARACTERS.indexOf(c) > -1);
	}
	
	private boolean isValidKey(String Key)
	{
		/* A valid key is a hexadecimal string, with 32 characters */ 
    if (Key == null)
			return false;
			
		if (Key.length() != 32)
			return false;
		
		for (int i=0; i<Key.length() ;i++)
			if (!ishex(Key.charAt(i)))
				return false;
	  
		return true;
		
	}
	
	protected void generateAndSendBlock(byte type, byte[] payload, int payload_len)
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
		TCPSend(buffer, payload_len+2);

	}
	
	private void sendHeloOK(byte[] buffer)
	{
		if (buffer == null)
		{
			/* Corresponds to the non-secured mode */
      generateAndSendBlock((byte) (IWM2_MK2_PROTO_HELO_OK | IWM2_MK2_PROTO_VERSION), null, 0);
		} else
		{
			/* Correspondes to the secured mode */
      generateAndSendBlock((byte) (IWM2_MK2_PROTO_HELO_OK | IWM2_MK2_PROTO_VERSION), buffer, buffer.length);
		}
	}
	
	private void sendHeloAuth(byte type)
	{
    /* Type can be 'Operation' or 'Administration' */
    generateAndSendBlock((byte) (IWM2_MK2_PROTO_HELO_AUTH | type), null, 0);
	}
	
	private void sendHeloAuth2(byte[] EkChrp)
	{
		if (EkChrp != null)
			generateAndSendBlock((byte) (IWM2_MK2_PROTO_HELO_AUTH), EkChrp, EkChrp.length);
	}
	
	protected void sendInCorrectMode(byte[] payload)
	{
		/* The method sends the payload, in the secured or non-secured mode */
    /* If the payload is null, the message must be a 'Keep-Alive'       */
    
    int payload_len;
    
		if (payload == null)
			payload_len = 0;
		else
			payload_len = payload.length;
			
		if (curState == STATE.ACTIVE)
		{
			/* Mode is non-secured: send the payload as-is */
      generateAndSendBlock(IWM2_MK2_PROTO_I_BLOCK, payload, payload_len);
		} else
		if (curState == STATE.ACTIVE_S)
		{
			/* Mode is secured: Add CMAC + cipher the payload first, and then send it */
      byte[] cryptPL = generateCryptoPayload(IWM2_MK2_PROTO_I_S_BLOCK, payload);
			generateAndSendBlock(IWM2_MK2_PROTO_I_S_BLOCK, cryptPL, cryptPL.length);
		}
	}
	
	private void sendKeepAlive()
	{
		/* Send a 'Keep alive' message: a message with no payload ! */
    sendInCorrectMode(null);
	}
	
	private void planifyKeepAlives()
	{
		/* Use a Timer and a TimerTask to launch the "sendKeepAlive" method every 45 secondes */
    KeepAlive = new TimerTask()
		{
			@Override
			public void run() 
			{
				sendKeepAlive();
			}	
		};
		timer_KeepAlive = new Timer();
		timer_KeepAlive.scheduleAtFixedRate(KeepAlive, FREQUENCY_KEEP_ALIVE*1000, FREQUENCY_KEEP_ALIVE*1000);
	}
	
  /* Method that asks the global status of the device */
	public void getGlobalStatus()
	{  
    /* Generate the TLV */
    byte[] TLV = new byte[2];
		TLV[0] = 0x00;
		TLV[1] = 0x00;
    
		if (curState == STATE.ACTIVE)
		{
			/* Send in non-secured mode */
      generateAndSendBlock(IWM2_MK2_PROTO_I_BLOCK, TLV, 2);
		}	else 
		if (curState == STATE.ACTIVE_S)
		{
			/* Secured mode: add CMAC + cipher the payload, then send it */
      byte[] cryptPL = generateCryptoPayload(IWM2_MK2_PROTO_I_S_BLOCK, TLV);
			generateAndSendBlock(IWM2_MK2_PROTO_I_S_BLOCK, cryptPL, cryptPL.length);
		}
	}
	
  /* Method that analyses the TLV payload */
	private void analyseTLVPayload(byte[] buffer, int len)
	{ 
    /* First byte should be the type */
    switch (buffer[0])
		{
			case (byte) IWM2_MK2_DEVICE_ID 	:
        /* This TLV contains the device ID */
				if (buffer[1] != 0x00)
				{
					System.out.println(tcpsocket.toString() + ": Invalid Tag value");
					dropConnexion();
					return;
				}
				
				if (buffer[2] != len - 3)
				{
					System.out.println(tcpsocket.toString() + ": Invalid Length value");
					dropConnexion();
					return;
				}
				
				String Id;
				try
				{
					Id = new String( Arrays.copyOfRange(buffer, 3, len), "UTF-8");
					System.out.println(tcpsocket.toString() + ": Device identifier: " + Id);
				} catch (UnsupportedEncodingException e)
				{
					System.out.println(tcpsocket.toString() + ": "+e.toString());
				}

				break;
				
			
			case IWM2_MK2_CMD_PUT_TAMPERS	:
				/* This TLV contains the value of the FunkyGate tampers */
        if (buffer[1] != 0x01)
				{
					System.out.println(tcpsocket.toString() + ": Incorrect L value");
					dropConnexion();
					return;
				}
				if (len != 3)
				{
					System.out.println(tcpsocket.toString() + ": Incorrect length");
					dropConnexion();
					return;
				}
        
				if (buffer[2] != 0x00)
					System.out.println(tcpsocket.toString() + ": Warning - tamper value: " + buffer[2]);
				else
					System.out.println(tcpsocket.toString() + ": Tampers ok");
				
        break;
				
				
			case (byte) IWM2_MK2_CMD_PUT_TAG	:
        /* This TLV contains a badge number that has just been read by a FunkyGate */
				if (buffer[1] != 0x00)
				{
					System.out.println(tcpsocket.toString() + ": Invalid Tag value");
					dropConnexion();
					return;
				}
				
				if (buffer[2] != len - 3)
				{
					System.out.println(tcpsocket.toString() + ": Invalid Length value");
					dropConnexion();
					return;
				}
				System.out.println(tcpsocket.toString() + ": Badge Read:");
				PrintArray.internal(4, tcpsocket.toString(), Arrays.copyOfRange(buffer, 3, len), len-3);
				onBadgeRead(Arrays.copyOfRange(buffer, 3, len), len-3);
				break;		
			
			
			case (byte) IWM2_MK2_CMD_PUT_TAG_INSERT_EJECT :
        /* This TLV contains a badge number that has been inserted or removed: this is not supported */
				System.out.println(tcpsocket.toString() + ": tag inserted or removed - No other actions");
				break;
			
			case (byte) HANDY_DRUMMER_INPUT_CHANGED :
        /* This TLV contains the information that an "exit button" has been pressed or released on the HandyDrumer */
				if (len < 3)
				{
					System.out.println(tcpsocket.toString() + ": Handy Drummer: Invalid length received");
					dropConnexion();
					return;
				}
				
				if (buffer[2] != 1)
				{
					System.out.println(tcpsocket.toString() + ": Handy Drummer: Invalid Length value");
					dropConnexion();
					return;
				}
				onInNotified(buffer[1], buffer[3]);
				break;
			
			default:
				/* This TLV is unrecognized */
        System.out.println(tcpsocket.toString() + ": Unrecognized Tag value");

		}
		
		return;
	}
	
  /* Method that analyses the received frame */
	private void analyseFrame(byte[] buffer, int len)
	{   
    /* The first byte should indicate the length */
    if (buffer == null)
		{
			System.out.println(tcpsocket.toString() + ": Empty buffer received");
			return;
		}
		if (buffer[0] == len)
		{
			/* This is a valid frame --> Analyse its content ! */
      analyseBock(buffer, len);
		} else
		if (buffer[0] > len)
		{
			/* Frame too short: drop connexion */
			System.out.println(tcpsocket.toString() + ": Error: length received");
			dropConnexion();
		} else
		{
			/* There must be several blocks within this frame: analyse them one after another	*/
			int offset=0;
			while(offset < len)
			{
				analyseBock(Arrays.copyOfRange(buffer, offset, offset + buffer[offset]), buffer[offset]);
				offset += buffer[offset];
			}
		}

	}
	
  /* Method that analyses a received block */
	private void analyseBock(byte[] buffer, int len)
	{   
    /* The block must be valid: the first byte indicates the length */
    if (buffer == null)
		{
			System.out.println(tcpsocket.toString() + ": Empty buffer received");
			return;
		}
    
		if (buffer[0] != len)
		{
			System.out.println(tcpsocket.toString() + ": Error: length received");
			dropConnexion();
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
						analyseTLVPayload(Arrays.copyOfRange(buffer, 2, len), len-2);
				} else
				{
					/* STATE is not ACTIVE: no I-Block is allowed at this step -> Drop the connexion */
					System.out.println(tcpsocket.toString() + ": No I-Block allowed at this step");
					dropConnexion();
				}
        
				break;

			case IWM2_MK2_PROTO_I_S_BLOCK	:
				/* We've received an Is block ... */
        if (curState == STATE.ACTIVE_S)
				{
					if (len > 2)
					{	
						/* ... first retrieve the payload (decipher it and check CMAC) ... */
            byte[] cryptoPayload = Arrays.copyOfRange(buffer, 2, len);
						byte[] payload = retrieveCryptoPayload(buffer[1], cryptoPayload);
						
            /* ... then analyse it ! */
            if (payload != null)
							if (payload.length > 0)
                analyseTLVPayload(payload, payload.length);
					}
				} else
				{
					/* STATE is not ACTIVE_S: no Is Block is allowed at this step -> Drop the connexion */
					System.out.println(tcpsocket.toString() + ": No Is-Block allowed at this step");
					dropConnexion();
				}
        
				break;
				
			default :		
				System.out.println("Invalid I-Block received");
				dropConnexion();
				break;
			
		}

	}
	
	private void thread_listen()
	{
    InputStream is;
		int len;
		byte[] buffer = new byte[256];

    /* This thread runs as long as there is no commnication error */
		while ((curState == STATE.ACTIVE) || curState == STATE.ACTIVE_S)
		{
			/* Blocking read until something arrives ... */
      ReadBuffer rb = TCPRead(0);
      
      /* ... then, analyse the received frame       */
			analyseFrame(rb.buffer, rb.len);			
		}

		return;
	}
	
	private void listen()
	{
		/* This method starts a thread: thread_listen() */
    listen_thread = new Thread("") 
		{
      public void run()
			{
        thread_listen();
      }
		};

		listen_thread.start();
	}
	
	private boolean isValidHelloBlock(byte[] buffer, int len)
	{
		/* First byte should be the length */
    if (buffer[0] != len)
		{
			System.out.println(tcpsocket.toString() + ": Error: length received");
			return false;
		}

    /* Second byte should be the type */
		if ( (buffer[1] & IWM2_MK2_PROTO_TYPE_MASK) != IWM2_MK2_PROTO_HELO )
		{
			System.out.println(tcpsocket.toString() + ": Error: wrong block received");
			return false;
		}

    /* Check protocol version */
		if (!isProtocolVersionOK(buffer[1]))
			return false;

    /* Check MAC Address */
		if (!isMACAddressOK(Arrays.copyOfRange(buffer, 2, len)))
		{
			System.out.println("Incorrect MAC adress");
			return false;
		}
		
		return true;

	}
	
	private boolean isValidAuthBlock(byte[] buffer, int len)
	{
		/* First byte should be the length */
    if (buffer[0] != len)
		{
			System.out.println(tcpsocket.toString() + ": Error: length received");
			return false;
		}

		if (len != 0x12)
		{
			System.out.println(tcpsocket.toString() + ": Error: incorrect length received");
			return false;
		}
		
    /* Second byte should be the type */
		if ( buffer[1] == IWM2_MK2_PROTO_HELO_AUTH_RCVD )
		{
			System.out.println(tcpsocket.toString() + ": Error: wrong block received");
			return false;
		}
		
		return true;
		
	}
	
	private boolean waitForHello()
	{
		/* Read (wait 3 seconds max)	*/
    ReadBuffer rb = TCPRead(5000); 
		if (rb == null)
			return false;
		
    /* Check that the received frame is a valid 'Hello' block */
		if (!isValidHelloBlock(rb.buffer, rb.len))
			return false;
	
		return true;			

	}
	
	private byte[] waitAndDecipherAuth()
	{
		/* Read (wait 3 seconds max)	*/
    ReadBuffer rb = TCPRead(5000); 
		if (rb == null)
			return null;
	
    /* Check that the received frame is a valid 'Authentication' block */
		if (!isValidAuthBlock(rb.buffer, rb.len))
			return null;
	
    /* Decipher the received buffer */
		byte[] Cr = decipherBuffer(Arrays.copyOfRange(rb.buffer, 2, rb.len));
	
		return Cr;		
	}
	
	private boolean generateAndSendAuth2(byte[] Cr, byte[] Ch)
	{	
		/* Rotate Cr */
		byte[] Crp = SCrypto.rotateLeftBit(Cr);
		if (Crp == null)
			return false;
			
		PrintArray.internal(5, tcpsocket.toString() + "-Cr'", Crp, Crp.length);

		/* Concatenate Cr' and Ch	*/
		byte[] concat = new byte[Crp.length + Ch.length];
		int offset=0;	
		for (int i = 0; i<Ch.length; i++)
			concat[offset++] = Ch[i];
		for (int i = 0; i<Crp.length; i++)
			concat[offset++] = Crp[i];
		
		PrintArray.internal(5, tcpsocket.toString() + "-Ch||Cr'", concat, concat.length);
		
		/* Cipher Cr'|Cr	*/
		byte[] EkChrp= cipherBuffer(concat);
		if (EkChrp == null)
			return false;
		
    /* Send Authentication, step 2 */
		sendHeloAuth2(EkChrp);
		
		return true;
	
	}
	
	protected void dropConnexion()
	{
		/* A communication error occured ! */
    curState = STATE.ERROR;
		
    /* Stop listening to the device */
		if (listen_thread != null)
			listen_thread.interrupt();
		
    /* Cancel the sending of 'Keep Alive' messages */
		if (KeepAlive != null)
			KeepAlive.cancel();
		
		if (timer_KeepAlive != null)
		{
			timer_KeepAlive.cancel();
			timer_KeepAlive.purge();
		}
		
    /* Close the socket */
		try
		{
			if (tcpsocket != null)
				tcpsocket.close();
		} catch (IOException e)
		{
			System.out.println(IP + "-"+ PORT + ": Error closing socket: " + e.toString());
		}
		
    /* Alert the listener that the device is not connected any more*/
		listener.onConnexionDropped(IP, PORT);
		
	}
	
	private boolean connexionFirstStep()
	{
		/* Connect and wait for 'Hello' block */
    
    tcpsocket = new Socket();
		try
		{
			/* Connect (wait 3 seconds max) */
      tcpsocket.connect(new InetSocketAddress(IP, PORT), 3000);
		} catch (IOException e)
		{
			System.out.println(IP + "-" + PORT + ": " + e.toString());
			return false;
		}
		
    /* Wait for Hello block */
		if (!waitForHello())
			return false;
			
		return true;
		
	}
	
	public boolean connect()
	{
		/* First step: connect and wait for 'Hello' block */
    if (!connexionFirstStep())
			return false;

		/* Send HELO-OK to the Reader */
		sendHeloOK(null);
		
    /* The current state becomes ACTIVE */
		curState = STATE.ACTIVE;
		
    /* Planify the Keep-Alive messages, to be sent to the device */
		planifyKeepAlives();		
		
    /* Read everything the device sends */
		listen();
		
		/* Ask for Device's ID right after */
		getGlobalStatus();
				
		return true;
		
	}
		
	private boolean connectSecure(String Key, byte type)
	{
		/* Both Init Vectors have the following value at the begining: "0000..00"  */
    IVDevice = StringBytes.hexStringToByteArray("00000000000000000000000000000000");
		IVHost 	 = StringBytes.hexStringToByteArray("00000000000000000000000000000000");
		
    /* Check that Key is valid */
    if (!isValidKey(Key))
			return false;	
		
    KeyDevice = StringBytes.hexStringToByteArray(Key);

    /* First step: connect and wait for 'Hello' block */
		if (!connexionFirstStep())
			return false;
		
		/* Send HELO-Auth to the Reader */
		sendHeloAuth(type);

		/* Get Cr from first AUTH block	*/
		byte[] Cr = waitAndDecipherAuth();
		if (Cr == null)
			return false;
		PrintArray.internal(5, tcpsocket.toString() + "-Cr", Cr, Cr.length);

		/* Generate Ch and Ch'	*/
		SecureRandom random = new SecureRandom();
		byte[] Ch = new byte[16];
		random.nextBytes(Ch);
		PrintArray.internal(5, tcpsocket.toString() + "-Ch", Ch, 16);
		byte[] Chp = SCrypto.rotateLeftBit(Ch);
		
		/* Generate and send Authentication, step 2 */
		IVHost 	 = StringBytes.hexStringToByteArray("00000000000000000000000000000000");
		if (!generateAndSendAuth2(Cr, Ch))
			return false;

		/* Wait for Authentication, step 3	*/
		IVDevice = StringBytes.hexStringToByteArray("00000000000000000000000000000000");
		byte[] Chp_r = waitAndDecipherAuth();
		if (Chp_r == null)
			return false;
		
    PrintArray.internal(5, tcpsocket.toString() + "-Ch'", Chp, 16);
		PrintArray.internal(5, tcpsocket.toString() + "-Ch' received", Chp_r, 16);
		
		/* Check Ch' Received	*/
		for (int i=0; i<Chp.length; i++)
			if (Chp[i] != Chp_r[i])
				return false;
		
		/* Generate CipherKey	*/
		byte[] T = new byte[16];
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
		IVHost = StringBytes.hexStringToByteArray("00000000000000000000000000000000");
		CipherKey = cipherBuffer(T);
		PrintArray.internal(6, tcpsocket.toString() + "-CipherKey", CipherKey, 16);
		
		/* Generate CMACKey	*/
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
		IVHost = StringBytes.hexStringToByteArray("00000000000000000000000000000000");
		CMACKey = cipherBuffer(T);
		PrintArray.internal(6, tcpsocket.toString() + "-CMACKey", CMACKey, 16);
				
		/* Generate Nh	*/
		random = new SecureRandom();
		byte[] Nh = new byte[16];
		random.nextBytes(Nh);
		PrintArray.internal(5, tcpsocket.toString() + "-Nh", Nh, 16);
		
		/* Generate Payload	*/
		SeqH = StringBytes.hexStringToByteArray("00000000");
		SeqR = StringBytes.hexStringToByteArray("00000000");
		IVHost = StringBytes.hexStringToByteArray("00000000000000000000000000000000");
		
		byte[] payload = generateCryptoPayload((byte) (IWM2_MK2_PROTO_HELO_OK | IWM2_MK2_PROTO_VERSION), Nh);
		
		/* Copy the IVs	*/
		for (int i=0; i<IVHost.length; i++)
			IVDevice[i] = IVHost[i];
		
		/* Send final Helo-ok block	*/
		sendHeloOK(payload);
		
    /* The current state becomes ACTIVE */
		curState = STATE.ACTIVE_S;

    /* Planify the Keep-Alive messages, to be sent to the device */
		planifyKeepAlives();
		
    /* Read everything the device sends */
    listen();
    
		/* Ask for Device's ID right after */
		getGlobalStatus();
		
		return true;
	
	}

	public boolean connectSecureOperation(String OperationKey)
	{		
		return connectSecure(OperationKey, SECURE_MODE_OPERATION);
	}
	
	public boolean connectSecureAdministration(String AdministrationKey)
	{		
		return connectSecure(AdministrationKey, SECURE_MODE_ADMINISTRATION);
	}
  
  public Connector(String IP, Listener listener)
  {
		this.IP       = IP;
		this.PORT     = 3999;
		this.MAC      = null;
		this.listener = listener;    
		curState      = STATE.UNKNOWN;    
  }
  
  public Connector(String IP, int PORT, Listener listener)
  {
		this.IP       = IP;
		this.PORT     = PORT;
		this.MAC      = null;
		this.listener = listener;    
		curState      = STATE.UNKNOWN;    
  }

  public Connector(String IP, int PORT, Listener listener, String MAC)
  {
		this.IP       = IP;
		this.PORT     = 3999;
		this.MAC      = MAC.toUpperCase();
		this.listener = listener;    
		curState      = STATE.UNKNOWN;    
  }

}