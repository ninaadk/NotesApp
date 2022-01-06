#pragma once
#include "notesdb.h"
class CNotesDbFile :
   public CNotesDb
{
public:
   CNotesDbFile(void);
   ~CNotesDbFile(void);

   bool Init(const wchar_t* wstrPath);
   bool CreateNewDb(const wchar_t * wstrPath);
   
   void Load(CNote * pNote);
   void Save(CNote * pNote);
   
   std::wstring LoadMru();
   bool         SaveMru(std::wstring & wsMru);

   CNote * GetRoot();
   CNote * GetNote(const wchar_t * wszNotePath);
   std::wstring GetNewNoteTitle(CNote * pParentNote);

   void  DeleteNote(CNote * pNote);

   bool IsNotesAppFolder(const wchar_t* wstrPath);
   bool IsEmpty(const wchar_t* wstrPath);
   bool IsWritable(const wchar_t* wstrPath);

private:
   std::wstring m_wsRootFolder;
   CNote * m_ntRoot;

   std::wstring GetNotePath(CNote * pNote);
   void LoadEmptyNotes(CNote * pNote, LPCWSTR wszFolder);
   void DeleteDirectory(LPCWSTR wszDir);
};