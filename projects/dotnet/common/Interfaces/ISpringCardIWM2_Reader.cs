/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 09/12/2015
 * Time: 13:50
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;

namespace SpringCard.IWM2
{	
	
	/* This interface inherits from the ISpringCardIWM2_Device interface.					*/
	/* It defines prototypes of methods, relevant to all IWM2 SpringCard readers. */	
	public interface ISpringCardIWM2_Reader : ISpringCardIWM2_Device
	{
	
		/* This method defines the callback method to be called, when the reader reads a badge */ 
		void SetBadgeReceivedCallback(BadgeReceivedCallback cb);

		/* This method defines the callback method to be called, when the reader detects a badge insertion */
		void SetBadgeInsertedCallback(BadgeInsertedCallBack cb);
		
		/* This method defines the callback method to be called, when the reader detects a badge removal */
		void SetBadgeRemovedCallback(BadgeRemovedCallBack cb);
		
		/* This method starts or stops the reader 									*/
		/* The "mode" can have the following values :								*/
		/* - 00: Reader goes OFF (RF field OFF, no activity on RF)	*/
		/* - 01: reader goes ON																			*/
		void StartStopReader(byte mode);
		
		/* This method clears the LEDs */
		void ClearLeds();
		
		/* This method sets the red and green LEDs 	 */
		/* Each LED may have a different value:	 	 	 */
		/* - 00: LED OFF														 */
		/* - 01: LED ON															 */
		/* - 02: LED blinks slowly									 */
		/* - 03: LED blinks quickly									 */
		void SetLeds(byte red, byte green);
		
		/* This method sets the red and green LEDs, for a specific duration in seconds */
		/* Each LED may have a different value:		 	 */
		/* - 00: LED OFF														 */
		/* - 01: LED ON															 */
		/* - 02: LED blinks slowly									 */
		/* - 03: LED blinks quickly									 */
		void SetLeds(byte red, byte green, UInt16 sec);
		
		/* This method sets the buzzer 					*/
		/* The followign values are accepted:		*/
		/* - 00: buzzer OFF											*/
		/* - 01: buzzer ON											*/
		/* - 02: buzzer short sequence					*/
		/* - 03: buzzer long sequence						*/
		void SetBuzzer(byte sequence);		
	
	}
	
}
