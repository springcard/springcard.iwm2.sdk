/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 09/12/2015
 * Time: 15:44
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;

namespace SpringCard.IWM2
{
	
	/* All the following delegates are used to set the callback methods */
	/* ---------------------------------------------------------------- */
	
	public delegate void DeviceStatusChangedCallback(byte status);
	public delegate void BadgeReceivedCallback(string badge_read);
	public delegate void BadgeInsertedCallBack(string badge_read);
	public delegate void BadgeRemovedCallBack();
	public delegate void InputChangedCallBack(byte input_idx, byte input_state);
	public delegate void TlvReceivedCallback(byte tag, byte tag_idx, byte len, byte[] val);
	public delegate void TamperChangedCallback(byte tamper_state);
	

	/* This interface defines prototypes of methods, relevant to all IWM2 SpringCard devices */
	public interface ISpringCardIWM2_Device
	{

		/* This method defines the callback method to be called, when the device's status changes */ 
		void SetDeviceStatusChangedCallback(DeviceStatusChangedCallback cb);
		
		/* This method defines the callback method to be called, when an unknown TLV is reveived from the device */
		void SetTlvReceivedCallback(TlvReceivedCallback cb);
		
		/* This method asks for the device's global status */
		void GetGlobalStatus();
		
		/* This method writes the value of a configuration register */
		void WriteConfigurationRegister(byte register, byte[] value, byte value_len);
		
		/* This method erases the value of a configuration register */
		void EraseConfigurationRegister(byte register);
		
		/* This method resets the device */ 
		void ResetDevice();
		
		/* This method starts the device */
		void Start();
		
		/* This method terminates the communication with the device */
		void Terminate();
		
	}

}
