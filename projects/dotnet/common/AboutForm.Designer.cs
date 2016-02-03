/*
 * Created by SharpDevelop.
 * User: jerome.i
 * Date: 02/03/2012
 * Time: 10:27
 * 
 * To change this template use Tools | Options | Coding | Edit Standard Headers.
 */
namespace SpringCardApplication
{
	partial class AboutForm
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
			if (disposing) 
			{
				if (components != null)
					components.Dispose();
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AboutForm));
			this.pBottom = new System.Windows.Forms.Panel();
			this.lbWeb = new System.Windows.Forms.Label();
			this.btnOK = new System.Windows.Forms.Button();
			this.pMain = new System.Windows.Forms.Panel();
			this.lbCopyright = new System.Windows.Forms.Label();
			this.lbVersion = new System.Windows.Forms.Label();
			this.lbCompanyProduct = new System.Windows.Forms.Label();
			this.imgTop = new System.Windows.Forms.PictureBox();
			this.pBottom.SuspendLayout();
			this.pMain.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.imgTop)).BeginInit();
			this.SuspendLayout();
			// 
			// pBottom
			// 
			this.pBottom.Controls.Add(this.lbWeb);
			this.pBottom.Controls.Add(this.btnOK);
			this.pBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.pBottom.Location = new System.Drawing.Point(0, 218);
			this.pBottom.Name = "pBottom";
			this.pBottom.Size = new System.Drawing.Size(401, 43);
			this.pBottom.TabIndex = 1;
			// 
			// lbWeb
			// 
			this.lbWeb.Cursor = System.Windows.Forms.Cursors.Hand;
			this.lbWeb.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbWeb.ForeColor = System.Drawing.Color.Brown;
			this.lbWeb.Location = new System.Drawing.Point(12, 13);
			this.lbWeb.Name = "lbWeb";
			this.lbWeb.Size = new System.Drawing.Size(194, 23);
			this.lbWeb.TabIndex = 1;
			this.lbWeb.Text = "www.springcard.com";
			this.lbWeb.Click += new System.EventHandler(this.LbWebClick);
			// 
			// btnOK
			// 
			this.btnOK.Location = new System.Drawing.Point(304, 8);
			this.btnOK.Name = "btnOK";
			this.btnOK.Size = new System.Drawing.Size(85, 23);
			this.btnOK.TabIndex = 0;
			this.btnOK.Text = "OK";
			this.btnOK.UseVisualStyleBackColor = true;
			this.btnOK.Click += new System.EventHandler(this.BtnOKClick);
			// 
			// pMain
			// 
			this.pMain.Controls.Add(this.lbCopyright);
			this.pMain.Controls.Add(this.lbVersion);
			this.pMain.Controls.Add(this.lbCompanyProduct);
			this.pMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pMain.Location = new System.Drawing.Point(0, 90);
			this.pMain.Name = "pMain";
			this.pMain.Size = new System.Drawing.Size(401, 128);
			this.pMain.TabIndex = 2;
			// 
			// lbCopyright
			// 
			this.lbCopyright.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbCopyright.ForeColor = System.Drawing.Color.Black;
			this.lbCopyright.Location = new System.Drawing.Point(12, 45);
			this.lbCopyright.Name = "lbCopyright";
			this.lbCopyright.Size = new System.Drawing.Size(370, 16);
			this.lbCopyright.TabIndex = 2;
			this.lbCopyright.Text = "label1";
			// 
			// lbVersion
			// 
			this.lbVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbVersion.ForeColor = System.Drawing.Color.Black;
			this.lbVersion.Location = new System.Drawing.Point(12, 29);
			this.lbVersion.Name = "lbVersion";
			this.lbVersion.Size = new System.Drawing.Size(370, 16);
			this.lbVersion.TabIndex = 1;
			this.lbVersion.Text = "label1";
			// 
			// lbCompanyProduct
			// 
			this.lbCompanyProduct.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lbCompanyProduct.ForeColor = System.Drawing.Color.Black;
			this.lbCompanyProduct.Location = new System.Drawing.Point(12, 13);
			this.lbCompanyProduct.Name = "lbCompanyProduct";
			this.lbCompanyProduct.Size = new System.Drawing.Size(370, 16);
			this.lbCompanyProduct.TabIndex = 0;
			this.lbCompanyProduct.Text = "label1";
			// 
			// imgTop
			// 
			this.imgTop.Cursor = System.Windows.Forms.Cursors.Hand;
			this.imgTop.Dock = System.Windows.Forms.DockStyle.Top;
			this.imgTop.Image = ((System.Drawing.Image)(resources.GetObject("imgTop.Image")));
			this.imgTop.Location = new System.Drawing.Point(0, 0);
			this.imgTop.Name = "imgTop";
			this.imgTop.Size = new System.Drawing.Size(401, 90);
			this.imgTop.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
			this.imgTop.TabIndex = 0;
			this.imgTop.TabStop = false;
			this.imgTop.Click += new System.EventHandler(this.ImgTopClick);
			// 
			// AboutForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.SystemColors.Control;
			this.ClientSize = new System.Drawing.Size(401, 261);
			this.Controls.Add(this.pMain);
			this.Controls.Add(this.imgTop);
			this.Controls.Add(this.pBottom);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "AboutForm";
			this.ShowIcon = false;
			this.ShowInTaskbar = false;
			this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "About ...";
			this.pBottom.ResumeLayout(false);
			this.pMain.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.imgTop)).EndInit();
			this.ResumeLayout(false);
		}
		private System.Windows.Forms.Label lbWeb;
		private System.Windows.Forms.PictureBox imgTop;
		private System.Windows.Forms.Label lbCompanyProduct;
		private System.Windows.Forms.Label lbVersion;
		private System.Windows.Forms.Label lbCopyright;
		private System.Windows.Forms.Panel pMain;
		private System.Windows.Forms.Button btnOK;
		private System.Windows.Forms.Panel pBottom;
	}
}
