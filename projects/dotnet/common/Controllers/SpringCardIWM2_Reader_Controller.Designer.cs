/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 27/11/2015
 * Time: 10:07
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
namespace SpringCard.IWM2
{
	partial class SpringCardIWM2_Reader_Controller
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
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(SpringCardIWM2_Reader_Controller));
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.lbLastBadgeRead = new System.Windows.Forms.Label();
			this.tbBadgeRead = new System.Windows.Forms.TextBox();
			this.btGreen = new System.Windows.Forms.Button();
			this.btRed = new System.Windows.Forms.Button();
			this.lbLEDs = new System.Windows.Forms.Label();
			this.btBuzzer = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.tbCurrentState = new System.Windows.Forms.TextBox();
			this.lbCurrentState = new System.Windows.Forms.Label();
			this.tbWhen = new System.Windows.Forms.TextBox();
			this.lbWhen = new System.Windows.Forms.Label();
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.btStop = new System.Windows.Forms.Button();
			this.lbStop = new System.Windows.Forms.Label();
			this.tbIP = new System.Windows.Forms.TextBox();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			this.SuspendLayout();
			// 
			// pictureBox1
			// 
			this.pictureBox1.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("pictureBox1.BackgroundImage")));
			this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.pictureBox1.Location = new System.Drawing.Point(0, 0);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(100, 121);
			this.pictureBox1.TabIndex = 0;
			this.pictureBox1.TabStop = false;
			// 
			// lbLastBadgeRead
			// 
			this.lbLastBadgeRead.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbLastBadgeRead.Location = new System.Drawing.Point(133, 39);
			this.lbLastBadgeRead.Name = "lbLastBadgeRead";
			this.lbLastBadgeRead.Size = new System.Drawing.Size(223, 32);
			this.lbLastBadgeRead.TabIndex = 1;
			this.lbLastBadgeRead.Text = "Last badge read";
			this.lbLastBadgeRead.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// tbBadgeRead
			// 
			this.tbBadgeRead.Enabled = false;
			this.tbBadgeRead.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbBadgeRead.Location = new System.Drawing.Point(133, 74);
			this.tbBadgeRead.Name = "tbBadgeRead";
			this.tbBadgeRead.Size = new System.Drawing.Size(223, 26);
			this.tbBadgeRead.TabIndex = 2;
			this.tbBadgeRead.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// btGreen
			// 
			this.btGreen.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
			this.btGreen.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btGreen.Location = new System.Drawing.Point(534, 77);
			this.btGreen.Name = "btGreen";
			this.btGreen.Size = new System.Drawing.Size(14, 23);
			this.btGreen.TabIndex = 4;
			this.btGreen.UseVisualStyleBackColor = false;
			this.btGreen.Click += new System.EventHandler(this.BtGreenClick);
			// 
			// btRed
			// 
			this.btRed.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(128)))), ((int)(((byte)(128)))));
			this.btRed.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btRed.Location = new System.Drawing.Point(534, 48);
			this.btRed.Name = "btRed";
			this.btRed.Size = new System.Drawing.Size(14, 23);
			this.btRed.TabIndex = 5;
			this.btRed.UseVisualStyleBackColor = false;
			this.btRed.Click += new System.EventHandler(this.BtRedClick);
			// 
			// lbLEDs
			// 
			this.lbLEDs.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbLEDs.Location = new System.Drawing.Point(455, 13);
			this.lbLEDs.Name = "lbLEDs";
			this.lbLEDs.Size = new System.Drawing.Size(156, 32);
			this.lbLEDs.TabIndex = 6;
			this.lbLEDs.Text = "Send LED command";
			this.lbLEDs.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// btBuzzer
			// 
			this.btBuzzer.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("btBuzzer.BackgroundImage")));
			this.btBuzzer.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.btBuzzer.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.btBuzzer.ForeColor = System.Drawing.SystemColors.ControlLight;
			this.btBuzzer.Location = new System.Drawing.Point(647, 57);
			this.btBuzzer.Name = "btBuzzer";
			this.btBuzzer.Size = new System.Drawing.Size(34, 32);
			this.btBuzzer.TabIndex = 7;
			this.btBuzzer.UseVisualStyleBackColor = true;
			this.btBuzzer.Click += new System.EventHandler(this.BtBuzzerClick);
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.Location = new System.Drawing.Point(609, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(109, 32);
			this.label1.TabIndex = 8;
			this.label1.Text = "Send a beep";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
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
			this.tbCurrentState.TabIndex = 10;
			this.tbCurrentState.Text = "Created";
			this.tbCurrentState.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// lbCurrentState
			// 
			this.lbCurrentState.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbCurrentState.Location = new System.Drawing.Point(732, 24);
			this.lbCurrentState.Name = "lbCurrentState";
			this.lbCurrentState.Size = new System.Drawing.Size(179, 32);
			this.lbCurrentState.TabIndex = 11;
			this.lbCurrentState.Text = "Current state";
			this.lbCurrentState.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// tbWhen
			// 
			this.tbWhen.Enabled = false;
			this.tbWhen.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbWhen.Location = new System.Drawing.Point(376, 74);
			this.tbWhen.Name = "tbWhen";
			this.tbWhen.Size = new System.Drawing.Size(83, 26);
			this.tbWhen.TabIndex = 12;
			this.tbWhen.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// lbWhen
			// 
			this.lbWhen.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbWhen.Location = new System.Drawing.Point(376, 39);
			this.lbWhen.Name = "lbWhen";
			this.lbWhen.Size = new System.Drawing.Size(83, 32);
			this.lbWhen.TabIndex = 13;
			this.lbWhen.Text = "Since";
			this.lbWhen.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// timer1
			// 
			this.timer1.Tick += new System.EventHandler(this.Timer1Tick);
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
			this.btStop.TabIndex = 14;
			this.btStop.UseVisualStyleBackColor = true;
			this.btStop.Click += new System.EventHandler(this.BtStopClick);
			// 
			// lbStop
			// 
			this.lbStop.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbStop.Location = new System.Drawing.Point(934, 24);
			this.lbStop.Name = "lbStop";
			this.lbStop.Size = new System.Drawing.Size(43, 32);
			this.lbStop.TabIndex = 15;
			this.lbStop.Text = "Stop";
			this.lbStop.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// tbIP
			// 
			this.tbIP.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
			this.tbIP.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.tbIP.ForeColor = System.Drawing.SystemColors.ButtonFace;
			this.tbIP.Location = new System.Drawing.Point(133, 10);
			this.tbIP.Name = "tbIP";
			this.tbIP.ReadOnly = true;
			this.tbIP.Size = new System.Drawing.Size(223, 26);
			this.tbIP.TabIndex = 16;
			this.tbIP.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// SpringCardIWM2_NetworkReader_Controller
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.ControlLight;
			this.Controls.Add(this.tbIP);
			this.Controls.Add(this.lbStop);
			this.Controls.Add(this.btStop);
			this.Controls.Add(this.lbWhen);
			this.Controls.Add(this.tbWhen);
			this.Controls.Add(this.lbCurrentState);
			this.Controls.Add(this.tbCurrentState);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.btBuzzer);
			this.Controls.Add(this.lbLEDs);
			this.Controls.Add(this.btRed);
			this.Controls.Add(this.btGreen);
			this.Controls.Add(this.tbBadgeRead);
			this.Controls.Add(this.lbLastBadgeRead);
			this.Controls.Add(this.pictureBox1);
			this.Name = "SpringCardIWM2_NetworkReader_Controller";
			this.Size = new System.Drawing.Size(1000, 122);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();
		}
		private System.Windows.Forms.TextBox tbIP;
		private System.Windows.Forms.Label lbStop;
		private System.Windows.Forms.Button btStop;
		private System.Windows.Forms.Timer timer1;
		private System.Windows.Forms.Label lbWhen;
		private System.Windows.Forms.TextBox tbWhen;
		private System.Windows.Forms.Label lbCurrentState;
		private System.Windows.Forms.TextBox tbCurrentState;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button btBuzzer;
		private System.Windows.Forms.Label lbLEDs;
		private System.Windows.Forms.Button btRed;
		private System.Windows.Forms.Button btGreen;
		private System.Windows.Forms.TextBox tbBadgeRead;
		private System.Windows.Forms.Label lbLastBadgeRead;
		private System.Windows.Forms.PictureBox pictureBox1;
	}
}
