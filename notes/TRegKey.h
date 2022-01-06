#pragma once
#include <Windows.h>
#include <string>
#include <vector>

class TRegKey
{
private:
   HKEY m_hParent;
   HKEY m_hKey;
   std::wstring m_wsKey;
   LSTATUS m_status;
   REGSAM m_sam;

public:
   TRegKey(HKEY hParent = HKEY_LOCAL_MACHINE);
   ~TRegKey();

   HKEY Handle()
   {
      return m_hKey;
   }
   
   LSTATUS LastError()
   {
      return m_status;
   }

   void Use32BitKeys()
   {
      m_sam = KEY_WOW64_32KEY;
   }

   void Use64BitKeys()
   {
      m_sam = KEY_WOW64_64KEY;
   }

   bool Create(LPCWSTR wstrKey);
   bool Open(LPCWSTR wstrKey, bool bWrite = false);
   bool Close();
   bool Delete();
   static bool DeleteKey(HKEY hParent, LPCWSTR wstrKey, REGSAM sam = 0);

   std::vector<std::wstring>  EnumSubKeys();

   std::vector< std::pair<std::wstring, DWORD> > EnumVals();
   
   bool                       DeleteVal(LPCWSTR wstrVal);

   bool ReadDwordVal       (LPCWSTR wstrVal, DWORD & dwVal);
   bool ReadQwordVal       (LPCWSTR wstrVal, __int64 & qwVal);
   bool ReadStringVal      (LPCWSTR wstrVal, std::wstring & wsVal);
   bool ReadExpandSzVal    (LPCWSTR wstrVal, std::wstring & wsVal);
   bool ReadMultiStringVal (LPCWSTR wstrVal, std::vector<std::wstring> & mstrVal);
   bool ReadBinaryVal      (LPCWSTR wstrVal, std::vector<BYTE> & binVal);
   bool ReadBinaryVal      (LPCWSTR wstrVal, std::unique_ptr<BYTE []> & binVal, DWORD & dwSize);

   bool WriteDwordVal       (LPCWSTR wstrVal, DWORD dwVal);
   bool WriteQwordVal       (LPCWSTR wstrVal, __int64 qwVal);
   bool WriteStringVal      (LPCWSTR wstrVal, std::wstring wsVal);
   bool WriteExpandSzVal    (LPCWSTR wstrVal, std::wstring wsVal);
   bool WriteMultiStringVal (LPCWSTR wstrVal, const std::vector<std::wstring> & mstrVal);
   bool WriteBinaryVal      (LPCWSTR wstrVal, const std::vector<BYTE> & binVal);
   bool WriteBinaryVal      (LPCWSTR wstrVal, const BYTE * binVal, DWORD dwSize);

   bool CopyFrom(HKEY hkCopyFrom);  // vista and above
   bool DeleteTree();  // vista and above
   static bool RenameKey(HKEY hParent, LPCWSTR wstrOld, LPCWSTR wstrNew);  // vista and above

private:

   bool ReadValue(LPCWSTR wstrVal, DWORD dwType, BYTE* &o_buffer, DWORD & o_bufferSize);
   
   bool WriteVal(LPCWSTR wstrVal, DWORD dwType, const BYTE * pVal, DWORD size);
};

