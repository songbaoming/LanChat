#pragma once


#define SOUND_BUF_NUM	4
#define SOUND_BUF_SIZE	1024*3
class CMsgDlg;

class CSound
{
public:
	CSound();
	~CSound();

	void Initialize(CMsgDlg *pdlg);
	void BeginRecord();
	void CloseRecord();
	void ClosePlay();

public:
	HWAVEIN m_hWaveIn;
	HWAVEOUT m_hWaveOut;
	LPSTR m_pInBuf[SOUND_BUF_NUM];
	LPSTR m_pOutBuf[SOUND_BUF_NUM];
	PWAVEHDR m_pWaveHdrIn[SOUND_BUF_NUM];
	PWAVEHDR m_pWaveHdrOut[SOUND_BUF_NUM];
	WAVEFORMATEX m_waveForm;
	CTypedPtrList<CPtrList, PWAVEHDR> m_RecvList;
};

