#include "StdAfx.h"
#include "TRegKey.h"


TRegKey::TRegKey(HKEY hParent /* = HKEY_LOCAL_MACHINE */)
   :m_hParent(hParent),
   m_hKey(NULL),
   m_sam(0),
   m_status(0)
{
}


TRegKey::~TRegKey()
{
   Close();
}

bool TRegKey::Create(LPCWSTR wstrKey)
{
   bool bRetVal = false;
   Close();

   if(wstrKey)
   {
      m_status = RegCreateKeyExW(m_hParent, wstrKey, 0, NULL, 0, KEY_ALL_ACCESS|m_sam, NULL, &m_hKey, NULL);
      
      if(ERROR_SUCCESS == m_status)
      {
         m_wsKey = wstrKey;
         bRetVal = true;
      }
      else
      {
         m_hKey = NULL;
      }
   }

   return bRetVal;
}

bool TRegKey::Open(LPCWSTR wstrKey, bool bWrite /* = false */)
{
   bool bRetVal = false;
   Close();

   if(wstrKey)
   {
      REGSAM sam = bWrite ? (KEY_READ | KEY_WRITE | DELETE) : KEY_READ;
      sam |= m_sam;

      m_status = RegOpenKeyExW(m_hParent, wstrKey, 0, sam, &m_hKey);
      
      if(ERROR_SUCCESS == m_status)
      {
         m_wsKey = wstrKey;
         bRetVal = true;
      }
      else
      {
         m_hKey = NULL;
      }
   }

   return bRetVal;
}

bool TRegKey::Close()
{
   if(m_hKey)
   {
      m_status = RegCloseKey(m_hKey);
      m_hKey = NULL;
   }
   else
   {
      m_status = ERROR_SUCCESS;
   }
   
   m_wsKey.clear();

   return (ERROR_SUCCESS == m_status);
}

bool TRegKey::Delete()
{
   bool bRetVal = false;

   if(m_hKey)
   {
      m_status = RegCloseKey(m_hKey);
   
      if(ERROR_SUCCESS == m_status)
      {
         m_hKey = NULL;
         m_status = RegDeleteKeyExW(m_hParent, m_wsKey.c_str(), m_sam, 0);

         bRetVal = (ERROR_SUCCESS == m_status);

         m_wsKey.clear();
      }
   }

   return bRetVal;
}

bool TRegKey::DeleteKey(HKEY hParent, LPCWSTR wstrKey, REGSAM sam /* = 0 */ )
{
   bool bRetVal = false;

   bRetVal = ( ERROR_SUCCESS == RegDeleteKeyExW(hParent, wstrKey, sam, 0));

   return bRetVal;
}

std::vector<std::wstring> TRegKey::EnumSubKeys()
{
   std::vector<std::wstring> retval;

   if(m_hKey)
   {
      DWORD dwIndex = 0, dwBufferSize = 512;
      wchar_t * wstrName = new wchar_t[512];
      m_status = ERROR_SUCCESS;

      while(m_status == ERROR_SUCCESS)
      {
         dwBufferSize = 512;
         m_status = RegEnumKeyExW(m_hKey, dwIndex++, wstrName, &dwBufferSize, NULL, NULL, NULL, NULL);

         if(ERROR_SUCCESS == m_status)
         {
            retval.push_back(wstrName);
         }
      }

      delete [] wstrName;
      wstrName = NULL;
   }

   return retval;
}


bool TRegKey::ReadDwordVal(LPCWSTR wstrVal, DWORD & dwVal)
{
   bool bRetVal = false;
   
   if(m_hKey)
   {
      DWORD dwType = 0, dwBuffer = 0, cbData = sizeof(DWORD);

      m_status = RegQueryValueExW(m_hKey, wstrVal, NULL, &dwType, (LPBYTE)&dwBuffer, &cbData);

      if((ERROR_SUCCESS == m_status) && (dwType == REG_DWORD))
      {
         dwVal = dwBuffer;
         bRetVal = true;
      }
   }

   return bRetVal;
}

bool TRegKey::ReadQwordVal(LPCWSTR wstrVal, __int64 & qwVal)
{
   bool bRetVal = false;
   
   if(m_hKey)
   {
      DWORD dwType = 0, cbData = sizeof(__int64);
      __int64 qwBuffer = 0;

      m_status = RegQueryValueExW(m_hKey, wstrVal, NULL, &dwType, (LPBYTE)&qwBuffer, &cbData);

      if((ERROR_SUCCESS == m_status) && (dwType == REG_QWORD))
      {
         qwVal = qwBuffer;
         bRetVal = true;
      }
   }

   return bRetVal;
}

bool TRegKey::ReadValue(LPCWSTR wstrVal, DWORD dwType, BYTE* &o_buffer, DWORD & o_bufferSize)
{
   bool bRetVal = false;
   
   if(m_hKey && wstrVal)
   {
      DWORD dwTypeRead = 0, cbData = 0;

      m_status = RegQueryValueExW(m_hKey, wstrVal, NULL, &dwTypeRead, NULL, &cbData);
      
      if((ERROR_SUCCESS == m_status) && (dwTypeRead == dwType) && (cbData>0))
      {
         BYTE * pBuffer = new BYTE[cbData];
         memset(pBuffer, 0, cbData);

         m_status = RegQueryValueExW(m_hKey, wstrVal, NULL, &dwTypeRead, pBuffer, &cbData);

         if(ERROR_SUCCESS == m_status)
         {
            o_buffer     = pBuffer;
            o_bufferSize = cbData;
            bRetVal      = true;
         }
         else
         {
            delete pBuffer;
            pBuffer = NULL;
         }
      }
   }
   return bRetVal;
}

bool TRegKey::ReadStringVal(LPCWSTR wstrVal, std::wstring & wsVal)
{
   bool bRetVal = false;

   BYTE * pBuffer = NULL;
   DWORD dwBufferSize = 0;

   if( ReadValue(wstrVal, REG_SZ, pBuffer, dwBufferSize))
   {
      wsVal.clear();
      wsVal.assign((wchar_t *)pBuffer, dwBufferSize/sizeof(wchar_t));
      
      if(wsVal[wsVal.length()-1] != L'\0')
      {
         wsVal += L'\0';
      }

      delete pBuffer;
      pBuffer = NULL;
      bRetVal = true;
   }
   
   return bRetVal;
}

bool TRegKey::ReadExpandSzVal(LPCWSTR wstrVal, std::wstring & wsVal)
{
   bool bRetVal = false;

   BYTE * pBuffer = NULL;
   DWORD dwBufferSize = 0;

   if( ReadValue(wstrVal, REG_EXPAND_SZ, pBuffer, dwBufferSize))
   {
      wsVal.clear();
      wsVal.assign((wchar_t *)pBuffer, dwBufferSize/sizeof(wchar_t));
      
      if(wsVal[wsVal.length()-1] != L'\0')
      {
         wsVal += L'\0';
      }

      delete pBuffer;
      pBuffer = NULL;
      bRetVal = true;
   }
   
   return bRetVal;
}

bool TRegKey::ReadMultiStringVal(LPCWSTR wstrVal, std::vector<std::wstring> & mstrVal)
{
   bool bRetVal = false;

   BYTE * pBuffer = NULL;
   DWORD dwBufferSize = 0;

   if( ReadValue(wstrVal, REG_MULTI_SZ, pBuffer, dwBufferSize))
   {
      wchar_t * p     = (wchar_t *)pBuffer;
      wchar_t * pLast = p + (dwBufferSize/sizeof(wchar_t)) - 1;

      if(*pLast == L'\0')
      {
         while((*p != L'\0') && (p<pLast))
         {
            std::wstring wsTemp = p;

            mstrVal.push_back(wsTemp);

            p += (wsTemp.length() + 1);
         }
      }
      delete pBuffer;
      pBuffer = NULL;
      bRetVal = true;
   }
   return bRetVal;
}

bool TRegKey::ReadBinaryVal(LPCWSTR wstrVal, std::vector<BYTE> & binVal)
{
   bool bRetVal = false;

   BYTE * pBuffer = NULL;
   DWORD dwBufferSize = 0;

   if( ReadValue(wstrVal, REG_BINARY, pBuffer, dwBufferSize))
   {
      binVal.assign(pBuffer, pBuffer+dwBufferSize);

      delete pBuffer;
      pBuffer = NULL;

      bRetVal = true;
   }

   return bRetVal;
}

bool TRegKey::ReadBinaryVal(LPCWSTR wstrVal, std::unique_ptr<BYTE []> & binVal, DWORD & dwSize)
{
   bool bRetVal = false;

   BYTE * pBuffer = NULL;
   DWORD dwBufferSize = 0;

   if( ReadValue(wstrVal, REG_BINARY, pBuffer, dwBufferSize))
   {
      binVal.reset(pBuffer);
      dwSize = dwBufferSize;
      bRetVal = true;
   }

   return bRetVal;
}

bool TRegKey::WriteVal(LPCWSTR wstrVal, DWORD dwType, const BYTE * pVal, DWORD size)
{
   bool bRetVal = false;

   if(m_hKey && wstrVal && pVal)
   {
      m_status = RegSetValueExW(m_hKey, wstrVal, 0, dwType, pVal, size);

      bRetVal = (ERROR_SUCCESS == m_status);
   }

   return bRetVal;
}

bool TRegKey::WriteDwordVal(LPCWSTR wstrVal, DWORD dwVal)
{
   return WriteVal(wstrVal, REG_DWORD, (BYTE *)&dwVal, sizeof(DWORD));
}

bool TRegKey::WriteQwordVal(LPCWSTR wstrVal, __int64 qwVal)
{
   return WriteVal(wstrVal, REG_QWORD, (BYTE *)&qwVal, sizeof(__int64));
}

bool TRegKey::WriteStringVal(LPCWSTR wstrVal, std::wstring wsVal)
{
   return WriteVal(wstrVal, REG_SZ, (BYTE *)wsVal.c_str(), (DWORD) (sizeof(wchar_t)*(wsVal.length()+1)) );
}

bool TRegKey::WriteExpandSzVal(LPCWSTR wstrVal, std::wstring wsVal)
{
   return WriteVal(wstrVal, REG_EXPAND_SZ, (BYTE *)wsVal.c_str(), (DWORD) (sizeof(wchar_t)*(wsVal.length()+1)) );
}

bool TRegKey::WriteMultiStringVal(LPCWSTR wstrVal, const std::vector<std::wstring> & mstrVal)
{
   std::wstring wsMultiStr;

   for(std::vector<std::wstring>::const_iterator it=mstrVal.begin(); it!=mstrVal.end(); ++it)
   {
      wsMultiStr.append(it->begin(), it->end());
      wsMultiStr.push_back(L'\0');
   }
   wsMultiStr.push_back(L'\0');

   return WriteVal(wstrVal, REG_MULTI_SZ, (const BYTE *)wsMultiStr.c_str(), (DWORD) (wsMultiStr.size() * sizeof(wchar_t)) );
}

bool TRegKey::WriteBinaryVal(LPCWSTR wstrVal, const std::vector<BYTE> & binVal)
{
   return WriteVal(wstrVal, REG_BINARY, (const BYTE *)&binVal[0], (DWORD)binVal.size());
}

bool TRegKey::WriteBinaryVal(LPCWSTR wstrVal, const BYTE * binVal, DWORD dwSize)
{
   return WriteVal(wstrVal, REG_BINARY, binVal, dwSize);
}

bool TRegKey::DeleteVal(LPCWSTR wstrVal)
{
   bool bRetVal = false;

   if(wstrVal && m_hKey)
   {
      m_status = RegDeleteValueW(m_hKey, wstrVal);

      bRetVal = (ERROR_SUCCESS == m_status);
   }
   
   return bRetVal;
}

std::vector<std::pair<std::wstring, DWORD> > TRegKey::EnumVals()
{
   std::vector<std::pair<std::wstring, DWORD> > RetVal;

   if(m_hKey)
   {
      wchar_t * pBuffer = (wchar_t *) new BYTE[32768];
      DWORD dwSizeInChar = 32768/sizeof(wchar_t);
      int index = 0;
      m_status = ERROR_SUCCESS;

      while(m_status == ERROR_SUCCESS)
      {
         DWORD dwSize = dwSizeInChar, dwType = 0;
         m_status = RegEnumValueW(m_hKey, index++, (LPWSTR)pBuffer, &dwSize, NULL, &dwType, NULL, NULL);

         if(ERROR_SUCCESS == m_status)
         {
            pBuffer[dwSize] = L'\0';
             
            RetVal.push_back( std::pair<std::wstring, DWORD> (pBuffer, dwType));
         }
      }
      delete []pBuffer;
   }

   return RetVal;
}

bool TRegKey::CopyFrom(HKEY hkCopyFrom)
{
   bool bRetVal = false;

   if(m_hKey && hkCopyFrom)
   {
      m_status = RegCopyTreeW(hkCopyFrom, NULL, m_hKey);

      bRetVal = (ERROR_SUCCESS == m_status);
   }

   return bRetVal;
}

bool TRegKey::DeleteTree()
{
   bool bRetval = false;

   if(m_hKey)
   {
      m_status = RegCloseKey(m_hKey);
      if(ERROR_SUCCESS == m_status)
      {
         m_status = RegDeleteTreeW(m_hParent, m_wsKey.c_str());
      
         if(ERROR_SUCCESS == m_status)
         {
            bRetval = true;
            m_hKey = NULL;
            m_wsKey.clear();
         }
      }
   }
   return bRetval;
}

bool TRegKey::RenameKey(HKEY hParent, LPCWSTR wstrOld, LPCWSTR wstrNew)
{
   bool bRetVal = false;

   TRegKey rkOld(hParent);

   if(rkOld.Open(wstrOld, true))
   {
      TRegKey rkNew(hParent);

      if(rkNew.Create(wstrNew))
      {
         if(rkNew.CopyFrom(rkOld.Handle()))
         {
            bRetVal = rkOld.DeleteTree();
         }
      }
   }
   return bRetVal;
}