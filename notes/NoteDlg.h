#pragma once
#include "afxwin.h"
#include "Note.h"
#include <list>
#include <memory>


//-----------------------------------------------------------------------------------------
// UI for selecting notes (ctrl+N)

class INoteSwitcherUI
{
public:
   virtual ~INoteSwitcherUI(){}
   virtual void Show(CNote * pCurrentNote) = 0;
   virtual void Hide(bool bCancel = false) = 0;
   virtual void MoveRight() = 0;
   virtual void MoveLeft() = 0;
   virtual void MoveUp() = 0;
   virtual void MoveDown() = 0;
   virtual CNote * GetCurrentNote() = 0;
};

class CEditNoteSwitcher : public INoteSwitcherUI
{
public:
   CEditNoteSwitcher(CWnd* pParentWnd, CWnd* pEditNote);
   ~CEditNoteSwitcher();
   void Show(CNote* pCurrentNote);
   void Hide(bool bCancel = false);
   void MoveRight();
   void MoveLeft();
   void MoveUp();
   void MoveDown();
   CNote* GetCurrentNote();

private:
   CWnd* m_pParentWnd;
   CWnd* m_pEditNote;
   CNote* m_pParentNote;
   CNote* m_pCurrentNote;
   CEdit* m_pThumbNailWnd;
   bool m_bAddChild;
   bool m_bAddChildAtTop;
   bool m_bAddChildAtEnd;

   void ShowCurrentNote();
};

//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// MRU (Most Recently Used) list UI (ctrl+tab)

class CCtrlTabUI
{
public:
   CCtrlTabUI();
   void Show(CWnd * pParentWnd, CEdit * pEditNote);
   void Rotate(bool bForward = true);
   void Hide();
   CNote * GetCurrentNote();
   bool IsVisible(){return m_bVisible;}

private:
   unsigned nCurrentNote;
   unsigned nSelectedNote;
   CEdit*   m_pThumbnailWnd;
   bool     m_bVisible;
};

//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// CNoteDlg dialog

class CNoteDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNoteDlg)

public:
	CNoteDlg(CWnd* pParent, CNote * pNote);
	virtual ~CNoteDlg();

// Dialog Data
	enum { IDD = IDD_NOTEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
   CEdit m_edtNote;
   CEdit m_edtTitle;
   bool m_bPaste;

private:
   // Currently displayed note
   CNote * m_pNote;

   // UI for changing current note
   std::unique_ptr<INoteSwitcherUI> m_pSwitcher;
   CCtrlTabUI m_CtrlTabUi;

   // dialog and cursor position size etc:
   RECT m_rectPosition;
   DWORD m_dwSelection;
   int   m_nFirstLine;

   // functions called by window handler:
public:
   void SetNote(CNote * pNote) {m_pNote = pNote;}
   void Paste();
   void ShowNote(DWORD dwSel, const RECT &rectPosition, int nFirstLine);

   // private helper functions:
private:
   void ShowNote(CNote * pNote);
   void Save();
   void ResizeChildWindows();
   void ResizeWindow(RECT rect);

   // functions for keyboard shortcuts:

   bool m_bSwitcherDisplayed;
   BOOL PreTranslateMessage(MSG* pMsg);
   BOOL OnKeyDown(MSG* pMsg);
   BOOL OnKeyUp(MSG* pMsg);
   void Hotkey_Ctrl_N();
   BOOL HotKey_ENTER();
   void HotKey_Ctrl_D();
   BOOL HotKey_Arrow(UINT nChar);
   void HotKey_Ctrl_Tab(bool bShiftDown);
   void HotKey_Ctrl_Up();
   BOOL HotKey_Escape();


   // message handlers and overloaded functions:
public:
   virtual BOOL OnInitDialog();
   virtual void OnCancel();
   virtual void PostNcDestroy();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg void OnEnChangeEditTitle();
   afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
   afx_msg void OnEnKillfocusEditNote();
   afx_msg void OnEnSetfocusEditNote();
   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
protected:
   afx_msg LRESULT OnShowSwitcher(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnHideSwitcher(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnMoveRight(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnMoveLeft(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnMoveUp(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnMoveDown(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnDoMru(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnFinishMru(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnDeleteNote(WPARAM wParam, LPARAM lParam);
   afx_msg LRESULT OnSaveState(WPARAM wParam, LPARAM lParam);
};
