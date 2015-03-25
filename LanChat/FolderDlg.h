#pragma once
class CFolderDlg : public BROWSEINFO
{
public:
	CFolderDlg();
	CFolderDlg(CWnd *pwnd);
	~CFolderDlg();
	
private:
	ITEMIDLIST *m_pidl;
	TCHAR m_pszPath[MAX_PATH];
public:
	bool DoModal();
	CString GetPathName();
};

