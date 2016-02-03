import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.Properties;
import java.util.Calendar;
import java.util.Date;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.text.ParseException;

import java.io.*;

import com.springcard.fkgtcp.*;

public class fkgtcpDemo implements com.springcard.fkgtcp.Listener
{
	class ReceivedEvent
	{
		public int door;
		public ReceivedEvent(int d)
		{
			door = d;
		}	
	}
	
	class BadgeReceived extends ReceivedEvent
	{
		public String badge;
		public BadgeReceived(int d, String b)
		{
			super(d);
			badge = b;
		}
	}
	
	class InputReceived extends ReceivedEvent
	{
		public byte inputValue;
		public InputReceived(int d, byte iV)
		{
			super(d);
			inputValue = iV;
		}	
	}
	
	private static String keyVal = null;
	private static int secureMode = 0;
	
	private FunkyGateConnector reader;
	private HandyDrummerConnector hd;
		
	LinkedBlockingQueue<ReceivedEvent> queue;
	
	public fkgtcpDemo()
	{
		/* Instanciate the blocking queue */
    queue = new LinkedBlockingQueue<ReceivedEvent>();
	}
  
	public static void main(String args[]) 
	{ 
		System.setProperty("java.net.preferIPv4Stack" , "true");
    
    /* Run! */
    fkgtcpDemo demo = new fkgtcpDemo();
		demo.run();
	}
	
	public void run()
	{			
    reader = new FunkyGateConnector("127.0.0.1", this);
    
    if (!reader.connect())
    {
      System.out.println("Can't connect to FunkyGate (" + reader.getIP() +"-"+ reader.getPort()+")");	      
      return;
    }
      
    System.out.println("Connected succesfully to FunkyGate (" + reader.getIP() +"-"+ reader.getPort()+") !");      

		for (;;)
		{
			/* Infinite loop */
      try
			{
				/* Wait here, until an event is received in the queue */
        ReceivedEvent event = queue.poll(360L, TimeUnit.DAYS); /* Wait for a long time !! */
				if (event instanceof BadgeReceived)
				{
          System.out.println("BadgeReceived");
          
//          if (isAccesGranted(event.door, ((BadgeReceived) event).badge))
				} else
				if (event instanceof InputReceived)
				{
					System.out.println("InputReceived");          
				}
				
			} catch (InterruptedException e)
			{
				System.out.println(e.toString());
        return;
			}		
		}
  }
    

    
	private String formatBadgeNb(byte[] badge, int len)
	{
		/* We want a 16-character-String	(8 bytes) */
		StringBuffer buffer = new StringBuffer();
		
		if (len < 8)
		{
			for (int i=0; i<(8-len); i++)
				buffer.append("00");
				
			for (byte b : badge)
				buffer.append(String.format("%02X" , b));
				
		} else
		{
			for (int i=len-8; i<len; i++)
				buffer.append(String.format("%02X" , badge[i]));
		}
		
		return buffer.toString().toUpperCase();   
	
	}
	
	public void onBadgeReceived(byte[] buffer, int len, int door)
	{
		/* This method is called by the FunkyGateConnector object, once a badge is read */
    
    /* First: format the badge, ... */
    String badge = formatBadgeNb(buffer, len);
		System.out.println("Door " + door +": badge received '" + badge + "'");
    
    /* ... then add it in the queue */
    queue.add(new BadgeReceived(door, badge));
    
	}	
	
	public void onInputChanged(byte input, byte value)
	{
		/* This method is called by the HandyDrummerConnector object, once a badge is read */
    System.out.println("Door " + input + ": New input value=" + value);
		
		/* Only process when value = 1 (button pressed):	add it in the queue */
		if (value == 1)
			queue.add(new InputReceived(input, value));

	}
	
	public void onConnexionDropped(String IP, int PORT)
	{
		/* Just print the information - we'll try to reconnect later (via "reconnect" method) */
    System.out.println(IP + "-" + PORT + ": Connexion lost");
	}
	

		

}