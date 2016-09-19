
// MailSenderDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxdtctl.h"

// CMailSenderDlg 对话框
class CMailSenderDlg : public CDialogEx
{
// 构造
public:
	CMailSenderDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAILSENDER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSend();

private:
	bool checkCanSend(const char* cdk);
	int commitSendOk(const char* cdk);
	
private:
	CRect m_rcOldSendButton;
	CString m_strFrom;
	CString m_strTo;
	CString m_strReply;
	CString m_strProxy;
	CString m_strTitle;
	CString m_strContent;
	CButton m_btnSend;

public:
	CDateTimeCtrl m_ctrlDate;
private:
	BOOL m_bUseCurrentTime;
public:
	CString m_strFromServer;
	CString m_strMailer;
	CString m_strMessageId;
	CString m_strCDK;
	CString m_strBigContent;
};
