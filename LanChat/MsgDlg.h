#pragma once



struct ItemData;
#include "Resource.h"
#include "VoiceSocket.h"
#include "Sound.h"


#define TIPS_TIMER	1
#define VOICE_TIMER	2

// CMsgDlg 对话框

class CMsgDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMsgDlg();

// 对话框数据
	enum { IDD = IDD_MSGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	WORD m_wNum;
	WORD m_wLastRecvNum;
	ItemData *m_pdata;
	CString m_strTitle;
	bool m_bIsItOnLine;
	CMapStringToString m_mapSendFile;
	volatile bool m_bCancel;
	CString m_strRecvFolder;
	CVoiceServer m_vs;
	CVoiceClient m_vc;
	bool m_bChatInVoice;
	CSound m_sound;
	SYSTEMTIME m_timeBegin;
	bool m_bInFileTrans;

	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void OnCancel();
	afx_msg void OnClickedSendmsg();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClickedSendfile();
	afx_msg void OnClickedCancel();
	afx_msg void OnBnClickedSendfolder();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClickedVoice();
	afx_msg void OnDestroy();
	LRESULT OnWimOpen(WPARAM wParam, LPARAM lParam);
	LRESULT OnWimData(WPARAM wParam, LPARAM lParam);
	LRESULT OnWinClose(WPARAM wParam, LPARAM lParam);
	LRESULT OnWomOpen(WPARAM wParam, LPARAM lParam);
	LRESULT OnWomDone(WPARAM wParam, LPARAM lParam);
	LRESULT OnWomClose(WPARAM wParam, LPARAM lParam);

	void AddNewMsg(LPCTSTR lpszMsg, LPCTSTR lpszFromName = nullptr);
	// 状态更改函数，参数传入为，上线true，下线false
	void StatusChange(bool bEnter);
	void RecvFile(MyData* lpData, int nDataLen);
	void MsgOfTrans(MyData* lpData,int nDataLen);
	void SendFile(MyData* lpData);
	inline void CancelTransFile();
	void SendMsgOfTrans(Operate ope, ULONGLONG &ullLength, CString &name);
	void AddSysMsg(LPCTSTR lpszMsg);
	void AskForVoice(MyData* lpData, int nDataLen);
	void AcceptVoice(MyData *lpData);
	void ConnectForVoice();
	void ConnectIsBulided();
	void Reinstatement();
	void RecvSound();
	void EndOfVoice();
	
	void SendVoiceMsg(WORD wSubOper);
};


//待发送文件（夹）列表元素数据结构
struct ItemData
{
	SOCKADDR_IN *m_addr;
	CMsgDlg *m_pdlg;

	ItemData();
	ItemData(SOCKADDR_IN *addr);
	~ItemData();
};
//发送文件（夹）线程参数结构
struct ThreadArg
{
	CMsgDlg *dlg;
	SOCKET s;
	USHORT port;
	ULONGLONG ullFileSize;
	CString fileName;
};
//传送进度用结构
struct MyStruct
{
	bool *bError;
	CStatic *pname;
	CStatic *ppt;
	CProgressCtrl *pprog;
};

void BrowseFolder(ThreadArg* p, CString strPath, ULONGLONG &ullSended, MyStruct *ps);
void SendFileInFolder(ThreadArg* p, CString strFileName, ULONGLONG &ullSended,MyStruct *ps);
void SendFileInfo(SOCKET s, Operate type, CString fileName,ULONGLONG ullFileSize);
ULONGLONG GetDirectorySize(CString path);
