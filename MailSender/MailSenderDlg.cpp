
// MailSenderDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MailSender.h"
#include "MailSenderDlg.h"
#include "afxdialogex.h"

#include "CSmtp.h"
#include "socket/dnsmx.h"
#include <iostream>
#include "string/String.h"
#include <thread>
#include "socket/base64.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMailSenderDlg 对话框



CMailSenderDlg::CMailSenderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MAILSENDER_DIALOG, pParent)
	, m_strFrom(_T(""))
	, m_strTo(_T(""))
	, m_strReply(_T(""))
	, m_strProxy(_T(""))
	, m_strTitle(_T(""))
	, m_strContent(_T(""))
	, m_bUseCurrentTime(TRUE)
	, m_strFromServer(_T(""))
	, m_strMailer(_T(""))
	, m_strMessageId(_T(""))
	, m_strCDK(_T(""))
	, m_strBigContent(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMailSenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FROM, m_strFrom);
	DDX_Text(pDX, IDC_EDIT_TO, m_strTo);
	DDX_Text(pDX, IDC_EDIT_REPLY, m_strReply);
	DDX_Text(pDX, IDC_EDIT_PROXY, m_strProxy);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_strTitle);
	DDX_Text(pDX, IDC_EDIT_CONTENT, m_strContent);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_btnSend);
	DDX_Control(pDX, IDC_DATETIMEPICKER, m_ctrlDate);
	DDX_Check(pDX, IDC_CHECK_USE_CURRENT_TIME, m_bUseCurrentTime);
	DDX_Text(pDX, IDC_EDIT_FROM_SERVER, m_strFromServer);
	DDX_Text(pDX, IDC_EDIT_MAILER, m_strMailer);
	DDX_Text(pDX, IDC_EDIT_MESSAGE_ID, m_strMessageId);
	DDX_Text(pDX, IDC_EDIT_CDK, m_strCDK);
	DDX_Text(pDX, IDC_RICHEDIT_CONTENT, m_strBigContent);
}

BEGIN_MESSAGE_MAP(CMailSenderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMailSenderDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMailSenderDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CMailSenderDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// CMailSenderDlg 消息处理程序

BOOL CMailSenderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	m_ctrlDate.SetFormat("yyy-MM-dd HH:mm:ss");

	GetPrivateProfileString("email", "from", "", m_strFrom.GetBuffer(256), 256, "./config.ini");
	GetPrivateProfileString("email", "fromServer", "", m_strFromServer.GetBuffer(256), 256, "./config.ini");
	GetPrivateProfileString("email", "to", "", m_strTo.GetBuffer(256), 256, "./config.ini");
	//GetPrivateProfileString("email", "reply", "", m_strReply.GetBuffer(256), 256, "./config.ini");
	GetPrivateProfileString("email", "proxy", "", m_strProxy.GetBuffer(256), 256, "./config.ini");
	GetPrivateProfileString("email", "cdk", "", m_strCDK.GetBuffer(256), 256, "./config.ini");
	//GetPrivateProfileString("email", "mailer", "", m_strMailer.GetBuffer(256), 256, "./config.ini");
	GetPrivateProfileString("email", "messageId", "", m_strMessageId.GetBuffer(256), 256, "./config.ini");
	m_bUseCurrentTime = GetPrivateProfileInt("email", "useCurrentTime", 1, "./config.ini");
	GetPrivateProfileString("email", "title", "", m_strTitle.GetBuffer(256), 256, "./config.ini");
	GetPrivateProfileString("email", "content", "", m_strBigContent.GetBuffer(1024000), 1024000, "./config.ini");
	m_strBigContent = base64_decode(m_strBigContent.GetBuffer(0)).c_str();
	
	if (m_strFromServer.IsEmpty())
		m_strFromServer = "smtp.163.com";
	//if (m_strMailer.IsEmpty())
	//	m_strMailer = "Foxmail (v1.02) Professional";
	if (m_strFrom.IsEmpty() || m_strFrom.GetLength() < 3)
		m_strFrom = "张三<888888@126.com>";
	if (m_strTo.IsEmpty() || m_strTo.GetLength() < 3)
		m_strTo = "李四<666666@qq.com>";
	UpdateData(FALSE);

	((CRichEditCtrl*)GetDlgItem(IDC_RICHEDIT_CONTENT))->LimitText(-1);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMailSenderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMailSenderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMailSenderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMailSenderDlg::OnBnClickedOk()
{
	//CDialogEx::OnOK();
}


void CMailSenderDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

#pragma comment(lib,"Iphlpapi.lib")   
#pragma comment(lib,"DnsAPI.Lib")   
#pragma comment(lib,"WS2_32.Lib")

std::string getMX(const std::string host)
{
	std::string ret;
	DNS_RECORD* pQueryResultsSet;
	DNS_STATUS statusDNS;
	statusDNS = ::DnsQuery("qq.com", DNS_TYPE_MX, DNS_QUERY_STANDARD, NULL, &pQueryResultsSet, NULL);
	if (statusDNS == ERROR_SUCCESS)
	{
		char* MXDomain = pQueryResultsSet->Data.MX.pNameExchange;
		if (MXDomain)
		{
			struct hostent* tmpip = gethostbyname(MXDomain);
			if (tmpip)
			{
				struct sockaddr_in tmpAddr;
				tmpAddr.sin_addr = *((LPIN_ADDR)*tmpip->h_addr_list);
				char* MXIp = inet_ntoa(tmpAddr.sin_addr);
				if (MXIp)
					ret.append(MXIp);
			}
		}
	}

	return ret;
}

void CMailSenderDlg::OnBnClickedButtonSend()
{
	//m_strFrom = "abc<abc@luo.com>";
	//m_strTo = "915921500@qq.com";
	//m_strProxy = "lfq:123@lfq1989-dianxin.ticp.net:1080";
	//m_strTitle = "title";
	//m_strContent = "content";
	//m_bUseCurrentTime = FALSE;
	//UpdateData(FALSE);

	UpdateData(TRUE);
	if (m_strFrom.IsEmpty())
	{
		AfxMessageBox("请先填写发件箱", MB_OK | MB_ICONERROR);
		return;
	}

	if (m_strTo.IsEmpty())
	{
		AfxMessageBox("请先填写收件箱", MB_OK | MB_ICONERROR);
		return;
	}

	//if (m_strReply.IsEmpty())
	//	m_strReply = m_strFrom;

	if (m_strTitle.IsEmpty())
	{
		AfxMessageBox("请先填写标题", MB_OK | MB_ICONERROR);
		return;
	}

	if (m_strBigContent.IsEmpty())
	{
		AfxMessageBox("请先填写内容", MB_OK | MB_ICONERROR);
		return;
	}
	
	std::string proxyHost, proxyUsername, proxyPassword;
	std::string fromName;
	unsigned short proxyPort;
	if (!m_strProxy.IsEmpty())
	{
		CTString tstring;
		std::vector<std::string> ret = tstring.split(m_strProxy.GetBuffer(0), "@");
		std::string strUsernamePassword, strHostPort;
		if (ret.size() == 1)
			strHostPort = m_strProxy.GetBuffer(0);
		else if (ret.size() == 2)
		{
			strUsernamePassword = ret[0];
			strHostPort = ret[1];
		}
		else
		{
			AfxMessageBox("填写的代理格式错误", MB_OK | MB_ICONERROR);
			return;
		}

		ret = tstring.split(strHostPort, ":");
		if (ret.size() != 2)
		{
			AfxMessageBox("填写的代理格式错误", MB_OK | MB_ICONERROR);
			return;
		}
		proxyHost = ret[0];
		proxyPort = atoi(ret[1].c_str());

		if (strUsernamePassword.length())
		{
			ret = tstring.split(strUsernamePassword, ":");
			if (ret.size() != 2)
			{
				AfxMessageBox("填写的代理格式错误", MB_OK | MB_ICONERROR);
				return;
			}

			proxyUsername = ret[0];
			proxyPassword = ret[1];
		}

		if (proxyPort == 0)
		{
			if (ret.size() != 2)
			{
				AfxMessageBox("填写的代理格式错误", MB_OK | MB_ICONERROR);
				return;
			}
		}
	}

	if (m_strCDK.IsEmpty())
	{
		AfxMessageBox("cdk不能为空", MB_OK | MB_ICONERROR);
		return;
	}

	if (!checkCanSend(m_strCDK))
	{
		//AfxMessageBox("cdk不正确或者使用次数已用完", MB_OK | MB_ICONERROR);
		//return;
	}

	if (m_strFromServer.IsEmpty())
		m_strFromServer = "smtp.163.com";
	//if (m_strMailer.IsEmpty())
	//	m_strMailer = "Foxmail (v1.02) Professional";
	m_strReply = m_strFrom;

	WritePrivateProfileString("email", "from", m_strFrom, "./config.ini");
	WritePrivateProfileString("email", "fromServer", m_strFromServer, "./config.ini");
	WritePrivateProfileString("email", "to", m_strTo, "./config.ini");
	//WritePrivateProfileString("email", "reply", m_strReply, "./config.ini");
	WritePrivateProfileString("email", "proxy", m_strProxy, "./config.ini");
	WritePrivateProfileString("email", "cdk", m_strCDK, "./config.ini");
	//WritePrivateProfileString("email", "mailer", m_strMailer, "./config.ini");
	WritePrivateProfileString("email", "messageId", m_strMessageId, "./config.ini");
	WritePrivateProfileString("email", "useCurrentTime", m_bUseCurrentTime ? "1" : "0", "./config.ini");
	WritePrivateProfileString("email", "title", m_strTitle, "./config.ini");
	WritePrivateProfileString("email", "reply", m_strReply, "./config.ini");
	WritePrivateProfileString("email", "content", base64_encode((const unsigned char *)m_strBigContent.GetBuffer(0), m_strBigContent.GetLength()).c_str(), "./config.ini");

	int pos = m_strFrom.Find("<");
	if (pos > 0)
	{
		fromName = m_strFrom.Left(pos);
		m_strFrom = m_strFrom.Mid(pos + 1, m_strFrom.GetLength() - pos - 2);
		m_strFrom.Trim();
	}

	pos = m_strTo.Find("<");
	if (pos > 0)
	{
		m_strTo = m_strTo.Mid(pos + 1, m_strTo.GetLength() - pos - 2);
		m_strTo.Trim();
	}

	/*CRect rcClient;
	GetClientRect(rcClient);
	m_btnSend.GetWindowRect(m_rcOldSendButton);
	ScreenToClient(m_rcOldSendButton);
	m_btnSend.MoveWindow(rcClient);
	::SetWindowPos(HWND_TOPMOST, m_btnSend.GetSafeHwnd(), 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	m_btnSend.EnableWindow(FALSE);
	*/
	m_btnSend.SetWindowText("正在发送中...");
	m_btnSend.EnableWindow(FALSE);
	std::thread tMain([this, fromName, proxyHost, proxyUsername, proxyPassword, proxyPort]()
	{
		bool bError = false;

		CTString tstring;
		std::vector<std::string> ret = tstring.split(m_strTo.GetBuffer(0), "@");
		if (ret.size() != 2)
		{
			AfxMessageBox("收件箱地址错误", MB_OK | MB_ICONERROR);
			return;
		}

		std::string csReceiverServer = ret[1];
		std::string mxip = getMX(csReceiverServer);
		if (!mxip.length())
		{
			AfxMessageBox("解析邮箱服务器失败", MB_OK | MB_ICONERROR);
			return;
		}

		try
		{
			CSmtp mail;
			mail.SetSMTPServer(mxip.c_str(), 25, false);
			if (proxyHost.length())
				mail.setProxy(proxyHost.c_str(), proxyPort, proxyUsername.c_str(), proxyPassword.c_str());
			//mail.SetSecurityType(USE_SSL);
			if (fromName.length())
				mail.SetSenderName(fromName.c_str());
			mail.SetLocalHostName(m_strFromServer);
			mail.setMessageId(m_strMessageId);
			mail.SetSenderMail(m_strFrom);
			mail.SetReplyTo(m_strFrom);
			mail.SetSubject(m_strTitle);
			mail.AddRecipient(m_strTo);
			mail.SetXPriority(XPRIORITY_NORMAL);
			mail.m_bHTML = true;

			//mail.SetXMailer(m_strMailer);
			int length = m_strBigContent.GetLength();
			int pos = 0;
			int perSize = BUFFER_SIZE - 3;
			while (length > 0)
			{
				mail.AddMsgLine(m_strBigContent.Mid(pos, perSize));
				length -= perSize;
				pos += perSize;
			}
			
			if (!m_bUseCurrentTime)
			{
				CTime time;
				m_ctrlDate.GetTime(time);
				mail.setSendTime(time.GetTime());
			}
			mail.Send();
		}
		catch (ECSmtp e)
		{
			bError = true;
			AfxMessageBox(e.GetErrorText().c_str(), MB_OK | MB_ICONERROR);
		}

		if (!bError)
		{
			int num = commitSendOk(m_strCDK);
			CString msg;
			msg.Format("发送成功, 剩余可发送次数 %d", num < 0 ? 0 : num);
			AfxMessageBox(msg, MB_OK);
		}

		m_btnSend.EnableWindow(TRUE);
		//m_btnSend.MoveWindow(m_rcOldSendButton);
		m_btnSend.SetWindowText("发送");
	});

	tMain.detach();
}

#include <iostream>  
#include <map>  
#include <string>  
#include <memory>  

#include "mysql_driver.h"  
#include "mysql_connection.h"  
#include "cppconn/driver.h"  
#include "cppconn/statement.h"  
#include "cppconn/prepared_statement.h"  
#include "cppconn/metadata.h"  
#include "cppconn/exception.h"  
using namespace std;
using namespace sql;

bool CMailSenderDlg::checkCanSend(const char* cdk)
{
	mysql::MySQL_Driver *driver = 0;
	sql::Connection *con = 0;
	Statement *stmt = 0;
	ResultSet *res = 0;
	PreparedStatement *prep_stmt = 0;
	char sql[256] = { 0 };
	bool bOk = false;

	try {
		driver = sql::mysql::get_driver_instance();
		con = driver->connect("", "", "");
		con->setAutoCommit(0);
		con->setSchema("weimail");
		stmt = con->createStatement();
		sql[sprintf(sql, "SELECT num FROM bs_email WHERE cdk = '%s'", cdk)] = 0;
		res = stmt->executeQuery(sql);
		if (res && res->next())
		{
			int num = res->getInt(1);
			if (num > 0)
				bOk = true;
		}
	}
	catch (sql::SQLException &)
	{

	}
	catch (std::bad_alloc &)
	{

	}
	catch (std::runtime_error &)
	{

	}

	if (con)
	{
		con->close();
		delete con;
	}
	if (res)
		delete res;
	if (stmt)
		delete stmt;
	if (prep_stmt)
		delete prep_stmt;

	return bOk;
}

int CMailSenderDlg::commitSendOk(const char* cdk)
{
	mysql::MySQL_Driver *driver = 0;
	sql::Connection *con = 0;
	Statement *stmt = 0;
	ResultSet *res = 0;
	PreparedStatement *prep_stmt = 0;
	char sql[256] = { 0 };
	bool bOk = false;
	int num = 0;

	try {
		driver = sql::mysql::get_driver_instance();
		con = driver->connect("", "", "");
		con->setAutoCommit(0);
		con->setSchema("weimail");
		sql[sprintf(sql, "UPDATE bs_email SET num = num - 1 WHERE cdk = '%s'", cdk)] = 0;
		prep_stmt = con->prepareStatement(sql);
		int updatecount = prep_stmt->executeUpdate();
		con->commit();
		if (updatecount >= 1)
		{
			bOk = true;
			stmt = con->createStatement();
			sql[sprintf(sql, "SELECT num FROM bs_email WHERE cdk = '%s'", cdk)] = 0;
			res = stmt->executeQuery(sql);
			if (res && res->next())
			{
				num = res->getInt(1);
			}
		}
			
	}
	catch (sql::SQLException &)
	{
	}
	catch (std::bad_alloc &)
	{

	}
	catch (std::runtime_error &)
	{

	}

	if (con)
	{
		con->close();
		delete con;
	}
	if (res)
		delete res;
	if (stmt)
		delete stmt;
	if (prep_stmt)
		delete prep_stmt;

	return num;
}