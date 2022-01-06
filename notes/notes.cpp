
// notes.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "notes.h"
#include "MainWnd.h"
#include "TRegKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CnotesApp

BEGIN_MESSAGE_MAP(CnotesApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CnotesApp construction

CnotesApp::CnotesApp()
   :m_db(),
   m_wh(&m_db)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CnotesApp object

CnotesApp theApp;


// CnotesApp initialization

BOOL CnotesApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Standard initialization

   // Initialize notes database (Load notes titles).
   // Create new in case of first time use.
   if(!InitNotesDb())
   {
      return false;
   }

   // Initialize window handler. (Mru list)
   m_wh.Init();

   // Register class for main window and create zero sized window
   LPCTSTR szClass = AfxRegisterWndClass(0);

   CMainWnd mainWnd(szClass);
   m_pMainWnd = &mainWnd;
   

   // Register hotkeys...

   // win+N for 'show note'
   BOOL b = RegisterHotKey(mainWnd.m_hWnd, 1, MOD_WIN, (UINT)'N');
   // win+shift+N for 'close app'
   b = RegisterHotKey(mainWnd.m_hWnd, 2, MOD_WIN| MOD_SHIFT, (UINT)'N');
   // win+V for 'paste in pastebin'
   b = RegisterHotKey(mainWnd.m_hWnd, 3, MOD_WIN, (UINT)'V');
   // start message pump...
   Run();

   //b = UnregisterHotKey(mainWnd.m_hWnd, 1);
   //DWORD dwLastError = GetLastError();
   //b = UnregisterHotKey(mainWnd.m_hWnd, 2);

	return FALSE;
}

std::wstring GetFolderPathFromRegistry()
{
   TRegKey key(HKEY_CURRENT_USER);

   if (key.Open(REG_KEY_APP))
   {
      std::wstring wsPath;
      if (key.ReadStringVal(REG_KEY_PATH, wsPath))
      {
         return wsPath;
      }
   }

   return L"";
}

bool SaveFolderPathInRegistry(LPCWSTR wstrPath)
{
   bool bRetVal = false;

   if (wstrPath)
   {
      TRegKey key(HKEY_CURRENT_USER);

      if (key.Create(REG_KEY_APP))
      {
         bRetVal = key.WriteStringVal(REG_KEY_PATH, wstrPath);
      }
   }

   return bRetVal;
}

std::wstring GetFolderPathFromUser(bool& bCancelled)
{
   CFolderPickerDialog dlg;

   if( IDCANCEL == dlg.DoModal())
   {
      bCancelled = true;
   }

   CStringW csPath = dlg.GetPathName();

   return csPath.GetString();
}

bool CnotesApp::InitNotesDb()
{
   // Get path from registry
   std::wstring wsPath = GetFolderPathFromRegistry();

   if (!wsPath.empty())
   {
      // Load db
      return m_db.Init(wsPath.c_str());
   }

   // First time use
   // Ask user to select folder for notes and save path in registry

   bool bRetVal = false;
   MessageBoxW(nullptr, L"Please select empty folder or existing NotesApp folder for saving notes...\n", NOTES_TITLE, MB_OK);
   bool bCancelled = false;

   while (!bCancelled)
   {
      wsPath = GetFolderPathFromUser(bCancelled);

      if (!wsPath.empty() && !bCancelled)
      {
         bool bIsEmptyFolder = m_db.IsEmpty(wsPath.c_str());
         if(!bIsEmptyFolder && !m_db.IsNotesAppFolder(wsPath.c_str()))
         {
            MessageBoxW(nullptr, L"Selected folder is not a NotesApp folder.\nPlease select empty folder or existing NotesApp folder.", NOTES_TITLE, MB_OK);
         }
         if (bIsEmptyFolder && !m_db.IsWritable(wsPath.c_str()))
         {
            MessageBoxW(nullptr, L"No write access to selected path.\nPlease select folder with write access.", NOTES_TITLE, MB_OK);
         }
         else
         {
            bool bOk = bIsEmptyFolder ? m_db.CreateNewDb(wsPath.c_str()) : m_db.Init(wsPath.c_str());
            if(bOk)
            {
               bRetVal = SaveFolderPathInRegistry(wsPath.c_str());
            }
            else
            {
               MessageBoxW(nullptr, L"Error creating notes database.", NOTES_TITLE, MB_OK);
            }
            break;
         }
      }
   }
   return bRetVal;
}
