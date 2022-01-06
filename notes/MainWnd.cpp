// MainWnd.cpp : implementation file
//

#include "stdafx.h"
#include "notes.h"
#include "MainWnd.h"
#include <thread>


// CMainWnd

IMPLEMENT_DYNAMIC(CMainWnd, CWnd)

CMainWnd::CMainWnd(LPCWSTR szClass)
{
   // create window with zero size.
   RECT InitWndRect = {0};
   CreateEx(NULL, szClass, L"notesmainwindow", WS_POPUPWINDOW, InitWndRect, NULL, 0); 
}

CMainWnd::~CMainWnd()
{
}


BEGIN_MESSAGE_MAP(CMainWnd, CWnd)
   ON_WM_HOTKEY()
END_MESSAGE_MAP()


// CMainWnd message handlers

void CMainWnd::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
   CWnd::OnHotKey(nHotKeyId, nKey1, nKey2);

   switch(nHotKeyId)
   {
   case 1:
      {
         // new note dialog  (Win+N)
         std::thread task(&CWindowHandler::ShowNote, &theApp.m_wh);
         task.detach();
         break;
      }
   case 2:
      {
         // close app (Win+Shift+N)
         // TODO: close any open windows
         DestroyWindow();
         break;
      }
   case 3:
      {
         // Paste clipboard contents to 'pastebin' note (Win+V)
         theApp.m_wh.PasteToPastebin();
         break;
      }
   }
}
