
// notes.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "WindowHandler.h"
#include "NotesDbFile.h"


// CnotesApp:
// See notes.cpp for the implementation of this class
//

class CnotesApp : public CWinApp
{
public:
	CnotesApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
   // window handler shows different notes in window
   CWindowHandler m_wh;

   CNotesDb * GetNotesDb(){return (CNotesDb *)&m_db;}

private :
   // notes database.
   CNotesDbFile m_db;

	DECLARE_MESSAGE_MAP()

private:

   bool InitNotesDb();
};

extern CnotesApp theApp;
