/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 09/12/2015
 * Time: 14:47
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

namespace IWM2_Serial_Devices
{

	public partial class MainForm : Form
	{
		/* This list contains all launched schedulers (a scheduler monitors a COM port, not a reader) */
		List<SpringCardIWM2_Serial_Scheduler> schedulers;
		
		/* This boolean indicates if all frames sent and received on the COM ports must be printed */
		bool ShowFrames = false;
		
		
		public MainForm()
		{
			InitializeComponent();
			msiShowDebug.Checked = true;
			schedulers = new List<SpringCardIWM2_Serial_Scheduler>() ;
		}
		
		
#region ToolStripMenu items + Debug console

		void PropagateShowFrames()
		{
			if (schedulers != null)
				foreach (SpringCardIWM2_Serial_Scheduler sched in schedulers)
					sched.SetShowFrames(ShowFrames);
		}
		
		void MsiShowFramesCheckedChanged(object sender, EventArgs e)
		{
			if (msiShowFrames.Checked)
			{
				ShowFrames = true;
			} else
			{
				ShowFrames = false;
			}
			PropagateShowFrames();
		
		}

		void MsiShowDebugCheckedChanged(object sender, EventArgs e)
		{
			if (msiShowDebug.Checked)
			{
				SpringCard.LibCs.SystemConsole.Show();
				Trace.Listeners.Add(new ConsoleTraceListener());
				msiShowFrames.Enabled = true;
			} else
			{
				Trace.Listeners.Clear();
				SpringCard.LibCs.SystemConsole.Hide();
				msiShowFrames.Checked = false;
				msiShowFrames.Enabled = false;
			}
			
		}

		void QuitToolStripMenuItemClick(object sender, EventArgs e)
		{
			Close();
		}
		
		void AboutToolStripMenuItem1Click(object sender, EventArgs e)
		{
			AboutForm af = new AboutForm();
			af.ShowDialog();
		}		
		
#endregion	


#region Manage the schedulers, and the readers
		
		/* This method removes all schedulers on the specified port, from the list of all launched schedulers */ 
		public void RefineSchedulersList(List<SpringCardIWM2_Serial_Scheduler> scheds, string PortName)
		{
			/* First: build a list of all items to remove */
			List<SpringCardIWM2_Serial_Scheduler> temp = new List<SpringCardIWM2_Serial_Scheduler>();
			foreach (SpringCardIWM2_Serial_Scheduler sched in schedulers)
				if (sched.GetPortName().Equals(PortName))
					temp.Add(sched);
			
			/* Then: remove them from main list */
			foreach (SpringCardIWM2_Serial_Scheduler sched in temp)
				scheds.Remove(sched);
			
		}
		
		/* This method is called when the user clicks on the '+' */
		void BtPlusClick(object sender, EventArgs e)
		{
			/* Show the form enabling to manage specific COM ports */
			/* --------------------------------------------------- */
			SpringCard_IWM2_Serial_Scheduler_Controller controller = new SpringCard_IWM2_Serial_Scheduler_Controller(this, schedulers, ShowFrames);
			controller.ShowDialog();
			
			
			/* The form has been closed: retrieve the scheduler and the potential reader */
			/* ------------------------------------------------------------------------- */
			SpringCardIWM2_Serial_Scheduler scheduler = controller.GetSelectedScheduler();		
			if (scheduler != null)
			{
				/* We have to update the list of schedulers, just in case a scheduler on a	 */ 
				/* COM port has been stopped and relaunched (in this case, there is another  */
 				/* object scheduling the same COM port)																			 */
 
				/* A scheduler with the same port name may already be present in list : */
				/* remove it first (it may have been closed by user in the dialog form)	*/
				RefineSchedulersList(schedulers, scheduler.GetPortName());
				
				/* and then add the new scheduler (it may be exactely the one we've removed) */
				schedulers.Add(scheduler);
				
			} 
			
			SpringCardIWM2_Serial_Reader reader = controller.GetSelectedReader();
			if (reader != null)
			{
				/* If a reader has been selected, add a new controller to interact with it */
				SpringCardIWM2_Reader_Controller c = new SpringCardIWM2_Reader_Controller(reader, reader.GetScheduler().GetPortName() + " - Addr. " + reader.GetAddress(), SpringCardIWM2_Reader_Controller.TYPE_FKG_RS485);
				flpMiddle.Controls.Add(c);
				flpMiddle.ScrollControlIntoView(c);
			}
		
		}

#endregion
	}
		
}
