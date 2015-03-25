#include "stdafx.h"
#include "Resource.h"
#include "MsgDlg.h"


//将文件大小转换成适合人类阅读的格式
CString GetSizeString(ULONGLONG &ullSize)
{
	TCHAR unit[][6] = {
		TEXT("B"),
		TEXT("KB"),
		TEXT("MB"),
		TEXT("GB"),
		TEXT("TB")
	};

	int i;
	double dsize = (double)ullSize;
	for (i = 0; dsize >= 1024; ++i){
		dsize /= 1024;
	}

	int dotNum = (int)((dsize - floor(dsize)) * 100);

	CString string;
	if (dotNum == 0)
		string.Format(TEXT("%.0lf%s"), dsize, unit[i]);
	else if (dotNum % 10 == 0)
		string.Format(TEXT("%.1lf%s"), dsize, unit[i]);
	else
		string.Format(TEXT("%.2lf%s"), dsize, unit[i]);

	return string;
}

//发送文件线程函数
UINT SendFileFunc(LPVOID pParam)
{
	auto p = (ThreadArg*)pParam;
	CString filePathName;
	if (!p->dlg->m_mapSendFile.Lookup(p->fileName, filePathName)){
		p->dlg->MessageBox(p->fileName + TEXT("未在发送列表中！"));
		delete p;
		return 0;
	}

	auto ps1 = (CStatic*)p->dlg->GetDlgItem(IDC_FILENAME);
	auto ps2 = (CStatic*)p->dlg->GetDlgItem(IDC_FILESIZE);
	auto ps3 = (CStatic*)p->dlg->GetDlgItem(IDC_PROGTEXT);
	auto btnCancel = (CButton*)p->dlg->GetDlgItem(IDC_CANCEL);
	auto pprog = (CProgressCtrl*)p->dlg->GetDlgItem(IDC_PROGRESS);
	ps1->SetWindowTextW(filePathName);
	ps2->ShowWindow(SW_HIDE);
	ps3->SetWindowTextW(TEXT("0%"));
	btnCancel->ShowWindow(SW_SHOW);
	btnCancel->EnableWindow();
	pprog->ShowWindow(SW_SHOW);
	pprog->SetRange(0, 100);
	pprog->SetPos(0);

	p->s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (p->s == INVALID_SOCKET){
		p->dlg->MessageBox(TEXT("创建套接字失败！"));
	}
	else{
		SOCKADDR_IN localaddr;
		localaddr.sin_family = AF_INET;
		localaddr.sin_port = 0;
		localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(p->s, (SOCKADDR*)&localaddr, sizeof(localaddr)) == SOCKET_ERROR){
			p->dlg->MessageBox(TEXT("绑定到本地端口时失败！"));
		}
		else{
			SOCKADDR_IN newaddr;
			newaddr.sin_family = AF_INET;
			newaddr.sin_port = p->port;
			newaddr.sin_addr = p->dlg->m_pdata->m_addr->sin_addr;
			if (connect(p->s, (SOCKADDR*)&newaddr, sizeof(newaddr)) == SOCKET_ERROR){
				p->dlg->MessageBox(TEXT("连接远程端口失败！"));
			}
			else{
				CFile file;
				if (!file.Open(filePathName, CFile::modeRead)){
					p->dlg->MessageBox(TEXT("打开文件失败！！！"));
				}
				else{
					p->ullFileSize = file.GetLength();
					ULONGLONG ullSended = 0;
					int curscale = 0;
					char *pbuf = new char[MAX_BUF_SIZE];
					memset(pbuf, 0, MAX_BUF_SIZE);

					while (ullSended < p->ullFileSize){
						if (p->dlg->m_bCancel){
							p->dlg->AddSysMsg(TEXT("文件“") + p->fileName + TEXT("”发送已取消！"));
							break;
						}

						int nCount = file.Read(pbuf, MAX_BUF_SIZE);

						int num,
							nLeft = nCount,
							nIndex = 0;
						bool bError = false;
						while (nLeft){
							num = send(p->s, &pbuf[nIndex], nLeft, 0);
							if (num == SOCKET_ERROR){
								CString string;
								string.Format(TEXT("发送数据出错：%d"), WSAGetLastError());
								p->dlg->MessageBox(string);
								bError = true;
								break;
							}
							nLeft -= num;
							nIndex += num;
						}
						if (bError)
							break;

						ullSended += nCount;
						int scale = (int)(ullSended * 100.0 / p->ullFileSize);
						if (curscale != scale){
							CString string;
							string.Format(TEXT("%d%%"), scale);
							ps3->SetWindowTextW(string);
							pprog->SetPos(scale);
							curscale = scale;
						}
					}
					if (curscale == 100){
						p->dlg->AddSysMsg(TEXT("文件“") + p->fileName + TEXT("”发送完成！"));
					}
					file.Close();
					delete[]pbuf;
				}
			}
		}
		closesocket(p->s);
	}
	p->dlg->m_mapSendFile.RemoveKey(p->fileName);
	p->dlg->Reinstatement();
	p->dlg->m_bInFileTrans = false;
	delete p;
	return 0;
}

//接收文件线程函数
UINT RecvFileFunc(LPVOID pParam)
{
	auto p = (ThreadArg*)pParam;

	auto ps1 = (CStatic*)p->dlg->GetDlgItem(IDC_FILENAME);
	auto ps2 = (CStatic*)p->dlg->GetDlgItem(IDC_FILESIZE);
	auto ps3 = (CStatic*)p->dlg->GetDlgItem(IDC_PROGTEXT);
	auto btnCancel = (CButton*)p->dlg->GetDlgItem(IDC_CANCEL);
	auto btnSendFile = (CButton*)p->dlg->GetDlgItem(IDC_SENDFILE);
	auto btnSendFolder = (CButton*)p->dlg->GetDlgItem(IDC_SENDFOLDER);
	auto pprog = (CProgressCtrl*)p->dlg->GetDlgItem(IDC_PROGRESS);
	ps1->SetWindowTextW(p->fileName);
	ps2->ShowWindow(SW_HIDE);
	ps3->SetWindowTextW(TEXT("0%"));
	btnCancel->ShowWindow(SW_SHOW);
	btnCancel->EnableWindow();
	pprog->ShowWindow(SW_SHOW);
	pprog->SetRange(0, 100);
	pprog->SetPos(0);

	ULONGLONG ullRecved = 0;
	CString tempName;
	CFile file;

	SOCKET newsocket;
	SOCKADDR_IN addr;
	int nAddrLen = sizeof(addr);
	if ((newsocket = accept(p->s, (SOCKADDR*)&addr, &nAddrLen)) == INVALID_SOCKET){
		p->dlg->MessageBox(TEXT("接收新连接时失败"));
		goto over;
	}
	closesocket(p->s);
	p->s = newsocket;

	tempName = p->fileName + TEXT(".trans");
	if (!file.Open(tempName, CFile::modeCreate | CFile::modeWrite)){
		p->dlg->MessageBox(TEXT("创建文件失败！"));
		goto over;
	}

	bool bError = false;
	int curscale = 0;
	char *pbuf = new char[MAX_BUF_SIZE];
	memset(pbuf, 0, MAX_BUF_SIZE);

	while (ullRecved < p->ullFileSize){
		if (p->dlg->m_bCancel){
			p->dlg->AddSysMsg(TEXT("文件“") + p->fileName + TEXT("”接收已取消！"));
			break;
		}

		int num = recv(p->s, pbuf, MAX_BUF_SIZE, 0);
		if (num == SOCKET_ERROR){
			CString string;
			string.Format(TEXT("接收数据出错：%d"), WSAGetLastError());
			ps2->SetWindowTextW(string);
			bError = true;
			break;
		}

		file.Write(pbuf, num);

		ullRecved += num;
		int scale = (int)(ullRecved * 100.0 / p->ullFileSize);

		if (curscale != scale){
			CString string;
			string.Format(TEXT("%d%%"), scale);
			ps3->SetWindowTextW(string);
			pprog->SetPos(scale);
			curscale = scale;
		}

	}
	if (curscale == 100){
		p->dlg->AddSysMsg(TEXT("文件“") + p->fileName + TEXT("”接收完成！"));
	}
	file.Close();
	delete[]pbuf;

	if (p->dlg->m_bCancel || bError){
		CFile::Remove(tempName);
	}
	else{
		CFile::Rename(tempName, p->fileName);
	}

over:
	p->dlg->Reinstatement();
	p->dlg->m_bInFileTrans = false;
	closesocket(p->s);
	delete p;
	return 0;
}

//发送文件夹线程函数
UINT SendFolderFunc(LPVOID pParam)
{
	auto p = (ThreadArg*)pParam;
	CString folderName = p->fileName;
	if (!p->dlg->m_mapSendFile.Lookup(folderName, p->fileName)){
		p->dlg->MessageBox(folderName + TEXT("未在发送列表中！"));
		delete p;
		return 0;
	}

	auto ps1 = (CStatic*)p->dlg->GetDlgItem(IDC_FILENAME);
	auto ps2 = (CStatic*)p->dlg->GetDlgItem(IDC_FILESIZE);
	auto ps3 = (CStatic*)p->dlg->GetDlgItem(IDC_PROGTEXT);
	auto btnCancel = (CButton*)p->dlg->GetDlgItem(IDC_CANCEL);
	auto btnSendFile = (CButton*)p->dlg->GetDlgItem(IDC_SENDFILE);
	auto btnSendFolder = (CButton*)p->dlg->GetDlgItem(IDC_SENDFOLDER);
	auto pprog = (CProgressCtrl*)p->dlg->GetDlgItem(IDC_PROGRESS);
	ps1->SetWindowTextW(p->fileName);
	ps2->ShowWindow(SW_HIDE);
	ps3->SetWindowTextW(TEXT("0%"));
	btnCancel->ShowWindow(SW_SHOW);
	btnCancel->EnableWindow();
	pprog->ShowWindow(SW_SHOW);
	pprog->SetRange(0, 100);
	pprog->SetPos(0);

	bool bError = false;
	MyStruct ms;
	ms.bError = &bError;
	ms.pname = ps1;
	ms.ppt = ps3;
	ms.pprog = pprog;

	p->s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (p->s == INVALID_SOCKET){
		p->dlg->MessageBox(TEXT("创建套接字失败！"));
	}
	else{
		SOCKADDR_IN localaddr;
		localaddr.sin_family = AF_INET;
		localaddr.sin_port = 0;
		localaddr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(p->s, (SOCKADDR*)&localaddr, sizeof(localaddr)) == SOCKET_ERROR){
			p->dlg->MessageBox(TEXT("绑定到本地端口时失败！"));
		}
		else{
			SOCKADDR_IN newaddr;
			newaddr.sin_family = AF_INET;
			newaddr.sin_port = p->port;
			newaddr.sin_addr = p->dlg->m_pdata->m_addr->sin_addr;

			if (connect(p->s, (SOCKADDR*)&newaddr, sizeof(newaddr)) == SOCKET_ERROR){
				p->dlg->MessageBox(TEXT("连接远程端口失败！"));
			}
			else{
				ULONGLONG ullSended = 0;
				p->ullFileSize = GetDirectorySize(p->fileName);
				BrowseFolder(p, p->fileName, ullSended, &ms);
				SendFileInfo(p->s, OVR, TEXT(""), 0);

				pprog->ShowWindow(SW_HIDE);
				ps2->ShowWindow(SW_SHOW);
				if (ullSended / p->ullFileSize)
					p->dlg->AddSysMsg(TEXT("文件夹“") + p->fileName + TEXT("”发送完成！"));
				else
					p->dlg->AddSysMsg(TEXT("文件夹“") + p->fileName + TEXT("”发送已取消！"));
			}
		}
		closesocket(p->s);
	}
	p->dlg->m_mapSendFile.RemoveKey(folderName);
	p->dlg->Reinstatement();
	p->dlg->m_bInFileTrans = false;
	delete p;
	return 0;
}

//接收文件夹线程函数
UINT RecvFolderFunc(LPVOID pParam)
{
	auto p = (ThreadArg*)pParam;

	auto ps1 = (CStatic*)p->dlg->GetDlgItem(IDC_FILENAME);
	auto ps2 = (CStatic*)p->dlg->GetDlgItem(IDC_FILESIZE);
	auto ps3 = (CStatic*)p->dlg->GetDlgItem(IDC_PROGTEXT);
	auto btnCancel = (CButton*)p->dlg->GetDlgItem(IDC_CANCEL);
	auto btnSendFile = (CButton*)p->dlg->GetDlgItem(IDC_SENDFILE);
	auto btnSendFolder = (CButton*)p->dlg->GetDlgItem(IDC_SENDFOLDER);
	auto pprog = (CProgressCtrl*)p->dlg->GetDlgItem(IDC_PROGRESS);
	ps1->SetWindowTextW(p->fileName);
	ps2->ShowWindow(SW_HIDE);
	ps3->SetWindowTextW(TEXT("0%"));
	btnCancel->ShowWindow(SW_SHOW);
	btnCancel->EnableWindow();
	pprog->ShowWindow(SW_SHOW);
	pprog->SetRange(0, 100);
	pprog->SetPos(0);

	ULONGLONG ullRecved = 0;

	SOCKET newsocket;
	SOCKADDR_IN addr;
	int nAddrLen = sizeof(addr);
	if ((newsocket = accept(p->s, (SOCKADDR*)&addr, &nAddrLen)) == INVALID_SOCKET){
		p->dlg->MessageBox(TEXT("接收新连接时失败"));
		goto over;
	}
	closesocket(p->s);
	p->s = newsocket;

	while (true){
		if (p->dlg->m_bCancel)
			break;

		WORD wInfoSize;
		int num = recv(p->s, (char*)&wInfoSize, sizeof(WORD), 0);
		if (num == SOCKET_ERROR){
			CString string;
			string.Format(TEXT("接收数据出错：%d"), WSAGetLastError());
			p->dlg->MessageBox(string);
			break;
		}
		if (p->dlg->m_bCancel)
			break;

		char *pinfo = new char[wInfoSize];
		memset(pinfo, 0, wInfoSize);
		num = recv(p->s, pinfo, wInfoSize, 0);
		if (num == SOCKET_ERROR){
			CString string;
			string.Format(TEXT("接收数据出错：%d"), WSAGetLastError());
			p->dlg->MessageBox(string);
			delete[] pinfo;
			break;
		}
		auto pFileInfo = (MyFileInfo*)pinfo;

		if (pFileInfo->subOper == OVR){
			delete[] pinfo;
			break;//文件传送完成
		}

		if (pFileInfo->subOper == FLD){
			if (!CreateDirectory(p->fileName + pFileInfo->fileName, NULL)){
				p->dlg->MessageBox(TEXT("创建文件夹失败"));
				delete[] pinfo;
				break;
			}
			delete[] pinfo;
			continue;
		}

		CFile file;
		CString tempName(pFileInfo->fileName);
		auto ullFileSize = pFileInfo->fileSize;
		tempName = p->fileName + tempName + TEXT(".trans");

		if (!file.Open(tempName, CFile::modeCreate | CFile::modeWrite)){
			p->dlg->MessageBox(TEXT("创建文件失败！"));
			delete[] pinfo;
			break;
		}

		ps1->SetWindowText(p->fileName + pFileInfo->fileName);

		bool bError = false;
		int curscale = 0;
		char *pbuf = new char[MAX_BUF_SIZE];
		memset(pbuf, 0, MAX_BUF_SIZE);

		while (ullFileSize){
			if (p->dlg->m_bCancel)
				break;

			num = recv(p->s, pbuf, (int)min(MAX_BUF_SIZE, ullFileSize), 0);
			if (num == SOCKET_ERROR){
				CString string;
				string.Format(TEXT("接收数据出错：%d"), WSAGetLastError());
				p->dlg->MessageBox(string);
				bError = true;
				break;
			}

			file.Write(pbuf, num);

			ullRecved += num;
			ullFileSize -= num;
			int scale = (int)(ullRecved * 100.0 / p->ullFileSize);

			if (curscale != scale){
				CString string;
				string.Format(TEXT("%d%%"), scale);
				ps3->SetWindowTextW(string);
				pprog->SetPos(scale);
				curscale = scale;
			}

		}
		if (curscale == 100){
			p->dlg->AddSysMsg(TEXT("文件夹“") + p->fileName + TEXT("”接收完成！"));
		}
		file.Close();

		if (p->dlg->m_bCancel || bError){
			CFile::Remove(tempName);
		}
		else{
			CFile::Rename(tempName, p->fileName + pFileInfo->fileName);
		}

		delete[] pbuf;
		delete[] pinfo;

		if (bError)
			break;
	}
	if (p->dlg->m_bCancel){
		p->dlg->AddSysMsg(TEXT("文件夹“") + p->fileName + TEXT("”接收已取消！"));
	}
over:
	p->dlg->m_bInFileTrans = false;
	p->dlg->Reinstatement();
	closesocket(p->s);
	delete p;
	return 0;
}

void BrowseFolder(ThreadArg* p, CString strPath, ULONGLONG &ullSended, MyStruct *ps)
{
	HANDLE hfind;
	WIN32_FIND_DATA wfd;

	if (strPath.Right(1) != TEXT("\\"))
		strPath += TEXT("\\");
	if ((hfind = ::FindFirstFile(strPath + TEXT("*.*"), &wfd)) != INVALID_HANDLE_VALUE){
		do{
			if (p->dlg->m_bCancel || *ps->bError)
				break;

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				CString string(wfd.cFileName);
				if (string != TEXT(".") && string != TEXT("..")){
					string = strPath + string;
					SendFileInfo(p->s, FLD, string.Mid(p->fileName.GetLength()), 0);
					BrowseFolder(p, string, ullSended, ps);
				}
			}
			else
				SendFileInFolder(p, strPath + wfd.cFileName, ullSended, ps);

		} while (::FindNextFile(hfind, &wfd));
		::FindClose(hfind);
	}
}

void SendFileInFolder(ThreadArg* p, CString strPathName, ULONGLONG &ullSended, MyStruct *ps)
{
	CFile file;
	if (!file.Open(strPathName, CFile::modeRead)){
		p->dlg->MessageBox(TEXT("打开文件失败"));
		return;
	}

	auto ullFileSize = file.GetLength();
	SendFileInfo(p->s, FIL, strPathName.Mid(p->fileName.GetLength()), ullFileSize);
	ps->pname->SetWindowTextW(strPathName);

	int curscale = 0;
	char *pbuf = new char[MAX_BUF_SIZE];
	memset(pbuf, 0, MAX_BUF_SIZE);

	while (ullFileSize){
		if (p->dlg->m_bCancel || *ps->bError)
			break;
		int nCount = file.Read(pbuf, MAX_BUF_SIZE);

		int num,
			nLeft = nCount,
			nIndex = 0;
		while (nLeft){
			num = send(p->s, &pbuf[nIndex], nLeft, 0);
			if (num == SOCKET_ERROR){
				CString string;
				string.Format(TEXT("发送数据出错：%d"), WSAGetLastError());
				p->dlg->MessageBox(string);
				*ps->bError = true;
				break;
			}
			nLeft -= num;
			nIndex += num;
		}

		ullFileSize -= nCount;
		ullSended += nCount;
		int scale = (int)(ullSended * 100.0 / p->ullFileSize);
		if (curscale != scale){
			CString string;
			string.Format(TEXT("%d%%"), scale);
			ps->ppt->SetWindowTextW(string);
			ps->pprog->SetPos(scale);
			curscale = scale;
		}
	}

	file.Close();
	delete[] pbuf;
	return;
}

void SendFileInfo(SOCKET s, Operate type, CString fileName, ULONGLONG ullFileSize)
{
	int nInfoLen = sizeof(WORD)+sizeof(MyFileInfo)+fileName.GetLength()*sizeof(TCHAR);
	char *pInfo = new char[nInfoLen];
	memset(pInfo, 0, nInfoLen);

	auto p1 = (WORD*)pInfo;
	*p1++ = nInfoLen - sizeof(WORD);
	auto p2 = (MyFileInfo*)p1;
	p2->subOper = type;
	p2->fileSize = ullFileSize;
	wcscpy_s(p2->fileName, fileName.GetLength() + 1, fileName);


	int num,
		nLeft = nInfoLen,
		nIndex = 0;
	while (nLeft){
		num = send(s, &pInfo[nIndex], nInfoLen, 0);
		if (num == SOCKET_ERROR){
			CString string;
			string.Format(TEXT("发送数据出错：%d"), WSAGetLastError());
			MessageBox(NULL, string, NULL, 0);
			break;
		}
		nLeft -= num;
		nIndex += num;
	}

	delete[] pInfo;
	return;
}

ULONGLONG GetDirectorySize(CString path)
{
	HANDLE hfind;
	WIN32_FIND_DATA wfd;
	ULONGLONG ullSize = 0;
	LARGE_INTEGER larg;

	if (path.Right(1) != TEXT("\\"))
		path += TEXT("\\");
	if ((hfind = ::FindFirstFile(path + TEXT("*.*"), &wfd)) != INVALID_HANDLE_VALUE){
		do{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				CString string(wfd.cFileName);
				if (string != TEXT(".") && string != TEXT("..")){
					ullSize += GetDirectorySize(path + string);
				}
			}
			else{
				larg.HighPart = wfd.nFileSizeHigh;
				larg.LowPart = wfd.nFileSizeLow;
				ullSize += larg.QuadPart;
			}
		} while (::FindNextFile(hfind, &wfd));
		::FindClose(hfind);
	}
	return ullSize;
}