/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 11/12/2015
 * Time: 11:11
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.IO.Ports;
using System.Threading;
using System.Collections.Generic;

namespace SpringCard.IWM2
{

	/* This object represents a serial scheduler: it manages a COM port. 							*/
 	/* It first probes all given adresses, and then talks to the readers that answer	*/
	/* its requests correctly. Every 10 seconds, it probes again all given adresses, 	*/
	/* to (re)detect (new) readers																										*/
	
	public class SpringCardIWM2_Serial_Scheduler
	{

		
#region Protocol constants
		private const byte ASCII_STX = 0x02;
		private const byte ASCII_ETX = 0x03;
		private const byte ASCII_DLE = 0x10;
		private const byte ASCII_ESC = 0x1B;
		
		private const byte IWM2_MK2_PROTO_I_BLOCK 		  = 0x00;	
		private const byte IWM2_MK2_PROTO_READER_HOST		= 0x80;
		
		private const byte IWM2_MK2_PROTO_S_WAIT				= 0x20;
		private const byte IWM2_MK2_PROTO_S_ENUM				= 0x30;
		private const byte IWM2_MK2_PROTO_R_OK					= 0x40;
		private const byte IWM2_MK2_PROTO_R_NACK				= 0x60;
		
		private const byte IWM2_MK2_THRESHOLD_T_TWO_BYTES = 0x80;

		private const int INTERVAL_BETWEEN_S_ENUM_LOOPS_SEC = 10; /* 10 seconds */
		
#endregion		


#region Private members
		
		/* The serial port managed by the scheduler */
		SerialPort port;
		
		/* This boolean indicates whether to continue or stop the communication loops */
		bool continueRunning = true;
		
		/* This boolean indicates whether all sent/received frames received on the COM port must be printed */
		bool show_frames = false;
		
		/* This flag indicates whether there is at least an installed reader, which has stopped responding */
		bool flag_reader_to_remove;
		
		/* This is the main thread, that performs the communication on the bus. 		*/
		/* Using a thread is mandatory, in order not to "freeze" the main screen.		*/
		Thread serialProtocolThread;
				
		/* Those are the minimum and maximum adresses to probe, on the bus (defined by the user) */
		private byte min_address;
		private byte max_address;
		
		/* This list contains all readers that answer on the bus	*/
		List<SpringCardIWM2_Serial_Reader> InstalledReaders;		
		
		/* This list contains all readers that are monitored by the user  */
		List<SpringCardIWM2_Serial_Reader> UserManagedReaders;	
		
#endregion
		
		
#region Callbacks		
		
		/* Those methods define the callback method to be called, when the scheduler detects a reader on the bus */
		public delegate void ReaderFoundCallback(string com, string protocol, byte address, bool already_monitored);
		private ReaderFoundCallback _reader_found_callback = null;
		public void SetReaderFoundCallback(ReaderFoundCallback cb)
		{
			_reader_found_callback = cb;
		}
		
		/* Those methods define the callback method to be called, when the scheduler drops an installed reader */
		public delegate void ReaderDroppedCallback(string com, byte address);
		private ReaderDroppedCallback _reader_dropped_callback = null;
		public void SetReaderDroppedCallback(ReaderDroppedCallback cb)
		{
			_reader_dropped_callback = cb;
		}		
		
		/* Those methods define the callback method to be called, when the scheduler stops */
		public delegate void SchedulerStoppedCallback(string com);
		private SchedulerStoppedCallback _scheduler_stopped_callback = null;
		public void SetSchedulerStoppedCallback(SchedulerStoppedCallback cb)
		{
			_scheduler_stopped_callback = cb;
		}
		

#endregion
		
		
#region Accessors
		public List<SpringCardIWM2_Serial_Reader> GetInstalledReaders()
		{
			return InstalledReaders;
		}
		
		public bool IsRunning()
		{
			return continueRunning;
		}
			
		public string GetPortName()
		{
			return port.PortName;		
		}
	
		public SerialPort GetPort()
		{
			return port;
		}
			
		
#endregion


#region Serial port management

		public bool Open()
		{
			if (port.IsOpen)
				return true;
			
			try
			{
				port.Open();
			} catch (Exception)
			{
				return false;
			}
			
			return true;
		}
		
		public bool Close()
		{
			if (!port.IsOpen)
				return true;
			
			try
			{
				port.Close();
			} catch (Exception)
			{
				return false;
			}
				
			return true;
		}

		/* This method reads the incoming bytes on the COM port, filters the frames	*/
		/* and returns a "filtered" message, cleared from all "escape" bytes.				*/
		/* See "RemovedEscapeBytes" method																					*/  
		private byte[] ReceiveEscapedFrame(int timeout_ms, out byte rcvd_len)
		{
			byte[] raw_rcvd = new byte[100];
			int raw_len = 0;
			
			byte[] escaped_rcvd = null;
			byte escaped_len=0;
			
			
			port.ReadTimeout = timeout_ms;
			rcvd_len = 0;
			
			for (;;)
			{
				try
				{
					/* Concatene every received byte(s), before the timeout */
					raw_len += port.Read(raw_rcvd, raw_len, raw_rcvd.Length - raw_len);
					if (raw_len >= 5)
					{
						/* Check if frame is complete */
						if (raw_rcvd[raw_len-1] == ASCII_ETX)
						{						
							if (raw_len < 0xFF)
							{
								escaped_rcvd = RemovedEscapeBytes(raw_rcvd, (byte) raw_len, out escaped_len);
								if (escaped_rcvd[escaped_len - 1] == ASCII_ETX)
									break;
							}
						}
					}
						
				} catch (Exception)
				{
					break;
				}
			}
			
			if (raw_len < 0xFF)
				if (show_frames)
					LogByteArrayReceived(raw_rcvd, (byte) raw_len);
			
			if (escaped_len < 0xFF)
				rcvd_len = escaped_len;
			
			return escaped_rcvd;
		}

#endregion
		

#region Low-level protocol
		
		private byte CalculateLRC(byte[] buffer, byte len)
		{
			byte result=0;
			
			/* Note: STX is not included - hence start at buffer[1] */
			for (byte i=1; i<len; i++)
				result ^= buffer[i];
			
			return result;
		}
		
		private bool IsLRCCorrect(byte[] buffer, byte len)
		{
			if (len < 2)
				return false;
			
			byte LRC = CalculateLRC(buffer, (byte) (len-2)); /* remove received LRC + ETX */
			return (LRC == buffer[len-2]);
		}
				
		private bool IsForbiddenByte(byte b)
		{
			if ((b==ASCII_STX) || (b==ASCII_ETX) || (b==ASCII_ESC) || (b==ASCII_DLE))
				return true;
			
			return false;
		}
		
		/* Each outcoming frame may contain "forbidden" bytes.							*/
 		/* This method "escapes" them, adding 0x10 before each one of them.	*/ 
 		/* See "IsForbiddenByte" method																			*/
		private byte[] EscapeForbiddenBytes(byte[] buffer, byte in_len, out byte out_len)
		{
			byte count=0;
			
			for (byte i=1; i<in_len - 1; i++)
				if (IsForbiddenByte(buffer[i]))
					count++;
			
			byte[] result = new byte[in_len + count];
			byte offset = 0;
			
			result[offset++] = buffer[0];
			for (byte i=1; i< (in_len-1); i++)
			{
				if (IsForbiddenByte(buffer[i]))
					result[offset++] = ASCII_DLE;
				
				result[offset++] = buffer[i];
				
			}
			result[offset++] = buffer[in_len - 1];  
			out_len = offset;
			
			return result;
			
		}
		
		
		/* Each incoming frame may contain "forbidden" bytes, which are	escaped			*/ 
 		/* with 0x10 (ASCII_DLE). This method removes all the	"escaped" characters, */
 		/* and returns the original (filtered) message															*/
 		/* See "IsForbiddenByte" method.																						*/
		private byte[] RemovedEscapeBytes(byte[] rcvd, byte rcvd_len, out byte filtered_len)
		{			
			filtered_len = 0;
			byte[] filtered = new byte[rcvd_len];
			byte offset = 0;
			
			/* Each frame must begin with 0x02 (ASCII_STX) and end with 0x03 (ASCII_ETX).	 */
			if ((rcvd[0] != ASCII_STX) || (rcvd[rcvd_len-1] != ASCII_ETX))
				return null;	
			
			filtered[offset++] = ASCII_STX;
			
			for (byte i=1; i<rcvd_len - 1; i++)
			{
				if (rcvd[i] != ASCII_DLE)
				{
					filtered[offset++] = rcvd[i];
				} else
				{
					i++;
					filtered[offset++] = rcvd[i];
				}
				
				
			}
			
			filtered[offset++] = ASCII_ETX;
			
			filtered_len = offset;
			return filtered;
			
		}
		
		/* This method sends an empty I-Block to the specified reader */
		private bool SendIBlock(SpringCardIWM2_Serial_Reader reader)
		{
			return SendIBlock(reader, null);
		}
		
		/* This method sends an I-Block with a payload to the specified reader */
		private bool SendIBlock(SpringCardIWM2_Serial_Reader reader, byte[] payload)
		{
			byte[] data;
			byte offset = 0;
			
			if (payload == null)
				data = new byte[5];
			else
				data = new byte[5 + payload.Length];
						
			data[offset++] = ASCII_STX;
			data[offset++] = (byte)  (IWM2_MK2_PROTO_I_BLOCK | reader.block_number);
			data[offset++] = reader.GetAddress();
			if (payload != null)
				for (byte i=0; i<payload.Length; i++)
					data[offset++] = payload[i];
			
			data[offset] = CalculateLRC(data, offset);
			offset++;
			data[offset++] = ASCII_ETX;
			
			byte out_len = 0;
			byte[] res = EscapeForbiddenBytes(data, offset, out out_len);
			
			if (show_frames)
				LogByteArrayEmitted(res, out_len);
			
			try
			{
				port.Write(res, 0, out_len);
				Thread.Sleep(1); 		/* slow down the bus */
			
			} catch (Exception)
			{
				return false;
			}
			
			return true;		
		}
		
		/* This method sends an R-Ok block to the specified reader */
		private bool SendROKBlock(SpringCardIWM2_Serial_Reader reader)
		{
			byte[] data = new byte[5];
			data[0] = ASCII_STX;
			data[1] = (byte)  (IWM2_MK2_PROTO_R_OK | reader.block_number);
			data[2] = reader.GetAddress();
			data[3] = CalculateLRC(data, 3);
			data[4] = ASCII_ETX;
			
			byte out_len = 0;
			byte[] res = EscapeForbiddenBytes(data, 5, out out_len);
			
			if (show_frames)
				LogByteArrayEmitted(res, out_len);
			
			try
			{
				port.Write(res, 0, out_len);
				Thread.Sleep(1); 		/* slow down the bus */
				
			} catch (Exception)
			{
				return false;
			}
			return true;			
		}
		
		/* This method sends an R-Nack block to the specified reader */
		private bool SendRNackBlock(SpringCardIWM2_Serial_Reader reader)
		{
			byte[] data = new byte[5];
			data[0] = ASCII_STX;
			data[1] = (byte)  (IWM2_MK2_PROTO_R_NACK | reader.block_number);
			data[2] = reader.GetAddress();
			data[3] = CalculateLRC(data, 3);
			data[4] = ASCII_ETX;
			
			byte out_len = 0;
			byte[] res = EscapeForbiddenBytes(data, 5, out out_len);
			
			if (show_frames)
				LogByteArrayEmitted(res, out_len);
			
			try
			{
				port.Write(res, 0, out_len);
				Thread.Sleep(1); 		/* slow down the bus */

			} catch (Exception)
			{
				return false;
			}
			return true;				
		}
		
#endregion
		

#region Log

		public void SetShowFrames(bool show)
		{
			show_frames = show;
		}
		
		public void LogString(String s)
		{
			SpringCard.LibCs.SystemConsole.Verbose(s);
		}
		
		private void LogByteArray(string direction, byte[] data, byte len)
		{
			String s = direction;
			for (byte i=0; i<len; i++)
				s += String.Format("{0:x02}", data[i]);
			
			LogString(s);
			
		}
		
		private void LogByteArrayEmitted(byte[] data, byte len)
		{
 			LogByteArray(port.PortName + ": +", data, len);
		}
		
		private void LogByteArrayReceived(byte[] data, byte len)
		{
 			LogByteArray(port.PortName + ": -", data, len);
		}
		
#endregion		


#region Readers management

		/* This method adds a reader in the "InstalledReaders" list. 		 */
		/* This list contains all the active (=responding) readers on		 */
		/* the bus. This is not the list of readers, which are monitored */
 		/* by the user (set LEDs, buzzer, etc ...) 		 									 */
		private void InstallReader(byte address)
		{
			LogString(port.PortName + ":Installing reader " + address);		

			SpringCardIWM2_Serial_Reader reader;
			
			/* Caution: if the reader is already monitored, but not installed,	*/
			/* this is because it has been detected in the past, then dropped.	*/
			/* In this case, don't create another object, but place the 				*/
			/* monitored reader in the "installed readers" list									*/
					
			if (IsMonitored(address))
			{
				reader = GetMonitoredReader(address);
			} else
			{
				reader = new SpringCardIWM2_Serial_Reader(this, address);
			}
			InstalledReaders.Add(reader);
			if ( _reader_found_callback != null)
				_reader_found_callback(port.PortName, "MK2", address, IsMonitored(address));

			if (IsMonitored(address))
				reader.SetScheduled();
			
		}
	
		/* This method indicates whether there is a responding reader at this specific address */
		private bool IsInstalled(byte address)
		{
			foreach (SpringCardIWM2_Serial_Reader reader in InstalledReaders)
				if (reader.GetAddress() == address)
					return true;
			
			return false;			
		}
		
		
		/* This method adds a reader in the "UserManagedReaders" list, which contains		 */
		/* the readers the user wants to manage (ie: set buzzer, LEDs, get badge number) */
		public void MonitorReader(byte address)
		{
			LogString(port.PortName + ":Monitoring reader " + address);
			foreach (SpringCardIWM2_Serial_Reader reader in InstalledReaders)
				if (reader.GetAddress() == address)
					UserManagedReaders.Add(reader);
		}
		
		/* This method indicates whether the reader at this address (responding or not) */
		/* is monitored by the user (get badge number, set buzzer or LEDs	...)					*/
		public bool IsMonitored(byte address)
		{
			foreach (SpringCardIWM2_Serial_Reader reader in UserManagedReaders)
				if (reader.GetAddress() == address)
					return true;
			
			return false;
		}
		
		/* This method changes the reader'status at the specified address, to the "dropped" state. */
 		/* This is mainly due because it has stopped responding correctly to I- or R-Blocks. 			 */
		public void DropReader(byte address)
		{
			LogString(port.PortName + ":Dropping reader " + address);
			SpringCardIWM2_Serial_Reader reader = GetInstalledReader(address);
			reader.SetDropped();
			flag_reader_to_remove = true;
			if (_reader_dropped_callback != null)
				_reader_dropped_callback(port.PortName, address);
		}
		
		/* This method removes the reader at the specified adress, from the "UserManagedReaders"	*/
		/* list. This is not because the reader has stopped responding, but because the user 			*/
		/* doesn't want to monitor the reader anymore.																						*/
		public void UnMonitorReader(byte address)
		{
			LogString(port.PortName + ":Stop monitoring reader " + address);
			SpringCardIWM2_Serial_Reader reader = GetMonitoredReader(address);
			if (reader != null)
			{
				List<SpringCardIWM2_Serial_Reader> readersToRemove = new List<SpringCardIWM2_Serial_Reader>();
				foreach (SpringCardIWM2_Serial_Reader mReader in UserManagedReaders) 
					if (mReader.GetAddress() == address)
						readersToRemove.Add(mReader);
			
				foreach (SpringCardIWM2_Serial_Reader mReader in readersToRemove)
				{
					mReader.SetTerminated();
					UserManagedReaders.Remove(mReader);
				}
						
			}

		}
		
		/* This method removes all the "dropped" readers, from the "InstalledReadersList" list. 			*/
		/* As a result, the scheduler won't send I- or R-blocks to those readesr, untill they answer  */
		/* correctly to an S-Enum block (sent to all adresses, every 10 seconds)											*/
		private void PurgeAllDroppedReaders()
		{
			List<SpringCardIWM2_Serial_Reader> readersToRemove = new List<SpringCardIWM2_Serial_Reader>();
			foreach (SpringCardIWM2_Serial_Reader reader in InstalledReaders) 
				if (!reader.is_scheduled)
					readersToRemove.Add(reader);
		
			foreach (SpringCardIWM2_Serial_Reader reader in readersToRemove)
				InstalledReaders.Remove(reader);

			
		}				
		
		/* This method retrieves the reader, at the specified address, from all the	*/ 
		/* readers which are managed by a user (set buzzer and LEDs, etc ...)				*/
		public SpringCardIWM2_Serial_Reader GetMonitoredReader(byte address)
		{
			foreach (SpringCardIWM2_Serial_Reader reader in UserManagedReaders)
				if (reader.GetAddress() == address)
					return reader;
			
			return null;
		}

		/* This method retreives the reader, at the specified address, from all the	*/ 
		/* installed readers (those that answer on the COM port)										*/		
		public SpringCardIWM2_Serial_Reader GetInstalledReader(byte address)
		{
			foreach (SpringCardIWM2_Serial_Reader reader in InstalledReaders)
				if (reader.GetAddress() == address)
					return reader;
			
			return null;
		}
		
#endregion


#region Analyse Received Frames
		
		/* This method analyses a received frame (S-Enum ? empty I-block ? I-block with payload ?) 		*/
		/* and takes appropriate actions (anaylses the payload, increments the number of consecutive  */	
 		/* errors, send a R-Nack, drop a readers, etc ...).																				 		*/
		public bool AnalyseReceivedFrame(byte[] data, byte len, byte expected_address)
		{
			if (data == null)
				return false;

			if (len < 5)
				return false;

			if (data[0] != ASCII_STX)
				return false;

			/* Retrieve and check address */
			byte address = data[2];
			if (address != expected_address)
			{
				LogString("Address received is not expected address");
				SpringCardIWM2_Serial_Reader reader = GetInstalledReader(expected_address);
				if (reader != null)
					reader.Enqueue_Send_RNack_Block();
				return false;
			}
			
			byte block_number;
			switch (data[1] & 0xF0)
			{
				case (IWM2_MK2_PROTO_READER_HOST | IWM2_MK2_PROTO_S_ENUM) : 
					if (!IsInstalled(address))
						InstallReader(address);

					break;
					
				case (IWM2_MK2_PROTO_READER_HOST | IWM2_MK2_PROTO_I_BLOCK) :
					/* Verify block number */
					block_number = (byte) (data[1] & 0x0F);
					if (IsInstalled(address))
					{
						SpringCardIWM2_Serial_Reader reader = GetInstalledReader(address);
						if (block_number != reader.block_number)
						{
							reader.Enqueue_Send_RNack_Block();
							return false;
						}
						
						byte offset = 3;
						
						/* Analyse all the potential TLVs in the payload */
						while(offset < (len - 2))
						{
							byte l; /* length of TLV */
													
							if (data[offset] >= IWM2_MK2_THRESHOLD_T_TWO_BYTES)
							{
								l = (byte) (data[offset + 2] + 3); /* add 2 bytes for T and 1 for L*/
							} else
							{
								l = (byte) (data[offset+1] + 2); 	 /* add 1 byte for T and 1 for L*/
							}
							
							byte[] tmp = new byte[l];
							if ((offset + l - 1) < (len - 2))
							{
								Array.ConstrainedCopy(data, offset, tmp, 0, l);
								offset += l;					
								if (reader.AnalyseTLVPayload(tmp, tmp.Length))
									reader.Enqueue_Send_ROK_Block();
							
							} else
							{
								DropReader(address);
								return false;
							}
						
						}
						
					}
					
					break;
					
				case (IWM2_MK2_PROTO_READER_HOST | IWM2_MK2_PROTO_S_WAIT) :
					/* Verify block number */
					block_number = (byte) (data[1] & 0x0F);
					if (IsInstalled(address))
					{
						SpringCardIWM2_Serial_Reader reader = GetInstalledReader(address);
						if (block_number != reader.block_number)
						{
							reader.Enqueue_Send_RNack_Block();
							return false;
						}
						
						reader.SetDelay();
						
					}
					break;
					
				default :
					break;
					
			}
			
			return true;
			
		}
		
#endregion


#region Timer-based devices (re)detection

		/* This indicates whether it is time to send a S-Enum loop right now.		*/
		/* This variable is shared between the main thread that performs loops,	*/
 		/* and the timer that sets its value every 10 seconds										*/
		private bool EnumerateNow = false;
		
		/* This timer is used to indicate whether it is time to send an S-Enum loop */
		private System.Timers.Timer timer = null;
		
		/* This is the mutex protecting the access to "EnumerateNow", 	*/
		/* which is accessed by the main thread, and the timer					*/
		private Mutex timerMutex	 = null;

		/* This method creates the timer, which indicates whether it is time to enumerate readers */
		private void PlanifyNextEnumerations()
		{

			timerMutex = new Mutex();
			
			timer = new System.Timers.Timer(1000 * INTERVAL_BETWEEN_S_ENUM_LOOPS_SEC);
			timer.AutoReset = true;
			timer.Enabled = true;
			timer.Elapsed += (object sender, System.Timers.ElapsedEventArgs e) => SetEnumerateNow(true);
			timer.Start();
			
		}

		/* This method sets the "EnumerateNow" boolean value	*/ 
		private void SetEnumerateNow(bool val)
		{	
			timerMutex.WaitOne(Timeout.Infinite);
			EnumerateNow = val;
			timerMutex.ReleaseMutex();			
		}
		
		/* This method gets the "EnumerateNow" boolean value */
		private bool GetEnumerateNow()
		{
			bool res;
			timerMutex.WaitOne(Timeout.Infinite);
			res = EnumerateNow;
			timerMutex.ReleaseMutex();
			
			return res;
		}
		
#endregion


#region Protocol: loops
	
		/* This method sends the appropriate command (default: empty I-Block) to all installed readers. */
		/* This is the main loop that ensures communication on the COM port 													  */
		public void TalkToReaders()
		{
			int timeout = 80;
			bool is_bus_ok;
			int wait_ms=0;
			while(continueRunning)
			{
				if (GetEnumerateNow())
				{
					bool first_time = false;
					SetEnumerateNow(false);
					Enumerate(first_time);
				}
				
				flag_reader_to_remove = false;
				
				if (InstalledReaders.Count <= 0)
				{
					/* Sleep 500 milliseconds when all readers have been		*/ 
 					/* dropped in order to avoid a full-speed infinite loop	*/									
					Thread.Sleep(500);
					continue;
				}

				foreach(SpringCardIWM2_Serial_Reader reader in InstalledReaders)
				{					
					/* If this reader needs to rest, retrieve the minimum time to wait, and skip it */
					if (reader.IsDelayed(ref wait_ms))
						continue;
										
					is_bus_ok = false;
					
					/* Is there a specific message to send (R-OK, R-Nack or Iblock with payload) ? */
					SpringCardIWM2_Serial_Reader.NextMessage msg = reader.GetMessageToSend();
					if (msg != null)
					{
						/* -> yes there is */
						switch (msg.type)
						{
							case SpringCardIWM2_Serial_Reader.READER_TO_SCHEDULER_SEND_I_BLOCK :
								is_bus_ok = SendIBlock(reader, msg.payload);
								break;
							
							case SpringCardIWM2_Serial_Reader.READER_TO_SCHEDULER_SEND_R_OK :
								is_bus_ok = SendROKBlock(reader);
								break;
							
							case SpringCardIWM2_Serial_Reader.READER_TO_SCHEDULER_SEND_R_NACK :
								reader.com_errors++;
								is_bus_ok = SendRNackBlock(reader);
								break;
								
							case SpringCardIWM2_Serial_Reader.READER_TO_SCHEDULER_TERMINATE_ME :
								DropReader(reader.GetAddress());
								UnMonitorReader(reader.GetAddress());
								is_bus_ok = true;
								break;
								
							default :
								break;
						}
						
					} else
					{
						/* -> no there is not: send an empty I-block */
						is_bus_ok = SendIBlock(reader);
					}
					
					
					if (is_bus_ok)
					{
						/* The message has been sent: get and analyse the reader's answer */
						byte rcvd_len=0;
						byte[] rcvd = ReceiveEscapedFrame(timeout, out rcvd_len);

						if (rcvd_len > 0)
						{
							if (IsLRCCorrect(rcvd, rcvd_len))
							{
								/* Increment block number only when the frame was successfully analyzed: */
								/* if a R-NACK needs to be sent, the block number must remain unchanged  */
								if (AnalyseReceivedFrame(rcvd, (byte) rcvd_len, reader.GetAddress()))
								{
									reader.IncrementBlockNumber();
									reader.com_errors = 0;
								}
								
							} else
							{
								reader.Enqueue_Send_RNack_Block();
							}
							
							
						} else
						{
							/* Nothing received */
							reader.com_errors++;
						}						
						
						/* If 3 errors in a row: stop talking to the reader */
						if (reader.com_errors >= 3)
							DropReader(reader.GetAddress());
						
					} else
					{
						/* Something isn't right: the message has not been sent on the bus -> Exit ! */	
						bool join_thread = false; /* Don't wait for the thread to exit: we are the thread and we are exiting ! */
						Terminate(join_thread);
						return;
						
					}
				
				}
				
				if (flag_reader_to_remove)
					PurgeAllDroppedReaders();
				
				/* wait_ms is the minimum time to wait, before speaking to the next reader */
				if (wait_ms > 0)
					Thread.Sleep(wait_ms);
				
			}
		
		}
		
		/* This method sends an S-Enum block to all the specified addresses on the COM port. 				*/
		/* It is called the first time the scheduler is launched, and every 10 seconds after that.	*/
		public void Enumerate(bool first_time)
		{
			byte[] buffer = new byte[100];
			byte offset = 0;
			
			if (first_time)
			{
				if (!Open())
				{
					bool join_thread = false; /* Don't wait for the thread to exit: we are the thread and we are exiting ! */
					Terminate(join_thread);
					return;
				}
			}
			
			while (continueRunning)
			{
				for (byte i=min_address; i<=max_address; i++)
				{
					/* stx */
					offset = 0;
					buffer[offset ++] = ASCII_STX;
					
					/* type */
					buffer[offset++] = IWM2_MK2_PROTO_S_ENUM;
	
					/* addr */
					buffer[offset++] = i;
	
					/* lrc */
					buffer[offset++] = CalculateLRC(buffer, 3);			
					
					/* etx */
					buffer[offset++] = ASCII_ETX;
					
					byte out_len = 0;
					byte[] res = EscapeForbiddenBytes(buffer, offset, out out_len);
					
					if (show_frames)
						LogByteArrayEmitted(res, out_len);
					
					try
					{
						port.Write(res, 0, out_len);
					} catch
					{
						break;
					}
					
					byte rcvd_len = 0;
					int timeout = 5;
					byte[] rcvd = ReceiveEscapedFrame(timeout, out rcvd_len);
					//LogByteArrayReceived(rcvd, rcvd_len);
					if (rcvd_len > 0)
						if (IsLRCCorrect(rcvd, rcvd_len))
							AnalyseReceivedFrame(rcvd, (byte)rcvd_len, i);

				}
				
				if (InstalledReaders.Count > 0)
				{
					if (first_time)
						PlanifyNextEnumerations();
					
					TalkToReaders();
					
				}	else
				{
					/* No readers detected */
					if (first_time)
					{
						/* Exit ! */
						bool join_thread = false; /* Don't wait for the thread to exit: we are the thread and we are exiting ! */
						Terminate(join_thread);
						return;
					} else
					{
						/* No reader is responding, but we got answers in the past (otherwise	*/
 						/* the thread would have exited): sleep 5 seconds and try again				*/
	 					Thread.Sleep(5000);
					}
				
				}
			
			}
			
		}

#endregion
	
	
#region Start / Stop

		/* Public method to start communication on the bus, via a 		*/
		/* background thread (in order not to freeze the main screen)	*/
		public void Start(byte AddressMin, byte AddressMax)
		{
			min_address = AddressMin;
			max_address = AddressMax;
			
			InstalledReaders 	 = new List<SpringCardIWM2_Serial_Reader>();
			UserManagedReaders = new List<SpringCardIWM2_Serial_Reader>();
			
			bool first_time = true;
			serialProtocolThread = new Thread(() => Enumerate(first_time));
			serialProtocolThread.Start();
			
		}

		/* This method stops all communications on the bus. The parameter		 */
		/* must be set to "false" if the method is called within the context */
		/* of the main thread, to "true" otherwise													 */
		private void Terminate(bool join_thread)
		{
			LogString(port.PortName + ":Stop scheduler");
			continueRunning = false;
			
			if (timer != null)
			{
				timer.Dispose();
				timer.Close();
				timer = null;
			}
			
			if (join_thread)
				serialProtocolThread.Join();

			if (UserManagedReaders != null)
			{
				foreach (SpringCardIWM2_Serial_Reader reader in UserManagedReaders)
				{
					LogString(port.PortName+":Unmonitor reader " + reader.GetAddress());
					reader.SetDisposed();
				}
			}
								
			if (InstalledReaders != null)
			{
				foreach (SpringCardIWM2_Serial_Reader reader in InstalledReaders)
				{
					LogString(port.PortName+":Uninstall reader " + reader.GetAddress());
					reader.SetDisposed();
				}
			}
			
			InstalledReaders = null;
			UserManagedReaders = null;			
			Close();
			
			if (_scheduler_stopped_callback != null)
				_scheduler_stopped_callback(port.PortName);			
		}
		
		/* Public method, called to stop all communications on the bus */
		public void Stop()
		{
 			bool join_thread = true;
			Terminate(join_thread);
		}
		
#endregion
	
	
#region Constructor
		public SpringCardIWM2_Serial_Scheduler(string com)
		{
			port = new SerialPort(com, 38400, Parity.None, 8, StopBits.One);
		}
#endregion		
		
	}
	
}
