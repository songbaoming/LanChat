// Seting.cpp : 实现文件
//

#include "stdafx.h"
#include "SetDlg.h"
#include "afxdialogex.h"


// CSetDlg 对话框

IMPLEMENT_DYNAMIC(CSetDlg, CDialogEx)

CSetDlg::CSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetDlg::IDD, pParent)
	, m_strName(_T(""))
	, m_nLocalPort(0)
	, m_nPeerPort(0)
{

}

CSetDlg::~CSetDlg()
{
}

void CSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SET_NAME, m_strName);
	DDX_Text(pDX, IDC_SET_LOCALPORT, m_nLocalPort);
	DDV_MinMaxUInt(pDX, m_nLocalPort, 1000, 65535);
	DDX_Text(pDX, IDC_SET_BCPORT, m_nPeerPort);
	DDV_MinMaxUInt(pDX, m_nPeerPort, 1000, 65535);
}


BEGIN_MESSAGE_MAP(CSetDlg, CDialogEx)
END_MESSAGE_MAP()


// CSetDlg 消息处理程序
