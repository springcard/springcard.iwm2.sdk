/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 27/11/2015
 * Time: 14:18
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
namespace TCPReader
{
	partial class AddDeviceForm
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
			this.btOK = new System.Windows.Forms.Button();
			this.btCancel = new System.Windows.Forms.Button();
			this.lbIP = new System.Windows.Forms.Label();
			this.tbIP = new System.Windows.Forms.TextBox();
			this.tbPort = new System.Windows.Forms.TextBox();
			this.lbPort = new System.Windows.Forms.Label();
			this.rbFunkyGate = new System.Windows.Forms.RadioButton();
			this.rbHandyDrummer = new System.Windows.Forms.RadioButton();
			this.rbPlain = new System.Windows.Forms.RadioButton();
			this.rbSecuredOperation = new System.Windows.Forms.RadioButton();
			this.rbSecuredAdministration = new System.Windows.Forms.RadioButton();
			this.pCom = new System.Windows.Forms.Panel();
			this.tbKeyValue = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.mkTxtUnlockKey = new System.Windows.Forms.MaskedTextBox();
			this.pDevice = new System.Windows.Forms.Panel();
			this.label2 = new System.Windows.Forms.Label();
			this.pBottom = new System.Windows.Forms.Panel();
			this.pCom.SuspendLayout();
			this.pDevice.SuspendLayout();
			this.pBottom.SuspendLayout();
			this.SuspendLayout();
			// 
			// btOK
			// 
			this.btOK.Location = new System.Drawing.Point(285, 18);
			this.btOK.Name = "btOK";
			this.btOK.Size = new System.Drawing.Size(75, 23);
			this.btOK.TabIndex = 0;
			this.btOK.Text = "OK";
			this.btOK.UseVisualStyleBackColor = true;
			this.btOK.Click += new System.EventHandler(this.BtOKClick);
			// 
			// btCancel
			// 
			this.btCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.btCancel.Location = new System.Drawing.Point(387, 18);
			this.btCancel.Name = "btCancel";
			this.btCancel.Size = new System.Drawing.Size(75, 23);
			this.btCancel.TabIndex = 1;
			this.btCancel.Text = "Cancel";
			this.btCancel.UseVisualStyleBackColor = true;
			// 
			// lbIP
			// 
			this.lbIP.Location = new System.Drawing.Point(162, 113);
			this.lbIP.Name = "lbIP";
			this.lbIP.Size = new System.Drawing.Size(49, 23);
			this.lbIP.TabIndex = 2;
			this.lbIP.Text = "IP: ";
			this.lbIP.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// tbIP
			// 
			this.tbIP.Location = new System.Drawing.Point(217, 116);
			this.tbIP.Name = "tbIP";
			this.tbIP.Size = new System.Drawing.Size(100, 20);
			this.tbIP.TabIndex = 3;
			// 
			// tbPort
			// 
			this.tbPort.Location = new System.Drawing.Point(217, 142);
			this.tbPort.Name = "tbPort";
			this.tbPort.Size = new System.Drawing.Size(33, 20);
			this.tbPort.TabIndex = 4;
			this.tbPort.Text = "3999";
			// 
			// lbPort
			// 
			this.lbPort.Location = new System.Drawing.Point(162, 140);
			this.lbPort.Name = "lbPort";
			this.lbPort.Size = new System.Drawing.Size(49, 23);
			this.lbPort.TabIndex = 5;
			this.lbPort.Text = "Port:";
			this.lbPort.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// rbFunkyGate
			// 
			this.rbFunkyGate.Location = new System.Drawing.Point(56, 113);
			this.rbFunkyGate.Name = "rbFunkyGate";
			this.rbFunkyGate.Size = new System.Drawing.Size(87, 24);
			this.rbFunkyGate.TabIndex = 6;
			this.rbFunkyGate.TabStop = true;
			this.rbFunkyGate.Text = "FunkyGate";
			this.rbFunkyGate.UseVisualStyleBackColor = true;
			// 
			// rbHandyDrummer
			// 
			this.rbHandyDrummer.Location = new System.Drawing.Point(56, 143);
			this.rbHandyDrummer.Name = "rbHandyDrummer";
			this.rbHandyDrummer.Size = new System.Drawing.Size(104, 24);
			this.rbHandyDrummer.TabIndex = 7;
			this.rbHandyDrummer.TabStop = true;
			this.rbHandyDrummer.Text = "HandyDrummer";
			this.rbHandyDrummer.UseVisualStyleBackColor = true;
			// 
			// rbPlain
			// 
			this.rbPlain.Location = new System.Drawing.Point(90, 87);
			this.rbPlain.Name = "rbPlain";
			this.rbPlain.Size = new System.Drawing.Size(155, 24);
			this.rbPlain.TabIndex = 8;
			this.rbPlain.TabStop = true;
			this.rbPlain.Text = "Plain communication";
			this.rbPlain.UseVisualStyleBackColor = true;
			this.rbPlain.CheckedChanged += new System.EventHandler(this.RbPlainCheckedChanged);
			// 
			// rbSecuredOperation
			// 
			this.rbSecuredOperation.Location = new System.Drawing.Point(90, 117);
			this.rbSecuredOperation.Name = "rbSecuredOperation";
			this.rbSecuredOperation.Size = new System.Drawing.Size(168, 24);
			this.rbSecuredOperation.TabIndex = 9;
			this.rbSecuredOperation.TabStop = true;
			this.rbSecuredOperation.Text = "Secured with operation key";
			this.rbSecuredOperation.UseVisualStyleBackColor = true;
			// 
			// rbSecuredAdministration
			// 
			this.rbSecuredAdministration.Location = new System.Drawing.Point(90, 147);
			this.rbSecuredAdministration.Name = "rbSecuredAdministration";
			this.rbSecuredAdministration.Size = new System.Drawing.Size(181, 24);
			this.rbSecuredAdministration.TabIndex = 10;
			this.rbSecuredAdministration.TabStop = true;
			this.rbSecuredAdministration.Text = "Secured with administration key";
			this.rbSecuredAdministration.UseVisualStyleBackColor = true;
			// 
			// pCom
			// 
			this.pCom.BackColor = System.Drawing.SystemColors.Control;
			this.pCom.Controls.Add(this.tbKeyValue);
			this.pCom.Controls.Add(this.label1);
			this.pCom.Controls.Add(this.mkTxtUnlockKey);
			this.pCom.Controls.Add(this.rbPlain);
			this.pCom.Controls.Add(this.rbSecuredAdministration);
			this.pCom.Controls.Add(this.rbSecuredOperation);
			this.pCom.Location = new System.Drawing.Point(387, 12);
			this.pCom.Name = "pCom";
			this.pCom.Size = new System.Drawing.Size(351, 259);
			this.pCom.TabIndex = 11;
			// 
			// tbKeyValue
			// 
			this.tbKeyValue.Location = new System.Drawing.Point(18, 194);
			this.tbKeyValue.Name = "tbKeyValue";
			this.tbKeyValue.Size = new System.Drawing.Size(66, 23);
			this.tbKeyValue.TabIndex = 28;
			this.tbKeyValue.Text = "Key value:";
			this.tbKeyValue.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label1.ForeColor = System.Drawing.Color.Red;
			this.label1.Location = new System.Drawing.Point(3, 32);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(344, 23);
			this.label1.TabIndex = 27;
			this.label1.Text = "Communication mode";
			this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// mkTxtUnlockKey
			// 
			this.mkTxtUnlockKey.Culture = new System.Globalization.CultureInfo("");
			this.mkTxtUnlockKey.Location = new System.Drawing.Point(90, 197);
			this.mkTxtUnlockKey.Mask = "aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa";
			this.mkTxtUnlockKey.Name = "mkTxtUnlockKey";
			this.mkTxtUnlockKey.Size = new System.Drawing.Size(245, 20);
			this.mkTxtUnlockKey.TabIndex = 26;
			this.mkTxtUnlockKey.Text = "00000000000000000000000000000000";
			// 
			// pDevice
			// 
			this.pDevice.BackColor = System.Drawing.SystemColors.Control;
			this.pDevice.Controls.Add(this.label2);
			this.pDevice.Controls.Add(this.rbFunkyGate);
			this.pDevice.Controls.Add(this.lbIP);
			this.pDevice.Controls.Add(this.rbHandyDrummer);
			this.pDevice.Controls.Add(this.tbIP);
			this.pDevice.Controls.Add(this.tbPort);
			this.pDevice.Controls.Add(this.lbPort);
			this.pDevice.Location = new System.Drawing.Point(12, 12);
			this.pDevice.Name = "pDevice";
			this.pDevice.Size = new System.Drawing.Size(351, 259);
			this.pDevice.TabIndex = 12;
			// 
			// label2
			// 
			this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.label2.ForeColor = System.Drawing.Color.Red;
			this.label2.Location = new System.Drawing.Point(3, 32);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(345, 23);
			this.label2.TabIndex = 28;
			this.label2.Text = "Device description";
			this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// pBottom
			// 
			this.pBottom.BackColor = System.Drawing.SystemColors.ControlLightLight;
			this.pBottom.Controls.Add(this.btOK);
			this.pBottom.Controls.Add(this.btCancel);
			this.pBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.pBottom.Location = new System.Drawing.Point(0, 298);
			this.pBottom.Name = "pBottom";
			this.pBottom.Size = new System.Drawing.Size(746, 59);
			this.pBottom.TabIndex = 13;
			// 
			// AddDeviceForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(746, 357);
			this.Controls.Add(this.pBottom);
			this.Controls.Add(this.pDevice);
			this.Controls.Add(this.pCom);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "AddDeviceForm";
			this.ShowIcon = false;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Add a device";
			this.pCom.ResumeLayout(false);
			this.pCom.PerformLayout();
			this.pDevice.ResumeLayout(false);
			this.pDevice.PerformLayout();
			this.pBottom.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		private System.Windows.Forms.Panel pBottom;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Panel pDevice;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label tbKeyValue;
		private System.Windows.Forms.MaskedTextBox mkTxtUnlockKey;
		private System.Windows.Forms.Panel pCom;
		private System.Windows.Forms.RadioButton rbSecuredAdministration;
		private System.Windows.Forms.RadioButton rbSecuredOperation;
		private System.Windows.Forms.RadioButton rbPlain;
		private System.Windows.Forms.RadioButton rbHandyDrummer;
		private System.Windows.Forms.RadioButton rbFunkyGate;
		private System.Windows.Forms.Label lbPort;
		private System.Windows.Forms.TextBox tbPort;
		private System.Windows.Forms.TextBox tbIP;
		private System.Windows.Forms.Label lbIP;
		private System.Windows.Forms.Button btCancel;
		private System.Windows.Forms.Button btOK;
	}
}
