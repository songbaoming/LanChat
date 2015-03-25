
// MainWnd.h : 头文件
//

#pragma once
#include "Resource.h"
#include "MyUdp.h"

class CMsgDlg;

// CMainWnd 对话框
class CMainWnd : public CDialogEx
{
// 构造
public:
	CMainWnd(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	//char *m_pBuf;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strName;
	CMyUdp m_udp;
	UINT m_nLocalPort;
	UINT m_nPeerPort;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedSeting();
	
	void NewMsg(MyData *pdata, int nMsgLen, SOCKADDR_IN * pnewaddr);
	void NewMember(LPCTSTR name, SOCKADDR_IN * pnewaddr);
	void SendMyInfo(Operate ope, SOCKADDR_IN *paddr);
	void EnterOrQuit(Operate ope);
	afx_msg void OnDestroy();
	void SomeoneQuit(LPCTSTR name, SOCKADDR_IN * paddr);
	afx_msg void OnDblclkMemlist();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SendNewMsg(CString &msg, SOCKADDR_IN * lpSockAddr, WORD wNum);
	
	void ShowNewMsg(LPCTSTR lpszMsg, SOCKADDR_IN * lpFromSockAddr, bool bIsSysMsg=false);
	CMsgDlg* GetMsgDlg(SOCKADDR_IN* lpFromSockAddr);
	virtual void OnOK();
};

