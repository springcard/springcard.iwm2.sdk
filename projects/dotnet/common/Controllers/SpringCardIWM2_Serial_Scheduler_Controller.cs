/*	
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 11/12/2015
 * Time: 15:35
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
using System;
using System.Drawing;
using System.Windows.Forms;
using System.IO.Ports;
using System.Collections.Generic;

using SpringCard.LibCs;

using SpringCard.IWM2;

namespace IWM2_Serial_Devices
{

	/* This "Controller" is a form enabling to stop and start schedulers. 		*/
	/* A scheduler is dedicated to a COM port: it probes the readers, trying	*/
	/* all the adresses in the specified range, and then maintains the 				*/
	/* communication with all detected (="installed") readers									*/
	
	public partial class SpringCard_IWM2_Serial_Scheduler_Controller : Form
	{
		
#region Private members

		/* The MainForm from which this controller comes from */
		private MainForm MF;

		/* This list contains all launched schedulers (a scheduler monitors a COM port, not a reader) */
		/* This list comes from the Mainform, and is then maintained here in this controller					*/
		private List<SpringCardIWM2_Serial_Scheduler> schedulers;  	

		/* The scheduler corresponding to the COM port chosen on the form */
		private SpringCardIWM2_Serial_Scheduler selectedScheduler;	

		/* The reader on which the user has just double-clicked	*/
		private SpringCardIWM2_Serial_Reader		selectedReader;			

		/* A boolean to indicate whether all frames sent and received on the COM ports must be printed */
		private bool show_frames;
		
#endregion


#region Constructing and showing the form 
		public SpringCard_IWM2_Serial_Scheduler_Controller(MainForm _MF, List<SpringCardIWM2_Serial_Scheduler> _schedulers, bool _show_frames)
		{
			InitializeComponent();
			MF = _MF;
			schedulers = _schedulers;
			show_frames = _show_frames;
		}		
		
		void SpringCard_IWM2_Scheduler_ControllerShown(object sender, EventArgs e)
		{
			
			/* Retrieve all COM ports on this machine, ... */
			string[] ports = SerialPort.GetPortNames();

			/* ... and print them */
			if (ports.Length > 0)
			{
				foreach (string port in ports)
					cbCOMPorts.Items.Add(port);		
				
				cbCOMPorts.SelectedIndex = 0;	
				
			}
			
		}
		
#endregion


#region Accessors
		public SpringCardIWM2_Serial_Scheduler GetSelectedScheduler()
		{
			return selectedScheduler;
		}
				
		public SpringCardIWM2_Serial_Reader GetSelectedReader()
		{
			return selectedReader;
		}
#endregion


#region Callback methods	

		/* This method is called by a scheduler, when a reader is found on this COM port */
		delegate void OnReaderFoundInvoker(string com, string protocol, byte address, bool already_monitored);
		public void OnReaderFound(string com, string protocol, byte address, bool already_monitored)
		{
			/* The OnReaderFound method is called as a delegate (callback) by the SpringCard_Serial_Scheduler   */
			/* within its backgroung thread. Therefore we must use the BeginInvoke syntax to switch back from   */
			/* the context of the background thread to the context of the application's main thread. Otherwise  */
			/* we'll get a security violation when trying to access the window's visual components (that belong */
			/* to the application's main thread and can't be safely manipulated by background threads).         */
			if (InvokeRequired)
			{
				this.BeginInvoke(new OnReaderFoundInvoker(OnReaderFound), com, protocol, address, already_monitored);
				return;
			}
			
			if (!com.Equals(cbCOMPorts.Text))
				return;
			
			/* Add a row in the DataGridView */
			string[] row_cells = new string[5];
			row_cells[0] = "FunkyGate";
			row_cells[1] = protocol;
			row_cells[2] = com;
			row_cells[3] = address.ToString();
			row_cells[4] = already_monitored ? "Yes" : "No";
			dataGridView.Rows.Add(row_cells);

			dataGridView.Rows[dataGridView.RowCount - 1].DefaultCellStyle.ForeColor = already_monitored ? Color.Green : Color.Red;
			
			lbClick.Text = "Double-click on a reader to interact with it";
			
		}
		
		/* This method is called by a scheduler, when a reader is dropped because it doesn't answer the requests anymore */
		delegate void OnReaderDroppedInvoker(string com, byte address);
		public void OnReaderDropped(string com, byte address)
		{
			/* The OnReaderDropped method is called as a delegate (callback) by the SpringCard_Serial_Scheduler */
			/* within its backgroung thread. Therefore we must use the BeginInvoke syntax to switch back from   */
			/* the context of the background thread to the context of the application's main thread. Otherwise  */
			/* we'll get a security violation when trying to access the window's visual components (that belong */
			/* to the application's main thread and can't be safely manipulated by background threads).         */
			if (InvokeRequired)
			{
				this.BeginInvoke(new OnReaderDroppedInvoker(OnReaderDropped), com, address);
				return;
			}
			
			/* Remove the reader from the DataGridView */			
			List<DataGridViewRow> rowsToDelete = new List<DataGridViewRow>();
			
			foreach (DataGridViewRow item in dataGridView.Rows)
			{
				if ( (com.Equals((string)dataGridView.Rows[item.Index].Cells[2].Value))
						&&
						(address == Byte.Parse((string) dataGridView.Rows[item.Index].Cells[3].Value))
					)
				{
					rowsToDelete.Add(item);
				}
			}
			
			foreach (DataGridViewRow item in rowsToDelete)
				dataGridView.Rows.Remove(item);		
					
		}
		
		/* This method is called by a scheduler, when it is stopped */ 
		delegate void OnSchedulerStoppedInvoker(string com);
		public void OnSchedulerStopped(string com)
		{
			/* The OnSchedulerStopped method is called as a delegate (callback) by the SpringCard_Serial_Scheduler */
			/* within its backgroung thread. Therefore we must use the BeginInvoke syntax to switch back from   	 */
			/* the context of the background thread to the context of the application's main thread. Otherwise  	 */
			/* we'll get a security violation when trying to access the window's visual components (that belong 	 */
			/* to the application's main thread and can't be safely manipulated by background threads).         	 */
			if (InvokeRequired)
			{
				this.BeginInvoke(new OnSchedulerStoppedInvoker(OnSchedulerStopped), com);
				return;
			}
			
			/* If this the selected COM port: enable the "Start" and disable the "Stop" buttons */
			if (com.Equals(cbCOMPorts.Text))
			{
				btStart.Enabled = true;
				btStop.Enabled = false;
			}
			
		}
		
		/* This method sets all the callbacks (methods defined here) that may be called from a scheduler (SpringCardIWM2_Serial_Scheduler object) */
		private void SetCallbacks(SpringCardIWM2_Serial_Scheduler selectedScheduler)
		{
			selectedScheduler.SetReaderFoundCallback(new SpringCardIWM2_Serial_Scheduler.ReaderFoundCallback(OnReaderFound));			
			selectedScheduler.SetReaderDroppedCallback(new SpringCardIWM2_Serial_Scheduler.ReaderDroppedCallback(OnReaderDropped));
			selectedScheduler.SetSchedulerStoppedCallback(new SpringCardIWM2_Serial_Scheduler.SchedulerStoppedCallback(OnSchedulerStopped));			
		}
		
#endregion


#region Schedulers management

		private bool IsPortAlreadyScheduled(string portName)
		{
			/* A port is already managed, if a running scheduler on this port exists in the list */
			if (schedulers == null)
				return false;
			
			foreach (SpringCardIWM2_Serial_Scheduler scheduler in schedulers)
				if (scheduler.GetPortName().Equals(portName))
					return scheduler.IsRunning();
			
			return false;	
		
		}

		private SpringCardIWM2_Serial_Scheduler RetrieveScheduler(string portName)
		{
			if (schedulers == null)
				return null;			
			
			foreach (SpringCardIWM2_Serial_Scheduler scheduler in schedulers)
				if (scheduler.GetPortName().Equals(portName))
					return scheduler;			
			
			return null;
		}
		
#endregion
		

#region Actions on readers found
		
		/* This method is called when the user double clicks on a reader */
		void DataGridView1CellDoubleClick(object sender, DataGridViewCellEventArgs e)
		{
			if (dataGridView.SelectedRows.Count == 1)
			{
				int index = dataGridView.SelectedRows[0].Index;
				
				/* Retrieve the reader which has been double-clicked ... */
				if (selectedScheduler != null)
				{
					byte address = Byte.Parse((string) dataGridView.Rows[index].Cells[3].Value);
					if (!selectedScheduler.IsMonitored(address))
					{
						selectedScheduler.MonitorReader(address);
						selectedReader = selectedScheduler.GetInstalledReader(address);
					}
					
					/* ... and close this form to get back to the MainForm */
					this.Close();
					
				}
				
			}

		}
#endregion


#region Start and stop buttons to create/terminate schedulers
		
		void BtStartClick(object sender, EventArgs e)
		{
			/* Create a new SpringCardIWM2_Serial_Scheduler object on this port, ... */
			selectedScheduler = new SpringCardIWM2_Serial_Scheduler(cbCOMPorts.Text);
			
			/* ... indicate whether all sent/received frames must be printed ...   */
			selectedScheduler.SetShowFrames(show_frames);
			
			/* ... sets all the callcacks to be informed of its activities ... */
			SetCallbacks(selectedScheduler);
			
			/* ... and launch it ! */ 
			selectedScheduler.Start((byte) nudAddMin.Value, (byte) nudAddMax.Value);

			/* If the user switches in the future to another port, remember this scheduler is active and running */ 
			MF.RefineSchedulersList(schedulers, cbCOMPorts.Text);
			schedulers.Add(selectedScheduler);
			
			/* Finally, enable the "Stop" and disable the "start" buttons */
			btStart.Enabled = false;
			btStop.Enabled = true;
			
		}
		
		void BtStopClick(object sender, EventArgs e)
		{
			if (selectedScheduler != null)
			{
				/* Stop and realease the specified scheduler */
				selectedScheduler.Stop();
				selectedScheduler = null;
				dataGridView.Rows.Clear();
				dataGridView.Refresh();
				lbClick.Text = "";
			}
			
			/* Enable the "Start" and disable the "Stop" buttons */
			btStart.Enabled = true;
			btStop.Enabled = false;
			
		}
		
#endregion


#region Select COM port on the Form
		
		void CbCOMPortsSelectedIndexChanged(object sender, EventArgs e)
		{
			if (IsPortAlreadyScheduled(cbCOMPorts.Text))
			{
				/* There already is a scheduler managing this COM port */
				/* --------------------------------------------------- */
				
				/* Retrieve it from the list ... */
				selectedScheduler = RetrieveScheduler(cbCOMPorts.Text);
				
				/* ... and set the apprpriate callbacks (they may have been set from another form, which has been previously closed) */
				SetCallbacks(selectedScheduler);
				
				/* Enable the "Stop" and disable the "Start" buttons */
				btStart.Enabled = false;
				btStop.Enabled = true;

				/* This already-running scheduler may already have installed readers: */
 				/* if this is the case, get them and print them in the DataGridView		*/
				if (selectedScheduler.GetInstalledReaders() != null)
				{
					foreach (SpringCardIWM2_Serial_Reader reader in selectedScheduler.GetInstalledReaders())
						OnReaderFound(cbCOMPorts.Text, "MK2", reader.GetAddress(), selectedScheduler.IsMonitored(reader.GetAddress()) );
				
				} else
				{
					lbClick.Text = "";
				}
					
			} else
			{
				/* There is no scheduler managing this COM port */
				/* -------------------------------------------- */

				/* Remove the potential readers from the DataGridView */ 
				dataGridView.Rows.Clear();
				dataGridView.Refresh();
				
				/* Enable the "Start" and disable the "Stop" buttons */
				btStart.Enabled = true;
				btStop.Enabled = false;				
				lbClick.Text = "";
			}			
		
		}
		
#endregion

	}
}
