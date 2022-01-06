#include "StdAfx.h"
#include "Note.h"
#include "NotesDb.h"
#include <algorithm>


CNote::CNote(CNote * pParent)
   :m_pParent(pParent),
   //m_data(NULL),
   m_bLoaded(false),
   m_bChanged(false)
{
}

CNote::~CNote(void)
{
   for(NoteItr it=m_ChildNotes.begin(); it!=m_ChildNotes.end(); ++it)
   {
      delete *it;
      *it = NULL;
   }
   m_ChildNotes.clear();
   //delete m_data;
}

std::wstring CNote::GetText()
{
   if(!m_bLoaded)
   {
      theApp.GetNotesDb()->Load(this);
      m_bLoaded = true;
   }
   return m_wsText;
}

void CNote::SetTitle(const wchar_t * szTitle)
{
   if(m_wsTitle.compare(szTitle) != 0)
   {
      if(m_wsOldTitle.empty())
      {
         m_wsOldTitle = m_wsTitle;
      }
      m_wsTitle = szTitle;
      m_bChanged = true;
   }
}

std::wstring CNote::GetFullName()
{
   std::wstring wsFullName;
   CNote * pNote = this;
   while(pNote->GetParent())
   {
      wsFullName.insert(0, pNote->GetTitle().c_str());
      pNote = pNote->GetParent();
      if(pNote->GetParent())
      {
         wsFullName.insert(0, L"\\");
      }
   }
   return wsFullName;
}

bool CNote::IsChanged(std::wstring * o_pwsOldTitle /*= NULL*/)
{
   if(o_pwsOldTitle && !m_wsOldTitle.empty())
   {
      *o_pwsOldTitle = m_wsOldTitle;
   }
   return m_bChanged;
}

void CNote::SetSaved()
{
   m_bChanged = false;
   m_wsOldTitle.clear();
}

void CNote::SetText(const wchar_t * szText)
{
   m_wsText = szText;
   m_bChanged = true;
}

const NotePtrList & CNote::GetChildNotes()
{
   return m_ChildNotes;
}

CNote * CNote::AddChild(LPCWSTR wszTitle)
{
   CNote * pChild = new CNote(this);
   pChild->m_wsTitle = wszTitle;
   m_ChildNotes.push_back(pChild);
   return pChild;
}

CNote * CNote::AddNewChild(bool bAtEnd /*= true*/)
{
   std::wstring wsTitle = theApp.GetNotesDb()->GetNewNoteTitle(this);

   CNote * pChild = new CNote(this);
   pChild->m_wsTitle = wsTitle;
   if(bAtEnd)
   {
      m_ChildNotes.push_back(pChild);
   }
   else
   {
      m_ChildNotes.insert(m_ChildNotes.begin(), pChild);
   }
   return pChild;
}

void CNote::DeleteNote()
{
   NoteConstItr itCurrentNote = std::find(m_pParent->GetChildNotes().begin(), m_pParent->GetChildNotes().end(), this);

   theApp.GetNotesDb()->DeleteNote(this);
   
   m_pParent->m_ChildNotes.erase(itCurrentNote);

   delete this;
}

class PredSameTitle
{
public:
   PredSameTitle(const wchar_t * wszTitle)
      :m_wsTitle(wszTitle)
   {
   }
   bool operator()(CNote *pNoteOther)
   {
      return (_wcsicmp(m_wsTitle.c_str(), pNoteOther->GetTitle().c_str()) == 0);
   }

private:
   std::wstring m_wsTitle;
};

CNote * CNote::GetNote(const wchar_t * wszNotePath)
{
   CNote * pRetVal = NULL;
   if(wszNotePath)
   {
      // find root node
      CNote * pRoot = this;
      while(pRoot->GetParent() != NULL)
      {
         pRoot = pRoot->GetParent();
      }
      CNote * pChild = pRoot;

      // find subsequent child notes...
      std::wstring wsNotePath = wszNotePath;
      std::wstring::size_type First = 0, Last = 0;

      while(Last != std::wstring::npos)
      {
         Last = wsNotePath.find(L'\\', First);
         // child note title...
         std::wstring ws(wsNotePath, First, (Last==std::wstring::npos) ? Last : Last-First);
         //g_tr.trace(ws.c_str());
         First = Last+1;

         // find child note with title ws
         NoteConstItr it = std::find_if( pChild->GetChildNotes().begin(), pChild->GetChildNotes().end(), PredSameTitle(ws.c_str()));
         if(it != pChild->GetChildNotes().end())
         {
            // child note found
            pChild = *it;
         }
         else
         {
            // not found
            pChild = NULL;
            break;
         }
      }
      if( (Last = std::wstring::npos)  &&  pChild)
      {
         pRetVal = pChild;
      }
   }
   return pRetVal;
}