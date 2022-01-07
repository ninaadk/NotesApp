// NoteDlg.cpp : implementation file
//

#include "stdafx.h"
#include "notes.h"
#include "NoteDlg.h"
#include "afxdialogex.h"
#include <algorithm>


//-----------------------------------------------------------------------------------------
// Note switcher UI

CEditNoteSwitcher::CEditNoteSwitcher(CWnd* pParentWnd, CWnd* pEditNote)
   :m_pParentWnd(pParentWnd),
   m_pEditNote(pEditNote),
   m_pParentNote(NULL),
   m_pCurrentNote(NULL),
   m_pThumbNailWnd(NULL),
   m_bAddChild(false),
   m_bAddChildAtTop(false),
   m_bAddChildAtEnd(false)
{
}

CEditNoteSwitcher::~CEditNoteSwitcher()
{
   delete m_pThumbNailWnd;
}

void CEditNoteSwitcher::Show(CNote* pCurrentNote)
{
   m_bAddChild = false;
   m_bAddChildAtTop = false;
   m_bAddChildAtEnd = false;

   if (pCurrentNote)
   {
      m_pCurrentNote = pCurrentNote;
      m_pParentNote = pCurrentNote->GetParent();

      ShowCurrentNote();
   }
}

CNote* CEditNoteSwitcher::GetCurrentNote()
{
   return m_pCurrentNote;
}

void CEditNoteSwitcher::ShowCurrentNote()
{
   // show list of notes

   if (!m_pThumbNailWnd && m_pParentWnd)
   {
      m_pThumbNailWnd = new CEdit();
      RECT rect = { 0 };
      m_pEditNote->GetWindowRect(&rect);
      m_pParentWnd->ScreenToClient(&rect);

      m_pThumbNailWnd->Create(WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE, rect, m_pParentWnd, 1);
   }

   // find current note in child notes of parent
   // draw 20 notes with current note highlighted
   CNote* pParent = m_pCurrentNote->GetParent();
   if (pParent)
   {
      const NotePtrList& vecSiblingNotes = pParent->GetChildNotes();

      NoteConstItr i = std::find(vecSiblingNotes.begin(), vecSiblingNotes.end(), m_pCurrentNote);

      NotePtrList::difference_type j = i - vecSiblingNotes.begin();

      std::wstring wsText = L"      ";
      wsText += vecSiblingNotes[j]->GetFullName();
      wsText += L"\r\n------------------------------------------------------------------\r\n";

      for (std::vector<CNote*>::difference_type k = j - 10; k < j + 10; ++k)
      {
         if (k == -1 && m_bAddChildAtTop)
         {
            wsText += L"^^^ Add New ^^^\r\n";
         }
         else if (k == vecSiblingNotes.size() && m_bAddChildAtEnd)
         {
            wsText += L"vvv Add New vvv\r\n";
         }
         else if (k < 0 || (unsigned)k >= vecSiblingNotes.size())
         {
            wsText += L"\r\n";
         }
         else
         {
            bool bDisplayed = (NULL != theApp.m_wh.IsDisplayed(vecSiblingNotes[k]));

            if (k == j)
            {
               wsText += bDisplayed ? L" >>*" : L">>>";
            }
            else
            {
               wsText += bDisplayed ? L"     *" : L"      ";
            }
            wsText += vecSiblingNotes[k]->GetTitle();

            if (vecSiblingNotes[k]->GetChildNotes().size() > 0)
            {
               wsText += L" >";
            }

            if (m_bAddChild && k == j)
            {
               wsText += L" >>> Add New";
            }

            wsText += L"\r\n";
         }
      }
      m_pThumbNailWnd->SetWindowTextW(wsText.c_str());
   }
}

void CEditNoteSwitcher::MoveRight()
{
   if (m_pCurrentNote && !m_bAddChildAtTop && !m_bAddChildAtEnd)
   {
      if (m_pCurrentNote->GetChildNotes().size() > 0)
      {
         m_pParentNote = m_pCurrentNote;
         m_pCurrentNote = *(m_pCurrentNote->GetChildNotes().begin());
      }
      else
      {
         m_bAddChild = true;
      }
      ShowCurrentNote();
   }
}

void CEditNoteSwitcher::MoveLeft()
{
   if (m_bAddChild)
   {
      m_bAddChild = false;
   }
   else if (m_pCurrentNote && m_pParentNote && !m_bAddChildAtTop && !m_bAddChildAtEnd)
   {
      if (m_pParentNote->GetParent() != NULL)
      {
         m_pCurrentNote = m_pParentNote;
         m_pParentNote = m_pParentNote->GetParent();
      }
   }
   ShowCurrentNote();
}

void CEditNoteSwitcher::MoveUp()
{
   if (m_pCurrentNote && m_pParentNote && !m_bAddChild)
   {
      if (m_bAddChildAtEnd)
      {
         m_bAddChildAtEnd = false;
      }
      else
      {
         NoteConstItr itCurrentNote = std::find(m_pParentNote->GetChildNotes().begin(), m_pParentNote->GetChildNotes().end(), m_pCurrentNote);
         if (itCurrentNote != m_pParentNote->GetChildNotes().begin())
         {
            m_pCurrentNote = *(itCurrentNote - 1);
         }
         else
         {
            m_bAddChildAtTop = true;
         }
      }
      ShowCurrentNote();
   }
}

void CEditNoteSwitcher::MoveDown()
{
   if (m_pCurrentNote && m_pParentNote && !m_bAddChild)
   {
      if (m_bAddChildAtTop)
      {
         m_bAddChildAtTop = false;
      }
      else
      {
         NoteConstItr itCurrentNote = std::find(m_pParentNote->GetChildNotes().begin(), m_pParentNote->GetChildNotes().end(), m_pCurrentNote);

         if ((itCurrentNote + 1) != m_pParentNote->GetChildNotes().end())
         {
            m_pCurrentNote = *(itCurrentNote + 1);
         }
         else
         {
            m_bAddChildAtEnd = true;
         }
      }
      ShowCurrentNote();
   }
}

void CEditNoteSwitcher::Hide()
{
   if (m_bAddChildAtTop && m_pParentNote)
   {
      m_pCurrentNote = m_pParentNote->AddNewChild(false);
   }
   else if (m_bAddChildAtEnd && m_pParentNote)
   {
      m_pCurrentNote = m_pParentNote->AddNewChild();
   }
   else if (m_bAddChild && m_pCurrentNote)
   {
      m_pCurrentNote = m_pCurrentNote->AddNewChild();
   }
   if (m_pThumbNailWnd)
   {
      m_pThumbNailWnd->DestroyWindow();
      delete m_pThumbNailWnd;
      m_pThumbNailWnd = NULL;
   }
}

//-----------------------------------------------------------------------------------------
// MRU (Most Recently Used) list UI

CCtrlTabUI::CCtrlTabUI()
   :m_pThumbnailWnd(NULL),
   nCurrentNote(0),
   nSelectedNote(0),
   m_bVisible(false)
{
}

void CCtrlTabUI::Show(CWnd * pParentWnd, CEdit* pEditNote)
{
   if(!m_pThumbnailWnd && pParentWnd)
   {
      m_pThumbnailWnd = new CEdit();

      RECT rect = { 0 };
      pEditNote->GetWindowRect(&rect);
      pParentWnd->ScreenToClient(&rect);

      m_pThumbnailWnd->Create(WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE, rect, pParentWnd, 1);
   }
   m_bVisible = true;
   Rotate();
}

void CCtrlTabUI::Rotate(bool bForward /*= true*/)
{
   if(bForward)
   {
      ++nSelectedNote;
      if(nSelectedNote >= theApp.m_wh.GetMruList().size())
      {
         nSelectedNote = 0;
      }
   }
   else
   {
      if(nSelectedNote > 0)
      {
         --nSelectedNote;
      }
      else
      {
         nSelectedNote = (unsigned int)theApp.m_wh.GetMruList().size()-1;
      }
   }
   nCurrentNote = nSelectedNote;

   if(m_pThumbnailWnd)
   {
      std::wstring wsText, wsLine;
      unsigned i = 0;
      for(  WndInfoItr it = theApp.m_wh.GetMruList().begin();
            it!= theApp.m_wh.GetMruList().end();
            ++it, ++i
            )
      {
         wsLine.clear();
         if (i == nSelectedNote)
         {
            wsLine += (it->pWnd) ? L" >>*" : L">>>";
         }
         else
         {
            wsLine += (it->pWnd) ? L"     *" : L"      ";
         }
         wsLine.append(it->pNote->GetFullName().c_str());
         wsLine.append(L"\r\n");

         wsText.append(wsLine);
      }
      m_pThumbnailWnd->SetWindowTextW(wsText.c_str());
   }
}

void CCtrlTabUI::Hide()
{
   m_pThumbnailWnd->DestroyWindow();
   delete m_pThumbnailWnd;
   m_pThumbnailWnd = NULL;
   nSelectedNote = 0;
   m_bVisible = false;
}

CNote * CCtrlTabUI::GetCurrentNote()
{
   CNote * pRetVal = NULL;
   WndInfoItr it = theApp.m_wh.GetMruList().begin();
   if(theApp.m_wh.GetMruList().size() >= (nCurrentNote +1))
   {
      for(unsigned i=0; i< nCurrentNote; ++i)
      {
         ++it;
      }
      if(it != theApp.m_wh.GetMruList().end())
      {
         pRetVal = it->pNote;
      }
   }
   return pRetVal;
}

//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// CNoteDlg dialog

constexpr UINT_PTR TIMER_PASTE = 1234;
constexpr UINT_PTR TIMER_CLOSE = 4321;

IMPLEMENT_DYNAMIC(CNoteDlg, CDialogEx)

CNoteDlg::CNoteDlg(CWnd* pParent, CNote * pNote)
	: CDialogEx(CNoteDlg::IDD, pParent),
   m_pNote(pNote),
   m_pSwitcher(),
   m_bPaste(false),
   m_dwSelection(0),
   m_nFirstLine(0),
   m_rectPosition{0,0,0,0},
   m_bSwitcherDisplayed(false)
{
}

CNoteDlg::~CNoteDlg()
{
}

void CNoteDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_EDIT_NOTE, m_edtNote);
   DDX_Control(pDX, IDC_EDIT_TITLE, m_edtTitle);
}


BEGIN_MESSAGE_MAP(CNoteDlg, CDialogEx)
   ON_WM_TIMER()
   ON_MESSAGE(WM_SHOW_SWITCHER, &CNoteDlg::OnShowSwitcher)
   ON_MESSAGE(WM_HIDE_SWITCHER, &CNoteDlg::OnHideSwitcher)
   ON_MESSAGE(WM_MOVE_RIGHT, &CNoteDlg::OnMoveRight)
   ON_MESSAGE(WM_MOVE_LEFT, &CNoteDlg::OnMoveLeft)
   ON_MESSAGE(WM_MOVE_UP, &CNoteDlg::OnMoveUp)
   ON_MESSAGE(WM_MOVE_DOWN, &CNoteDlg::OnMoveDown)
   ON_MESSAGE(WM_DO_MRU, &CNoteDlg::OnDoMru)
   ON_MESSAGE(WM_FINISH_MRU, &CNoteDlg::OnFinishMru)
   ON_MESSAGE(WM_DELETE_NOTE, &CNoteDlg::OnDeleteNote)
   ON_EN_CHANGE(IDC_EDIT_TITLE, &CNoteDlg::OnEnChangeEditTitle)
   ON_WM_SIZING()
   ON_EN_KILLFOCUS(IDC_EDIT_NOTE, &CNoteDlg::OnEnKillfocusEditNote)
   ON_EN_SETFOCUS(IDC_EDIT_NOTE, &CNoteDlg::OnEnSetfocusEditNote)
   ON_WM_ACTIVATE()
   ON_MESSAGE(WM_SAVE_STATE, &CNoteDlg::OnSaveState)
END_MESSAGE_MAP()


// CNoteDlg message handlers


void CNoteDlg::ShowNote(DWORD dwSel, const RECT &rectPosition, int nFirstLine)
{
   m_rectPosition = rectPosition;
   m_dwSelection = dwSel;
   m_nFirstLine = nFirstLine;

   DoModal();
}


BOOL CNoteDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   m_pSwitcher.reset(new CEditNoteSwitcher(this, &m_edtNote));
   m_edtNote.SetLimitText(0x7FFFFFFE);

   if(m_bPaste)
   {
      SetTimer(TIMER_PASTE, 200, NULL);
   }

   ResizeWindow(m_rectPosition);

   // show note contents on UI.
   ShowNote(m_pNote);

   return FALSE;
}

void CNoteDlg::ResizeWindow(RECT rect)
{
   RECT rectZero = {0};

   if(memcmp(&rect, &rectZero, sizeof(RECT)) != 0)
   {
      SetWindowPos(NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_NOZORDER);
      ResizeChildWindows();
   }
   else
   {
      CenterWindow();
   }
}

void CNoteDlg::ShowNote(CNote * pNote)
{
   // show note contents on UI.
   m_edtTitle.SetWindowTextW(pNote->GetTitle().c_str());
   m_edtNote.SetWindowTextW(pNote->GetText().c_str());
   m_edtNote.SetFocus();

   m_edtTitle.SetModify(FALSE);
   m_edtNote.SetModify(FALSE);

   m_edtNote.SetSel(m_dwSelection, TRUE);
   m_edtNote.LineScroll(m_nFirstLine);
   
   std::wstring wsTitle = NOTES_TITLE;
   wsTitle += L": ";
   wsTitle += pNote->GetFullName();
   SetWindowTextW(wsTitle.c_str());
}

void CNoteDlg::OnCancel()
{
   Save();

   GetWindowRect(&m_rectPosition);
   m_dwSelection = m_edtNote.GetSel();
   int nFirstLine = m_edtNote.GetFirstVisibleLine();

   theApp.m_wh.SaveWindowInfo(this, m_dwSelection, m_rectPosition, nFirstLine);
   CDialogEx::OnCancel();
}

void CNoteDlg::PostNcDestroy()
{
   CDialogEx::PostNcDestroy();

   theApp.m_wh.UpdateMruList(this);
}


void CNoteDlg::Save()
{
   // check if contents are changed
   // save contents in note
   if(m_edtNote.GetModify() || m_edtTitle.GetModify())
   {
      CString csText, csTitle;
      
      m_edtNote.GetWindowTextW(csText);
      m_pNote->SetText(csText.GetBuffer());
      
      m_edtTitle.GetWindowTextW(csTitle);
      m_pNote->SetTitle(csTitle.GetBuffer());

      theApp.GetNotesDb()->Save(m_pNote);
   }
}

void CNoteDlg::Paste()
{
   // bring to foreground if necessary
   ::SetForegroundWindow(m_hWnd);
   size_t nLengthOrig, nLengthNew;

   // append dashed line to original text
   std::wstring ws = m_pNote->GetText();
   nLengthOrig = ws.size();
   ws.append(L"\r\n---------------------------------------\r\n");
   m_pNote->SetText(ws.c_str());

   // set cursor to end of text
   m_edtNote.SetWindowTextW(ws.c_str());
   m_edtNote.SetSel(0, -1, FALSE);
   m_edtNote.SetSel(-1, 0, FALSE);

   // paste from clipboard
   m_edtNote.Paste();

   // append one more dashed line and select newly added text
   CString csText;
   m_edtNote.GetWindowTextW(csText);

   csText.Append(L"\r\n---------------------------------------\r\n");
   m_pNote->SetText(csText.GetString());

   nLengthNew = csText.GetLength();

   m_edtNote.SetWindowTextW(csText.GetString());
   m_edtNote.SetSel((int)nLengthOrig, (int)nLengthNew, FALSE);
   //m_edtNote.SetSel(-1, 0, FALSE);

   // modify flag for saving
   m_edtNote.SetModify();
}

void CNoteDlg::OnTimer(UINT_PTR nIDEvent)
{
   CDialogEx::OnTimer(nIDEvent);
   
   if(nIDEvent == TIMER_PASTE)
   {
      KillTimer(nIDEvent);

      Paste();

      SetTimer(TIMER_CLOSE, 200, NULL);
   }
   else if(nIDEvent == TIMER_CLOSE)
   {
      KillTimer(nIDEvent);
      OnCancel();
   }
}

afx_msg LRESULT CNoteDlg::OnShowSwitcher(WPARAM wParam, LPARAM lParam)
{
   if(m_pSwitcher)
   {
      m_edtTitle.SetReadOnly(TRUE);
      m_edtNote.ShowWindow(SW_HIDE);

      m_pSwitcher->Show(m_pNote);
   }
   return 0;
}

afx_msg LRESULT CNoteDlg::OnHideSwitcher(WPARAM wParam, LPARAM lParam)
{
   if (m_pSwitcher)
   {
      m_pSwitcher->Hide();
      m_edtTitle.SetReadOnly(FALSE);
      m_edtNote.ShowWindow(SW_SHOW);
      m_edtNote.SetFocus();

      CNote * pNote = m_pSwitcher->GetCurrentNote();
      if(pNote != m_pNote)
      {
         // is note already displayed
         CNoteDlg * pDisplayedWnd = theApp.m_wh.IsDisplayed(pNote);
         if(pDisplayedWnd)
         {
            // if yes, bring to forground
            ::SetForegroundWindow(pDisplayedWnd->m_edtNote);
            pDisplayedWnd->m_edtNote.SetFocus();
         }
         else
         {
            // if no, switch to the note
            Save();

            GetWindowRect(&m_rectPosition);
            m_dwSelection = m_edtNote.GetSel();
            m_nFirstLine = m_edtNote.GetFirstVisibleLine();

            WndInfo wi = theApp.m_wh.SwitchNote(this, pNote, m_dwSelection, m_rectPosition, m_nFirstLine);
         
            m_pNote = pNote;
            m_dwSelection = wi.dwSelection;
            m_nFirstLine = wi.nFirstVisibleLine;

            if (g_bResizeDuringSwitching)  // set to TRUE for updating window size/position while switching
            {
               m_rectPosition = wi.rectWndPosition;
               ResizeWindow(wi.rectWndPosition);
            }

            ShowNote(m_pNote);
         }
      }
   }
   return 0;
}

afx_msg LRESULT CNoteDlg::OnMoveRight(WPARAM wParam, LPARAM lParam)
{
   if(m_pSwitcher)
   {
      m_pSwitcher->MoveRight();
   }
   return 0;
}

afx_msg LRESULT CNoteDlg::OnMoveLeft(WPARAM wParam, LPARAM lParam)
{
   if (m_pSwitcher)
   {
      m_pSwitcher->MoveLeft();
   }
   return 0;
}

afx_msg LRESULT CNoteDlg::OnMoveUp(WPARAM wParam, LPARAM lParam)
{
   if(m_pSwitcher)
   {
      m_pSwitcher->MoveUp();
   }
   return 0;
}

afx_msg LRESULT CNoteDlg::OnMoveDown(WPARAM wParam, LPARAM lParam)
{
   if(m_pSwitcher)
   {
      m_pSwitcher->MoveDown();
   }
   return 0;
}

afx_msg LRESULT CNoteDlg::OnDoMru(WPARAM wParam, LPARAM lParam)
{
   if(!m_CtrlTabUi.IsVisible())
   {
      m_edtTitle.SetReadOnly(TRUE);
      m_edtNote.ShowWindow(SW_HIDE);
      m_CtrlTabUi.Show(this, &m_edtNote);
   }
   else
   {
      m_CtrlTabUi.Rotate(wParam!=0);
   }
   return 0;
}

afx_msg LRESULT CNoteDlg::OnFinishMru(WPARAM wParam, LPARAM lParam)
{
   if(m_CtrlTabUi.IsVisible())
   {
      m_CtrlTabUi.Hide();
      m_edtTitle.SetReadOnly(FALSE);
      m_edtNote.ShowWindow(SW_SHOW);
      m_edtNote.SetFocus();
      CNote * pNote = m_CtrlTabUi.GetCurrentNote();
      if(pNote != m_pNote)
      {
         // is note already displayed
         CNoteDlg * pDisplayedWnd = theApp.m_wh.IsDisplayed(pNote);
         if(pDisplayedWnd)
         {
            // if yes, bring to forground
            ::SetForegroundWindow(pDisplayedWnd->m_edtNote);
            pDisplayedWnd->m_edtNote.SetFocus();
         }
         else
         {
            // if no, switch to the note
            Save();

            GetWindowRect(&m_rectPosition);
            m_dwSelection = m_edtNote.GetSel();
            m_nFirstLine = m_edtNote.GetFirstVisibleLine();

            WndInfo wi = theApp.m_wh.SwitchNote(this, pNote, m_dwSelection, m_rectPosition, m_nFirstLine);
         
            m_pNote = pNote;
            m_dwSelection = wi.dwSelection;
            m_nFirstLine = wi.nFirstVisibleLine;

            if (g_bResizeDuringSwitching)  // set to TRUE for updating window size/position while switching
            {
               m_rectPosition = wi.rectWndPosition;
               ResizeWindow(wi.rectWndPosition);
            }

            ShowNote(m_pNote);
         }
      }
   }
   return 0;
}

afx_msg LRESULT CNoteDlg::OnDeleteNote(WPARAM wParam, LPARAM lParam)
{
   if(IDYES == MessageBoxW(L"Delete this note ?", NOTES_TITLE, MB_YESNO))
   {
      m_pNote->DeleteNote();

      WndInfo wi = theApp.m_wh.DeleteAndSwitch(this);

      m_pNote = wi.pNote;
      m_dwSelection = wi.dwSelection;
      m_rectPosition = wi.rectWndPosition;
      m_nFirstLine = wi.nFirstVisibleLine;

      ResizeWindow(m_rectPosition);
      ShowNote(m_pNote);
   }
   return 0;
}


void CNoteDlg::OnEnChangeEditTitle()
{
   // TODO:  If this is a RICHEDIT control, the control will not
   // send this notification unless you override the CDialogEx::OnInitDialog()
   // function and call CRichEditCtrl().SetEventMask()
   // with the ENM_CHANGE flag ORed into the mask.

   // TODO:  Add your control notification handler code here

   CString csTitle;
   m_edtTitle.GetWindowTextW(csTitle);
   if(csTitle.GetLength() > 0)
   {
      m_pNote->SetTitle(csTitle.GetString());
   }
}


void CNoteDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
   CDialogEx::OnSizing(fwSide, pRect);
   
   ResizeChildWindows();
}

void CNoteDlg::ResizeChildWindows()
{
   RECT clRect = {0};
   GetClientRect(&clRect);

   RECT rcTitle = {0}, rcText = {0};
   m_edtTitle.GetWindowRect(&rcTitle);
   LONG nTitleHeight = rcTitle.bottom-rcTitle.top;

   m_edtTitle.SetWindowPos(NULL, 0, 0,            clRect.right, nTitleHeight,               SWP_NOZORDER|SWP_NOMOVE);
   m_edtNote.SetWindowPos (NULL, 0, nTitleHeight, clRect.right, clRect.bottom-nTitleHeight, SWP_NOZORDER|SWP_NOMOVE);
}

void CNoteDlg::OnEnKillfocusEditNote()
{
   m_dwSelection = m_edtNote.GetSel();
}

void CNoteDlg::OnEnSetfocusEditNote()
{
   m_edtNote.SetSel(m_dwSelection, TRUE);
}

void CNoteDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CDialogEx::OnActivate(nState, pWndOther, bMinimized);

   if(nState == WA_INACTIVE)
   {
      theApp.m_wh.ForgetForgroundWindow(this);

      if (m_pSwitcher)
      {
         m_pSwitcher->Hide();
         m_bSwitcherDisplayed = false;
         m_edtTitle.SetReadOnly(FALSE);
         m_edtNote.ShowWindow(SW_SHOW);
      }
   }
}

afx_msg LRESULT CNoteDlg::OnSaveState(WPARAM wParam, LPARAM lParam)
{
   Save();

   GetWindowRect(&m_rectPosition);
   m_dwSelection = m_edtNote.GetSel();
   int nFirstLine = m_edtNote.GetFirstVisibleLine();

   theApp.m_wh.SaveWindowInfo(this, m_dwSelection, m_rectPosition, nFirstLine);
   theApp.m_wh.UpdateMruList(this);

   return 0;
}

bool IsCtrlPressed()
{
   return (0x8000 == (0x8000 & GetKeyState(VK_CONTROL)));
}

bool IsShiftPressed()
{
   return (0x8000 == (0x8000 & GetKeyState(VK_SHIFT)));
}

BOOL CNoteDlg::OnKeyDown(MSG* pMsg)
{
   BOOL bRetVal = FALSE;
   UINT nChar = (UINT)pMsg->wParam;
   bool bCtrl = IsCtrlPressed();

   if ((nChar == L'N') && bCtrl)
   {
      Hotkey_Ctrl_N();
      bRetVal = TRUE;
   }
   else if (nChar == VK_RETURN)
   {
      bRetVal = HotKey_ENTER();
   }
   else if ((nChar == L'D') && bCtrl)
   {
      HotKey_Ctrl_D();
      bRetVal = TRUE;
   }
   else if ((nChar == VK_LEFT) ||
      (nChar == VK_RIGHT) ||
      (nChar == VK_UP) ||
      (nChar == VK_DOWN)
      )
   {
      bRetVal = HotKey_Arrow(nChar);
   }
   else if ((nChar == VK_TAB) && bCtrl)
   {
      HotKey_Ctrl_Tab(IsShiftPressed());
      bRetVal = TRUE;
   }
   return bRetVal;
}

BOOL CNoteDlg::OnKeyUp(MSG* pMsg)
{
   BOOL bRetVal = FALSE;
   UINT nChar = (UINT)pMsg->wParam;
   if (nChar == VK_CONTROL)
   {
      HotKey_Ctrl_Up();
      bRetVal = TRUE;
   }
   return bRetVal;
}

void CNoteDlg::Hotkey_Ctrl_N()
{
   if (!m_bSwitcherDisplayed)
   {
      PostMessageW(WM_SHOW_SWITCHER);
      m_bSwitcherDisplayed = true;
   }
   else
   {
      PostMessageW(WM_HIDE_SWITCHER);
      m_bSwitcherDisplayed = false;
   }
}

BOOL CNoteDlg::HotKey_ENTER()
{
   BOOL bRetVal = FALSE;
   if (m_bSwitcherDisplayed)
   {
      PostMessageW(WM_HIDE_SWITCHER);
      m_bSwitcherDisplayed = false;
      bRetVal = TRUE;
   }
   return bRetVal;
}

void CNoteDlg::HotKey_Ctrl_D()
{
   PostMessageW(WM_DELETE_NOTE);
}

BOOL CNoteDlg::HotKey_Arrow(UINT nChar)
{
   BOOL bRetVal = FALSE;
   if (m_bSwitcherDisplayed)
   {
      if (nChar == VK_LEFT)
      {
         PostMessageW(WM_MOVE_LEFT);
      }
      else if (nChar == VK_RIGHT)
      {
         PostMessageW(WM_MOVE_RIGHT);
      }
      else if (nChar == VK_UP)
      {
         PostMessageW(WM_MOVE_UP);
      }
      else if (nChar == VK_DOWN)
      {
         PostMessageW(WM_MOVE_DOWN);
      }

      bRetVal = TRUE;
   }
   return bRetVal;
}

void CNoteDlg::HotKey_Ctrl_Tab(bool bShiftDown)
{
   PostMessageW(WM_DO_MRU, bShiftDown ? 0 : 1);
}

void CNoteDlg::HotKey_Ctrl_Up()
{
   PostMessageW(WM_FINISH_MRU);
}

bool IsHotKeyChar(UINT nChar)
{
   bool bRetVal = false;

   if ((nChar == VK_CONTROL) ||
      (nChar == VK_RETURN) ||
      (nChar == VK_LEFT) ||
      (nChar == VK_UP) ||
      (nChar == VK_RIGHT) ||
      (nChar == VK_DOWN) ||
      (nChar == L'N') ||
      (nChar == L'D') ||
      (nChar == VK_TAB) ||
      (nChar == VK_SHIFT)
      )
   {
      bRetVal = true;
   }
   return bRetVal;
}

BOOL CNoteDlg::PreTranslateMessage(MSG* pMsg)
{
   BOOL bRetVal = FALSE;
   if (pMsg->message == WM_KEYDOWN)
   {
      if (IsHotKeyChar((UINT)pMsg->wParam))
      {
         bRetVal = OnKeyDown(pMsg);
      }
   }
   else if (pMsg->message == WM_KEYUP)
   {
      UINT nChar = (UINT)pMsg->wParam;

      if (IsHotKeyChar((UINT)pMsg->wParam))
      {
         bRetVal = OnKeyUp(pMsg);
      }
   }

   if (!bRetVal)
   {
      bRetVal = CDialogEx::PreTranslateMessage(pMsg);
   }

   return bRetVal;
}
