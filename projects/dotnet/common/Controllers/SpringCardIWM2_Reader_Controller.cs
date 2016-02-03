/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 27/11/2015
 * Time: 10:07
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

using SpringCard.IWM2;

namespace SpringCard.IWM2
{

	/* This "Controller" is a form enabling to interact with readers. */
	/* It is added in the MainForm, when a reader is selected.				*/
	
	public partial class SpringCardIWM2_Reader_Controller : UserControl
	{

#region Constants: Reader type
		
		public const byte TYPE_FKG_RS485 = 0x01;
		public const byte TYPE_FKG_TCP	 = 0x02;
		
#endregion


#region Private members
		
		/* This is the reader "object", associated with this control: it can be a serial,		*/ 
 		/* or a TCP reader, as long as it implements the ISpringCardIWM2_Reader interface 	*/
		private ISpringCardIWM2_Reader reader;

		/* Those members enable to measure the time spent since the last badge read */
		private TimeSpan elapsedTime;
		private DateTime StartTime;
		
		/* Those booleans keep trace of the LED status */
		private bool btRedOn 		= false;
		private bool btGreenOn 	= false;
		
#endregion


#region Callback methods
		
		/* This method is called by the reader, when a badge number is read */
		delegate void OnBadgeReadInvoker(string badge_read);
		public void OnBadgeRead(string badge_read)
		{
			/* The OnBadgeRead method is called as a delegate (callback) by the ISpringCardIWM2_Reader		    	*/
			/* witin its backgroung thread. Therefore we must use the BeginInvoke syntax to switch back from   	*/
			/* the context of the background thread to the context of the application's main thread. Otherwise  */
			/* we'll get a security violation when trying to access the window's visual components (that belong */
			/* to the application's main thread and can't be safely manipulated by background threads).         */
			if (InvokeRequired)
			{
				this.BeginInvoke(new OnBadgeReadInvoker(OnBadgeRead), badge_read);
				return;
			}
			
			/* Launch the timer, print the badge number and set the elapsed time */
			StartTime = DateTime.Now;
			timer1.Interval = 1000;
			timer1.Enabled = true;
			timer1.Start();
			tbBadgeRead.Text = badge_read;
			tbWhen.Text = "00:00:00";
			
		}
		
		/* This method is called when the reader's status changes */
		delegate void OnDeviceStatusChangedInvoker(byte status);
		public void OnDeviceStatusChanged(byte status)
		{
			/* The OnDeviceStatusChanged method is called as a delegate (callback) by the reader object			    */
			/* witin its backgroung thread. Therefore we must use the BeginInvoke syntax to switch back from   	*/
			/* the context of the background thread to the context of the application's main thread. Otherwise  */
			/* we'll get a security violation when trying to access the window's visual components (that belong */
			/* to the application's main thread and can't be safely manipulated by background threads).         */
			if (InvokeRequired)
			{
				this.BeginInvoke(new OnDeviceStatusChangedInvoker(OnDeviceStatusChanged), status);
				return;
			}
			
			/* Update the status */
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
		
		/* This method is called every second, to update the "Since" text area */
		void Timer1Tick(object sender, EventArgs e)
		{
			elapsedTime = DateTime.Now - StartTime;
			tbWhen.Text = elapsedTime.ToString(@"hh\:mm\:ss"); 
		}
		
#endregion

		
#region Buttons

		/* This method is called when the communication with the reader */
 		/* is definitely stopped: disable all buttons from the form			*/ 
		private void DisableButtons()
		{
			btGreen.Enabled = false;
			btRed.Enabled = false;
			btBuzzer.Enabled = false;
			btGreen.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));					
			btRed.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))));
			if (timer1 != null)
				timer1.Dispose();
		}
		
		void BtBuzzerClick(object sender, EventArgs e)
		{
			reader.SetBuzzer(3);
		}
		
		void BtRedClick(object sender, EventArgs e)
		{
			if (!btRedOn)
			{
				btRed.BackColor = System.Drawing.Color.Red;
				
				if (btGreenOn)
				{
					reader.SetLeds(1, 1);
				} else
				{
					reader.SetLeds(1, 0);
				}

				btRedOn = true;
			} else
			{
				btRed.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))));
				btRedOn = false;
				if (btGreenOn)
				{
					reader.SetLeds(0, 1);
				} else
				{
					reader.SetLeds(0, 0);
				}
			}
				
		}	

		void BtGreenClick(object sender, EventArgs e)
		{
			if (!btGreenOn)
			{
				btGreen.BackColor = System.Drawing.Color.Green;
				if (btRedOn)
				{
					reader.SetLeds(1, 1);
				} else
				{
					reader.SetLeds(0, 1);
				}
				
				btGreenOn = true;
			} else
			{
				btGreen.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
				if (btRedOn)
				{
					reader.SetLeds(1, 0);
				} else
				{
					reader.SetLeds(0, 0);
				}
				btGreenOn = false;
			}

		}		

		void BtStopClick(object sender, EventArgs e)
		{
			reader.Terminate();
			if (timer1 != null)
				timer1.Dispose();
		}
		
#endregion		


#region Constructor

		public SpringCardIWM2_Reader_Controller(ISpringCardIWM2_Reader r, string title, byte type)
		{
			InitializeComponent();
			
			/* Set the picture and the title, depending on the reader's type ... */
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SpringCardIWM2_Reader_Controller));
			reader = r;
			tbIP.Text = title;
			if (type == TYPE_FKG_RS485)
			{
				pictureBox1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("funkygate-mk2-dw")));
			} else
			if (type == TYPE_FKG_TCP)
			{
				pictureBox1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("funkygate-mk2-ip")));	
			}
			
			/* ... set the appropriate callbacks, to be notified when a badge is read, or when the status changes ... */
			reader.SetDeviceStatusChangedCallback(new DeviceStatusChangedCallback(OnDeviceStatusChanged));
			reader.SetBadgeReceivedCallback(new BadgeReceivedCallback(OnBadgeRead));
			
			/* ... and finally start the reader */
			reader.Start();

		}

#endregion

	}

}
