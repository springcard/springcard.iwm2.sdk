package com.springcard.fkgtcp;

public class FunkyGateConnector extends Connector
{
	private static final int IWM2_MK2_CMD_SET_LEDS 		= 0xD0;
	private static final int IWM2_MK2_CMD_SET_BUZZER  = 0xD1;
	
	private int door = 0;
	
	public int getDoor()
	{
		return door;
	}
  
	protected void onBadgeRead(byte[] nb, int len)
	{
		/* When a badge is read: inform the listener !*/
    listener.onBadgeReceived(nb, len, door);
	}
	
	protected void onInNotified(byte input, byte value)
	{
		/* This is an error : a FunkyGate doesn't have inputs ! */
    System.out.println("An input change notification has been received from a FunkyGate !");
		dropConnexion();
	}

	public void clearLeds()
	{
		/* Clear the readers' leds */
    byte[] tlv = new byte[3];
		tlv[0] = (byte) IWM2_MK2_CMD_SET_LEDS;
		tlv[1] = 0x00;
		tlv[2] = 0x00;
		sendInCorrectMode(tlv);

	}
	
	public void setLeds(byte red, byte green)
	{
		/* Set the readers' red and gree leds (without duration) */
    byte[] tlv = new byte[5];
		tlv[0] = (byte) IWM2_MK2_CMD_SET_LEDS;
		tlv[1] = 0x00;
		tlv[2] = 0x02;
		tlv[3] = red;
		tlv[4] = green;
		sendInCorrectMode(tlv);
	}
	
	public void setLeds(byte red, byte green, short sec)
	{
		/* Set the readers' red and gree leds (with duration) */
    byte[] tlv = new byte[7];
		tlv[0] = (byte) IWM2_MK2_CMD_SET_LEDS;
		tlv[1] = 0x00;
		tlv[2] = 0x04;
		tlv[3] = red;
		tlv[4] = green;
		tlv[5] = (byte) (sec >>> 8);
		tlv[6] = (byte) (sec & 0x00FF);
		sendInCorrectMode(tlv);
	}
	
	public void setBuzzer(byte sequence)
	{
		/* Set the readers' buzzer */
    byte[] tlv = new byte[4];
		tlv[0] = (byte) IWM2_MK2_CMD_SET_BUZZER;
		tlv[1] = 0x00;
		tlv[2] = 0x01;
		tlv[3] = sequence;
		sendInCorrectMode(tlv);
	}
	
  public FunkyGateConnector(String IP, Listener listener)
  {
		super(IP, listener);
  }
  
  public FunkyGateConnector(String IP, int PORT, Listener listener)
  {
		super(IP, PORT, listener);
  }

  public FunkyGateConnector(String IP, int PORT, Listener listener, String MAC)
  {
		super(IP, PORT, listener, MAC);
  }

}