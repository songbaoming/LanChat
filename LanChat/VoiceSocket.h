#pragma once


class CMsgDlg;
// CVoiceServer ÃüÁîÄ¿±ê

class CVoiceServer : public CAsyncSocket
{
public:
	CVoiceServer(CMsgDlg *pdlg);
	virtual ~CVoiceServer();

	virtual void OnAccept(int nErrorCode);

private:
	CMsgDlg *pwnd;
};


class CVoiceClient : public CAsyncSocket
{
public:
	CVoiceClient(CMsgDlg *pdlg);
	virtual ~CVoiceClient();

	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);

private:
	CMsgDlg *pwnd;
};
