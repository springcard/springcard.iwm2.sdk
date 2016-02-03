/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 11/12/2015
 * Time: 15:35
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
namespace IWM2_Serial_Devices
{
	partial class SpringCard_IWM2_Serial_Scheduler_Controller
	{
		/// <summary>
		/// Designer variable used to keep track of non-visual components.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
		
		/// <summary>
		/// Disposes resources used by the form.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing) {
				if (components != null) {
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}
		
		/// <summary>
		/// This method is required for Windows Forms designer support.
		/// Do not change the method contents inside the source code editor. The Forms designer might
		/// not be able to load this method if it was changed manually.
		/// </summary>
		private void InitializeComponent()
		{
			this.cbCOMPorts = new System.Windows.Forms.ComboBox();
			this.nudAddMin = new System.Windows.Forms.NumericUpDown();
			this.nudAddMax = new System.Windows.Forms.NumericUpDown();
			this.lbAddMax = new System.Windows.Forms.Label();
			this.lbAddMin = new System.Windows.Forms.Label();
			this.lbCOMPort = new System.Windows.Forms.Label();
			this.pTop = new System.Windows.Forms.Panel();
			this.btStop = new System.Windows.Forms.Button();
			this.btStart = new System.Windows.Forms.Button();
			this.dataGridView = new System.Windows.Forms.DataGridView();
			this.Device = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Protocol = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.COM = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Address = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.AlreadyMonitored = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.pBottom = new System.Windows.Forms.Panel();
			this.lbClick = new System.Windows.Forms.Label();
			this.pMiddle = new System.Windows.Forms.Panel();
			((System.ComponentModel.ISupportInitialize)(this.nudAddMin)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.nudAddMax)).BeginInit();
			this.pTop.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView)).BeginInit();
			this.pBottom.SuspendLayout();
			this.pMiddle.SuspendLayout();
			this.SuspendLayout();
			// 
			// cbCOMPorts
			// 
			this.cbCOMPorts.FormattingEnabled = true;
			this.cbCOMPorts.Location = new System.Drawing.Point(208, 32);
			this.cbCOMPorts.Name = "cbCOMPorts";
			this.cbCOMPorts.Size = new System.Drawing.Size(78, 21);
			this.cbCOMPorts.TabIndex = 0;
			this.cbCOMPorts.SelectedIndexChanged += new System.EventHandler(this.CbCOMPortsSelectedIndexChanged);
			// 
			// nudAddMin
			// 
			this.nudAddMin.Location = new System.Drawing.Point(209, 59);
			this.nudAddMin.Name = "nudAddMin";
			this.nudAddMin.Size = new System.Drawing.Size(77, 20);
			this.nudAddMin.TabIndex = 1;
			this.nudAddMin.Value = new decimal(new int[] {
									1,
									0,
									0,
									0});
			// 
			// nudAddMax
			// 
			this.nudAddMax.Location = new System.Drawing.Point(209, 85);
			this.nudAddMax.Name = "nudAddMax";
			this.nudAddMax.Size = new System.Drawing.Size(77, 20);
			this.nudAddMax.TabIndex = 2;
			this.nudAddMax.Value = new decimal(new int[] {
									10,
									0,
									0,
									0});
			// 
			// lbAddMax
			// 
			this.lbAddMax.Location = new System.Drawing.Point(129, 82);
			this.lbAddMax.Name = "lbAddMax";
			this.lbAddMax.Size = new System.Drawing.Size(74, 23);
			this.lbAddMax.TabIndex = 3;
			this.lbAddMax.Text = "Max. Address";
			this.lbAddMax.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lbAddMin
			// 
			this.lbAddMin.Location = new System.Drawing.Point(129, 56);
			this.lbAddMin.Name = "lbAddMin";
			this.lbAddMin.Size = new System.Drawing.Size(74, 23);
			this.lbAddMin.TabIndex = 4;
			this.lbAddMin.Text = "Min. Address";
			this.lbAddMin.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lbCOMPort
			// 
			this.lbCOMPort.Location = new System.Drawing.Point(130, 30);
			this.lbCOMPort.Name = "lbCOMPort";
			this.lbCOMPort.Size = new System.Drawing.Size(73, 23);
			this.lbCOMPort.TabIndex = 5;
			this.lbCOMPort.Text = "COM port";
			this.lbCOMPort.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// pTop
			// 
			this.pTop.Controls.Add(this.btStop);
			this.pTop.Controls.Add(this.btStart);
			this.pTop.Controls.Add(this.lbCOMPort);
			this.pTop.Controls.Add(this.cbCOMPorts);
			this.pTop.Controls.Add(this.lbAddMin);
			this.pTop.Controls.Add(this.nudAddMin);
			this.pTop.Controls.Add(this.lbAddMax);
			this.pTop.Controls.Add(this.nudAddMax);
			this.pTop.Dock = System.Windows.Forms.DockStyle.Top;
			this.pTop.Location = new System.Drawing.Point(0, 0);
			this.pTop.Name = "pTop";
			this.pTop.Size = new System.Drawing.Size(544, 142);
			this.pTop.TabIndex = 6;
			// 
			// btStop
			// 
			this.btStop.Location = new System.Drawing.Point(334, 82);
			this.btStop.Name = "btStop";
			this.btStop.Size = new System.Drawing.Size(78, 23);
			this.btStop.TabIndex = 7;
			this.btStop.Text = "Stop";
			this.btStop.UseVisualStyleBackColor = true;
			this.btStop.Click += new System.EventHandler(this.BtStopClick);
			// 
			// btStart
			// 
			this.btStart.Location = new System.Drawing.Point(334, 56);
			this.btStart.Name = "btStart";
			this.btStart.Size = new System.Drawing.Size(78, 23);
			this.btStart.TabIndex = 6;
			this.btStart.Text = "Start";
			this.btStart.UseVisualStyleBackColor = true;
			this.btStart.Click += new System.EventHandler(this.BtStartClick);
			// 
			// dataGridView
			// 
			this.dataGridView.AllowUserToAddRows = false;
			this.dataGridView.AllowUserToDeleteRows = false;
			this.dataGridView.AllowUserToOrderColumns = true;
			this.dataGridView.BackgroundColor = System.Drawing.SystemColors.ControlLightLight;
			this.dataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
									this.Device,
									this.Protocol,
									this.COM,
									this.Address,
									this.AlreadyMonitored});
			this.dataGridView.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView.Location = new System.Drawing.Point(0, 0);
			this.dataGridView.MultiSelect = false;
			this.dataGridView.Name = "dataGridView";
			this.dataGridView.ReadOnly = true;
			this.dataGridView.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
			this.dataGridView.Size = new System.Drawing.Size(544, 324);
			this.dataGridView.TabIndex = 0;
			this.dataGridView.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.DataGridView1CellDoubleClick);
			// 
			// Device
			// 
			this.Device.HeaderText = "Device";
			this.Device.Name = "Device";
			this.Device.ReadOnly = true;
			// 
			// Protocol
			// 
			this.Protocol.HeaderText = "Protocol";
			this.Protocol.Name = "Protocol";
			this.Protocol.ReadOnly = true;
			// 
			// COM
			// 
			this.COM.HeaderText = "Port";
			this.COM.Name = "COM";
			this.COM.ReadOnly = true;
			// 
			// Address
			// 
			this.Address.HeaderText = "Address";
			this.Address.Name = "Address";
			this.Address.ReadOnly = true;
			// 
			// AlreadyMonitored
			// 
			this.AlreadyMonitored.HeaderText = "Already Monitored";
			this.AlreadyMonitored.Name = "AlreadyMonitored";
			this.AlreadyMonitored.ReadOnly = true;
			// 
			// pBottom
			// 
			this.pBottom.Controls.Add(this.lbClick);
			this.pBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.pBottom.Location = new System.Drawing.Point(0, 466);
			this.pBottom.Name = "pBottom";
			this.pBottom.Size = new System.Drawing.Size(544, 42);
			this.pBottom.TabIndex = 9;
			// 
			// lbClick
			// 
			this.lbClick.ForeColor = System.Drawing.Color.Green;
			this.lbClick.Location = new System.Drawing.Point(67, 10);
			this.lbClick.Name = "lbClick";
			this.lbClick.Size = new System.Drawing.Size(408, 23);
			this.lbClick.TabIndex = 0;
			this.lbClick.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// pMiddle
			// 
			this.pMiddle.Controls.Add(this.dataGridView);
			this.pMiddle.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pMiddle.Location = new System.Drawing.Point(0, 142);
			this.pMiddle.Name = "pMiddle";
			this.pMiddle.Size = new System.Drawing.Size(544, 324);
			this.pMiddle.TabIndex = 10;
			// 
			// SpringCard_IWM2_Serial_Scheduler_Controller
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(544, 508);
			this.Controls.Add(this.pMiddle);
			this.Controls.Add(this.pBottom);
			this.Controls.Add(this.pTop);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "SpringCard_IWM2_Serial_Scheduler_Controller";
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "SpringCard_IWM2_Scheduler_Controller";
			this.Shown += new System.EventHandler(this.SpringCard_IWM2_Scheduler_ControllerShown);
			((System.ComponentModel.ISupportInitialize)(this.nudAddMin)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.nudAddMax)).EndInit();
			this.pTop.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView)).EndInit();
			this.pBottom.ResumeLayout(false);
			this.pMiddle.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		private System.Windows.Forms.Label lbClick;
		private System.Windows.Forms.Button btStop;
		private System.Windows.Forms.Panel pBottom;
		private System.Windows.Forms.DataGridViewTextBoxColumn AlreadyMonitored;
		private System.Windows.Forms.DataGridViewTextBoxColumn COM;
		private System.Windows.Forms.DataGridViewTextBoxColumn Device;
		private System.Windows.Forms.DataGridViewTextBoxColumn Protocol;
		private System.Windows.Forms.DataGridViewTextBoxColumn Address;
		private System.Windows.Forms.DataGridView dataGridView;
		private System.Windows.Forms.Panel pMiddle;
		private System.Windows.Forms.Button btStart;
		private System.Windows.Forms.Panel pTop;
		private System.Windows.Forms.Label lbCOMPort;
		private System.Windows.Forms.Label lbAddMin;
		private System.Windows.Forms.Label lbAddMax;
		private System.Windows.Forms.NumericUpDown nudAddMax;
		private System.Windows.Forms.NumericUpDown nudAddMin;
		private System.Windows.Forms.ComboBox cbCOMPorts;
	}
}
