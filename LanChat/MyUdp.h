#pragma once


//发送消息队列中元素结构
struct Message
{
	MyData *m_pMsg;
	int m_nLen;
	SOCKADDR_IN *m_pAddr;
	float m_nNextTime;
	float m_nCurTime;
	float m_nSumTime;

	Message();
	Message(MyData *pdata, int nLen, SOCKADDR_IN *lpSockAddr);
	~Message();
};

// CMyUdp 命令目标

class CMyUdp : public CAsyncSocket
{
public:
	CMyUdp();
	virtual ~CMyUdp();

private:
	CTypedPtrList<CPtrList, Message *> m_msglist;

public:
	WORD m_wNum;//包编号
	
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	void Tick();
	int SendNewMsg(void * lpBuf, int nLength, SOCKADDR_IN * lpSockAddr, int nSockAddrLen);
	void ClearMsgList();
};


