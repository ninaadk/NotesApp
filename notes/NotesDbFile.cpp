#include "StdAfx.h"
#include "NotesDbFile.h"
#include <list>
#include <algorithm>
#include "file.h"

constexpr LPCWSTR wszRootTitle = L"Root";

CNotesDbFile::CNotesDbFile(void)
   :m_ntRoot(NULL)
{
}

CNotesDbFile::~CNotesDbFile(void)
{
   delete m_ntRoot;
   m_ntRoot = NULL;
}

bool CNotesDbFile::Init(const wchar_t* wstrPath)
{
   m_ntRoot = new CNote(NULL);
   m_ntRoot->SetTitle(wszRootTitle);

   m_wsRootFolder = wstrPath;
   LoadEmptyNotes(m_ntRoot, wstrPath);

   // add new note if root has no child items
   if(m_ntRoot->GetChildNotes().empty())
   {
      // TODO: add common folders like 'cases', 'daily notes' etc
      m_ntRoot->AddNewChild();
   }

   return true;
}

bool CNotesDbFile::CreateNewDb(const wchar_t * wstrPath)
{
   // create .na folder
   std::wstring wsNAFolder = wstrPath;
   wsNAFolder += L"\\.na";

   if(FALSE != CreateDirectoryW(wsNAFolder.c_str(), NULL))
   {
      return Init(wstrPath);
   }
   else
   {
      return false;
   }
}

bool CNotesDbFile::IsNotesAppFolder(const wchar_t* wstrPath)
{
   // check if .na folder is present
   std::wstring wsNAFolder = wstrPath;
   wsNAFolder += L"\\.na";

   DWORD dwAttr = GetFileAttributesW(wsNAFolder.c_str());

   return (FILE_ATTRIBUTE_DIRECTORY == dwAttr);
}

bool CNotesDbFile::IsEmpty(const wchar_t* wstrPath)
{
   // check if any file is present
   bool bEmpty = true;
   std::wstring wsFileFilter = wstrPath;
   wsFileFilter += L"\\*";

   WIN32_FIND_DATAW fd = { 0 };
   HANDLE hFile = FindFirstFileExW(wsFileFilter.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, NULL, 0);
   if (INVALID_HANDLE_VALUE != hFile)
   {
      do
      {
         if ((0 == wcscmp(L".", fd.cFileName)) || (0 == wcscmp(L"..", fd.cFileName)))
         {
            continue;
         }
         bEmpty = false;
         break;
      }
      while (FindNextFileW(hFile, &fd));

      FindClose(hFile);
      hFile = INVALID_HANDLE_VALUE;
   }
   return bEmpty;
}

bool CNotesDbFile::IsWritable(const wchar_t* wstrPath)
{
   // try to create file/folder
   std::wstring wsTestFolder = wstrPath;
   wsTestFolder += L"\\test";

   bool bWritable = (FALSE != CreateDirectoryW(wsTestFolder.c_str(), NULL));

   if(bWritable)
   {
      RemoveDirectoryW(wsTestFolder.c_str());
   }
   return bWritable;
}

void CNotesDbFile::LoadEmptyNotes(CNote * pNote, LPCWSTR wszFolder)
{
   // enum files in folder
   // create child note for each file
   // enum folders
   // find/create note for each folder
   // call recursively for child note

   if (!pNote || !wszFolder)
   {
      return;
   }

   std::wstring  wsFileFilter = wszFolder;
   wsFileFilter += L"\\*";

   // enum files...
   WIN32_FIND_DATAW fd = { 0 };
   HANDLE hFile = FindFirstFileExW(wsFileFilter.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, NULL, 0);
   if (INVALID_HANDLE_VALUE != hFile)
   {
      do
      {
         if ((0 == wcscmp(L".", fd.cFileName)) || (0 == wcscmp(L"..", fd.cFileName)))
         {
            continue;
         }
         if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
         {
            CNote * pChildNote = pNote->AddChild(fd.cFileName);
         }
      }
      while (FindNextFileW(hFile, &fd));
      FindClose(hFile);
      hFile = INVALID_HANDLE_VALUE;
   }
   // enum directories...
   hFile = FindFirstFileExW(wsFileFilter.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, NULL, 0);
   if (INVALID_HANDLE_VALUE != hFile)
   {
      do
      {
         std::wstring wsFolder = fd.cFileName;
         if((wsFolder == L".") || (wsFolder == L".."))
         {
            continue;
         }
         if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
         {
            if(*wsFolder.rbegin() != L'_')
            {
               continue;
            }
            // check if note with same name exists.
            std::wstring wsFolderWithoutUnderscore(wsFolder.begin(), wsFolder.end()-1);
            CNote * pChildNote = NULL;
            for (NoteConstItr it = pNote->GetChildNotes().begin(); it != pNote->GetChildNotes().end(); ++it)
            {
               if((*it)->GetTitle() == wsFolderWithoutUnderscore)
               {
                  pChildNote = *it;
                  break;
               }
            }
            if (!pChildNote)
            {
               pChildNote = pNote->AddChild(wsFolderWithoutUnderscore.c_str());
            }
            std::wstring wsChildFolderPath = wszFolder;
            wsChildFolderPath = wsChildFolderPath + L"\\" + wsFolder;

            LoadEmptyNotes(pChildNote, wsChildFolderPath.c_str());
         }
      }
      while (FindNextFileW(hFile, &fd));
      FindClose(hFile);
      hFile = INVALID_HANDLE_VALUE;
   }
}

std::wstring CNotesDbFile::GetNotePath(CNote * pNote)
{
   std::wstring wsFolder;
   if(pNote)
   {
      while (pNote->GetParent())
      {
         wsFolder.insert(0, pNote->GetTitle());
         wsFolder.insert(0, L"\\");
         pNote = pNote->GetParent();
         if(pNote->GetParent())
         {
            wsFolder.insert(0, L"_");
         }
      }
   }
   wsFolder.insert(0, m_wsRootFolder);
   return wsFolder;
}

void CNotesDbFile::Load(CNote * pNote)
{
   // find file path from note
   // read text file and set text in note
   std::wstring wsPath = GetNotePath(pNote);
   std::wstring wsText;

   CTextFile file;
   if (file.Open(wsPath.c_str()))
   {
      wsText = file.Read();
   }

   pNote->SetText(wsText.c_str());
}

void CNotesDbFile::Save(CNote * pNote)
{
   std::wstring wsOldTitle;
   if(pNote->IsChanged(&wsOldTitle))
   {
      std::wstring wsPath = GetNotePath(pNote);
      std::wstring wsFolderPath = wsPath;
      wsFolderPath.erase(wsFolderPath.rfind(L'\\'), std::wstring::npos);

      WIN32_FIND_DATAW data = {0};
      HANDLE folder = FindFirstFileW(wsFolderPath.c_str(), &data);
      if(INVALID_HANDLE_VALUE == folder)
      {
         // create folder
         CreateDirectoryW(wsFolderPath.c_str(), NULL);
      }
      else
      {
         FindClose(folder);
      }

      CTextFile file;
      if (file.OpenOrCreate(wsPath.c_str(), true))
      {
         file.Write(pNote->GetText());
         file.Close();
      }

      // If title changed, rename old file and folder
      if(!wsOldTitle.empty())
      {
         std::wstring wsOldFile = wsFolderPath + L"\\" + wsOldTitle;
         DeleteFileW(wsOldFile.c_str());

         std::wstring wsOldFolder = wsFolderPath + L"\\" + wsOldTitle + L"_";
         std::wstring wsNewFolder = wsFolderPath + L"\\" + pNote->GetTitle() + L"_";
         MoveFileW(wsOldFolder.c_str(), wsNewFolder.c_str());
      }
      pNote->SetSaved();
   }
}

std::wstring CNotesDbFile::LoadMru()
{
   std::wstring wsMruFile = m_wsRootFolder + L"\\.na\\mru";

   std::wstring wsRetVal;
   CTextFile mrufile;
   if(mrufile.Open(wsMruFile.c_str()))
   {
      wsRetVal = mrufile.Read();
   }
   return wsRetVal;
}

bool CNotesDbFile::SaveMru(std::wstring & wsMru)
{
   std::wstring wsMruFile = m_wsRootFolder + L"\\.na\\mru";

   bool bRetVal = false;
   CTextFile mrufile;
   if(mrufile.OpenOrCreate(wsMruFile.c_str(), true))
   {
      bRetVal = mrufile.Write(wsMru, false);
   }
   return bRetVal;
}

CNote * CNotesDbFile::GetRoot()
{
   return m_ntRoot;
}

CNote * CNotesDbFile::GetNote(const wchar_t * wszNotePath)
{
   return m_ntRoot->GetNote(wszNotePath);
}

std::wstring CNotesDbFile::GetNewNoteTitle(CNote * pParentNote)
{
   std::wstring wsParentNotePath = GetNotePath(pParentNote);

   // check file names and find new name for file (note_1, note_2  etc)
   std::wstring wsFileFilter = wsParentNotePath + L"\\note_*";

   std::list<std::wstring> fileList;

   // enum files...
   WIN32_FIND_DATAW fd = { 0 };
   HANDLE hFile = FindFirstFileExW(wsFileFilter.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, NULL, 0);
   if (INVALID_HANDLE_VALUE != hFile)
   {
      do
      {
         if ((0 == wcscmp(L".", fd.cFileName)) || (0 == wcscmp(L"..", fd.cFileName)))
         {
            continue;
         }
         if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
         {
            fileList.push_back(fd.cFileName);
         }
      }
      while (FindNextFileW(hFile, &fd));
      FindClose(hFile);
      hFile = INVALID_HANDLE_VALUE;
   }

   // add any unsaved note to list so that same name is not repeated...
   for(NoteConstItr it = pParentNote->GetChildNotes().begin();
      it != pParentNote->GetChildNotes().end();
      ++it)
   {
      fileList.push_back((*it)->GetTitle());
   }

   int nFileNo = 1;
   wchar_t szFileName[64] = {0};
   std::wstring wsDefaultTitle;
   while(true)
   {
      wsprintfW(szFileName, L"note_%d.txt", nFileNo++);
      bool bFound = ( std::find(fileList.begin(), fileList.end(), szFileName) != fileList.end());
      if(!bFound)
      {
         wsDefaultTitle = szFileName;
         break;
      }
   }
   return wsDefaultTitle;
}

void CNotesDbFile::DeleteDirectory(LPCWSTR wszDir)
{
   if(wszDir)
   {
      std::wstring wsFileFilter = wszDir;
      wsFileFilter += L"\\*";

      // enum directories...
      WIN32_FIND_DATAW fd = { 0 };
      HANDLE hFile = FindFirstFileExW(wsFileFilter.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, NULL, 0);
      if (INVALID_HANDLE_VALUE != hFile)
      {
         do
         {
            std::wstring wsFolder = fd.cFileName;
            if((wsFolder == L".") || (wsFolder == L".."))
            {
               continue;
            }
            std::wstring wsFileName = wszDir;
            wsFileName += L"\\";
            wsFileName += fd.cFileName;
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
            {
               DeleteDirectory(wsFileName.c_str());
            }
            else
            {
               DeleteFileW(wsFileName.c_str());
            }
         
         }
         while (FindNextFileW(hFile, &fd));
         FindClose(hFile);
         hFile = INVALID_HANDLE_VALUE;
      }
      RemoveDirectoryW(wszDir);
   }
}


void CNotesDbFile::DeleteNote(CNote * pNote)
{
   if(pNote)
   {
      std::wstring wsNotePath = GetNotePath(pNote);

      DeleteFileW(wsNotePath.c_str());

      if(! pNote->GetChildNotes().empty())
      {
         std::wstring wsFolderPath = wsNotePath + L"_";

         DeleteDirectory(wsFolderPath.c_str());
      }
   }
}
