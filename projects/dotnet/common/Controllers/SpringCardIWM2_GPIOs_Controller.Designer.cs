/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 08/12/2015
 * Time: 13:33
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
namespace TCPReader
{
	partial class SpringCardIWM2_GPIOs_Controller
	{
		/// <summary>
		/// Designer variable used to keep track of non-visual components.
		/// </summary>
		private System.ComponentModel.IContainer components = null;
		
		/// <summary>
		/// Disposes resources used by the control.
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SpringCardIWM2_GPIOs_Controller));
			this.tbIP = new System.Windows.Forms.TextBox();
			this.lbStop = new System.Windows.Forms.Label();
			this.btStop = new System.Windows.Forms.Button();
			this.lbCurrentState = new System.Windows.Forms.Label();
			this.tbCurrentState = new System.Windows.Forms.TextBox();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.bt0 = new System.Windows.Forms.Button();
			this.bt7 = new System.Windows.Forms.Button();
			this.bt6 = new System.Windows.Forms.Button();
			this.bt5 = new System.Windows.Forms.Button();
			this.bt4 = new System.Windows.Forms.Button();
			this.bt3 = new System.Windows.Forms.Button();
			this.bt2 = new System.Windows.Forms.Button();
			this.bt1 = new System.Windows.Forms.Button();
			this.in1 = new System.Windows.Forms.Button();
			this.in2 = new System.Windows.Forms.Button();
			this.in3 = new System.Windows.Forms.Button();
			this.in4 = new System.Windows.Forms.Button();
			this.in5 = new System.Windows.Forms.Button();
			this.in6 = new System.Windows.Forms.Button();
			this.in7 = new System.Windows.Forms.Button();
			this.in0 = new System.Windows.Forms.Button();
			this.panel1 = new System.Windows.Forms.Panel();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			this.panel1.SuspendLayout();
			this.SuspendLayout();
			// 
			// tbIP
			// 
			this.tbIP.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
			this.tbIP.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbIP.ForeColor = System.Drawing.SystemColors.ButtonFace;
			this.tbIP.Location = new System.Drawing.Point(133, 47);
			this.tbIP.Name = "tbIP";
			this.tbIP.ReadOnly = true;
			this.tbIP.Size = new System.Drawing.Size(223, 26);
			this.tbIP.TabIndex = 31;
			this.tbIP.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// lbStop
			// 
			this.lbStop.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbStop.Location = new System.Drawing.Point(934, 24);
			this.lbStop.Name = "lbStop";
			this.lbStop.Size = new System.Drawing.Size(43, 32);
			this.lbStop.TabIndex = 30;
			this.lbStop.Text = "Stop";
			this.lbStop.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// btStop
			// 
			this.btStop.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("btStop.BackgroundImage")));
			this.btStop.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.btStop.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btStop.ForeColor = System.Drawing.SystemColors.ControlLight;
			this.btStop.Location = new System.Drawing.Point(938, 59);
			this.btStop.Name = "btStop";
			this.btStop.Size = new System.Drawing.Size(34, 30);
			this.btStop.TabIndex = 29;
			this.btStop.UseVisualStyleBackColor = true;
			this.btStop.Click += new System.EventHandler(this.BtStopClick);
			// 
			// lbCurrentState
			// 
			this.lbCurrentState.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbCurrentState.Location = new System.Drawing.Point(723, 24);
			this.lbCurrentState.Name = "lbCurrentState";
			this.lbCurrentState.Size = new System.Drawing.Size(179, 32);
			this.lbCurrentState.TabIndex = 26;
			this.lbCurrentState.Text = "Current state";
			this.lbCurrentState.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// tbCurrentState
			// 
			this.tbCurrentState.BackColor = System.Drawing.Color.White;
			this.tbCurrentState.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbCurrentState.ForeColor = System.Drawing.SystemColors.ControlText;
			this.tbCurrentState.Location = new System.Drawing.Point(732, 59);
			this.tbCurrentState.Name = "tbCurrentState";
			this.tbCurrentState.ReadOnly = true;
			this.tbCurrentState.Size = new System.Drawing.Size(179, 26);
			this.tbCurrentState.TabIndex = 25;
			this.tbCurrentState.Text = "Created";
			this.tbCurrentState.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// pictureBox1
			// 
			this.pictureBox1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("pictureBox1.BackgroundImage")));
			this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
			this.pictureBox1.InitialImage = null;
			this.pictureBox1.Location = new System.Drawing.Point(0, 1);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(100, 121);
			this.pictureBox1.TabIndex = 17;
			this.pictureBox1.TabStop = false;
			// 
			// bt0
			// 
			this.bt0.BackColor = System.Drawing.Color.Red;
			this.bt0.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.bt0.Location = new System.Drawing.Point(21, 26);
			this.bt0.Name = "bt0";
			this.bt0.Size = new System.Drawing.Size(23, 23);
			this.bt0.TabIndex = 32;
			this.bt0.UseVisualStyleBackColor = false;
			this.bt0.Click += new System.EventHandler(this.Bt0Click);
			// 
			// bt7
			// 
			this.bt7.BackColor = System.Drawing.Color.Red;
			this.bt7.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.bt7.Location = new System.Drawing.Point(221, 26);
			this.bt7.Name = "bt7";
			this.bt7.Size = new System.Drawing.Size(23, 23);
			this.bt7.TabIndex = 33;
			this.bt7.UseVisualStyleBackColor = false;
			this.bt7.Click += new System.EventHandler(this.Bt7Click);
			// 
			// bt6
			// 
			this.bt6.BackColor = System.Drawing.Color.Red;
			this.bt6.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.bt6.Location = new System.Drawing.Point(192, 26);
			this.bt6.Name = "bt6";
			this.bt6.Size = new System.Drawing.Size(23, 23);
			this.bt6.TabIndex = 34;
			this.bt6.UseVisualStyleBackColor = false;
			this.bt6.Click += new System.EventHandler(this.Bt6Click);
			// 
			// bt5
			// 
			this.bt5.BackColor = System.Drawing.Color.Red;
			this.bt5.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.bt5.Location = new System.Drawing.Point(163, 26);
			this.bt5.Name = "bt5";
			this.bt5.Size = new System.Drawing.Size(23, 23);
			this.bt5.TabIndex = 35;
			this.bt5.UseVisualStyleBackColor = false;
			this.bt5.Click += new System.EventHandler(this.Bt5Click);
			// 
			// bt4
			// 
			this.bt4.BackColor = System.Drawing.Color.Red;
			this.bt4.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.bt4.Location = new System.Drawing.Point(134, 26);
			this.bt4.Name = "bt4";
			this.bt4.Size = new System.Drawing.Size(23, 23);
			this.bt4.TabIndex = 36;
			this.bt4.UseVisualStyleBackColor = false;
			this.bt4.Click += new System.EventHandler(this.Bt4Click);
			// 
			// bt3
			// 
			this.bt3.BackColor = System.Drawing.Color.Red;
			this.bt3.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.bt3.Location = new System.Drawing.Point(106, 26);
			this.bt3.Name = "bt3";
			this.bt3.Size = new System.Drawing.Size(22, 23);
			this.bt3.TabIndex = 37;
			this.bt3.UseVisualStyleBackColor = false;
			this.bt3.Click += new System.EventHandler(this.Bt3Click);
			// 
			// bt2
			// 
			this.bt2.BackColor = System.Drawing.Color.Red;
			this.bt2.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.bt2.Location = new System.Drawing.Point(78, 26);
			this.bt2.Name = "bt2";
			this.bt2.Size = new System.Drawing.Size(22, 23);
			this.bt2.TabIndex = 38;
			this.bt2.UseVisualStyleBackColor = false;
			this.bt2.Click += new System.EventHandler(this.Bt2Click);
			// 
			// bt1
			// 
			this.bt1.BackColor = System.Drawing.Color.Red;
			this.bt1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.bt1.Location = new System.Drawing.Point(50, 26);
			this.bt1.Name = "bt1";
			this.bt1.Size = new System.Drawing.Size(22, 23);
			this.bt1.TabIndex = 39;
			this.bt1.UseVisualStyleBackColor = false;
			this.bt1.Click += new System.EventHandler(this.Bt1Click);
			// 
			// in1
			// 
			this.in1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.in1.Enabled = false;
			this.in1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.in1.Location = new System.Drawing.Point(74, 68);
			this.in1.Name = "in1";
			this.in1.Size = new System.Drawing.Size(14, 23);
			this.in1.TabIndex = 47;
			this.in1.UseVisualStyleBackColor = false;
			// 
			// in2
			// 
			this.in2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.in2.Enabled = false;
			this.in2.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.in2.Location = new System.Drawing.Point(94, 68);
			this.in2.Name = "in2";
			this.in2.Size = new System.Drawing.Size(14, 23);
			this.in2.TabIndex = 46;
			this.in2.UseVisualStyleBackColor = false;
			// 
			// in3
			// 
			this.in3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.in3.Enabled = false;
			this.in3.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.in3.Location = new System.Drawing.Point(114, 68);
			this.in3.Name = "in3";
			this.in3.Size = new System.Drawing.Size(14, 23);
			this.in3.TabIndex = 45;
			this.in3.UseVisualStyleBackColor = false;
			// 
			// in4
			// 
			this.in4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.in4.Enabled = false;
			this.in4.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.in4.Location = new System.Drawing.Point(135, 68);
			this.in4.Name = "in4";
			this.in4.Size = new System.Drawing.Size(14, 23);
			this.in4.TabIndex = 44;
			this.in4.UseVisualStyleBackColor = false;
			// 
			// in5
			// 
			this.in5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.in5.Enabled = false;
			this.in5.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.in5.Location = new System.Drawing.Point(155, 68);
			this.in5.Name = "in5";
			this.in5.Size = new System.Drawing.Size(14, 23);
			this.in5.TabIndex = 43;
			this.in5.UseVisualStyleBackColor = false;
			// 
			// in6
			// 
			this.in6.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.in6.Enabled = false;
			this.in6.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.in6.Location = new System.Drawing.Point(175, 68);
			this.in6.Name = "in6";
			this.in6.Size = new System.Drawing.Size(14, 23);
			this.in6.TabIndex = 42;
			this.in6.UseVisualStyleBackColor = false;
			// 
			// in7
			// 
			this.in7.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.in7.Enabled = false;
			this.in7.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.in7.Location = new System.Drawing.Point(195, 68);
			this.in7.Name = "in7";
			this.in7.Size = new System.Drawing.Size(14, 23);
			this.in7.TabIndex = 41;
			this.in7.UseVisualStyleBackColor = false;
			// 
			// in0
			// 
			this.in0.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.in0.Enabled = false;
			this.in0.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.in0.Location = new System.Drawing.Point(54, 68);
			this.in0.Name = "in0";
			this.in0.Size = new System.Drawing.Size(14, 23);
			this.in0.TabIndex = 40;
			this.in0.UseVisualStyleBackColor = false;
			// 
			// panel1
			// 
			this.panel1.BackColor = System.Drawing.Color.LemonChiffon;
			this.panel1.Controls.Add(this.bt0);
			this.panel1.Controls.Add(this.in1);
			this.panel1.Controls.Add(this.bt7);
			this.panel1.Controls.Add(this.in2);
			this.panel1.Controls.Add(this.bt6);
			this.panel1.Controls.Add(this.in3);
			this.panel1.Controls.Add(this.bt5);
			this.panel1.Controls.Add(this.in4);
			this.panel1.Controls.Add(this.bt4);
			this.panel1.Controls.Add(this.in5);
			this.panel1.Controls.Add(this.bt3);
			this.panel1.Controls.Add(this.in6);
			this.panel1.Controls.Add(this.bt2);
			this.panel1.Controls.Add(this.in7);
			this.panel1.Controls.Add(this.bt1);
			this.panel1.Controls.Add(this.in0);
			this.panel1.Location = new System.Drawing.Point(411, 3);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(258, 116);
			this.panel1.TabIndex = 48;
			// 
			// SpringCardIWM2_GPIOs_Controller
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.ControlLight;
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.tbIP);
			this.Controls.Add(this.lbStop);
			this.Controls.Add(this.btStop);
			this.Controls.Add(this.lbCurrentState);
			this.Controls.Add(this.tbCurrentState);
			this.Controls.Add(this.pictureBox1);
			this.Name = "SpringCardIWM2_GPIOs_Controller";
			this.Size = new System.Drawing.Size(1000, 122);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			this.panel1.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();
		}
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button in0;
		private System.Windows.Forms.Button in7;
		private System.Windows.Forms.Button in6;
		private System.Windows.Forms.Button in5;
		private System.Windows.Forms.Button in4;
		private System.Windows.Forms.Button in3;
		private System.Windows.Forms.Button in2;
		private System.Windows.Forms.Button in1;
		private System.Windows.Forms.Button bt1;
		private System.Windows.Forms.Button bt2;
		private System.Windows.Forms.Button bt3;
		private System.Windows.Forms.Button bt4;
		private System.Windows.Forms.Button bt5;
		private System.Windows.Forms.Button bt6;
		private System.Windows.Forms.Button bt7;
		private System.Windows.Forms.Button bt0;
		private System.Windows.Forms.PictureBox pictureBox1;
		private System.Windows.Forms.TextBox tbCurrentState;
		private System.Windows.Forms.Label lbCurrentState;
		private System.Windows.Forms.Button btStop;
		private System.Windows.Forms.Label lbStop;
		private System.Windows.Forms.TextBox tbIP;
	}
}
