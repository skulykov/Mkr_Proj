using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Net;
using System.Data;

using SIP_Proxy_demo.Resources;
using MkrSoft.Net;
using MkrSoft.Net.Log;
using MkrSoft.Net.SIP;
using MkrSoft.Net.SIP.Message;
using MkrSoft.Net.SIP.Stack;
using MkrSoft.Net.SIP.Proxy;

namespace SIP_Proxy_demo
{
    /// <summary>
    /// Application main window.
    /// </summary>
    public class wfrm_Main : Form
    {
        #region class UserInfo

        /// <summary>
        /// This class holds user info.
        /// </summary>
        private class UserInfo
        {
            private string m_UserName = "";
            private string m_Password = "";
            private string m_AOR      = "";

            /// <summary>
            /// Default constructor.
            /// </summary>
            /// <param name="userName">User name.</param>
            /// <param name="password">Password.</param>
            /// <param name="aor">Address of record.</param>
            public UserInfo(string userName,string password,string aor)
            {
                m_UserName = userName;
                m_Password = password;
                m_AOR      = aor;
            }


            #region Properties Implementation

            /// <summary>
            /// Gets user name.
            /// </summary>
            public string UserName
            {
                get{ return m_UserName; }
            }

            /// <summary>
            /// Gets password.
            /// </summary>
            public string Password
            {
                get{ return m_Password; }
            }

            /// <summary>
            /// Gets address of record.
            /// </summary>
            public string AddressOfRecord
            {
                get{ return m_AOR; }
            }

            #endregion

        }

        #endregion

        private delegate void AddError(Exception x);

        private delegate void AddLog(LogEntry[] entries);

        private TabControl m_pTab           = null;
        private ToolStrip  m_pServerToolbar = null;
        private Button     m_pShutDown      = null;
        // Settings tab
        private Label         mt_TabSettings_ProxyType = null;
        private ComboBox      m_pTabSettings_ProxyType = null;
        private Label         mt_TabSettings_ForkMode  = null;
        private ComboBox      m_pTabSettings_ForkMode  = null;
        private Label         mt_TabSettings_HostName  = null;
        private TextBox       m_pTabSettings_HostName  = null;
        private Label         mt_TabSettings_Port      = null;
        private NumericUpDown m_pTabSettings_Port      = null;
        private ToolStrip     m_pTabSettings_Toolbar   = null;
        private ListView      m_pTabSettings_Users     = null;
        // Registrations tab
        private PictureBox m_pTabRegistrations_NoteIcon      = null;
        private Label      m_pTabRegistrations_Note          = null;
        private ToolStrip  m_pTabRegistrations_Toolbar       = null;
        private ListView   m_pTabRegistrations_Registrations = null;
        // Calls tab
        private ToolStrip m_pTabCalls_Toolbar = null;
        private ListView  m_pTabCalls_Calls   = null;
        // Gateways
        private ToolStrip m_pTabGateways_Toolbar  = null;
        private ListView  m_pTabGateways_Gateways = null;
        // Errors tab
        private Button   m_pTabErrors_Clear  = null;
        private ListView m_pTabErrors_Errors = null;
        // Log tab
        private CheckBox    m_pTabLog_Log     = null;
        private Button      m_pTabLog_Clear   = null;
        private RichTextBox m_pTabLog_LogText = null;

        private DataSet        m_pDsSettings = null;
        private List<UserInfo> m_pUsers      = null;
        private SIP_Stack      m_pSipStack   = null;
        private bool           m_OddLogEntry = true;

        /// <summary>
        /// Default constructor.
        /// </summary>
        public wfrm_Main()
        {
            InitUI();

            m_pUsers = new List<UserInfo>();

            m_pSipStack = new SIP_Stack();
            m_pSipStack.Error += new SIP_ErrorEventHandler(m_pSipStack_Error);
            //m_pSipStack.Logger = new Logger();
            m_pSipStack.Logger.WriteLog += new WriteLogHandler(Logger_WriteLog);
            ((SIP_ProxyCore)m_pSipStack.SipCore).ProxyMode = SIP_ProxyMode.Registrar | SIP_ProxyMode.Statefull;
            ((SIP_ProxyCore)m_pSipStack.SipCore).AddressExists += new SIP_AddressExistsEventHandler(SIP_AddressExists);
            ((SIP_ProxyCore)m_pSipStack.SipCore).Authenticate += new SIP_AuthenticateEventHandler(SIP_Authenticate);
            ((SIP_ProxyCore)m_pSipStack.SipCore).Registrar.CanRegister += new SIP_CanRegisterEventHandler(SIP_CanRegister);
            ((SIP_ProxyCore)m_pSipStack.SipCore).IsLocalUri += new SIP_IsLocalUriEventHandler(SIP_IsLocalUri);
            ((SIP_ProxyCore)m_pSipStack.SipCore).GetGateways += new SIP_GetGatewaysEventHandler(SIP_GetGateways);

            LoadData();
        }
                               
        #region method InitUI

        /// <summary>
        /// Creates and initializes UI.
        /// </summary>
        private void InitUI()
        {
            this.ClientSize = new Size(500,400);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.Text = "SIP Proxy Server demo";
            this.Icon = ResManager.GetIcon("app.ico");
            this.FormClosed += new FormClosedEventHandler(wfrm_Main_FormClosed);

            m_pTab = new TabControl();
            m_pTab.Size = new Size(490,350);
            m_pTab.Location = new Point(5,5);
            m_pTab.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Top;

            #region Settings tab

            mt_TabSettings_ProxyType = new Label();
            mt_TabSettings_ProxyType.Size = new Size(100,20);
            mt_TabSettings_ProxyType.Location = new Point(0,10);
            mt_TabSettings_ProxyType.TextAlign = ContentAlignment.MiddleRight;
            mt_TabSettings_ProxyType.Text = "Proxy type:";

            m_pTabSettings_ProxyType = new ComboBox();
            m_pTabSettings_ProxyType.Size = new Size(370,20);
            m_pTabSettings_ProxyType.Location = new Point(105,10);
            m_pTabSettings_ProxyType.DropDownStyle = ComboBoxStyle.DropDownList;
            m_pTabSettings_ProxyType.Items.Add("Statefull");
            m_pTabSettings_ProxyType.Items.Add("Stateless");
            m_pTabSettings_ProxyType.Items.Add("B2BUA");
            m_pTabSettings_ProxyType.SelectedIndex = 0;
            m_pTabSettings_ProxyType.SelectedIndexChanged += new EventHandler(m_pTabSettings_ProxyType_SelectedIndexChanged);

            mt_TabSettings_ForkMode = new Label();
            mt_TabSettings_ForkMode.Size = new Size(100,20);
            mt_TabSettings_ForkMode.Location = new Point(0,35);
            mt_TabSettings_ForkMode.TextAlign = ContentAlignment.MiddleRight;
            mt_TabSettings_ForkMode.Text = "Fork mode:";

            m_pTabSettings_ForkMode = new ComboBox();
            m_pTabSettings_ForkMode.Size = new Size(370,20);
            m_pTabSettings_ForkMode.Location = new Point(105,35);
            m_pTabSettings_ForkMode.DropDownStyle = ComboBoxStyle.DropDownList;
            m_pTabSettings_ForkMode.Items.Add("None");
            m_pTabSettings_ForkMode.Items.Add("Sequential");
            m_pTabSettings_ForkMode.Items.Add("Parallel");
            m_pTabSettings_ForkMode.SelectedIndex = 2;
                        
            mt_TabSettings_HostName = new Label();
            mt_TabSettings_HostName.Size = new Size(100,20);
            mt_TabSettings_HostName.Location = new Point(0,60);
            mt_TabSettings_HostName.TextAlign = ContentAlignment.MiddleRight;
            mt_TabSettings_HostName.Text = "Host name:";

            m_pTabSettings_HostName = new TextBox();
            m_pTabSettings_HostName.Size = new Size(370,20);
            m_pTabSettings_HostName.Location = new Point(105,60);
            
            mt_TabSettings_Port = new Label();
            mt_TabSettings_Port.Size = new Size(100,20);
            mt_TabSettings_Port.Location = new Point(0,85);
            mt_TabSettings_Port.TextAlign = ContentAlignment.MiddleRight;
            mt_TabSettings_Port.Text = "Listening Port:";
           
            m_pTabSettings_Port = new NumericUpDown();
            m_pTabSettings_Port.Size = new Size(70,20);
            m_pTabSettings_Port.Location = new Point(105,85);
            m_pTabSettings_Port.Minimum = 1;
            m_pTabSettings_Port.Maximum = 99999;
            m_pTabSettings_Port.Value = 5060;

            m_pTabSettings_Toolbar = new ToolStrip();
            m_pTabSettings_Toolbar.Dock = DockStyle.None;
            m_pTabSettings_Toolbar.Size = new Size(55,20);
            m_pTabSettings_Toolbar.Location = new Point(425,90);
            m_pTabSettings_Toolbar.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            m_pTabSettings_Toolbar.GripStyle = ToolStripGripStyle.Hidden;
            m_pTabSettings_Toolbar.BackColor = this.BackColor;
            m_pTabSettings_Toolbar.Renderer = new ToolBarRendererEx();
            m_pTabSettings_Toolbar.ItemClicked += new ToolStripItemClickedEventHandler(m_pTabSettings_Toolbar_ItemClicked);
            // Add button
            ToolStripButton settings_button_Add = new ToolStripButton();
            settings_button_Add.Image = ResManager.GetIcon("add.ico").ToBitmap();
            settings_button_Add.Tag = "add";
            settings_button_Add.ToolTipText = "Add";
            m_pTabSettings_Toolbar.Items.Add(settings_button_Add);
            // Delete button
            ToolStripButton settings_button_Delete = new ToolStripButton();
            settings_button_Delete.Enabled = false;
            settings_button_Delete.Image = ResManager.GetIcon("delete.ico").ToBitmap();
            settings_button_Delete.Tag = "delete";
            settings_button_Delete.ToolTipText  = "Delete";
            m_pTabSettings_Toolbar.Items.Add(settings_button_Delete);

            m_pTabSettings_Users = new ListView();
            m_pTabSettings_Users.Size = new Size(470,200);
            m_pTabSettings_Users.Location = new Point(5,115);
            m_pTabSettings_Users.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Top;
            m_pTabSettings_Users.View = View.Details;
            m_pTabSettings_Users.HideSelection = false;
            m_pTabSettings_Users.FullRowSelect = true;
            m_pTabSettings_Users.Columns.Add("User",100);
            m_pTabSettings_Users.Columns.Add("Address of Record",345);
            m_pTabSettings_Users.SelectedIndexChanged += new EventHandler(m_pSettingsTab_Users_SelectedIndexChanged);

            m_pTab.TabPages.Add("Settings");
            m_pTab.TabPages[0].Size = new Size(482,324);
            m_pTab.TabPages[0].Controls.Add(mt_TabSettings_ProxyType);
            m_pTab.TabPages[0].Controls.Add(m_pTabSettings_ProxyType);
            m_pTab.TabPages[0].Controls.Add(mt_TabSettings_ForkMode);
            m_pTab.TabPages[0].Controls.Add(m_pTabSettings_ForkMode);
            m_pTab.TabPages[0].Controls.Add(mt_TabSettings_HostName);
            m_pTab.TabPages[0].Controls.Add(m_pTabSettings_HostName);
            m_pTab.TabPages[0].Controls.Add(mt_TabSettings_Port);
            m_pTab.TabPages[0].Controls.Add(m_pTabSettings_Port);
            m_pTab.TabPages[0].Controls.Add(m_pTabSettings_Toolbar);
            m_pTab.TabPages[0].Controls.Add(m_pTabSettings_Users);

            #endregion

            #region Registrations tab

            m_pTab.TabPages.Add("Registrations");
            m_pTab.TabPages[1].Size = new Size(482,324);

            m_pTabRegistrations_NoteIcon = new PictureBox();
            m_pTabRegistrations_NoteIcon.Size = new Size(20,20);
            m_pTabRegistrations_NoteIcon.Location = new Point(10,7);
            m_pTabRegistrations_NoteIcon.SizeMode = PictureBoxSizeMode.StretchImage;
            m_pTabRegistrations_NoteIcon.Image = ResManager.GetIcon("info.ico").ToBitmap();

            m_pTabRegistrations_Note = new Label();
            m_pTabRegistrations_Note.Size = new Size(300,30);
            m_pTabRegistrations_Note.Location = new Point(30,7);
            m_pTabRegistrations_Note.Text = "Note: Normally you don't need add registrations manaully, SIP phone does it !";
                        
            m_pTabRegistrations_Toolbar = new ToolStrip();            
            m_pTabRegistrations_Toolbar.Dock = DockStyle.None;
            m_pTabRegistrations_Toolbar.Location = new Point(375,15);
            m_pTabRegistrations_Toolbar.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            m_pTabRegistrations_Toolbar.GripStyle = ToolStripGripStyle.Hidden;
            m_pTabRegistrations_Toolbar.BackColor = this.BackColor;
            m_pTabRegistrations_Toolbar.Renderer = new ToolBarRendererEx();
            m_pTabRegistrations_Toolbar.ItemClicked += new ToolStripItemClickedEventHandler(m_pTabRegistrations_Toolbar_ItemClicked);
            // Add button
            ToolStripButton button_Add = new ToolStripButton();
            button_Add.Image = ResManager.GetIcon("add.ico").ToBitmap();
            button_Add.Tag = "add";
            button_Add.ToolTipText = "Add";
            m_pTabRegistrations_Toolbar.Items.Add(button_Add);
            // Delete button
            ToolStripButton button_Delete = new ToolStripButton();
            button_Delete.Enabled = false;
            button_Delete.Image = ResManager.GetIcon("delete.ico").ToBitmap();
            button_Delete.Tag = "delete";
            button_Delete.ToolTipText  = "Delete";
            m_pTabRegistrations_Toolbar.Items.Add(button_Delete);
            // Separator
            m_pTabRegistrations_Toolbar.Items.Add(new ToolStripSeparator());
            // Refresh button
            ToolStripButton button_Refresh = new ToolStripButton();
            button_Refresh.Image = ResManager.GetIcon("refresh.ico").ToBitmap();
            button_Refresh.Tag = "refresh";
            button_Refresh.ToolTipText  = "Refresh";
            m_pTabRegistrations_Toolbar.Items.Add(button_Refresh);
            // View button
            ToolStripButton button_View = new ToolStripButton();
            button_View.Enabled = false;
            button_View.Image = ResManager.GetIcon("viewmessages.ico").ToBitmap();
            button_View.Tag = "view";
            button_View.ToolTipText  = "View Contacts";
            m_pTabRegistrations_Toolbar.Items.Add(button_View);

            m_pTabRegistrations_Registrations = new ListView();
            m_pTabRegistrations_Registrations.Size = new Size(470,275);
            m_pTabRegistrations_Registrations.Location = new Point(5,40);
            m_pTabRegistrations_Registrations.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Top;
            m_pTabRegistrations_Registrations.View = View.Details;
            m_pTabRegistrations_Registrations.FullRowSelect = true;
            m_pTabRegistrations_Registrations.HideSelection = false;
            m_pTabRegistrations_Registrations.Columns.Add("User",100,HorizontalAlignment.Left);
            m_pTabRegistrations_Registrations.Columns.Add("Address of Record",340,HorizontalAlignment.Left);
            m_pTabRegistrations_Registrations.SelectedIndexChanged += new EventHandler(m_pTabRegistrations_Registrations_SelectedIndexChanged);
            m_pTabRegistrations_Registrations.DoubleClick += new EventHandler(m_pTabRegistrations_Registrations_DoubleClick);
            
            m_pTab.TabPages[1].Controls.Add(m_pTabRegistrations_NoteIcon);
            m_pTab.TabPages[1].Controls.Add(m_pTabRegistrations_Note);
            m_pTab.TabPages[1].Controls.Add(m_pTabRegistrations_Toolbar);
            m_pTab.TabPages[1].Controls.Add(m_pTabRegistrations_Registrations);

            #endregion

            #region Calls tab

            m_pTab.TabPages.Add("Active Calls");
            m_pTab.TabPages[2].Size = new Size(482,324);

            m_pTabCalls_Toolbar = new ToolStrip();            
            m_pTabCalls_Toolbar.Dock = DockStyle.None;
            m_pTabCalls_Toolbar.Location = new Point(375,15);
            m_pTabCalls_Toolbar.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            m_pTabCalls_Toolbar.GripStyle = ToolStripGripStyle.Hidden;
            m_pTabCalls_Toolbar.BackColor = this.BackColor;
            m_pTabCalls_Toolbar.Renderer = new ToolBarRendererEx();
            m_pTabCalls_Toolbar.ItemClicked += new ToolStripItemClickedEventHandler(m_pTabCalls_Toolbar_ItemClicked);
            // Delete button
            ToolStripButton callsButton_Delete = new ToolStripButton();
            callsButton_Delete.Enabled = false;
            callsButton_Delete.Image = ResManager.GetIcon("delete.ico").ToBitmap();
            callsButton_Delete.Tag = "delete";
            callsButton_Delete.ToolTipText  = "Delete";
            m_pTabCalls_Toolbar.Items.Add(callsButton_Delete);
            // Separator
            m_pTabCalls_Toolbar.Items.Add(new ToolStripSeparator());
            // Refresh button
            ToolStripButton callsButton_Refresh = new ToolStripButton();
            callsButton_Refresh.Image = ResManager.GetIcon("refresh.ico").ToBitmap();
            callsButton_Refresh.Tag = "refresh";
            callsButton_Refresh.ToolTipText  = "Refresh";
            m_pTabCalls_Toolbar.Items.Add(callsButton_Refresh);
                        
            m_pTabCalls_Calls = new ListView();
            m_pTabCalls_Calls.Size = new Size(470,275);
            m_pTabCalls_Calls.Location = new Point(5,40);
            m_pTabCalls_Calls.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Top;
            m_pTabCalls_Calls.View = View.Details;
            m_pTabCalls_Calls.FullRowSelect = true;
            m_pTabCalls_Calls.HideSelection = false;
            m_pTabCalls_Calls.Columns.Add("From:",150);
            m_pTabCalls_Calls.Columns.Add("To:",150);
            m_pTabCalls_Calls.Columns.Add("Call start time:",100);
            m_pTabCalls_Calls.SelectedIndexChanged += new EventHandler(m_pTabCalls_Calls_SelectedIndexChanged);

            m_pTab.TabPages[2].Controls.Add(m_pTabCalls_Toolbar);
            m_pTab.TabPages[2].Controls.Add(m_pTabCalls_Calls);

            #endregion

            #region Gateways

            m_pTab.TabPages.Add("Gateways");
            m_pTab.TabPages[3].Size = new Size(482,324);

            m_pTabGateways_Toolbar = new ToolStrip();            
            m_pTabGateways_Toolbar.Dock = DockStyle.None;
            m_pTabGateways_Toolbar.Location = new Point(375,15);
            m_pTabGateways_Toolbar.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            m_pTabGateways_Toolbar.GripStyle = ToolStripGripStyle.Hidden;
            m_pTabGateways_Toolbar.BackColor = this.BackColor;
            m_pTabGateways_Toolbar.Renderer = new ToolBarRendererEx();
            m_pTabGateways_Toolbar.ItemClicked += new ToolStripItemClickedEventHandler(m_pTabGateways_Toolbar_ItemClicked);
            // Add button
            ToolStripButton gwButton_Add = new ToolStripButton();
            gwButton_Add.Image = ResManager.GetIcon("add.ico").ToBitmap();
            gwButton_Add.Tag = "add";
            gwButton_Add.ToolTipText = "Add";
            m_pTabGateways_Toolbar.Items.Add(gwButton_Add);
            // Edit button
            ToolStripButton gwButton_Edit = new ToolStripButton();
            gwButton_Edit.Image = ResManager.GetIcon("edit.ico").ToBitmap();
            gwButton_Edit.Tag = "edit";
            gwButton_Edit.ToolTipText = "Edit";
            m_pTabGateways_Toolbar.Items.Add(gwButton_Edit);
            // Delete button
            ToolStripButton gwButton_Delete = new ToolStripButton();
            gwButton_Delete.Enabled = false;
            gwButton_Delete.Image = ResManager.GetIcon("delete.ico").ToBitmap();
            gwButton_Delete.Tag = "delete";
            gwButton_Delete.ToolTipText  = "Delete";
            m_pTabGateways_Toolbar.Items.Add(gwButton_Delete);

            m_pTabGateways_Gateways = new ListView();
            m_pTabGateways_Gateways.Size = new Size(470,275);
            m_pTabGateways_Gateways.Location = new Point(5,40);
            m_pTabGateways_Gateways.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Top;
            m_pTabGateways_Gateways.View = View.Details;
            m_pTabGateways_Gateways.FullRowSelect = true;
            m_pTabGateways_Gateways.HideSelection = false;
            m_pTabGateways_Gateways.Columns.Add("URI scheme",80);
            m_pTabGateways_Gateways.Columns.Add("Transport",60);
            m_pTabGateways_Gateways.Columns.Add("Host",240);
            m_pTabGateways_Gateways.Columns.Add("Port",60);
            m_pTabGateways_Gateways.SelectedIndexChanged += new EventHandler(m_pTabGateways_Gateways_SelectedIndexChanged);
                        
            m_pTab.TabPages[3].Controls.Add(m_pTabGateways_Toolbar);
            m_pTab.TabPages[3].Controls.Add(m_pTabGateways_Gateways);

            #endregion

            #region Errors tab

            m_pTabErrors_Clear = new Button();
            m_pTabErrors_Clear.Size = new Size(70,20);
            m_pTabErrors_Clear.Location = new Point(405,10);
            m_pTabErrors_Clear.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            m_pTabErrors_Clear.Text = "Clear";
            m_pTabErrors_Clear.Click += new EventHandler(m_pTabErrors_Clear_Click);

            ImageList errors_imgList = new ImageList();
            errors_imgList.Images.Add(ResManager.GetIcon("error.ico"));
            m_pTabErrors_Errors = new ListView();
            m_pTabErrors_Errors.Size = new Size(470,280);
            m_pTabErrors_Errors.Location = new Point(5,35);
            m_pTabErrors_Errors.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Top;
            m_pTabErrors_Errors.View = View.Details;
            m_pTabErrors_Errors.HideSelection = false;
            m_pTabErrors_Errors.FullRowSelect = true;
            m_pTabErrors_Errors.SmallImageList = errors_imgList;
            m_pTabErrors_Errors.Columns.Add("Error message",440);
            m_pTabErrors_Errors.DoubleClick += new EventHandler(m_pTabErrors_Errors_DoubleClick);
            
            m_pTab.TabPages.Add("Errors");
            m_pTab.TabPages[4].Size = new Size(482,324);
            m_pTab.TabPages[4].Controls.Add(m_pTabErrors_Clear);
            m_pTab.TabPages[4].Controls.Add(m_pTabErrors_Errors);

            #endregion

            #region Log tab

            m_pTabLog_Log = new CheckBox();
            m_pTabLog_Log.Size = new Size(100,20);
            m_pTabLog_Log.Location = new Point(5,10);
            m_pTabLog_Log.Checked = true;
            m_pTabLog_Log.Text = "Log SIP";

            m_pTabLog_Clear = new Button();
            m_pTabLog_Clear.Size = new Size(70,20);
            m_pTabLog_Clear.Location = new Point(405,10);
            m_pTabLog_Clear.Anchor = AnchorStyles.Right | AnchorStyles.Top;
            m_pTabLog_Clear.Text = "Clear";
            m_pTabLog_Clear.Click += new EventHandler(m_pTabLog_Clear_Click);

            Panel m_pTabLog_LogText_border = new Panel();
            m_pTabLog_LogText_border.BorderStyle = BorderStyle.FixedSingle;
            m_pTabLog_LogText_border.Size = new Size(470,280);
            m_pTabLog_LogText_border.Location = new Point(5,35);
            m_pTabLog_LogText_border.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right | AnchorStyles.Top;
            m_pTabLog_LogText = new RichTextBox();            
            m_pTabLog_LogText.Dock = DockStyle.Fill;
            m_pTabLog_LogText.BorderStyle = BorderStyle.None;
            m_pTabLog_LogText_border.Controls.Add(m_pTabLog_LogText);
            
            m_pTab.TabPages.Add("Log");
            m_pTab.TabPages[5].Size = new Size(482,324);
            m_pTab.TabPages[5].Controls.Add(m_pTabLog_Log);
            m_pTab.TabPages[5].Controls.Add(m_pTabLog_Clear);
            m_pTab.TabPages[5].Controls.Add(m_pTabLog_LogText_border);

            #endregion

            m_pServerToolbar = new ToolStrip();
            m_pServerToolbar.Dock = DockStyle.None;
            m_pServerToolbar.Size = new Size(55,20);
            m_pServerToolbar.Location = new Point(350,372);
            m_pServerToolbar.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            m_pServerToolbar.GripStyle = ToolStripGripStyle.Hidden;
            m_pServerToolbar.BackColor = this.BackColor;
            m_pServerToolbar.Renderer = new ToolBarRendererEx();
            m_pServerToolbar.ItemClicked += new ToolStripItemClickedEventHandler(m_pServerToolbar_ItemClicked);
            // Start button
            ToolStripButton button_Start = new ToolStripButton();
            button_Start.Image = ResManager.GetIcon("server_running.ico").ToBitmap();
            button_Start.Tag = "start";
            button_Start.ToolTipText = "Start";
            m_pServerToolbar.Items.Add(button_Start);
            // Stop button
            ToolStripButton button_Stop = new ToolStripButton();
            button_Stop.Enabled = false;
            button_Stop.Image = ResManager.GetIcon("server_stopped.ico").ToBitmap();
            button_Stop.Tag = "stop";
            button_Stop.ToolTipText  = "Stop";
            m_pServerToolbar.Items.Add(button_Stop);

            m_pShutDown = new Button();
            m_pShutDown.Size = new Size(80,20);
            m_pShutDown.Location = new Point(410,370);
            m_pShutDown.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            m_pShutDown.Text = "Shut down";
            m_pShutDown.Click += new EventHandler(m_pShutDown_Click);

            this.Controls.Add(m_pTab);
            this.Controls.Add(m_pServerToolbar);
            this.Controls.Add(m_pShutDown);
        }
                                                                                                                                                                                                                                                                
        #endregion


        #region Events Handling

        #region method m_pTabSettings_ProxyType_SelectedIndexChanged

        private void m_pTabSettings_ProxyType_SelectedIndexChanged(object sender,EventArgs e)
        {
            if(m_pTabSettings_ProxyType.SelectedItem.ToString() == "Stateless"){
                m_pTabSettings_ForkMode.Enabled = false;
            }
            else{
                m_pTabSettings_ForkMode.Enabled = true;
            }
        }

        #endregion

        #region method m_pTabSettings_Toolbar_ItemClicked

        private void m_pTabSettings_Toolbar_ItemClicked(object sender,ToolStripItemClickedEventArgs e)
        {
            if(e.ClickedItem.Tag == null){
            }
            else if(e.ClickedItem.Tag.ToString() == "add"){
                wfrm_User frm = new wfrm_User();
                if(frm.ShowDialog(this) == DialogResult.OK){
                    // Check if specified user exists
                    foreach(UserInfo uInfo in m_pUsers){
                        if(uInfo.UserName.ToLower() == frm.UserName.ToLower()){
                            MessageBox.Show(this,"Specified user '" + uInfo.UserName + "' already exists !","Error:",MessageBoxButtons.OK,MessageBoxIcon.Error);
                            return;
                        }
                    }

                    // Add specified user to local users.
                    UserInfo userInfo = new UserInfo(frm.UserName,frm.Password,frm.AddressOfRecord);
                    m_pUsers.Add(userInfo);

                    // Add specified user to UI.
                    ListViewItem item = new ListViewItem(frm.UserName);
                    item.SubItems.Add(frm.AddressOfRecord);
                    item.Tag = userInfo;
                    m_pTabSettings_Users.Items.Add(item);

                    SaveData();
                }
            }
            else if(e.ClickedItem.Tag.ToString() == "delete"){
                UserInfo userInfo = (UserInfo)m_pTabSettings_Users.SelectedItems[0].Tag;
                if(MessageBox.Show(this,"Are you sure you want to delete user '" + userInfo.UserName + "' ?","Confirm delete:",MessageBoxButtons.YesNo,MessageBoxIcon.Question) == DialogResult.Yes){
                    m_pTabSettings_Users.SelectedItems[0].Remove();
                    m_pUsers.Remove(userInfo);

                    SaveData();
                }
            }
        }

        #endregion

        #region method m_pSettingsTab_Users_SelectedIndexChanged

        private void m_pSettingsTab_Users_SelectedIndexChanged(object sender,EventArgs e)
        {
            if(m_pTabSettings_Users.SelectedItems.Count > 0){
                m_pTabSettings_Toolbar.Items[1].Enabled = true;
            }
            else{
                m_pTabSettings_Toolbar.Items[1].Enabled = false;
            }
        }

        #endregion


        #region method m_pTabRegistrations_Toolbar_ItemClicked

        private void m_pTabRegistrations_Toolbar_ItemClicked(object sender,ToolStripItemClickedEventArgs e)
        {
            if(e.ClickedItem.Tag == null){
                return;
            }
            
            if(e.ClickedItem.Tag.ToString() == "add"){                
                wfrm_SipRegistration frm = new wfrm_SipRegistration((SIP_ProxyCore)m_pSipStack.SipCore);
                if(frm.ShowDialog(this) == DialogResult.OK){
                    LoadRegistrations();
                }
            }
            else if(e.ClickedItem.Tag.ToString() == "delete"){
                SIP_Registration registration = (SIP_Registration)m_pTabRegistrations_Registrations.SelectedItems[0].Tag;
                if(MessageBox.Show(this,"Are you sure you want to remove SIP registration '" + registration.Address + "' ?","Remove Registration",MessageBoxButtons.YesNo,MessageBoxIcon.Question) == DialogResult.Yes){
                    ((SIP_ProxyCore)m_pSipStack.SipCore).Registrar.DeleteRegistration(registration.Address);
                    m_pTabRegistrations_Registrations.SelectedItems[0].Remove();
                }
            }
            else if(e.ClickedItem.Tag.ToString() == "refresh"){
                LoadRegistrations();
            }
            else if(e.ClickedItem.Tag.ToString() == "view"){
                ShowContacts();
            }
        }

        #endregion

        #region method m_pTabRegistrations_Registrations_SelectedIndexChanged

        private void m_pTabRegistrations_Registrations_SelectedIndexChanged(object sender,EventArgs e)
        {
            if(m_pTabRegistrations_Registrations.SelectedItems.Count > 0){
                m_pTabRegistrations_Toolbar.Items[1].Enabled = true;
                m_pTabRegistrations_Toolbar.Items[4].Enabled = true;
            }
            else{
                m_pTabRegistrations_Toolbar.Items[1].Enabled = false;
                m_pTabRegistrations_Toolbar.Items[4].Enabled = false;
            }
        }

        #endregion

        #region method m_pTabRegistrations_Registrations_DoubleClick

        private void m_pTabRegistrations_Registrations_DoubleClick(object sender,EventArgs e)
        {
            ShowContacts();
        }

        #endregion


        #region method m_pTabCalls_Toolbar_ItemClicked

        private void m_pTabCalls_Toolbar_ItemClicked(object sender,ToolStripItemClickedEventArgs e)
        {
            if(e.ClickedItem.Tag == null){
            }
            else if(e.ClickedItem.Tag.ToString() == "delete"){
                if(MessageBox.Show(this,"Are you sure you want to terminate selected call ?","Terminate call",MessageBoxButtons.YesNo,MessageBoxIcon.Question) == DialogResult.Yes){
                    SIP_B2BUA_Call call = (SIP_B2BUA_Call)m_pTabCalls_Calls.SelectedItems[0].Tag;
                    call.Terminate();
                    m_pTabCalls_Calls.SelectedItems[0].Remove();
                }
            }
            else if(e.ClickedItem.Tag.ToString() == "refresh"){
                LoadCalls();
            }
        }

        #endregion

        #region method m_pTabCalls_Calls_SelectedIndexChanged

        private void m_pTabCalls_Calls_SelectedIndexChanged(object sender,EventArgs e)
        {
            if(m_pTabCalls_Calls.SelectedItems.Count > 0){
                m_pTabCalls_Toolbar.Items[0].Enabled = true;
            }
            else{
                m_pTabCalls_Toolbar.Items[0].Enabled = false;
            }
        }

        #endregion


        #region mehtod m_pTabGateways_Toolbar_ItemClicked

        private void m_pTabGateways_Toolbar_ItemClicked(object sender,ToolStripItemClickedEventArgs e)
        {
            if(e.ClickedItem.Tag.ToString() == "add"){
                wfrm_Gateway frm = new wfrm_Gateway();
                if(frm.ShowDialog(this) == DialogResult.OK){
                    DataRow dr = m_pDsSettings.Tables["Gateways"].NewRow();
                    dr["UriScheme"] = frm.UriScheme;
                    dr["Transport"] = frm.Transport;
                    dr["Host"]      = frm.Host;
                    dr["Port"]      = frm.Port;
                    dr["Realm"]     = frm.Realm;
                    dr["UserName"]  = frm.UserName;
                    dr["Password"]  = frm.Password;
                    m_pDsSettings.Tables["Gateways"].Rows.Add(dr);

                    ListViewItem item = new ListViewItem(dr["UriScheme"].ToString());
                    item.SubItems.Add(dr["Transport"].ToString());
                    item.SubItems.Add(dr["Host"].ToString());
                    item.SubItems.Add(dr["Port"].ToString());
                    item.Tag = dr;
                    m_pTabGateways_Gateways.Items.Add(item);

                    SaveData();
                }
            }
            else if(e.ClickedItem.Tag.ToString() == "edit"){
                DataRow dr = (DataRow)m_pTabGateways_Gateways.SelectedItems[0].Tag;
                wfrm_Gateway frm = new wfrm_Gateway(
                    dr["UriScheme"].ToString(),
                    dr["Transport"].ToString(),
                    dr["Host"].ToString(),
                    Convert.ToInt32(dr["Port"]),
                    dr["Realm"].ToString(),
                    dr["UserName"].ToString(),
                    dr["Password"].ToString()
                );
                if(frm.ShowDialog(this) == DialogResult.OK){
                    dr["UriScheme"] = frm.UriScheme;
                    dr["Transport"] = frm.Transport;
                    dr["Host"]      = frm.Host;
                    dr["Port"]      = frm.Port;
                    dr["Realm"]     = frm.Realm;
                    dr["UserName"]  = frm.UserName;
                    dr["Password"]  = frm.Password;

                    m_pTabGateways_Gateways.SelectedItems[0].SubItems[0].Text = frm.UriScheme;
                    m_pTabGateways_Gateways.SelectedItems[0].SubItems[0].Text = frm.Transport;
                    m_pTabGateways_Gateways.SelectedItems[0].SubItems[0].Text = frm.Host;
                    m_pTabGateways_Gateways.SelectedItems[0].SubItems[0].Text = frm.Port.ToString();

                    SaveData();
                }
            }
            else if(e.ClickedItem.Tag.ToString() == "delete"){
                DataRow dr = (DataRow)m_pTabGateways_Gateways.SelectedItems[0].Tag;
                if(MessageBox.Show(this,"Are you sure you want to remove SIP selected gateway ?","Remove Gateway",MessageBoxButtons.YesNo,MessageBoxIcon.Question) == DialogResult.Yes){
                    dr.Delete();
                    m_pTabGateways_Gateways.SelectedItems[0].Remove();

                    SaveData();
                }
            }
        }

        #endregion

        #region method m_pTabGateways_Gateways_SelectedIndexChanged

        private void m_pTabGateways_Gateways_SelectedIndexChanged(object sender,EventArgs e)
        {
            if(m_pTabGateways_Gateways.SelectedItems.Count > 0){
                m_pTabGateways_Toolbar.Items[1].Enabled = true;
                m_pTabGateways_Toolbar.Items[2].Enabled = true;
            }
            else{
                m_pTabGateways_Toolbar.Items[1].Enabled = false;
                m_pTabGateways_Toolbar.Items[2].Enabled = false;
            }
        }

        #endregion


        #region method m_pTabErrors_Clear_Click

        private void m_pTabErrors_Clear_Click(object sender,EventArgs e)
        {
            m_pTabErrors_Errors.Items.Clear();
        }

        #endregion

        #region method m_pTabErrors_Errors_DoubleClick

        private void m_pTabErrors_Errors_DoubleClick(object sender,EventArgs e)
        {
            if(m_pTabErrors_Errors.SelectedItems.Count > 0){
                Form frm = new Form();
                frm.Size = new Size(600,400);
                frm.StartPosition = FormStartPosition.CenterScreen;
                frm.Text = "Error:";
                frm.Icon = ResManager.GetIcon("error.ico");
                TextBox errorText = new TextBox();
                errorText.Multiline = true;
                errorText.ReadOnly = true;
                errorText.Dock = DockStyle.Fill;
                errorText.Text = m_pTabErrors_Errors.SelectedItems[0].Tag.ToString();
                frm.Controls.Add(errorText);
                frm.ShowDialog(this);
            }
        }

        #endregion


        #region method m_pTabLog_Clear_Click

        private void m_pTabLog_Clear_Click(object sender,EventArgs e)
        {
            m_pTabLog_LogText.Text = "";
        }

        #endregion


        #region method m_pServerToolbar_ItemClicked

        private void m_pServerToolbar_ItemClicked(object sender,ToolStripItemClickedEventArgs e)
        {
            if(e.ClickedItem.Tag == null){
            }
            else if(e.ClickedItem.Tag.ToString() == "start"){
                m_pServerToolbar.Items[0].Enabled = false;
                m_pServerToolbar.Items[1].Enabled = true;

                m_pSipStack.HostName = m_pTabSettings_HostName.Text;
                m_pSipStack.MinimumExpireTime = 30;
                BindInfo[] bindInfo = new BindInfo[]{
                    new BindInfo(BindInfoProtocol.UDP,IPAddress.Any,(int)m_pTabSettings_Port.Value),
                    new BindInfo(BindInfoProtocol.TCP,IPAddress.Any,(int)m_pTabSettings_Port.Value)
                };
                m_pSipStack.BindInfo = bindInfo;
                if(m_pTabSettings_ProxyType.SelectedItem.ToString() == "Statefull"){
                    ((SIP_ProxyCore)m_pSipStack.SipCore).ProxyMode = SIP_ProxyMode.Registrar | SIP_ProxyMode.Statefull;
                }
                else if(m_pTabSettings_ProxyType.SelectedItem.ToString() == "Stateless"){
                    ((SIP_ProxyCore)m_pSipStack.SipCore).ProxyMode = SIP_ProxyMode.Registrar | SIP_ProxyMode.Stateless;
                }
                else if(m_pTabSettings_ProxyType.SelectedItem.ToString() == "B2BUA"){
                    ((SIP_ProxyCore)m_pSipStack.SipCore).ProxyMode = SIP_ProxyMode.Registrar | SIP_ProxyMode.B2BUA;
                }
                if(m_pTabSettings_ForkMode.SelectedItem.ToString() == "None"){
                    ((SIP_ProxyCore)m_pSipStack.SipCore).ForkingMode = SIP_ForkingMode.None;
                }
                else if(m_pTabSettings_ForkMode.SelectedItem.ToString() == "Parallel"){
                    ((SIP_ProxyCore)m_pSipStack.SipCore).ForkingMode = SIP_ForkingMode.Parallel;
                }
                else if(m_pTabSettings_ForkMode.SelectedItem.ToString() == "Sequential"){
                    ((SIP_ProxyCore)m_pSipStack.SipCore).ForkingMode = SIP_ForkingMode.Sequential;
                }
                m_pSipStack.Start();
            }
            else if(e.ClickedItem.Tag.ToString() == "stop"){
                m_pServerToolbar.Items[0].Enabled = true;
                m_pServerToolbar.Items[1].Enabled = false;

                m_pSipStack.Stop();
            }
        }

        #endregion

        #region method m_pShutDown_Click

        private void m_pShutDown_Click(object sender,EventArgs e)
        {
            ShutDown();
            this.Close();
        }

        #endregion

        #region method wfrm_Main_FormClosed

        private void wfrm_Main_FormClosed(object sender,FormClosedEventArgs e)
        {
            ShutDown();
        }

        #endregion


        #region method SIP_AddressExists

        /// <summary>
        /// This method is called by SIP server if it needs to decide if specified address exists.
        /// </summary>
        /// <param name="address">Address of record.</param>
        /// <returns></returns>
        private bool SIP_AddressExists(string address)
        {
            lock(m_pUsers){
                // Search for specified address.
                foreach(UserInfo userInfo in m_pUsers){
                    // Wanted user, do uth.
                    if(userInfo.AddressOfRecord.ToLower() == address.ToLower()){
                        return true;
                    }
                }
            }

            return false;
        }

        #endregion

        #region method SIP_Authenticate

        /// <summary>
        /// This method is called when SIP proxy server gets authentication request.
        /// </summary>
        /// <param name="e">Event data.</param>
        private void SIP_Authenticate(SIP_AuthenticateEventArgs e)
        {
            lock(m_pUsers){
                // Search for specified user.
                foreach(UserInfo userInfo in m_pUsers){
                    // Wanted user, do uth.
                    if(userInfo.UserName == e.AuthContext.UserName){
                        e.Authenticated = e.AuthContext.Authenticate(userInfo.UserName,userInfo.Password);
                        return;
                    }
                }
            }
        }

        #endregion

        #region method SIP_CanRegister

        /// <summary>
        /// This method is called by SIP proxy server when server needs to know if specified user can register specified address.
        /// </summary>
        /// <param name="userName">User name.</param>
        /// <param name="address">Address or record.</param>
        /// <returns></returns>
        private bool SIP_CanRegister(string userName,string address)
        {
            lock(m_pUsers){
                // Search for specified user.
                foreach(UserInfo userInfo in m_pUsers){
                    // Wanted user, do uth.
                    if(userInfo.UserName == userName){
                        if(userInfo.AddressOfRecord.ToLower() == address.ToLower()){
                            return true;
                        }
                        else{
                            return false;
                        }
                    }
                }
            }

            return false;
        }

        #endregion

        #region method SIP_IsLocalUri

        /// <summary>
        /// This method is called by SIP proxy server when server needs to know if specified request URI is 
        /// local or remote URI.
        /// </summary>
        /// <param name="uri">Request URI.</param>
        /// <returns>Returns true if URI is local, othrwise false.</returns>
        private bool SIP_IsLocalUri(string uri)
        {
            lock(m_pUsers){
                foreach(UserInfo userInfo in m_pUsers){
                    if(userInfo.AddressOfRecord.IndexOf('@') > -1){
                        string d = userInfo.AddressOfRecord.Split('@')[1];
                        if(d.ToLower() == uri.ToLower()){
                            return true;
                        }
                    }
                }
            }

            return false;
        }

        #endregion

        #region method wfrm_Main_GetGateways

        /// <summary>
        /// This method is called by SIP proxy server when server needs to get specified 
        /// URI scheme gateways.
        /// </summary>
        /// <param name="e">Event data.</param>
        private void SIP_GetGateways(SIP_GatewayEventArgs e)
        {
            foreach(DataRow dr in m_pDsSettings.Tables["Gateways"].Rows){
                if(dr["UriScheme"].ToString().ToLower() == e.UriScheme.ToLower()){
                    e.Gateways.Add( new SIP_Gateway(
                        dr["Transport"].ToString(),
                        dr["Host"].ToString(),
                        Convert.ToInt32(dr["Port"]),
                        dr["Realm"].ToString(),
                        dr["UserName"].ToString(),
                        dr["Password"].ToString()
                    ));
                }
            }
        }

        #endregion

        #region method m_pSipStack_Error

        /// <summary>
        /// Is called when SIP stack get unknown error.
        /// </summary>
        /// <param name="x">Excepion happened.</param>
        private void m_pSipStack_Error(Exception x)
        {
            m_pTabErrors_Errors.Invoke(new AddError(this.AddErrorOnUIThread),new object[]{x});
        }

        private void AddErrorOnUIThread(Exception x)
        {
            ListViewItem item = new ListViewItem(x.Message);
            item.ImageIndex = 0;
            item.Tag = x;
            m_pTabErrors_Errors.Items.Add(item);
        }

        #endregion

        #region method Logger_WriteLog

        /// <summary>
        /// Is called from SIP stack if log is available.
        /// </summary>
        /// <param name="entries"></param>
        private void Logger_WriteLog(LogEntry[] entries)
        {
            m_pTabLog_LogText.Invoke(new AddLog(this.AddLogOnUIThread),new object[]{entries});
        }

        private void AddLogOnUIThread(MkrSoft.Net.Log.LogEntry[] entries)
        {
            if(m_pTabLog_Log.Checked && entries.Length > 0 && entries[0].Text != null){
                string text = entries[0].Text + "\n\n";
               
                if(m_OddLogEntry){
                    m_OddLogEntry = false;
                    m_pTabLog_LogText.SelectionColor = Color.Gray;                    
                }
                else{
                    m_OddLogEntry = true;
                    m_pTabLog_LogText.SelectionColor = Color.LightSeaGreen;
                }

                m_pTabLog_LogText.AppendText(text);
            }
        }

        #endregion

        #endregion


        #region method LoadRegistrations

        /// <summary>
        /// Loads current registrations to UI.
        /// </summary>
        private void LoadRegistrations()
        {
            m_pTabRegistrations_Registrations.Items.Clear();

            SIP_ProxyCore proxy = (SIP_ProxyCore)m_pSipStack.SipCore;
            foreach(SIP_Registration registration in proxy.Registrar.Registrations){
                ListViewItem item = new ListViewItem(registration.UserName);
                item.SubItems.Add(registration.Address);
                item.Tag = registration;
                m_pTabRegistrations_Registrations.Items.Add(item);
            }

            m_pTabRegistrations_Registrations_SelectedIndexChanged(null,null);
        }

        #endregion

        #region method LoadCalls

        /// <summary>
        /// Loads active calls to UI.
        /// </summary>
        private void LoadCalls()
        {
            m_pTabCalls_Calls.Items.Clear();
            foreach(SIP_B2BUA_Call call in ((SIP_ProxyCore)m_pSipStack.SipCore).B2BUA.Calls){
                ListViewItem item = new ListViewItem(call.CallerDialog.RemoteUri);
                item.SubItems.Add(call.CallerDialog.LocalUri);
                item.SubItems.Add(call.StartTime.ToString("HH:mm:ss"));
                item.Tag = call;
                m_pTabCalls_Calls.Items.Add(item);
            }
        }

        #endregion

        #region method ShowContacts

        /// <summary>
        /// Shows selected address contacts window.
        /// </summary>
        private void ShowContacts()
        {
            if(m_pTabRegistrations_Registrations.SelectedItems.Count > 0){
                SIP_Registration registration = (SIP_Registration)m_pTabRegistrations_Registrations.SelectedItems[0].Tag;
                wfrm_SipRegistration frm = new wfrm_SipRegistration((SIP_ProxyCore)m_pSipStack.SipCore,registration);
                frm.ShowDialog(this);
            }
        }

        #endregion

        #region method ShutDown

        /// <summary>
        /// Shuts down SIP server.
        /// </summary>
        private void ShutDown()
        {
            SaveData();

            if(m_pSipStack != null){
                m_pSipStack.Dispose();
                m_pSipStack = null;
            }
        }

        #endregion

        #region method LoadData

        /// <summary>
        /// Loads configuration data.
        /// </summary>
        private void LoadData()
        {
            m_pDsSettings = new DataSet("dsSettings"); 
            m_pDsSettings.Tables.Add("SIP_Settings");
            m_pDsSettings.Tables["SIP_Settings"].Columns.Add("ProxyType");
            m_pDsSettings.Tables["SIP_Settings"].Columns.Add("ForkMode");
            m_pDsSettings.Tables["SIP_Settings"].Columns.Add("HostName");
            m_pDsSettings.Tables["SIP_Settings"].Columns.Add("Port");
            m_pDsSettings.Tables.Add("Users");
            m_pDsSettings.Tables["Users"].Columns.Add("UserName");
            m_pDsSettings.Tables["Users"].Columns.Add("Password");
            m_pDsSettings.Tables["Users"].Columns.Add("AddressOfRecord");
            m_pDsSettings.Tables.Add("Gateways");
            m_pDsSettings.Tables["Gateways"].Columns.Add("UriScheme");
            m_pDsSettings.Tables["Gateways"].Columns.Add("Transport");
            m_pDsSettings.Tables["Gateways"].Columns.Add("Host");
            m_pDsSettings.Tables["Gateways"].Columns.Add("Port");
            m_pDsSettings.Tables["Gateways"].Columns.Add("Realm");
            m_pDsSettings.Tables["Gateways"].Columns.Add("UserName");
            m_pDsSettings.Tables["Gateways"].Columns.Add("Password");

            if(File.Exists(Application.StartupPath + "/SIP-Proxy-Settings.xml")){
                m_pDsSettings.ReadXml(Application.StartupPath + "/SIP-Proxy-Settings.xml");
            }

            // Load SIP settings
            if(m_pDsSettings.Tables["SIP_Settings"].Rows.Count > 0){
                DataRow dr = m_pDsSettings.Tables["SIP_Settings"].Rows[0];

                if(dr["ProxyType"].ToString().ToLower() == "statefull"){
                    m_pTabSettings_ProxyType.SelectedIndex = 0;
                }
                else if(dr["ProxyType"].ToString().ToLower() == "stateless"){
                    m_pTabSettings_ProxyType.SelectedIndex = 1;
                }
                else if(dr["ProxyType"].ToString().ToLower() == "b2bua"){
                    m_pTabSettings_ProxyType.SelectedIndex = 2;
                }
                if(dr["ForkMode"].ToString().ToLower() == "none"){
                    m_pTabSettings_ForkMode.SelectedIndex = 0;
                }
                else if(dr["ForkMode"].ToString().ToLower() == "sequential"){
                    m_pTabSettings_ForkMode.SelectedIndex = 1;
                }
                else if(dr["ForkMode"].ToString().ToLower() == "parallel"){
                    m_pTabSettings_ForkMode.SelectedIndex = 2;
                }
                m_pTabSettings_HostName.Text = dr["HostName"].ToString();
                m_pTabSettings_Port.Value = Convert.ToDecimal(dr["Port"]);
            }

            // Load users
            foreach(DataRow dr in m_pDsSettings.Tables["Users"].Rows){
                // Add specified user to local users.
                UserInfo userInfo = new UserInfo(dr["UserName"].ToString(),dr["Password"].ToString(),dr["AddressOfRecord"].ToString());
                m_pUsers.Add(userInfo);

                // Add specified user to UI.
                ListViewItem item = new ListViewItem(userInfo.UserName);
                item.SubItems.Add(userInfo.AddressOfRecord);
                item.Tag = userInfo;
                m_pTabSettings_Users.Items.Add(item);
            }

            // Load gateways
            foreach(DataRow dr in m_pDsSettings.Tables["Gateways"].Rows){
                // Add specified gateway to UI.
                ListViewItem item = new ListViewItem(dr["UriScheme"].ToString());
                item.SubItems.Add(dr["Transport"].ToString());
                item.SubItems.Add(dr["Host"].ToString());
                item.SubItems.Add(dr["Port"].ToString());
                item.Tag = dr;
                m_pTabGateways_Gateways.Items.Add(item);
            }
            m_pTabGateways_Gateways_SelectedIndexChanged(null,null);
        }

        #endregion

        #region method SaveData

        /// <summary>
        /// Saves configuration data.
        /// </summary>
        private void SaveData()
        {
            // Save SIP settings
            DataRow drSipSettings = m_pDsSettings.Tables["SIP_Settings"].Rows[0];
            drSipSettings["ProxyType"] = m_pTabSettings_ProxyType.SelectedItem.ToString();
            drSipSettings["ForkMode"]  = m_pTabSettings_ForkMode.SelectedItem.ToString();
            drSipSettings["HostName"]  = m_pTabSettings_HostName.Text;
            drSipSettings["Port"]      = m_pTabSettings_Port.Value;

            // Save users
            m_pDsSettings.Tables["Users"].Clear();
            foreach(UserInfo userInfo in m_pUsers){
                DataRow dr = m_pDsSettings.Tables["Users"].NewRow();
                dr["UserName"]        = userInfo.UserName;
                dr["Password"]        = userInfo.Password;
                dr["AddressOfRecord"] = userInfo.AddressOfRecord;
                m_pDsSettings.Tables["Users"].Rows.Add(dr);
            }

            m_pDsSettings.WriteXml(Application.StartupPath + "/SIP-Proxy-Settings.xml");
        }

        #endregion

    }
}
