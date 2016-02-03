package com.springcard.fkgtcp;

public class StringBytes
{
	/* Class containing only static functions, converting */
  /* hexadecimal strings to byte arrays, and vice versa */
  
  public static byte[] hexStringToByteArray(String s) 
	{
		s = s.replace(" ", "");
	    int len = s.length();
	    byte[] data = new byte[len / 2];
	    for (int i = 0; i < len; i += 2) 
			{
	        data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
	                             + Character.digit(s.charAt(i+1), 16));
	    }
	    return data;
	}
	
	public static String byteArrayToHexString(byte[] bArray)
	{
		StringBuffer buffer = new StringBuffer();
		
		for(byte b : bArray)
			buffer.append(String.format("%02X" , b));

		return buffer.toString().toUpperCase();    
	}
	
	private static boolean IsHex(char c)
	{
		if ((c >= '0') && (c <= '9'))
			return true;
		
		if ((c >= 'a') && (c <= 'f'))
			return true;
			
		if ((c >= 'A') && (c <= 'F'))
			return true;
		
		return false;
	}
	
	public static boolean IsCorrectHexString(String s, int expected_len)
	{
		/* Check that the string is a valid hexadecimal string */
		if (s.length() != expected_len*2)
			return false;
		
		for (int i=0; i<s.length(); i++)
			if (!IsHex(s.charAt(i)))
				return false;
			
		return true;
	
	}
  
}