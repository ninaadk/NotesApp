#pragma once
#include "NotesDb.h"
#include <list>
#include <memory>
#include <mutex>

//-----------------------------------------------------------------------------------------
// CForegroundWindow
// 
// class to handle focus while showing notes
// get() functions retrieves and remembers current foreground window
// then we can show our window which will be new foreground window
// revert() function sets focus to the original window
class CForegroundWindow
{
private:
   HWND m_hFGWnd;

public:
   CForegroundWindow()
      :m_hFGWnd(NULL)
   {
   }
   void get()
   {
      m_hFGWnd = ::GetForegroundWindow();
   }
   void revert()
   {
      if(m_hFGWnd)
      {
         ::SetForegroundWindow(m_hFGWnd);
         m_hFGWnd = NULL;
      }
   }
   void Forget()
   {
      m_hFGWnd = NULL;
   }
   void assign(HWND hwnd)
   {
      m_hFGWnd = hwnd;
   }
};

//-----------------------------------------------------------------------------------------



class CNoteDlg;
//-----------------------------------------------------------------------------------------
// WndInfo
// structure to hold various info of notes in MRU list
class WndInfo
{
public:
   CNote *     pNote;
   CNoteDlg *  pWnd;
   DWORD       dwSelection;
   RECT        rectWndPosition;
   int         nFirstVisibleLine;
   CForegroundWindow Fgw;

   WndInfo(CNoteDlg * pDlg, DWORD dwSel, const RECT & rectPos, int nFirstLine, CForegroundWindow fgw)
      :pWnd(pDlg),
      pNote(NULL),
      dwSelection(dwSel),
      rectWndPosition(rectPos),
      nFirstVisibleLine(nFirstLine),
      Fgw(fgw)
   {
   }

   WndInfo(const std::wstring & wsInfo, CNotesDb * pDb);

   WndInfo()
      :pNote(NULL),
      pWnd(NULL),
      dwSelection(0),
      nFirstVisibleLine(0)
   {
      memset(&rectWndPosition, 0, sizeof(RECT));
   }

   std::wstring ToString();
};

class IsSameWnd
{
public:

   IsSameWnd(const CNoteDlg * pDlg)
      :m_Dlg(pDlg)
   {
   }
   
   bool operator () (const WndInfo & wi)
   {
      return (wi.pWnd == m_Dlg);
   }

private:
   const CNoteDlg * m_Dlg;
};

class IsSameNote
{
public:

   IsSameNote(const CNote * pNote)
      :m_pNote(pNote)
   {
   }
   
   bool operator () (const WndInfo & wi)
   {
      return (wi.pNote == m_pNote);
   }

private:
   const CNote * m_pNote;
};

//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// WndInfoList
// MRU list structure

typedef std::list<WndInfo>           WndInfoList;
typedef std::list<WndInfo>::iterator WndInfoItr;

//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// CWindowHandler
// class to handle the UI windows showing notes...
class CWindowHandler
{
public:
   CWindowHandler(CNotesDb * pDb);
   ~CWindowHandler(void);

   void Init();

   // Show current note and handle navigation to other notes
   void ShowNote();
   void PasteToPastebin();
   void CloseAll();

   void SaveWindowInfo(CNoteDlg* pWnd, DWORD dwSelection, const RECT rectPos, int nFirstLine);
   void UpdateMruList(CNoteDlg* pWnd);
   WndInfo SwitchNote(CNoteDlg * pWnd, CNote * pNoteSwitchTo, DWORD dwSelection, const RECT rectPos, int nFirstLine);
   WndInfo DeleteAndSwitch(CNoteDlg * pWnd);
   CNoteDlg * IsDisplayed(const CNote * pNote);
   WndInfoList & GetMruList();
   void ForgetForgroundWindow(CNoteDlg * pDlg);
   void SetPreviousForgroundWindow(CNoteDlg* pDlg, HWND hPrevious);

private:
   CNotesDb * m_pDb;
   WndInfoList m_mruList;
   std::recursive_mutex m_csMru;

   bool LoadMruList();
   bool SaveMruList();
   void TruncateMruList();
};
