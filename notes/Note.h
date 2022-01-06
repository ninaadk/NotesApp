#pragma once
#include <string>
#include <vector>

class CNote;

typedef std::vector<CNote *>        NotePtrList;
typedef NotePtrList::iterator       NoteItr;
typedef NotePtrList::const_iterator NoteConstItr;

class CNote
{
public:
   CNote(CNote * pParent);
   ~CNote(void);

   CNote *              GetParent() {return m_pParent;}
   const NotePtrList &  GetChildNotes();
   
   std::wstring GetTitle() {return m_wsTitle;}
   std::wstring GetText();
   void SetTitle(const wchar_t * szTitle);
   void SetText(const wchar_t * szText);
   std::wstring GetFullName();

   bool IsChanged(std::wstring * o_pwsOldTitle = NULL);
   void SetSaved();
   CNote * AddChild(LPCWSTR wszTitle);
   CNote * AddNewChild(bool bAtEnd = true);
   void    DeleteNote();
   CNote * GetNote(const wchar_t * wszNotePath);
   
private:
   bool m_bLoaded;
   bool m_bChanged;

   std::wstring m_wsTitle;
   std::wstring m_wsText;
   std::wstring m_wsOldTitle;
   //BYTE * m_data;

   CNote * m_pParent;
   NotePtrList m_ChildNotes;
};