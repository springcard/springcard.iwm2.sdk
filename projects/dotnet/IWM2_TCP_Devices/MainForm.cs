/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 27/11/2015
 * Time: 09:44
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using SpringCard.LibCs;
using SpringCardApplication;

using SpringCard.IWM2;

namespace TCPReader
{

	public partial class MainForm : Form
	{
		/* This list contains all TCP devices added by the user */
		List<SpringCardIWM2_Network_Device> devices;
		
		/* This boolean indicates if all cryptographic details must be printed */
		bool ShowCrypto = false;
		
		public MainForm()
		{
			InitializeComponent();
			msiShowDebug.Checked = true;
			devices = new List<SpringCardIWM2_Network_Device>();
		}

		
#region Manage new devices
		
		/* This method is called when the user click on the '+' button */
		void BtPlusClick(object sender, EventArgs e)
		{
			/* Show the form enabling to specify a device */
			/* ------------------------------------------ */
			AddDeviceForm form = new AddDeviceForm();
			DialogResult dr = form.ShowDialog();
			
			/* The form has been closed: retrieve the desired device's characteristics */
			/* ----------------------------------------------------------------------- */
			if (dr == DialogResult.OK)
			{
				if (form.device_type == AddDeviceForm.FUNKYGATE)
				{
					/* The user has chosen a FunkyGate TCP reader: create the object, ... */
					SpringCardIWM2_Network_Reader reader = new SpringCardIWM2_Network_Reader(form.ip, form.port, form.com_type, form.key);
					
					/* ... indicate whether the cryptographic detail must be printed, ... */
					reader.SetShowCrypto(ShowCrypto);
					
					/* ... add it in the list, ... */
					devices.Add(reader);
					
					/* ..., the controller, ... */
					SpringCardIWM2_Reader_Controller c = new SpringCardIWM2_Reader_Controller(reader, form.ip.ToString() + ":" + form.port, SpringCardIWM2_Reader_Controller.TYPE_FKG_TCP);
					
					/* ... add the controller on the form, ... */
					flpMiddle.Controls.Add(c);
				
					/* ... and finally scroll to it */
					flpMiddle.ScrollControlIntoView(c);
					
				} else
				{
					/* The user has chosen a Handydrummer: create the object, ... */
					SpringCardIWM2_Network_GPIOs hd = new SpringCardIWM2_Network_GPIOs(form.ip, form.port, form.com_type, form.key);

					/* ... indicate whether the cryptographic detail must be printed, ... */
					hd.SetShowCrypto(ShowCrypto);
					
					/* ... add it in the list, ... */
					devices.Add(hd);
					
					/* ... the controller, ... */
					SpringCardIWM2_GPIOs_Controller c = new SpringCardIWM2_GPIOs_Controller(hd, form.ip.ToString() + ":" + form.port);
					
					/* ... add the controller on the form, ... */
					flpMiddle.Controls.Add(c);
					
					/* ... and finally scroll to it */
					flpMiddle.ScrollControlIntoView(c);
					
				}
			}

		}
#endregion		


#region ToolstripMenu items + debug console
		void QuitToolStripMenuItemClick(object sender, EventArgs e)
		{
			Close();
		}

		void PropagateShowCrypto()
		{
			if (devices != null)
				foreach (SpringCardIWM2_Network_Device device in devices)
					device.SetShowCrypto(ShowCrypto);
		}
		
		void ShowDebugConsoleToolStripMenuItemCheckedChanged(object sender, EventArgs e)
		{
			if (msiShowDebug.Checked)
			{
				SpringCard.LibCs.SystemConsole.Show();
				Trace.Listeners.Add(new ConsoleTraceListener());
				msiShowCrypto.Enabled = true;
			} else
			{
				Trace.Listeners.Clear();
				SpringCard.LibCs.SystemConsole.Hide();
				msiShowCrypto.Checked = false;
				msiShowCrypto.Enabled = false;
			}
			
		}
		
		void MsiShowCryptoCheckedChanged(object sender, EventArgs e)
		{
			if (msiShowCrypto.Checked)
			{
				ShowCrypto = true;
			} else
			{
				ShowCrypto = false;
			}
			
			PropagateShowCrypto();
		}	
		
		void AboutToolStripMenuItem1Click(object sender, EventArgs e)
		{
			AboutForm af = new AboutForm();
			af.ShowDialog();			
		}
		
#endregion

	}
	
}
