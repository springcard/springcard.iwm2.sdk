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
	/* This interface inherits from the ISpringCardIWM2_Device interface.	It 				 */
	/* defines prototypes of methods, relevant to all IWM2 SpringCard Handydrummers. */
	public interface ISpringCardIWM2_GPIOs : ISpringCardIWM2_Device
	{
		/* This method defines the callback method to be called, when an input (=contact) on the HandyDrummer changes */
		void SetInputChangedCallBack(InputChangedCallBack cb);
		
		/* This method asks for the value of all the HandyDrummer's inputs (=contacts) */ 
		void ReadInputs();
		
		/* This method sets the output (=relay) passed as a parameter */
		void SetOutput(byte output);
		
		/* This method sets the output (=relay) passed as a parameter, for a duration of "sec" seconds */
		void SetOutput(byte output, UInt16 sec);
		
		/* this method clears the output (=relay) */
		void ClearOutput(byte output);

	}
	
}
