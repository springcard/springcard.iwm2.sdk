/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 30/11/2015
 * Time: 11:17
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Diagnostics;
using Microsoft.Win32;
using System.Reflection;

namespace SpringCard.LibCs
{

	public class SystemConsole
	{
		[DllImport("kernel32.dll", SetLastError = true)]
		public static extern bool AllocConsole();

		[DllImport("kernel32.dll", SetLastError = true)]
		public static extern bool FreeConsole();

		[DllImport("kernel32", SetLastError = true)]
		public static extern bool AttachConsole(int dwProcessId);

		[DllImport("user32.dll")]
		public static extern IntPtr GetForegroundWindow();

		[DllImport("user32.dll", SetLastError = true)]
		public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out int lpdwProcessId);
		
		private static bool Visible = false;
		
		public static void Show()
		{
			if (!Visible)
			{
				IntPtr ptr = GetForegroundWindow();
				int  u;
				GetWindowThreadProcessId(ptr, out u);
				Process process = Process.GetProcessById(u);
				string TitleName = ((AssemblyTitleAttribute)Attribute.GetCustomAttribute(Assembly.GetExecutingAssembly(), typeof(AssemblyTitleAttribute), false)).Title;
				string Version = Assembly.GetExecutingAssembly().GetName().Version.ToString();
				if (process.ProcessName.Equals("cmd"))    //Is the uppermost window a cmd process?
				{
					AttachConsole(process.Id);
					Console.WriteLine("\n" + TitleName + " v."+Version+"\n");
				}	else
				{
					AllocConsole();
					Console.WriteLine("\n" + TitleName + " v."+Version+"\n");
				}
				Visible = true;
			}
		}
		
		public static void Hide()
		{
			if (Visible)
				FreeConsole();
			Visible = false;
		}
		
		public static void Verbose(string s)
		{
			if (Visible)
				Console.WriteLine(s);
		}
	}
}
