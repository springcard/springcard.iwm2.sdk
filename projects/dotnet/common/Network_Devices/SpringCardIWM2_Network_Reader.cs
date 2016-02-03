/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 27/11/2015
 * Time: 14:59
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Net;

namespace SpringCard.IWM2
{
	/* This class represents a SpringCard FunkyGate IP. It implements the ISpringCardIWM2_Reader:		*/
 	/* therefore it must implement all the methods defined in the interface. It also inherits from	*/ 
 	/* the SpringCardIWM2_Network_Device																														*/

	public class SpringCardIWM2_Network_Reader : SpringCardIWM2_Network_Device, ISpringCardIWM2_Reader
	{

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

		
#region Constructor		
		public SpringCardIWM2_Network_Reader(IPAddress _ip, int _port, byte _com_type, byte[] _key) : base(_ip, _port, _com_type, _key){}
#endregion		
		
	}
	
}
