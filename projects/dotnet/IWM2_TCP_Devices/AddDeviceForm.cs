/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 27/11/2015
 * Time: 14:18
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Net;
using System.Drawing;
using System.Windows.Forms;
using SpringCard.IWM2;

namespace TCPReader
{

	/* This class is a form enabling to specify the characteristics			*/
	/* of the desired TCP device: IP, port, authentication mode and key */
	public partial class AddDeviceForm : Form
	{

		public const byte FUNKYGATE 		= 0x01;
		public const byte HANDYDRUMMER 	= 0x02;

		public IPAddress ip;
		public int port;	
		public byte device_type;
			
		public byte com_type;
		public byte[] key;


		/* This method returns wether a specified character, is hexadecimal */
		private bool isHex(char c)
		{
			string hexCharacters = "0123456789abcdefABCDEF";
			return hexCharacters.Contains(""+c);
		}
		
		/* This method is called when the user clicks on the "OK" button */
		void BtOKClick(object sender, EventArgs e)
		{
			/* Verify the IP address */
			if (!IPAddress.TryParse(tbIP.Text, out ip))
			{
				MessageBox.Show("Invalid IP address", "IP error", MessageBoxButtons.OK);
				return;
			}
			
			/* Verify the port */
			if (!Int32.TryParse(tbPort.Text, out port))
			{
				MessageBox.Show("Invalid port", "port error", MessageBoxButtons.OK);
				return;
			}

			/* Check the type */
			if (rbFunkyGate.Checked)
			{
				device_type = FUNKYGATE;
			}	else
			{
				device_type = HANDYDRUMMER;
			}
			
			/* Check the communication mode */
			if (rbSecuredOperation.Checked)
			{
				com_type = SpringCardIWM2_Network_Device.COM_TYPE_SECURED_OPERATION;
			}	else
			if (rbSecuredAdministration.Checked)
			{
				com_type = SpringCardIWM2_Network_Device.COM_TYPE_SECURED_ADMINISTRATION;
			} else
			{
				com_type = SpringCardIWM2_Network_Device.COM_TYPE_PLAIN;
			}

			/* Check the key: must be a 32-characters hexadecimal	*/
 			/* string, to be converted in a array of 16 bytes			*/ 
			if (com_type != SpringCardIWM2_Network_Device.COM_TYPE_PLAIN)
			{
				String key_str = mkTxtUnlockKey.Text.Replace(" ", "");
				if (key_str.Length != 32)
				{
					MessageBox.Show("Invalid key length", "Key error", MessageBoxButtons.OK);
					return;
				}
				
				foreach (char c in key_str)
				{
					if (!isHex(c))
					{
						MessageBox.Show("Invalid key value (non-hexadecimal format)", "Key error", MessageBoxButtons.OK);
						return;						
					}
				}
				
				key = new byte[16];
				for (int i = 0; i <key_str.Length ; i += 2)
		    	key[i / 2] = Convert.ToByte(key_str.Substring(i, 2), 16);
				
			}
			
			/* If everything is ok: close the form */
			this.DialogResult = DialogResult.OK;
			
		}
		
		/* This method is called each time the "Plain" communication mode is changed */
		void RbPlainCheckedChanged(object sender, EventArgs e)
		{
			if (rbPlain.Checked)
			{
				/* Mode is "plain": deactivate the key */
				mkTxtUnlockKey.Enabled = false;
			} else
			{
				/* Mode is not "plain": activate the key */
				mkTxtUnlockKey.Enabled = true;
			}
		}
		
		/* This is the constructor */
		public AddDeviceForm()
		{
			InitializeComponent();
			rbFunkyGate.Checked = true;
			rbSecuredOperation.Checked = true;
		}
		
	}
	
}
