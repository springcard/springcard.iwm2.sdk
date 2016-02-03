package com.springcard.fkgtcp;

public interface Listener
{
	/* Simple interface, enabling the "AccessControl" object to listen to the FunkyGateConnector  */
  /* and HandyDrummerConnector objects (a badge has been read or a button has been pressed)     */
  void onBadgeReceived(byte[] badge, int len, int door);
	void onInputChanged(byte input, byte value);
	void onConnexionDropped(String IP, int PORT);
}
