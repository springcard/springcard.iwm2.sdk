package com.springcard.fkgtcp;

public class HandyDrummerConnector extends Connector
{
	private final static int HANDY_DRUMMER_SET_OUTPUT			 = 0x90;
	private final static int HANDY_DRUMMER_CLR_OUTPUT			 = 0xA0;
	
	protected void onBadgeRead(byte[] nb, int len)
	{
		/* This is an error : a HandyDrummer cannot read badges ! */
    System.out.println("A badge number has been received from a HandyDrummer !");
		dropConnexion();
	}
	
	protected void onInNotified(byte input, byte value)
	{
		/* When an input has changed: inform the listener ! */
    listener.onInputChanged(input, value);
	}
	
  
	public void setOutput(byte output)
	{
		/* Open (without duration) */
    byte[] tlv = new byte[3];
		tlv[0] = (byte) HANDY_DRUMMER_SET_OUTPUT;
		tlv[1] = output;
		tlv[2] = 0x00;
		sendInCorrectMode(tlv);
	}
	
	public void setOutput(byte output, short sec)
	{
		/* Open (with duration) */
    byte[] tlv = new byte[5];
		tlv[0] = (byte) HANDY_DRUMMER_SET_OUTPUT;
		tlv[1] = output;
		tlv[2] = 0x02;
		tlv[3] = (byte) (sec >>> 8);
		tlv[4] = (byte) (sec & 0x00FF);
		sendInCorrectMode(tlv);
	}
	
	
	public void clearOutput(byte output)
	{
		/* Close the door */
    byte[] tlv = new byte[3];
		tlv[0] = (byte) HANDY_DRUMMER_CLR_OUTPUT;
		tlv[1] = output;
		tlv[2] = 0x00;
		sendInCorrectMode(tlv);
	}
	
  public HandyDrummerConnector(String IP, Listener listener)
  {
		super(IP, listener);
  }
  
  public HandyDrummerConnector(String IP, int PORT, Listener listener)
  {
		super(IP, PORT, listener);
  }

  public HandyDrummerConnector(String IP, int PORT, Listener listener, String MAC)
  {
		super(IP, PORT, listener, MAC);
  }
  
}