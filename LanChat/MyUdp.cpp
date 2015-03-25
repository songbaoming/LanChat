// MyUdp.cpp : 实现文件
//

#include "stdafx.h"
#include "MyUdp.h"
#include "MainWnd.h"
#include "MsgDlg.h"



Message::Message()
: m_pMsg(nullptr)
, m_nLen(0)
, m_pAddr(nullptr)
, m_nNextTime(1.5)
, m_nCurTime(0.0)
, m_nSumTime(0.0)
{

}

Message::Message(MyData *pdata, int nLen, SOCKADDR_IN *lpSockAddr)
: m_pMsg(pdata)
, m_nLen(nLen)
, m_pAddr(lpSockAddr)
, m_nNextTime(1.5)
, m_nCurTime(0.0)
, m_nSumTime(0.0)
{

}

Message::~Message()
{
	if (m_pMsg)
		delete[](char*)m_pMsg;
}

// CMyUdp

CMyUdp::CMyUdp()
: m_wNum(0)
{
}

CMyUdp::~CMyUdp()
{
}


// CMyUdp 成员函数


void CMyUdp::OnReceive(int nErrorCode)
{
	// TODO:  在此添加专用代码和/或调用基类
	char *pbuf = new char[MAX_MSG_SIZE];
	if (!pbuf){
		MessageBox(NULL, TEXT("内存分配错误！"), TEXT("错误"), 0);
		return;
	}
	memset(pbuf, 0, MAX_MSG_SIZE);

	SOCKADDR_IN *pnewaddr = new SOCKADDR_IN;
	if (!pnewaddr){
		MessageBox(NULL, TEXT("内存分配错误！"), TEXT("错误"), 0);
		delete[] pbuf;
		return;
	}
	int nLen = sizeof(SOCKADDR_IN);
	int nRecvLen;

	nRecvLen = ReceiveFrom(pbuf, MAX_MSG_SIZE, (SOCKADDR *)pnewaddr, &nLen);
	if (nRecvLen == SOCKET_ERROR){
		TRACE(TEXT("获取消息错误：%d\n"), WSAGetLastError());
		delete pnewaddr;
		delete[] pbuf;
	}
	else{
		MyData *p = (MyData *)pbuf;
		if (p->operate == ACK){
			//从消息链中删除
			auto pos = m_msglist.GetHeadPosition();
			while (pos){
				auto mem = m_msglist.GetAt(pos);
				if (mem->m_pAddr->sin_addr.s_addr == pnewaddr->sin_addr.s_addr &&
					mem->m_pAddr->sin_port == pnewaddr->sin_port &&
					mem->m_pMsg->number == p->number){
					delete mem;
					m_msglist.RemoveAt(pos);
					break;
				}
				m_msglist.GetNext(pos);
			}
			delete[] pbuf;
			delete pnewaddr;
		}
		else{
#pragma message("需进一步完善，防止包失序和重复")

			//给对方发送ACK
			auto prep = new MyData;
			if (prep){
				prep->operate = ACK;
				prep->number = p->number;//需进一步完善，防止包失序和重复
				SendTo(prep, sizeof(MyData), (SOCKADDR*)pnewaddr, sizeof(SOCKADDR_IN));
				delete prep;
			}
			
			CMainWnd *pwnd = (CMainWnd *)AfxGetMainWnd();
			pwnd->NewMsg(p, nRecvLen, pnewaddr);
		}
	}

	CAsyncSocket::OnReceive(nErrorCode);
}


void CMyUdp::OnSend(int nErrorCode)
{
	// TODO:  在此添加专用代码和/或调用基类

	CAsyncSocket::OnSend(nErrorCode);
}



void CMyUdp::Tick()
{
	auto pos = m_msglist.GetHeadPosition();
	while (pos){
		auto mem = m_msglist.GetAt(pos);
		mem->m_nCurTime += 1.5;
		mem->m_nSumTime += 1.5;
		if (mem->m_nCurTime >= mem->m_nNextTime){
			SendTo(mem->m_pMsg, mem->m_nLen, (SOCKADDR*)mem->m_pAddr, sizeof(SOCKADDR_IN));
			mem->m_nCurTime = 0.0;
			mem->m_nNextTime = min(mem->m_nNextTime * 2, 64);
		}
		if (mem->m_nSumTime > MAX_MSG_TIME){
			//提示用户该段内容发送失败
			CMainWnd *pwnd = (CMainWnd *)AfxGetMainWnd();

			CString string;
			if (mem->m_pMsg->operate == FIL || mem->m_pMsg->operate == FLD){
				auto pinfo = (MyFileInfo*)mem->m_pMsg->data;
				CString opeName;
				switch (pinfo->subOper)
				{
				case FILE_SEND:
				case FILE_ACCEPT:
					opeName = TEXT("传送");
					break;
				default:
					break;
				}
				if (!opeName.IsEmpty()){
					string.Format(TEXT("%s%s\"%s\"失败！"), opeName,
						mem->m_pMsg->operate == FIL ? TEXT("文件") : TEXT("文件夹"),
						pinfo->fileName);
					pwnd->ShowNewMsg(string, mem->m_pAddr, true);
				}
					
				auto pdlg = pwnd->GetMsgDlg(mem->m_pAddr);
				pdlg->Reinstatement();
			}
			else if (mem->m_pMsg->operate == TXT){
				string.Format(TEXT("%s\r\n发送失败！"), mem->m_pMsg->data);
				pwnd->ShowNewMsg(string, mem->m_pAddr, true);
			}

			auto temp = pos;
			delete mem;
			m_msglist.GetNext(pos);
			m_msglist.RemoveAt(temp);
		}
		else{
			m_msglist.GetNext(pos);
		}
	}
}


int CMyUdp::SendNewMsg(void * lpBuf, int nLength, SOCKADDR_IN * lpSockAddr, int nSockAddrLen)
{
	auto p = new Message((MyData*)lpBuf, nLength, lpSockAddr);
	if (!p){
		MessageBox(NULL, TEXT("内存分配错误！"), TEXT("错误"), 0);
		return -1;
	}
	m_msglist.AddTail(p);

	return SendTo(lpBuf, nLength, (SOCKADDR*)lpSockAddr, nSockAddrLen);
}


void CMyUdp::ClearMsgList()
{
	auto pos = m_msglist.GetHeadPosition();
	while (pos){
		delete m_msglist.GetNext(pos);
	}

	m_msglist.RemoveAll();
}
