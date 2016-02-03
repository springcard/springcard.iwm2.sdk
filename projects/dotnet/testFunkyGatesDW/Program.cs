using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SpringCard.IWM2;
using System.Net;


namespace testFunkyGatesDW
{
	class Program
	{
		public static BadgeReceivedCallback cb = new BadgeReceivedCallback(OnBadgeRead);
		public static SpringCardIWM2_Serial_Reader reader;
		public static SpringCardIWM2_Serial_Scheduler scheduler;

		// Method called when a card was read
		public static void OnBadgeRead(string badge_read)
		{
			Console.WriteLine("Badge => " + badge_read);
		}

		public static void SchedulerStart()
		{
			Console.WriteLine("SchedulerStart");
		}

		public static void SchedulerStop()
		{
			Console.WriteLine("SchedulerStop");
		}

		public static void OnReaderFound(string com, string protocol, byte address, bool already_monitored)
		{
			Console.WriteLine("ReaderFound, com: " + com + ", Protocol: " + protocol);
			reader = scheduler.GetInstalledReader(1);
			if (reader == null)
			{
				Console.WriteLine("Error reader is null");
				return;
			}
			reader.SetBadgeReceivedCallback(new BadgeReceivedCallback(OnBadgeRead));
			reader.Start();
		}

		public static void ReaderDropped()
		{
			Console.WriteLine("ReaderDropped");
		}

		static void Main(string[] args)
		{
			Console.WriteLine("Start");
			scheduler = new SpringCardIWM2_Serial_Scheduler("COM11");
			scheduler.SetReaderFoundCallback(new SpringCardIWM2_Serial_Scheduler.ReaderFoundCallback(OnReaderFound));
			scheduler.Start((byte) 1, (byte) 10);
		}
	}
}
