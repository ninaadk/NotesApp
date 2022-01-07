#include "StdAfx.h"
#include "WindowHandler.h"
#include "NoteDlg.h"
#include <algorithm>
#include "StdStrHelper.h"


//-----------------------------------------------------------------------------------------
// WndInfo
WndInfo::WndInfo(const std::wstring& wsInfo, CNotesDb* pDb)
   :pNote(NULL),
   pWnd(NULL),
   dwSelection(0),
   nFirstVisibleLine(0),
   rectWndPosition{0,0,0,0},
   Fgw()
{
   auto tokens = StdStr::Tokanize(wsInfo, L"|");

   if ((tokens.size() == 7) && pDb)
   {
      pNote = pDb->GetNote(tokens[0].c_str());

      dwSelection = std::stoul(tokens[1]);

      rectWndPosition.bottom  = std::stol(tokens[2]);
      rectWndPosition.left    = std::stol(tokens[3]);
      rectWndPosition.right   = std::stol(tokens[4]);
      rectWndPosition.top     = std::stol(tokens[5]);

      nFirstVisibleLine       = std::stoi(tokens[6]);
   }
}

std::wstring WndInfo::ToString()
{
   std::wstring wsRetVal;

   if (pNote)
   {
      wsRetVal += pNote->GetFullName();
      wsRetVal += L"|";

      wsRetVal += std::to_wstring(dwSelection);
      wsRetVal += L"|";

      wsRetVal += std::to_wstring(rectWndPosition.bottom);
      wsRetVal += L"|";

      wsRetVal += std::to_wstring(rectWndPosition.left);
      wsRetVal += L"|";

      wsRetVal += std::to_wstring(rectWndPosition.right);
      wsRetVal += L"|";

      wsRetVal += std::to_wstring(rectWndPosition.top);
      wsRetVal += L"|";

      wsRetVal += std::to_wstring(nFirstVisibleLine);
   }

   return wsRetVal;
}

//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
// CWindowHandler

CWindowHandler::CWindowHandler(CNotesDb * pDb)
   :m_pDb(pDb)
{
}


CWindowHandler::~CWindowHandler(void)
{
}

void CWindowHandler::Init()
{
   LoadMruList();

   if (m_mruList.empty())
   {
      WndInfo wi;
      wi.pNote = m_pDb->GetRoot()->GetChildNotes()[0];
      
      m_mruList.push_front(wi);
   }
}

WndInfoList & CWindowHandler::GetMruList()
{
   return m_mruList;
}

bool NotCurrentlyDisplayed(const WndInfo & info)
{
   return (info.pWnd == NULL);
}

bool CWindowHandler::LoadMruList()
{
   m_mruList.clear();

   std::wstring wsMru = m_pDb->LoadMru();

   auto tokens = StdStr::TokanizeByWord(wsMru, L"\r\n");

   for (auto& wi : tokens)
   {
      if (wi.size() > 0)
      {
         WndInfo info(wi, m_pDb);
         if(info.pNote)
         {
            m_mruList.push_back(info);
         }
      }
   }

   return true;
}

bool CWindowHandler::SaveMruList()
{
   std::wstring wsMru;
   for (auto& wi : m_mruList)
   {
      wsMru += wi.ToString();
      wsMru += L"\r\n";
   }
   return m_pDb->SaveMru(wsMru);
}

void CWindowHandler::ShowNote()
{
   // window info for mru entry
   WndInfo wi;

   // note dialog
   CNoteDlg dlg(CWnd::GetDesktopWindow(), nullptr);

   using_lock_guard
   {
      std::lock_guard<std::recursive_mutex> lockmru(m_csMru);

      // find note from MRU which is not already displayed
      WndInfoItr it = std::find_if(m_mruList.begin(), m_mruList.end(), NotCurrentlyDisplayed);

      if(it != m_mruList.end())
      {
         // found in mru.
         // display this note and set it to latest in mru
         wi = *it;
         m_mruList.erase(it); // erase old entry
      }
      else
      {
         // not found in mru.
         // create new note and display.
         // set as latest in mru
         CNote * pNewNote = theApp.GetNotesDb()->GetRoot()->AddNewChild();

         wi.pNote = pNewNote;
      }
      
      dlg.SetNote(wi.pNote);
      wi.pWnd = &dlg;
      wi.Fgw.get();
      m_mruList.push_front(wi);  // add new entry in mru
   }

   dlg.ShowNote(wi.dwSelection, wi.rectWndPosition, wi.nFirstVisibleLine);
}


void CWindowHandler::SaveWindowInfo(CNoteDlg * pWnd, DWORD dwSelection, const RECT rectPos, int nFirstLine)
{
   if(pWnd)
   {
      std::lock_guard<std::recursive_mutex> lockmru(m_csMru);

      // find the window in Mru list and update position, selection data
      WndInfoItr it = std::find_if(m_mruList.begin(), m_mruList.end(), IsSameWnd(pWnd));
      if(it != m_mruList.end())
      {
         it->rectWndPosition = rectPos;
         it->dwSelection = dwSelection;
         it->nFirstVisibleLine = nFirstLine;
         it->Fgw.revert();
      }
   }
}

void CWindowHandler::UpdateMruList(CNoteDlg * pWnd)
{
   if(pWnd)
   {
      std::lock_guard<std::recursive_mutex> lockmru(m_csMru);

      // find the window in Mru list and bring it to front
      // Also save mru list to disk
      WndInfoItr it = std::find_if(m_mruList.begin(), m_mruList.end(), IsSameWnd(pWnd));
      if(it != m_mruList.end())
      {
         // mark as not currently displayed
         it->pWnd = NULL;

         WndInfo wi = *it;
         m_mruList.erase(it);
         m_mruList.push_front(wi);
         SaveMruList();
      }
   }
}

void CWindowHandler::ForgetForgroundWindow(CNoteDlg * pDlg)
{
   if(pDlg)
   {
      std::lock_guard<std::recursive_mutex> lockmru(m_csMru);

      WndInfoItr it = std::find_if(m_mruList.begin(), m_mruList.end(), IsSameWnd(pDlg));
      if(it != m_mruList.end())
      {
         it->Fgw.Forget();
      }
   }
}

void CWindowHandler::SetPreviousForgroundWindow(CNoteDlg* pDlg, HWND hPrevious)
{
   if (pDlg)
   {
      std::lock_guard<std::recursive_mutex> lockmru(m_csMru);

      WndInfoItr it = std::find_if(m_mruList.begin(), m_mruList.end(), IsSameWnd(pDlg));
      if (it != m_mruList.end())
      {
         it->Fgw.assign(hPrevious);
      }
   }
}

CNoteDlg * CWindowHandler::IsDisplayed(const CNote * pNote)
{
   CNoteDlg * pRetVal = NULL;

   std::lock_guard<std::recursive_mutex> lockmru(m_csMru);

   WndInfoItr it = std::find_if(m_mruList.begin(), m_mruList.end(), IsSameNote(pNote));

   if(it != m_mruList.end())
   {
      pRetVal = it->pWnd;
   }

   return pRetVal;
}

WndInfo CWindowHandler::SwitchNote(CNoteDlg * pWnd, CNote * pNoteSwitchTo, DWORD dwSelection, const RECT rectPos, int nFirstLine)
{
   std::lock_guard<std::recursive_mutex> lockmru(m_csMru);

   // remove window handle from old note
   CForegroundWindow fgwOrig;
   WndInfoItr itOrig = std::find_if(m_mruList.begin(), m_mruList.end(), IsSameWnd(pWnd));
   if(itOrig != m_mruList.end())
   {
      itOrig->rectWndPosition = rectPos;
      itOrig->dwSelection = dwSelection;
      itOrig->nFirstVisibleLine = nFirstLine;
      fgwOrig = itOrig->Fgw;
      itOrig->Fgw.Forget();
      itOrig->pWnd = NULL;
   }

   // add window handle to new note and set as latest
   WndInfo wi;
   WndInfoItr itNew = std::find_if(m_mruList.begin(), m_mruList.end(), IsSameNote(pNoteSwitchTo));
   if(itNew != m_mruList.end())
   {
      wi = *itNew;
      wi.pWnd = pWnd;
      wi.Fgw = fgwOrig;

      m_mruList.erase(itNew);
      m_mruList.push_front(wi);
   }
   else
   {
      wi.pNote = pNoteSwitchTo;
      wi.pWnd = pWnd;
      wi.Fgw = fgwOrig;
      m_mruList.push_front(wi);
   }
   return wi;
}

WndInfo CWindowHandler::DeleteAndSwitch(CNoteDlg * pWnd)
{
   WndInfo wi;
   std::lock_guard<std::recursive_mutex> lockmru(m_csMru);

   // find window in list
   WndInfoItr itOrig = std::find_if(m_mruList.begin(), m_mruList.end(), IsSameWnd(pWnd));
   if(itOrig != m_mruList.end())
   {
      
      WndInfoItr itNextNote = std::find_if(itOrig, m_mruList.end(), NotCurrentlyDisplayed);

      if(itNextNote != m_mruList.end())
      {
         itNextNote->pWnd = pWnd;
         itNextNote->Fgw = itOrig->Fgw;
         wi = *itNextNote;
         m_mruList.push_front(wi);
         m_mruList.erase(itNextNote);
      }
      else
      {
         wi = *itOrig;
         wi.pNote = theApp.GetNotesDb()->GetRoot()->AddNewChild();
         m_mruList.push_front(wi);
      }
      m_mruList.erase(itOrig);
   }

   return wi;
}


void CWindowHandler::PasteToPastebin()
{
   // find or create pastebin note
   // create dlg and set bPaste

   CNote * pNotePasteBin = m_pDb->GetNote(L"pastebin");
   if(!pNotePasteBin)
   {
      pNotePasteBin = m_pDb->GetRoot()->AddNewChild(false);
      pNotePasteBin->SetTitle(L"pastebin");
   }

   // window info for mru entry
   WndInfo wi;
   wi.pNote = pNotePasteBin;

   // note window
   CNoteDlg dlg(CWnd::GetDesktopWindow(), pNotePasteBin);
   bool bAlreadyDisplayed = false;

   using_lock_guard
   {
      std::lock_guard<std::recursive_mutex> lockmru(m_csMru);

      // find note in mru list
      WndInfoItr it = std::find_if(m_mruList.begin(), m_mruList.end(), IsSameNote(pNotePasteBin));

      if(it != m_mruList.end())
      {
         wi = *it;
         m_mruList.erase(it);
      }

      wi.Fgw.get();

      if (wi.pWnd)
      {
         // already displayed
         m_mruList.push_front(wi);  // add new entry in mru
         wi.pWnd->Paste();
         bAlreadyDisplayed = true;
      }
      else
      {
         dlg.m_bPaste = true;
         wi.pWnd = &dlg;
         m_mruList.push_front(wi);  // add new entry in mru
      }
   }

   if(!bAlreadyDisplayed)
   {
      dlg.ShowNote(wi.dwSelection, wi.rectWndPosition, wi.nFirstVisibleLine);
   }
}

void CWindowHandler::CloseAll()
{
   std::vector<CNoteDlg*> vOpenWindows;

   {
      std::lock_guard<std::recursive_mutex> lockmru(m_csMru);
      for (auto& info : m_mruList)
      {
         if (info.pWnd)
         {
            vOpenWindows.push_back(info.pWnd);
         }
      }
   }

   for (auto & wnd : vOpenWindows)
   {
      wnd->SendMessage(WM_SAVE_STATE);
   }
}
