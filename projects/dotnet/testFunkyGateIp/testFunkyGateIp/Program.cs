using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//using SpringCard.LibCs;
//using SpringCardApplication;
using SpringCard.IWM2;
using System.Net;

namespace testFunkyGateIp
{
	class Program
	{
		public static BadgeReceivedCallback cb = new BadgeReceivedCallback(OnBadgeRead);

		// Method called when a card was read
		public static void OnBadgeRead(string badge_read)
		{
			Console.WriteLine("Badge => " + badge_read);
		}

		static void Main(string[] args)
		{
			Console.WriteLine("Start");
			IPAddress IP = new IPAddress(new byte[] { 192, 168, 16, 91 });
			// For a communication in plain mode
			//byte com_type = SpringCardIWM2_Network_Device.COM_TYPE_PLAIN;
			//byte[] key = null;

			// For a secured communication
			byte com_type = SpringCardIWM2_Network_Device.COM_TYPE_SECURED_OPERATION;
			// In which cas you have to provide a security key
			byte[] key = new byte[16] {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			

			SpringCardIWM2_Network_Reader reader = new SpringCardIWM2_Network_Reader(IP, 3999, com_type, key);
			reader.SetShowCrypto(true);
			reader.SetBadgeReceivedCallback(new BadgeReceivedCallback(cb));
			reader.Start();
		}
	}
}
