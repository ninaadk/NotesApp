#include "stdafx.h"
#include "file.h"


//-----------------------------------------------------------------------------------------------
// TFile class
//-----------------------------------------------------------------------------------------------

TFile::TFile()
   :m_handle(INVALID_HANDLE_VALUE),
   m_dwError(0)
{

}

TFile::~TFile()
{
   Close();
}

bool TFile::Create(LPCWSTR wstrPath)
{
   bool bRetVal = false;

   if((m_handle == INVALID_HANDLE_VALUE) && (wstrPath != NULL))
   {
      m_handle = CreateFileW(wstrPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
      m_dwError = GetLastError();
      bRetVal = (m_handle != INVALID_HANDLE_VALUE);
   }

   return bRetVal;
}

bool TFile::Open(LPCWSTR wstrPath, bool bWrite /*= false*/)
{
   bool bRetVal = true;

   if((m_handle == INVALID_HANDLE_VALUE) && (wstrPath != NULL))
   {
      DWORD dwAccess = bWrite ? (GENERIC_READ|GENERIC_WRITE) : GENERIC_READ;

      m_handle = CreateFileW(wstrPath, dwAccess, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
      m_dwError = GetLastError();
      bRetVal = (m_handle != INVALID_HANDLE_VALUE);
   }

   return bRetVal;
}

bool TFile::OpenOrCreate(LPCWSTR wstrPath, bool bWrite /*= false*/)
{
   bool bRetVal = true;

   if((m_handle == INVALID_HANDLE_VALUE) && (wstrPath != NULL))
   {
      DWORD dwAccess = bWrite ? (GENERIC_READ|GENERIC_WRITE) : GENERIC_READ;

      m_handle = CreateFileW(wstrPath, dwAccess, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
      m_dwError = GetLastError();
      bRetVal = (m_handle != INVALID_HANDLE_VALUE);
   }

   return bRetVal;
}

void TFile::Close()
{
   if(m_handle != INVALID_HANDLE_VALUE)
   {
      CloseHandle(m_handle);
      m_handle = INVALID_HANDLE_VALUE;
   }
}

__int64 TFile::GetSize()
{
   LARGE_INTEGER liRetVal = {0};

   if(m_handle != INVALID_HANDLE_VALUE)
   {
      if(!GetFileSizeEx(m_handle, &liRetVal))
      {
         m_dwError = GetLastError();
      }
   }
   return liRetVal.QuadPart;
}

bool TFile::ReadAll(BYTE * &o_buffer, DWORD &size)
{
   bool bRetVal = false;

   if(INVALID_HANDLE_VALUE != m_handle)
   {
      __int64 nSize = GetSize();
      if(nSize > 0)
      {
         BYTE * pBuffer = new BYTE[(size_t)nSize];
         if(pBuffer)
         {
            if(Read(pBuffer, 0, (DWORD)nSize))
            {
               o_buffer = pBuffer;
               size = (DWORD)nSize;
               bRetVal = true;
            }
            else
            {
               o_buffer = NULL;
               size = 0;
               delete [] pBuffer;
            }
         }
      }
   }
   return bRetVal;
}

UPBuffer TFile::ReadAll(DWORD * puiSize)
{
   BYTE * pBuffer = NULL;
   DWORD uiSize = 0;
   *puiSize = 0;

   if(ReadAll(pBuffer, uiSize))
   {
      *puiSize = uiSize;
   }

   return UPBuffer(pBuffer);
}

bool TFile::WriteAll(const BYTE * buffer, DWORD size)
{
   bool bRetVal = false;

   if((INVALID_HANDLE_VALUE != m_handle) && (size > 0))
   {
      if(SetPosition(FILE_BEGIN, 0))
      {
         if(Write(buffer, size))
         {
            bRetVal = SetEOF();
         }
      }
   }
   return bRetVal;
}

bool TFile::Read(BYTE * o_buffer, __int64 uiStartOffset, DWORD size)
{
   bool bRetVal = false;

   if(o_buffer && (size>0) && (uiStartOffset>=0) && (INVALID_HANDLE_VALUE != m_handle))
   {
      if(SetPosition(FILE_BEGIN, uiStartOffset))
      {
         bRetVal = Read(o_buffer, size);
      }
   }
   
   return bRetVal;
}

bool TFile::Write(const BYTE * buffer, __int64 uiStartOffset, DWORD size)
{
   bool bRetVal = false;

   if(buffer && (size>0) && (uiStartOffset>=0) && (INVALID_HANDLE_VALUE != m_handle))
   {
      if(SetPosition(FILE_BEGIN, uiStartOffset))
      {
         bRetVal = Write(buffer, size);
      }
   }
   
   return bRetVal;
}

bool TFile::SetPosition(DWORD dwMoveFrom, __int64 uiMoveBy)
{
   bool bRetVal = false;

   if(INVALID_HANDLE_VALUE != m_handle)
   {
      LARGE_INTEGER li;
      li.QuadPart = uiMoveBy;
      if( FALSE != SetFilePointerEx(m_handle, li, NULL, dwMoveFrom))
      {
         bRetVal = true;
      }
      else
      {
         m_dwError = GetLastError();
      }
   }
   
   return bRetVal;
}

bool TFile::Read(BYTE * o_buffer, DWORD size)
{
   bool bRetVal = false;

   if((INVALID_HANDLE_VALUE != m_handle) && (o_buffer != NULL) && (size > 0))
   {
      DWORD dwBytesRead = 0;
      
      if(ReadFile(m_handle, o_buffer, size, &dwBytesRead, NULL))
      {
         bRetVal = true;
      }
      else
      {
         m_dwError = GetLastError();
      }
   }
   
   return bRetVal;
}

bool TFile::Write(const BYTE * buffer, DWORD size)
{
   bool bRetVal = false;

   if((INVALID_HANDLE_VALUE != m_handle) && (buffer != NULL) && (size > 0))
   {
      DWORD dwBytesRead = 0;
      
      if(WriteFile(m_handle, buffer, size, &dwBytesRead, NULL))
      {
         bRetVal = true;
      }
      else
      {
         m_dwError = GetLastError();
      }
   }
   
   return bRetVal;
}

bool TFile::SetEOF(__int64 uiPos /*= -1*/)
{
   bool bRetVal = false;

   if(INVALID_HANDLE_VALUE != m_handle)
   {
      bool bPosSet = true;
      if(uiPos != -1)
      {
         bPosSet = SetPosition(FILE_BEGIN, uiPos);
      }

      if(bPosSet)
      {
         bRetVal = (FALSE != SetEndOfFile(m_handle));
      }
      m_dwError = GetLastError();
   }
   
   return bRetVal;
}


//-----------------------------------------------------------------------------------------------
// CTextFile class
//-----------------------------------------------------------------------------------------------

CTextFile::CTextFile()
{
}

CTextFile::~CTextFile()
{
}

std::wstring CTextFile::Read()
{
   std::wstring wsRetVal;

   if(m_handle != INVALID_HANDLE_VALUE)
   {
      __int64 uiFileSize = GetSize();

      if(uiFileSize > 0)
      {
         __int64 uiBufferSize = uiFileSize + sizeof(wchar_t);  // zero char at end

         BYTE * pBuffer = new BYTE[(size_t)uiBufferSize];
         if(pBuffer)
         {
            pBuffer[uiBufferSize-1] = 0;
            pBuffer[uiBufferSize-2] = 0;

            if(TFile::Read(pBuffer, 0, (DWORD)uiFileSize))
            {
               wchar_t * wstrContents = (wchar_t *) pBuffer;
               if(wstrContents[0] == (wchar_t)0xfeff)
               {
                  ++wstrContents;
               }
               wsRetVal = wstrContents;
            }
            delete [] pBuffer;
         }
      }
   }
   return wsRetVal;
}

bool CTextFile::Write(const std::wstring & wsContents, bool bAddBOM /*= true*/)
{
   bool bRetVal = false;
   
   if(m_handle != INVALID_HANDLE_VALUE)
   {
      if(TFile::SetPosition(FILE_BEGIN, 0))
      {
         if(!wsContents.empty())
         {
            bool bBomAdded = true;
            if(bAddBOM)
            {
               wchar_t cBom = 0xfeff;
               bBomAdded = TFile::Write((const BYTE *)&cBom, 0, 2);
            }
            if(bBomAdded)
            {
               if(TFile::Write((const BYTE *)wsContents.c_str(), (DWORD)(sizeof(wchar_t)*wsContents.size())) )
               {
                  bRetVal = TFile::SetEOF();
               }
            }
         }
         else
         {
            bRetVal = TFile::SetEOF();
         }
      }
   }

   return bRetVal;
}