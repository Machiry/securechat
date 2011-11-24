using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Threading;
using System.Linq;
using Chatter.ChatterClient;
using System.Text;
using System.Windows.Forms;

namespace Chatter.ChatterClient
{
    public partial class ChatterConsole : Form
    {
        ClientTalker clientTalker;   
        public ChatterConsole(ClientTalker cli)
        {
            InitializeComponent();
            this.messageBox.KeyDown += new KeyEventHandler(messageBox_KeyDown);
            this.FormClosing += new FormClosingEventHandler(ChatterConsole_FormClosing);
            this.clientTalker = cli;
            this.messageBox.BringToFront();
            this.AutoScroll = true;
            this.chatBoard.SelectionAlignment = HorizontalAlignment.Left;
            this.chatBoard.Dock = DockStyle.Fill;
            this.chatBoard.ScrollBars = RichTextBoxScrollBars.ForcedBoth;
            this.messageBox.Focus();
            this.Focus();
        }

        void ChatterConsole_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.clientTalker.TearDownConnection();
        }

        void messageBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyData == Keys.Return)
            {
                try
                {
                    Thread msgSender = new Thread(this.clientTalker.SendMessage);
                    msgSender.Start(this.messageBox.Text.Trim());
                    this.messageBox.Text = "";
                }
                catch (Exception)
                {
                    this.chatBoard.Text = this.chatBoard.Text + "\n" + "Problem occured when communicating with Server";
                    this.chatBoard.Text = this.chatBoard.Text + "\n" + "Closing The Connection";
                    this.chatBoard.Refresh();
                    if (MessageBox.Show("Problem Occured While Sending Message To Server.Closing The Connection.\n Do You Want to Close The Application?", "Server Problem", MessageBoxButtons.YesNo, MessageBoxIcon.Error) == System.Windows.Forms.DialogResult.Yes)
                    {
                        this.Close();
                    }
                    this.messageBox.Enabled = false;
                }
            }
        }

        public void RaiseProblem()
        {
            if (this.messageBox.InvokeRequired)
            {
                this.messageBox.Invoke(new System.Windows.Forms.MethodInvoker(delegate
                {
                    this.messageBox.Enabled = false;
                }));
            }
            if (MessageBox.Show("Problem occured while trying to communicate with the server. Closing the connection.\nDo you want to close the application?", "Server communication breakdown", MessageBoxButtons.YesNo, MessageBoxIcon.Stop) == System.Windows.Forms.DialogResult.Yes)
            {
                this.Close();
            }
        }

        private void richTextBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void ChatterConsole_Load(object sender, EventArgs e)
        {

        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {

        }
    }
}
