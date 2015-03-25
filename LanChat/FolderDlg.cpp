#include "stdafx.h"
#include "FolderDlg.h"


CFolderDlg::CFolderDlg()
{
	hwndOwner = nullptr;
	pidlRoot = nullptr;
	pszDisplayName = m_pszPath;
	lpszTitle = TEXT("Ñ¡ÔñÎÄ¼þ¼Ð");
	ulFlags = BIF_RETURNONLYFSDIRS;
	lpfn = nullptr;
	lParam = 0;
	iImage = 0;
}

CFolderDlg::CFolderDlg(CWnd *pwnd)
:CFolderDlg()
{
	hwndOwner = pwnd->GetSafeHwnd();
}

CFolderDlg::~CFolderDlg()
{
}


bool CFolderDlg::DoModal()
{
	m_pidl = SHBrowseForFolder(this);
	return m_pidl;
}


CString CFolderDlg::GetPathName()
{
	if (!m_pidl)
		return TEXT("");

	CString string;
	SHGetPathFromIDList(m_pidl, m_pszPath);
	return m_pszPath;
}
