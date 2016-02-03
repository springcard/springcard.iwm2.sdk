using System;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;

namespace SpringCardApplication
{
	public partial class AboutForm : Form
	{
		public AboutForm()
		{
			InitializeComponent();
			FileVersionInfo i = FileVersionInfo.GetVersionInfo(System.AppDomain.CurrentDomain.FriendlyName);
			lbCompanyProduct.Text = i.CompanyName + " " + i.ProductName;
			lbVersion.Text = "Version " + i.ProductVersion;
			lbCopyright.Text =i.LegalCopyright;
		}
		
		void ImgTopClick(object sender, EventArgs e)
		{
			System.Diagnostics.Process.Start("http://www.springcard.com");
		}
		
		void LbWebClick(object sender, EventArgs e)
		{
			System.Diagnostics.Process.Start("http://www.springcard.com");
		}
		
		void BtnOKClick(object sender, EventArgs e)
		{
			Close();
		}
		
		void LbIconsClick(object sender, EventArgs e)
		{
			System.Diagnostics.Process.Start("http://icons8.com");
		}
	}
}
