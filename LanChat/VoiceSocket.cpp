// VoiceServer.cpp : 实现文件
//

#include "stdafx.h"
#include "VoiceSocket.h"
#include "MsgDlg.h"

// CVoiceServer

CVoiceServer::CVoiceServer(CMsgDlg *pdlg)
: pwnd(pdlg)
{
}

CVoiceServer::~CVoiceServer()
{
}


// CVoiceServer 成员函数


void CVoiceServer::OnAccept(int nErrorCode)
{
	// TODO:  在此添加专用代码和/或调用基类
	pwnd->ConnectForVoice();
	CAsyncSocket::OnAccept(nErrorCode);
}


//CVoiceClient函数实现

CVoiceClient::CVoiceClient(CMsgDlg *pdlg)
:pwnd(pdlg)
{

}


CVoiceClient::~CVoiceClient()
{

}

void CVoiceClient::OnClose(int nErrorCode)
{
	// TODO:  在此添加专用代码和/或调用基类
	//pwnd->MessageBox(TEXT("连接关闭"));
	pwnd->EndOfVoice();
	CAsyncSocket::OnClose(nErrorCode);
}


void CVoiceClient::OnConnect(int nErrorCode)
{
	// TODO:  在此添加专用代码和/或调用基类
	pwnd->ConnectIsBulided();
	CAsyncSocket::OnConnect(nErrorCode);
}


void CVoiceClient::OnReceive(int nErrorCode)
{
	// TODO:  在此添加专用代码和/或调用基类
	pwnd->SetWindowTextW(TEXT("已接收到声音"));

	if (!pwnd->m_sound.m_RecvList.IsEmpty()){
		auto p2 = pwnd->m_sound.m_RecvList.RemoveHead();
		waveOutWrite(pwnd->m_sound.m_hWaveOut, p2, sizeof(WAVEHDR));
	}

	pwnd->RecvSound();
	CAsyncSocket::OnReceive(nErrorCode);
}

