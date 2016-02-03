/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 11/12/2015
 * Time: 10:26
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Collections.Concurrent;

namespace SpringCard.IWM2
{
	
	/* This object represents a SpringCard FunkyGate reader(MK2 protocol on RS485 bus).												*/
 	/* It implements the ISpringCardIWM2_Reader: it must define all the methods defined in the interface. 		*/
	/* This object also inherits from the SpringCardIWM2_Device class : it must override the abstract methods */
	
	public class SpringCardIWM2_Serial_Reader : SpringCardIWM2_Device, ISpringCardIWM2_Reader
	{

#region Constants to communicate with the scheduler
		public const byte READER_TO_SCHEDULER_SEND_I_BLOCK	= 0x01;
		public const byte READER_TO_SCHEDULER_SEND_R_OK			= 0x02;
		public const byte READER_TO_SCHEDULER_SEND_R_NACK		= 0x03;
		public const byte READER_TO_SCHEDULER_TERMINATE_ME	= 0x04;
#endregion


#region Private members

		/* Current block number (4 lower bits in the TYPE byte) */
		public byte block_number;
		
		/* Number of communication errors in a row */
		public byte com_errors;
		
		/* Boolean indicating if the reader is monitored by a "serial scheduler" object */
		public bool is_scheduled;
	
		/* The "serial scheduler" talking to the reader */
		private SpringCardIWM2_Serial_Scheduler sched;
		
		/* The reader's address on the COM port */
		private byte address;

		/* Members enabling to stop talking to the reader	*/
		/* for a time, when a S-WAIT block is received 		*/
		private TimeSpan keep_cool_delay;					
		private DateTime keep_cool_until;

		/* Thread safe FIFO queue, to send messages to the serial scheduler managing the COM port */
		private BlockingCollection<NextMessage> queue;
		
#endregion
	
	
#region Accessors
		public SpringCardIWM2_Serial_Scheduler GetScheduler()
		{
			return sched;
		}
		
		public byte GetAddress()
		{
			return address;
		}
#endregion


#region Block number management
		public void IncrementBlockNumber()
		{
			block_number++;
			if (block_number >= 0x10)
				block_number = 0;
		}
#endregion


#region Reader delaying, when a S-WAIT block is received		
		
		/* This method, called by the scheduler, indicates that the reader must */
		/* be left alone for at least 500 ms (a S-Wait block has been received) */
		public void SetDelay()
		{
			DateTime now = DateTime.Now;
			keep_cool_until = now.Add(keep_cool_delay);
		}

		/* This method indicates if the reader must be left alone.		*/ 
		/* The parameter is used as a reference, to indicate the time	*/
		/* left before talking to the reader again										*/
		public bool IsDelayed(ref int wait_ms)
		{	
			DateTime now = DateTime.Now;
			TimeSpan span = now - keep_cool_until;
			int diff_ms = (int) span.TotalMilliseconds;
			
			if (diff_ms < 0 )
			{
				/* This reader must be left alone for at least "diff_ms" milliseconds */
				
				/* Only assign a new value for wait_ms, when wait_ms is not set, or when	*/ 
				/* wait_ms is greater than diff_ms (so that the scheduler sleeps less)		*/
				if ((wait_ms==0) || (diff_ms < wait_ms))
					wait_ms = diff_ms;
				
				return true;
				
			} else
			{
				/* No need to wait */
				return false;
			}
			
		}
		
#endregion	
		
		
#region Callbacks			
		
		/* Callbacks are methods called by this object, when a specific event 	*/
 		/* has occured. See the notes in the "ISpringCardIWM2_Reader" interface */
 		/* to understand the role of each one of them 													*/
		
		public void SetBadgeReceivedCallback(BadgeReceivedCallback cb)
		{
			_badge_received_callback = cb;
		}
		
		public void SetBadgeInsertedCallback(BadgeInsertedCallBack cb)
		{
			_badge_inserted_callback = cb;
		}
		
		public void SetBadgeRemovedCallback(BadgeRemovedCallBack cb)
		{
			_badge_removed_callback = cb;
		}
		
#endregion


#region Communicate with serial scheduler through a queue		
		
		/* Internal class to store the messages for the serial scheduler */
		public class NextMessage
		{
			public byte type;
			public byte[] payload;
			
			public NextMessage(byte _type, byte[] _payload)
			{
				type = _type;
				payload = _payload;
			}

		}
		
		/* This method is called by the serial scheduler, to know if there is			*/
		/* a specific message to send to the reader, instead of an empty I-block  */
		public NextMessage GetMessageToSend()
		{
			NextMessage msg;
			if (queue.TryTake(out msg, 0))
				return msg;
		
			return null;
		}
		
		protected override void EnqueueCommand(byte[] cmd)
		{
			NextMessage msg = new NextMessage(READER_TO_SCHEDULER_SEND_I_BLOCK, cmd);
			queue.Add(msg);
		}


#endregion


#region Commands received through the Reader Controller (user clicks on the form)
		
		public void Start()
		{
			/* The reader has already been installed and started via the serial */
 			/* scheduler, so, send all status from "CREATED" to "CONNECTED"			*/ 
			
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_CREATED);
			
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_STARTING);
			
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_STARTED);	
			
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_CONNECTED);		
						
			/* Ask for its status */
			GetGlobalStatus();
			
		}
	
		public void Terminate()
		{
			NextMessage msg = new NextMessage(READER_TO_SCHEDULER_TERMINATE_ME, null);
			queue.Add(msg);
		}		
		
#endregion


#region Commands received through the serial scheduler

		/* This method enables the scheduler to remember to send an R-OK block	next time it speaks to the reader */
		public void Enqueue_Send_ROK_Block()
		{
			NextMessage msg = new NextMessage(READER_TO_SCHEDULER_SEND_R_OK, null);
			queue.Add(msg);
		}
			
		/* This method enables the scheduler to remember to send an R-Nack block	next time it speaks to the reader */	
		public void Enqueue_Send_RNack_Block()
		{
			NextMessage msg = new NextMessage(READER_TO_SCHEDULER_SEND_R_NACK, null);
			queue.Add(msg);
		}
			
		/* This method is called when there has been more than 3 errors in a row, or when the	*/ 
 		/* payload is not well-formed, or when the user wants to terminate the communication	*/
		public void SetDropped()
		{
			is_scheduled = false;
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_DISCONNECTED);		
		}
		
		/* This method is called when a previously monitored reader, that has stopped responding  */
		/* and has been dropped as a consequence, starts answering probing requests again					*/
		public void SetScheduled()
		{
			is_scheduled = true;
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_CONNECTED);		
		}
		
		/* This method is called, when the serial scheduler acknowledges 	*/
 		/* that the user doesn't want to monitor the reader anymore				*/
		public void SetTerminated()
		{
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_TERMINATED);
		}
		
		/* This method is called when the user asks the serial scheduler to stop: 	*/
		/* when the COM port is released, the reader is disposed										*/
		public void SetDisposed()
		{
			if (_device_status_changed_callback != null)
				_device_status_changed_callback(IWM2_DEVICE_STATUS_DISPOSED);
		}


#endregion


#region Misc: overriden methods
		protected override void log(string msg)
		{
			sched.LogString(sched.GetPortName() + ":ADD" + String.Format("{0:d02}", address) + ":" + msg);
		}	

		protected override void OnCommunicationError()
		{
			sched.DropReader(address);
		}
#endregion


#region Constructor
		
		public SpringCardIWM2_Serial_Reader(SpringCardIWM2_Serial_Scheduler _sched, byte _address)
		{
			sched 					= _sched;
			address 				= _address;
			block_number 		= 0;
			com_errors			= 0;
			is_scheduled		= true;
			keep_cool_delay	= new TimeSpan(0, 0, 0, 0, 500); 		/* 500 milliseconds */
			keep_cool_until = DateTime.Now;
			queue						= new BlockingCollection<SpringCardIWM2_Serial_Reader.NextMessage>();

		}
		
#endregion

	}
}
