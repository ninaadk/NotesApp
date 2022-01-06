#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <memory>

typedef std::unique_ptr<BYTE []> UPBuffer; 

class TFile
{
public:
   TFile();
   virtual ~TFile();

   bool Create(LPCWSTR wstrPath);
   bool Open(LPCWSTR wstrPath, bool bWrite = false);
   bool OpenOrCreate(LPCWSTR wstrPath, bool bWrite = false);
   void Close();

   __int64 GetSize();
   bool ReadAll(BYTE * &o_buffer, DWORD &size);
   bool WriteAll(const BYTE * buffer, DWORD size);
   UPBuffer ReadAll(DWORD * puiSize);

   bool Read(BYTE * o_buffer, __int64 uiStartOffset, DWORD size);
   bool Write(const BYTE * buffer, __int64 uiStartOffset, DWORD size);

   bool SetPosition(DWORD dwMoveFrom, __int64 uiMoveBy); // FILE_BEGIN/FILE_CURRENT/FILE_END
   bool Read(BYTE * o_buffer, DWORD size);
   bool Write(const BYTE * buffer, DWORD size);

   bool SetEOF(__int64 uiPos = -1); // -1 means current position

   DWORD Error() {return m_dwError;}

protected:
   HANDLE m_handle;
   DWORD m_dwError;
};

class CTextFile : public TFile
{
public:
   CTextFile();
   ~CTextFile();
   
   std::wstring Read();
   bool Write(const std::wstring & wsContents, bool bAddBOM = true);
};