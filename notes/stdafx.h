
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#include "notes.h"

//#include "tracer.h"
//extern Tracer g_tr;


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif




constexpr LPCWSTR REG_KEY_APP    = L"Software\\NotesApp";
constexpr LPCWSTR REG_KEY_PATH   = L"path";

constexpr LPCWSTR NOTES_TITLE    = L"Notes";

constexpr UINT WM_SHOW_SWITCHER  = WM_USER + 1;
constexpr UINT WM_HIDE_SWITCHER  = WM_USER + 2;
constexpr UINT WM_MOVE_RIGHT     = WM_USER + 3;
constexpr UINT WM_MOVE_LEFT      = WM_USER + 4;
constexpr UINT WM_MOVE_UP        = WM_USER + 5;
constexpr UINT WM_MOVE_DOWN      = WM_USER + 6;
constexpr UINT WM_DO_MRU         = WM_USER + 7;
constexpr UINT WM_FINISH_MRU     = WM_USER + 8;
constexpr UINT WM_DELETE_NOTE    = WM_USER + 9;
constexpr UINT WM_PASTEBIN       = WM_USER + 10;
constexpr UINT WM_SAVE_STATE     = WM_USER + 11;

constexpr bool       g_bResizeDuringSwitching = true;
constexpr unsigned   g_ulMaxMruSize = 25;

// macro for marking lock_guard scope
#define using_lock_guard
