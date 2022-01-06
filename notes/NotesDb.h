#pragma once

#include "Note.h"

// interface for notes database.
class CNotesDb
{
public:
   // NotesApp database
   virtual bool Init(const wchar_t * wstrPath) = 0;
   virtual bool CreateNewDb(const wchar_t * wstrPath) = 0;

   // selected note in DB
   virtual void Load(CNote * pNote) = 0;
   virtual void Save(CNote * pNote) = 0;

   // MRU
   virtual std::wstring LoadMru() = 0;
   virtual bool         SaveMru(std::wstring & wsMru) = 0;

   // select note in memory
   virtual CNote * GetRoot() = 0;
   virtual CNote * GetNote(const wchar_t * wszNotePath) = 0;
   virtual std::wstring GetNewNoteTitle(CNote * pParentNote) = 0;

   // delete selected note from DB
   virtual void  DeleteNote(CNote * pNote) = 0;
};

