package com.springcard.fkgtcp;

import javax.crypto.*;
import javax.crypto.spec.*;

public class SCrypto
{	
	
  /* Class containing only static functions, needed to perform cryptographic operations */
  
  public static byte[] AES_Cipher(byte[] PlainText, byte[] key, byte[] iv)
	{
    try
    {
      int i, j, l;       
      l = PlainText.length;    
      int[] buffer = new int[l];
			byte[] result = new byte[l];
			
      for (i=0; i<l; i++)
        buffer[i] = (int) 0xDA;
        
      l /= 16;
 
      Cipher c = Cipher.getInstance("AES/CBC/NoPadding");
      c.init(Cipher.ENCRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(new byte[16]));
    
      for (i = 0; i < l; i++)
      {
        byte[] t = new byte[16];
        for (j = 0; j < 16; j++)
          t[j] = (byte) ((PlainText[16 * i + j] ^ iv[j]) & 0x00FF); /* P  <- P XOR IV  */
        t = c.doFinal(t); 																					/* IV <- 3DES(P)   */
        for (j = 0; j < 16; j++) 																		/* P  <- IV        */
        {
          iv[j] = t[j];
          buffer[16 * i + j] = t[j];
        }
      }
      
			for (i=0; i<buffer.length; i++)
				result[i] = (byte)  (buffer[i] & 0x00FF);
			
			return result;
			
    } catch (Exception e)
    {
			e.printStackTrace();
      return null;
    }
	
	}

	public static byte[] AES_Decipher(byte [] ciphered, byte[] key, byte[] iv)
  {  
		try
    {
      int i, j, l;       
      l = ciphered.length;    
      int [] buffer = new int[l];
			byte[] result = new byte[l];
			
      for (i=0; i<l; i++)
        buffer[i] = (int) 0xDA;
        
      l /= 16;

      Cipher c = Cipher.getInstance("AES/CBC/NoPadding");
      c.init(Cipher.DECRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(new byte[16]));  

      for (i = 0; i < l; i++)
      {
        byte[] t = new byte[16];
        for (j = 0; j < 16; j++)
          t[j] = (byte) ((ciphered[16 * i + j]) & 0x00FF); 		/* B  <- P        */
				t = c.doFinal(t); 																		/* P  <- AES-1(B) */

				for (j = 0; j < 16; j++)
        {
          buffer[16 * i + j] = (byte) (t[j] ^ iv[j]);  				/* P  <- P XOR IV */
          iv[j] = ciphered[16 * i + j]; 											/* IV <- B        */
        }

      }

			for (i=0; i<buffer.length; i++)
				result[i] = (byte)  (buffer[i] & 0x00FF);

			return result;
			
    } catch (Exception e)
    {
      e.printStackTrace();
      return null;
    }	
	
  }

	public static byte[] rotateLeftBit(byte[] entry)
	{
		if (entry == null)
			return null;
		
		byte[] result = new byte[entry.length];	
		
		for (int i=0; i<entry.length; i++)
		{        
			result[i] = (byte) (entry[i] << 1);
			if (i == (entry.length-1))
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
	
	public static byte[] incrementSeq(byte[] seq)
	{	
		long value=0;
		byte[] res;
		if (seq == null)
			return null;
		
		if (seq.length != 4)
			return null;	

		if (StringBytes.byteArrayToHexString(seq).equals("FFFFFFFF"))
			return StringBytes.hexStringToByteArray("00000000");
			
		value  = (int) (seq[0] & 0x00FF);
		value <<= 8;
		value += (int) (seq[1] & 0x00FF);
		value <<= 8;
		value += (int) (seq[2] & 0x00FF);
		value <<= 8;
		value += (int) (seq[3] & 0x00FF);
		
		value++;
		
		res = new byte[4];

		res[0] = (byte)(value >>> 24);
		res[1] = (byte)(value >>> 16);
		res[2] = (byte)(value >>> 8);
		res[3] = (byte)(value >>> 0);

		return res;

	}
	
	public static byte[] calculateCMAC(byte type, byte[] payload, byte[] key, byte[] seq)
	{
		if (key == null)
			return null;
			
		if (key.length != 16)
			return null;
			
		if (seq == null)
			return null;
			
		if (seq.length != 4)
			return null;
		
		byte payloadLen;
		if (payload == null)
			payloadLen=0;
		else
			payloadLen = (byte) (payload.length);
		
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
		if ((payloadLen + H.length)%16 > 0)
			TLength = ((payloadLen + H.length)/16 + 1) * 16;
		else
			TLength = (payloadLen + H.length);
		
		T = new byte[TLength];
		int offset = 0;
		for (int i=0; i<H.length; i++)		
			T[offset++] = H[i];
		for (int i=0; i<payloadLen; i++)		
			T[offset++] = payload[i];
		
		/* Padd T if necessary	*/
		if ((payloadLen + H.length)%16 > 0)
		{
			T[offset++] = (byte) 0x80;
			for (int i=offset; i<T.length; i++)
				T[i] = 0x00;
		}
		PrintArray.internal(6, "T", T, T.length);
		
		/* Cipher T	*/
		byte[] IV = StringBytes.hexStringToByteArray("00000000000000000000000000000000");
		byte[] C = AES_Cipher(T, key, IV);
		PrintArray.internal(6, "C", C, C.length);
		PrintArray.internal(6, "IV", IV, IV.length);
		
		/* Get CLast	*/
		byte[] CLast = new byte[16];
		for (int i=0; i<16; i++)
			CLast[i] = C[C.length-16+i];
		PrintArray.internal(6, "Clast", CLast, CLast.length);
		
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