/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 08/12/2015
 * Time: 14:19
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Net;

namespace SpringCard.IWM2
{

	public class SpringCardIWM2_Network_GPIOs : SpringCardIWM2_Network_Device, ISpringCardIWM2_GPIOs 
	{
		
#region Callbacks
		public void SetInputChangedCallBack(InputChangedCallBack cb)
		{
			_input_changed_callback = cb;
		}	
#endregion


#region Constructor
		public SpringCardIWM2_Network_GPIOs(IPAddress _ip, int _port, byte _com_type, byte[] _key) : base(_ip, _port, _com_type, _key){}
#endregion

	}
}
