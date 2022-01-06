#pragma once


// CMainWnd

class CMainWnd : public CWnd
{
	DECLARE_DYNAMIC(CMainWnd)

public:
	CMainWnd(LPCWSTR szClass);
	virtual ~CMainWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
};