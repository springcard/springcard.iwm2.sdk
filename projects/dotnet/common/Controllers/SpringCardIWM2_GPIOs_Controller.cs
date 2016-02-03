/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 08/12/2015
 * Time: 13:33
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

using SpringCard.IWM2;

namespace TCPReader
{
	/* This "Controller" is a form enabling to interact with HandyDrummers. */
	/* It is added in the MainForm, when a HandyDrummer is selected.				*/
	
	public partial class SpringCardIWM2_GPIOs_Controller : UserControl
	{
		/* This is the HanddyDrummer "object", associated with this control: it can be */
		/* anything, as long as it implements the ISpringCardIWM2_GPIOs interface 	*/	
		ISpringCardIWM2_GPIOs handydrummer;
		
#region Callbacks	

		/* This method is called when the HandyDrummer's status changes */
		delegate void OnDeviceStatusChangedInvoker(byte status);
		public void OnDeviceStatusChanged(byte status)
		{
			/* The OnDeviceStatusChanged method is called as a delegate (callback) by the HandyDrummer object,  */
			/* witin its backgroung thread. Therefore we must use the BeginInvoke syntax to switch back from   	*/
			/* the context of the background thread to the context of the application's main thread. Otherwise  */
			/* we'll get a security violation when trying to access the window's visual components (that belong */
			/* to the application's main thread and can't be safely manipulated by background threads).         */
			if (InvokeRequired)
			{
				this.BeginInvoke(new OnDeviceStatusChangedInvoker(OnDeviceStatusChanged), status);
				return;
			}
			
			switch (status)
			{
				case SpringCardIWM2_Device.IWM2_DEVICE_STATUS_CREATED :
					tbCurrentState.Text = "Created";
					tbCurrentState.BackColor = System.Drawing.Color.White;
					tbCurrentState.ForeColor = System.Drawing.SystemColors.ControlText;
					break;
					
				case SpringCardIWM2_Device.IWM2_DEVICE_STATUS_STARTING :
					tbCurrentState.Text = "Starting";
					tbCurrentState.BackColor = System.Drawing.Color.Yellow;
					tbCurrentState.ForeColor = System.Drawing.SystemColors.ControlText;
					break;

				case SpringCardIWM2_Device.IWM2_DEVICE_STATUS_STARTED :
					tbCurrentState.Text = "Started";
					tbCurrentState.BackColor = System.Drawing.Color.Orange;
					tbCurrentState.ForeColor = System.Drawing.SystemColors.ControlText;
					break;				
					
				case SpringCardIWM2_Device.IWM2_DEVICE_STATUS_DISCONNECTED :
					tbCurrentState.Text = "Disconnected";
					tbCurrentState.BackColor = System.Drawing.Color.Red;
					tbCurrentState.ForeColor = System.Drawing.SystemColors.Info;
					break;				
										
				case SpringCardIWM2_Device.IWM2_DEVICE_STATUS_CONNECTION_FAILED :
					tbCurrentState.Text = "Connection failed";
					tbCurrentState.BackColor = System.Drawing.Color.Red;
					tbCurrentState.ForeColor = System.Drawing.SystemColors.Info;
					break;	
				
				case SpringCardIWM2_Device.IWM2_DEVICE_STATUS_CONNECTED :
					tbCurrentState.Text = "Connected";
					tbCurrentState.BackColor = System.Drawing.Color.Green;
					tbCurrentState.ForeColor = System.Drawing.SystemColors.Info;
					break;	
					
				case SpringCardIWM2_Device.IWM2_DEVICE_STATUS_TERMINATED :
					tbCurrentState.Text = "Terminated";			
					tbCurrentState.BackColor = System.Drawing.Color.BlueViolet;
					tbCurrentState.ForeColor = System.Drawing.SystemColors.Info;
					DisableButtons();
					break;	
				
				case SpringCardIWM2_Device.IWM2_DEVICE_STATUS_DISPOSED :
					tbCurrentState.Text = "Disposed";
					tbCurrentState.BackColor = System.Drawing.SystemColors.Highlight;
					tbCurrentState.ForeColor = System.Drawing.SystemColors.ControlText;
					DisableButtons();
					break;
										
				default :
					break;
			}

			Application.DoEvents();
			
		}
		
		/* This method is called when the value of an input changes on the HandyDrummer */
		delegate void OnInputChangedInvoker(byte input_idx, byte input_state);
		public void OnInputChanged(byte input_idx, byte input_state)
		{
			/* The OnInputChanged method is called as a delegate (callback) by the ISpringCardIWM2_GPIOs object, */
			/* witin its backgroung thread. Therefore we must use the BeginInvoke syntax to switch back from   	 */
			/* the context of the background thread to the context of the application's main thread. Otherwise   */
			/* we'll get a security violation when trying to access the window's visual components (that belong  */
			/* to the application's main thread and can't be safely manipulated by background threads).          */
			
			if (InvokeRequired)
			{
				this.BeginInvoke(new OnInputChangedInvoker(OnInputChanged), input_idx, input_state);
				return;
			}
			
			switch (input_idx)
			{
				case 0:
					in0.BackColor = (input_state == 1) ? System.Drawing.Color.DarkOrange : System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
					break;

				case 1:
					in1.BackColor = (input_state == 1) ? System.Drawing.Color.DarkOrange : System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
					break;
				
				case 2:
					in2.BackColor = (input_state == 1) ? System.Drawing.Color.DarkOrange : System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
					break;
				
				case 3:
					in3.BackColor = (input_state == 1) ? System.Drawing.Color.DarkOrange : System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
					break;
				
				case 4:
					in4.BackColor = (input_state == 1) ? System.Drawing.Color.DarkOrange : System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
					break;
				
				case 5:
					in5.BackColor = (input_state == 1) ? System.Drawing.Color.DarkOrange : System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
					break;
				
				case 6:
					in6.BackColor = (input_state == 1) ? System.Drawing.Color.DarkOrange : System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
					break;
				
				case 7:
					in7.BackColor = (input_state == 1) ? System.Drawing.Color.DarkOrange : System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
					break;					
				
				default :
					break;
			}
			
		}
		
		/* This method is called when an unknown TLV is received from the HandyDrummer */
		/* This is just another example: the method does nothing on the screen				 */
		delegate void OnTlvReceivedInvoker(byte tag, byte tag_idx, byte len, byte[] val);
		public void OnTlvReceived(byte tag, byte tag_idx, byte len, byte[] val)
		{
			/* Do nothing */
		}
		
#endregion

		
#region Buttons
		
		/* Terminate the HandyDrummer object */
		void BtStopClick(object sender, EventArgs e)
		{
			handydrummer.Terminate();	
		}
		
		/* Set/clear output (=relay) 0 */
		bool bt0_On = false;
		void Bt0Click(object sender, EventArgs e)
		{
			if (!bt0_On)
			{
				handydrummer.SetOutput(0);
				bt0.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
				bt0_On = true;
			} else
			{
				handydrummer.ClearOutput(0);
				bt0.BackColor = System.Drawing.Color.Red;
				bt0_On = false;
			}
				
		}
		
		/* Set/clear output (=relay) 1 */
		bool bt1_On = false;
		void Bt1Click(object sender, EventArgs e)
		{
			if (!bt1_On)
			{
				handydrummer.SetOutput(1);
				bt1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
				bt1_On = true;
			} else
			{
				handydrummer.ClearOutput(1);
				bt1.BackColor = System.Drawing.Color.Red;
				bt1_On = false;
			}
		}
		
		/* Set/clear output (=relay) 2 */
		bool bt2_On = false;
		void Bt2Click(object sender, EventArgs e)
		{
			if (!bt2_On)
			{
				handydrummer.SetOutput(2);
				bt2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
				bt2_On = true;
			} else
			{
				handydrummer.ClearOutput(2);
				bt2.BackColor = System.Drawing.Color.Red;
				bt2_On = false;
			}
		}
		
		/* Set/clear output (=relay) 3 */
		bool bt3_On = false;
		void Bt3Click(object sender, EventArgs e)
		{
			if (!bt3_On)
			{
				handydrummer.SetOutput(3);
				bt3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
				bt3_On = true;
			} else
			{
				handydrummer.ClearOutput(3);
				bt3.BackColor = System.Drawing.Color.Red;
				bt3_On = false;
			}
		}
		
		/* Set/clear output (=relay) 4 */
		bool bt4_On = false;
		void Bt4Click(object sender, EventArgs e)
		{
			if (!bt4_On)
			{
				handydrummer.SetOutput(4);
				bt4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
				bt4_On = true;
			} else
			{
				handydrummer.ClearOutput(4);
				bt4.BackColor = System.Drawing.Color.Red;
				bt4_On = false;
			}
		}
		
		/* Set/clear output (=relay) 5 */
		bool bt5_On = false;
		void Bt5Click(object sender, EventArgs e)
		{
			if (!bt5_On)
			{
				handydrummer.SetOutput(5);
				bt5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
				bt5_On = true;
			} else
			{
				handydrummer.ClearOutput(5);
				bt5.BackColor = System.Drawing.Color.Red;
				bt5_On = false;
			}			
		}
		
		/* Set/clear output (=relay) 6 */
		bool bt6_On = false;
		void Bt6Click(object sender, EventArgs e)
		{
			if (!bt6_On)
			{
				handydrummer.SetOutput(6);
				bt6.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
				bt6_On = true;
			} else
			{
				handydrummer.ClearOutput(6);
				bt6.BackColor = System.Drawing.Color.Red;
				bt6_On = false;
			}			
		}
		
		/* Set/clear output (=relay) 7 */
		bool bt7_On = false;
		void Bt7Click(object sender, EventArgs e)
		{
			if (!bt7_On)
			{
				handydrummer.SetOutput(7);
				bt7.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
				bt7_On = true;
			} else
			{
				handydrummer.ClearOutput(7);
				bt7.BackColor = System.Drawing.Color.Red;
				bt7_On = false;
			}			
		}
		
		void DisableButtons()
		{
			bt0.Enabled = false;
			bt1.Enabled = false;
			bt2.Enabled = false;
			bt3.Enabled = false;
			bt4.Enabled = false;
			bt5.Enabled = false;
			bt6.Enabled = false;
			bt7.Enabled = false;
			
		}
		
#endregion


#region Constructor
		
		public SpringCardIWM2_GPIOs_Controller(ISpringCardIWM2_GPIOs hd, string title)
		{
			InitializeComponent();
			
			handydrummer = hd;
			tbIP.Text = title;
			
			/* Set the appropriate callbacks */
			handydrummer.SetDeviceStatusChangedCallback(new DeviceStatusChangedCallback(OnDeviceStatusChanged));
			handydrummer.SetInputChangedCallBack(new InputChangedCallBack(OnInputChanged));
			handydrummer.SetTlvReceivedCallback(new TlvReceivedCallback(OnTlvReceived));

			/* Start, and get all input values */			
			handydrummer.Start();
			handydrummer.ReadInputs();

		}
#endregion

	}
	
}
