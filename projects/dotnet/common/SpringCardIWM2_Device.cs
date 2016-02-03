/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 09/12/2015
 * Time: 16:05
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;

namespace SpringCard.IWM2
{
	/* This abstract class represents a SpringCard IWM2 Device. HandyDrummer	*/ 
 	/* and FunkyGate (TCP or serial) objects inherit from this class. 				*/
	
 	public abstract class SpringCardIWM2_Device
	{

#region Constants: Device's status
		public const byte IWM2_DEVICE_STATUS_CREATED 						= 1;
		public const byte IWM2_DEVICE_STATUS_STARTING 					= 2;
		public const byte IWM2_DEVICE_STATUS_STARTED 						= 3;
		public const byte IWM2_DEVICE_STATUS_DISCONNECTED 			= 4;
		public const byte IWM2_DEVICE_STATUS_CONNECTION_FAILED 	= 5;
		public const byte IWM2_DEVICE_STATUS_CONNECTED 					= 6;
		public const byte IWM2_DEVICE_STATUS_TERMINATED 				= 7;
		public const byte IWM2_DEVICE_STATUS_DISPOSED 					= 8;
#endregion


#region Constants: Application layer
		private 	const byte IWM2_MK2_DEVICE_ID								 = 0x81;
		private 	const byte IWM2_MK2_CMD_PUT_TAMPERS       	 = 0x2F;
		private 	const byte IWM2_MK2_CMD_PUT_TAG              = 0xB0;
		private 	const byte IWM2_MK2_CMD_PUT_TAG_INSERT_EJECT = 0xB1;
		private 	const byte IWM2_MK2_MIO_INPUT_CHANGED		 		 = 0xC0;			
		protected const byte IWM2_MK2_CMD_START_STOP					 = 0x0A;
		protected const byte IWM2_MK2_CMD_SET_LEDS 						 = 0xD0;
		protected const byte IWM2_MK2_CMD_SET_BUZZER  				 = 0xD1;
		protected const byte IWM2_MK2_CMD_READ_INPUT					 = 0x04;
		protected const byte IWM2_MK2_CMD_SET_OUTPUT					 = 0x90;
		protected const byte IWM2_MK2_CMD_CLR_OUTPUT					 = 0xA0;
		protected const byte IWM2_MK2_CMD_CONFIGURATION	 			 = 0x0C;
#endregion


#region Callback methods

		/* Callbacks are specific methods called by a device, when a specific event */
 		/* has occured. See the notes in the "ISpringCardIWM2_Device" interface to 	*/
 		/* understand the role of each one of them 																	*/
 		
		protected DeviceStatusChangedCallback _device_status_changed_callback = null;
		private 	TamperChangedCallback _tamper_changed_callback = null;	
		protected BadgeReceivedCallback _badge_received_callback = null;
		protected BadgeInsertedCallBack _badge_inserted_callback = null;
		protected BadgeRemovedCallBack _badge_removed_callback = null;
		protected InputChangedCallBack _input_changed_callback = null;
		private 	TlvReceivedCallback _tlv_received_callback = null;
		
		public void SetDeviceStatusChangedCallback(DeviceStatusChangedCallback cb)
		{
			_device_status_changed_callback = cb;
		}
					
		public void SetTamperChangedCallback(TamperChangedCallback cb)
		{
			_tamper_changed_callback = cb;
		}		
		
		public void SetTlvReceivedCallback(TlvReceivedCallback cb)
		{
			_tlv_received_callback = cb;
		}
#endregion


#region Abstract methods

		/* Those three methods *must* be overriden in all children classes */
		protected abstract void log(string msg);		
		protected abstract void OnCommunicationError();
		protected abstract void EnqueueCommand(byte[] tlv);
		
#endregion


#region "Application layer" commands

		/* This method analyses the "Application layer" TLVs received from the reader, and takes	*/ 
		/* appropriate action (calling a callback method, or reporting a communication error).		*/
 		/* It returns true, when the TLV has been successfully analysed	.													*/
		public bool AnalyseTLVPayload(byte[] buffer, int len)
		{ 
			/* First byte should be the type */
	    switch (buffer[0])
			{
				case IWM2_MK2_DEVICE_ID 	:
	        /* This TLV contains the device ID */
					if (buffer[1] != 0x00)
					{
						log("Invalid Tag value");
						OnCommunicationError();
						return false;
					}
					
					if (buffer[2] != (len - 3))
					{
						log("Invalid Length value");
						OnCommunicationError();
						return false;
					}
					
					try
					{
						byte[] id = new byte[buffer[2]];
						Array.ConstrainedCopy(buffer, 3, id, 0, buffer[2]);
						String deviceId = System.Text.Encoding.ASCII.GetString(id);
						log("Device Identifier is: " + deviceId);
					} catch (Exception)
					{
						return false;
					}
					break;
					
				
				case IWM2_MK2_CMD_PUT_TAMPERS	:
					/* This TLV contains the value of the FunkyGate tampers */
	        if (buffer[1] != 0x01)
					{
						log("Incorrect L value");
						OnCommunicationError();
						return false;
					}
					if (len != 3)
					{
						log("Incorrect length");
						OnCommunicationError();
						return false;
					}
	        
					if (buffer[2] != 0x00)
						log("Warning - tamper value: " + buffer[2]);
					else
						log("Tampers ok");
					
					if (_tamper_changed_callback != null)
						_tamper_changed_callback(buffer[2]);
						
	        break;
					
					
				case IWM2_MK2_CMD_PUT_TAG	:
	        /* This TLV contains a badge number that has just been read by a FunkyGate */
					if (buffer[1] != 0x00)
					{
						log("Invalid Tag value");
						OnCommunicationError();
						return false;
					}
					
					if (buffer[2] != (len - 3))
					{
						log("Invalid Length value");
						OnCommunicationError();
						return false;
					}
					
					string s = "";
					for (int i=3;i<len;i++)
						s+=String.Format("{0:X02}", buffer[i]);
					log("Badge Read: " + s);
					
					if (_badge_received_callback != null)
						_badge_received_callback(s);

					break;		
				
				
				case IWM2_MK2_CMD_PUT_TAG_INSERT_EJECT :
	        /* This TLV contains a badge number that has been inserted or removed */
					if (buffer[1] != 0x00)
					{
						log("Invalid Tag value");
						OnCommunicationError();
						return false;
					}
					
					if (buffer[2] != (len - 3))
					{
						log("Invalid Length value");
						OnCommunicationError();
						return false;
					}
					
					
					if (buffer[2] == 0)
					{
						/* Badge has been removed */
						log("Badge Removed");
						if (_badge_removed_callback != null)
							_badge_removed_callback();
						
					} else
					{
						/* Badge has been inserted in the reader */									
						string b = "";
						for (int i=3;i<len;i++)
							b+=String.Format("{0:X02}", buffer[i]);
						log("Badge Inserted: " + b);
						
						if (_badge_inserted_callback != null)
							_badge_inserted_callback(b);
					}
					
					break;	
					
				case IWM2_MK2_MIO_INPUT_CHANGED :
	        /* This TLV contains the information that an input has changed on the HandyDrumer */
					if (len < 3)
					{
						log("Handy Drummer: Invalid length received");
						OnCommunicationError();
						return false;
					}
					
					if (buffer[2] != 1)
					{
						log("Handy Drummer: Invalid Length value");
						OnCommunicationError();
						return false;
					}
					
					log("Input "+ buffer[1]+ ": value=" + buffer[3]);
					
					if (_input_changed_callback != null)
						_input_changed_callback(buffer[1], buffer[3]);

					break;
				
				default:
					/* This TLV is unrecognized */
	        log("Unrecognized Tag value");
	        if (_tlv_received_callback != null)
	        {
	        	byte tag;
	        	byte tag_idx = 0;
	        	byte[] val;
	        	byte l;
	        	byte offset;
	        	
	        	tag = buffer[0];
	        	if (tag >= 0x80)
	        	{
	        		tag_idx = buffer[1];
	        		l = buffer[2];
	        		offset = 3;	        		
	        	} else
	        	{
	        		l = buffer[1];
	        		offset = 2;
	        	}
	       	        	
        		val = new byte[l];
        		if (l == (len - offset))
        		{
        			Array.ConstrainedCopy(buffer, offset, val, 0, l);
        			_tlv_received_callback(tag, tag_idx, l, val);
        		}

	        	
	        }

					break;
			}
			
			return true;
		}


		/* For all the following commands, see the descriptions in the following interfaces: 	*/
		/* - ISpringCardIWM2_Device - ISpringCardIWM2_Reader - ISpringCardIWM2_GPIOs					*/ 
		
		public void GetGlobalStatus()
		{  
	    /* Generate the TLV  ... */
 	    byte[] tlv = new byte[2];
			tlv[0] = 0x00;
			tlv[1] = 0x00;
	    
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Get global status");		
		}

		public void WriteConfigurationRegister(byte register, byte[] value, byte value_len)
		{
			/* Generate the TLV  ... */
			byte[] tlv = new byte[3+value_len];
			tlv[0] = IWM2_MK2_CMD_CONFIGURATION;
			tlv[1] = (byte) (1 + value_len);
			tlv[2] = register;
			if (value != null)
				Array.ConstrainedCopy(value, 0, tlv, 3, value_len);

			/* and send it to the device */
			EnqueueCommand(tlv);
			
			if (value == null)
			{
				log("Erase configuration register");
			} else
			{
				log("Write configuration register");
			}
			
		}
		
		public void EraseConfigurationRegister(byte register)
		{
			WriteConfigurationRegister(register, null, 0);
		}
		
		public void ResetDevice()
		{
	    /* Generate the TLV  ... */
	    byte[] tlv = new byte[2];
			tlv[0] = IWM2_MK2_CMD_CONFIGURATION;
			tlv[1] = 0x00;
	    
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Reset the device");
			
		}

		public void StartStopReader(byte mode)
		{
			/* Generate the TLV  ... */
			byte[] tlv = new byte[3];
			tlv[0] = IWM2_MK2_CMD_START_STOP;
			tlv[1] = 0x01;
			tlv[2] = mode;
			
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Start/Stop the reader");
			
		}

		public void ClearLeds()
		{
	    /* Generate the TLV  ... */
	    byte[] tlv = new byte[3];
			tlv[0] = IWM2_MK2_CMD_SET_LEDS;
			tlv[1] = 0x00;
			tlv[2] = 0x00;
			
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Clear the LEDs");
			
		}

		public void SetLeds(byte red, byte green)
		{
		  /* Generate the TLV  ... */
		  byte[] tlv = new byte[5];
			tlv[0] = IWM2_MK2_CMD_SET_LEDS;
			tlv[1] = 0x00;
			tlv[2] = 0x02;
			tlv[3] = red;
			tlv[4] = green;
			
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Set the LEDs (red:" + red + ",green=" + green + ")");
		}

		public void SetLeds(byte red, byte green, UInt16 sec)
		{
			/* Generate the TLV  ... */
			byte[] tlv = new byte[7];
			tlv[0] = IWM2_MK2_CMD_SET_LEDS;
			tlv[1] = 0x00;
			tlv[2] = 0x04;
			tlv[3] = red;
			tlv[4] = green;
			tlv[5] = (byte) (sec >> 8);
			tlv[6] = (byte) (sec & 0x00FF);
			
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Set the LEDs (red:" + red + ",green=" + green + ") for " + String.Format("{0:d02}", sec) + " seconds");
		
		}

		public void SetBuzzer(byte sequence)
		{
			/* Generate the TLV  ... */
			byte[] tlv = new byte[4];
			tlv[0] = IWM2_MK2_CMD_SET_BUZZER;
			tlv[1] = 0x00;
			tlv[2] = 0x01;
			tlv[3] = sequence;
			
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Set buzzer (sequence: " + sequence + ")");
			
		}

		public void ReadInputs()
		{
			/* Generate the TLV  ... */
			byte[] tlv = new byte[2];
			tlv[0] = IWM2_MK2_CMD_READ_INPUT;
			tlv[1] = 0x00;
			
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Read inputs");
			
		}

		public void SetOutput(byte output)
		{			
	    /* Generate the TLV  ... */
	    byte[] tlv = new byte[3];
			tlv[0] = IWM2_MK2_CMD_SET_OUTPUT;
			tlv[1] = output;
			tlv[2] = 0x00;
			
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Set output " + output);
			
		}
		
		public void SetOutput(byte output, UInt16 sec)
		{		
	    /* Generate the TLV  ... */
	    byte[] tlv = new byte[5];
			tlv[0] = IWM2_MK2_CMD_SET_OUTPUT;
			tlv[1] = output;
			tlv[2] = 0x02;
			tlv[3] = (byte) (sec >> 8);
			tlv[4] = (byte) (sec & 0x00FF);
			
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Set output " + output + " for " + String.Format("{0:d02}", sec) + " seconds");
		
		}
		
		public void ClearOutput(byte output)
		{
	    /* Generate the TLV  ... */
	    byte[] tlv = new byte[3];
			tlv[0] = IWM2_MK2_CMD_CLR_OUTPUT;
			tlv[1] = output;
			tlv[2] = 0x00;
			
			/* and send it to the device */
			EnqueueCommand(tlv);
			
			log("Clear output " + output);
		
		}

#endregion
		
	}
	
}
