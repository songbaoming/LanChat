#include "stdafx.h"
#include "Resource.h"
#include "MsgDlg.h"
#include "Sound.h"


CSound::CSound()
{
}


CSound::~CSound()
{
}


void CSound::Initialize(CMsgDlg *pdlg)
{
	for (int i = 0; i < SOUND_BUF_NUM; ++i){
		m_pInBuf[i] = new CHAR[SOUND_BUF_SIZE];
		m_pWaveHdrIn[i] = new WAVEHDR;

		m_pWaveHdrIn[i]->lpData = m_pInBuf[i];
		m_pWaveHdrIn[i]->dwBufferLength = SOUND_BUF_SIZE;
		m_pWaveHdrIn[i]->dwBytesRecorded = 0;
		m_pWaveHdrIn[i]->dwUser = 0;
		m_pWaveHdrIn[i]->dwFlags = 0;
		m_pWaveHdrIn[i]->dwLoops = 1;
		m_pWaveHdrIn[i]->lpNext = NULL;
		m_pWaveHdrIn[i]->reserved = 0;

		m_pOutBuf[i] = new CHAR[SOUND_BUF_SIZE];
		m_pWaveHdrOut[i] = new WAVEHDR;

		m_pWaveHdrOut[i]->lpData = m_pOutBuf[i];
		m_pWaveHdrOut[i]->dwBufferLength = SOUND_BUF_SIZE;
		m_pWaveHdrOut[i]->dwBytesRecorded = 0;
		m_pWaveHdrOut[i]->dwUser = 0;
		m_pWaveHdrOut[i]->dwFlags = 0;
		m_pWaveHdrOut[i]->dwLoops = 1;
		m_pWaveHdrOut[i]->lpNext = NULL;
		m_pWaveHdrOut[i]->reserved;
	}

	m_waveForm.wFormatTag = WAVE_FORMAT_PCM;
	m_waveForm.nChannels = 1;
	m_waveForm.nSamplesPerSec = 11025;
	m_waveForm.nAvgBytesPerSec = 11025;
	m_waveForm.nBlockAlign = 1;
	m_waveForm.wBitsPerSample = 8;
	m_waveForm.cbSize = 0;

	if (waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_waveForm, 
		(DWORD)pdlg->GetSafeHwnd(), 0, CALLBACK_WINDOW) ||
		waveOutOpen(&m_hWaveOut,WAVE_MAPPER,&m_waveForm,
		(DWORD)pdlg->GetSafeHwnd(),0,CALLBACK_WINDOW)){
		for (int i = 0; i < SOUND_BUF_NUM; ++i){
			delete[] m_pInBuf[i];
			delete[] m_pOutBuf[i];
			delete m_pWaveHdrIn[i];
			delete m_pWaveHdrOut[i];

			m_pInBuf[i] = m_pOutBuf[i] = nullptr;
			m_pWaveHdrIn[i] = m_pWaveHdrOut[i] = nullptr;
		}

		pdlg->MessageBox(TEXT("设备打开失败！"));
	}

	for (int i = 0; i < SOUND_BUF_NUM; ++i){
		waveInPrepareHeader(m_hWaveIn, m_pWaveHdrIn[i], sizeof(WAVEHDR));
		waveOutPrepareHeader(m_hWaveOut, m_pWaveHdrOut[i], sizeof(WAVEHDR));
	}
}


void CSound::BeginRecord()
{
	for (int i = 0; i < SOUND_BUF_NUM; ++i){
		waveInAddBuffer(m_hWaveIn, m_pWaveHdrIn[i], sizeof(WAVEHDR));
	}
}



void CSound::CloseRecord()
{
	for (int i = 0; i < SOUND_BUF_NUM; ++i){
		waveInUnprepareHeader(m_hWaveIn, m_pWaveHdrIn[i], sizeof(WAVEHDR));
		delete[] m_pInBuf[i];
		delete m_pWaveHdrIn[i];
	}
}


void CSound::ClosePlay()
{
	for (int i = 0; i < SOUND_BUF_NUM; ++i){
		waveOutUnprepareHeader(m_hWaveOut, m_pWaveHdrOut[i], sizeof(WAVEHDR));
		delete[] m_pOutBuf[i];
		delete m_pWaveHdrOut[i];
	}
}