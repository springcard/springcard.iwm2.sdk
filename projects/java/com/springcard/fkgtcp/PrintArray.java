package com.springcard.fkgtcp;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.util.Calendar;
import java.util.Date;
import java.text.SimpleDateFormat;

public class PrintArray
{
	private static final Lock write_mutex = new ReentrantLock();
	static private void arrayprint(int level, String label, String io, byte[] array, int len)
	{
		/* Print the array if the level corresponds to the configuration */
    final SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd-HH:mm:ss");
    Calendar now = Calendar.getInstance(); 
    
    /* Use a mutex to avoid two processes that write at the same time */
    write_mutex.lock();
    System.out.print(dateFormat.format(now.getTime())+ "-"+label + ": "+ io);
    for (int i=0; i<len; i++)
      System.out.print(String.format("%02X " , array[i]));
    System.out.print("\n");
    
    /* Don't forget to release the mutex after writing */
    write_mutex.unlock();
	}
	
	static public void in(int level, String label, byte[] array, int len)
	{
		arrayprint(level, label, "-", array, len);
	}
	
	static public void out(int level, String label, byte[] array, int len)
	{
		arrayprint(level, label, "+", array, len);
	}
	
	static public void internal(int level, String label, byte[] array, int len)
	{
		arrayprint(level, label, "", array, len);
	}
	
}