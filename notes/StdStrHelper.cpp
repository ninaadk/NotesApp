#include "stdafx.h"
#include "StdStrHelper.h"
#include <algorithm>
#include <cctype>
#include <Windows.h>



void StdStr::ToUpper(std::wstring & ws)
{
   std::transform(ws.begin(), ws.end(), ws.begin(), std::toupper);
}


void StdStr::ToUpper(std::string  & s)
{
   std::transform(s.begin(), s.end(), s.begin(), std::toupper);
}


void StdStr::ToLower(std::wstring & ws)
{
   std::transform(ws.begin(), ws.end(), ws.begin(), std::tolower);
}


void StdStr::ToLower(std::string  & s)
{
   std::transform(s.begin(), s.end(), s.begin(), std::tolower);
}


std::string  StdStr::ToUtf8(const std::wstring & ws)
{
   std::string str;

   int nSizeInchars = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.length(), NULL, 0, NULL, NULL);

   if(nSizeInchars > 0)
   {
      char * buffer = new char[nSizeInchars+1];
      memset(buffer, 0, nSizeInchars+1);

      WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.length(), buffer, nSizeInchars, NULL, NULL);

      str = buffer;
      delete buffer;
   }

   return str;
}


std::wstring StdStr::ToWideChar(const std::string & s)
{
   std::wstring wstr;

   int nSizeInchars = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.length(), NULL, 0);

   if(nSizeInchars > 0)
   {
      wchar_t * buffer = new wchar_t[nSizeInchars + 1];
      memset(buffer, 0, sizeof(wchar_t)*(nSizeInchars+1));

      MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.length(), buffer, nSizeInchars);

      wstr = buffer;
      delete buffer;
   }

   return wstr;
}


//std::vector<std::wstring> StdStr::Tokanize(const std::wstring & ws, const wchar_t * wsSeperators)
//{
//   std::vector<std::wstring> RetVal;
//
//   std::wstring::const_iterator itStart = ws.begin(), itEnd;
//   std::wstring::size_type pos = 0;
//
//   while(true)
//   {
//      pos = ws.find_first_of(wsSeperators, pos);
//
//      if(pos == std::wstring::npos)
//      {
//         RetVal.push_back(std::wstring(itStart, ws.end()));
//         break;
//      }
//      else
//      {
//         RetVal.push_back(std::wstring(itStart, ws.begin()+pos));
//         ++pos;
//         itStart = ws.begin()+pos;
//      }
//   }
//
//   return RetVal;
//}

std::vector<std::wstring> StdStr::Tokanize(const std::wstring & ws, const wchar_t * wsSeperators)
{
   std::vector<std::wstring> RetVal;

   std::wstring::size_type posStart=0, posEnd=0;

   while(posStart != std::wstring::npos)
   {
      std::wstring::size_type posFound = ws.find_first_of(wsSeperators, posStart);

      posEnd = (posFound != std::wstring::npos) ? posFound : ws.length();

      RetVal.push_back(std::wstring(&ws[posStart], &ws[posEnd]));

      posStart = (posFound != std::wstring::npos) ? posFound+1 : std::wstring::npos;
   }

   return RetVal;
}

std::vector<std::wstring> StdStr::TokanizeByWord(const std::wstring & ws, const std::wstring & wsSeparator)
{
   std::vector<std::wstring> RetVal;

   if (ws.length() > 0)
   {
      std::wstring::size_type posStart = 0, posEnd = 0;

      while (posStart != std::wstring::npos)
      {

         std::wstring::size_type posFound = ws.find(wsSeparator, posStart);

         posEnd = (posFound != std::wstring::npos) ? posFound : ws.length();

         RetVal.push_back(std::wstring(&ws[posStart], &ws[posEnd]));

         posStart = (posFound != std::wstring::npos) ? posFound + wsSeparator.size() : std::wstring::npos;
      }
   }

   return RetVal;
}


std::vector<std::string>  StdStr::Tokanize(const std::string  & s,  const char * sSeperators)
{
   std::vector<std::string> RetVal;

   std::string::size_type posStart=0, posEnd=0;

   while(posStart != std::string::npos)
   {
      std::string::size_type posFound = s.find_first_of(sSeperators, posStart);

      posEnd = (posFound != std::string::npos) ? posFound : s.length();

      RetVal.push_back(std::string(&s[posStart], &s[posEnd]));

      posStart = (posFound != std::string::npos) ? posFound+1 : std::wstring::npos;
   }

   return RetVal;
}

std::vector<std::string> StdStr::TokanizeByWord(const std::string & s, const std::string & sSeparator)
{
   std::vector<std::string> RetVal;

   if (s.length() > 0)
   {
      std::string::size_type posStart = 0, posEnd = 0;

      while (posStart != std::string::npos)
      {
         std::string::size_type posFound = s.find(sSeparator, posStart);

         posEnd = (posFound != std::string::npos) ? posFound : s.length();

         RetVal.push_back(std::string(&s[posStart], &s[posEnd]));

         posStart = (posFound != std::string::npos) ? posFound + sSeparator.size() : std::string::npos;
      }
   }

   return RetVal;
}



void StdStr::Replace(std::wstring &ws, const wchar_t * wstrOrig, const wchar_t * wstrNew)
{
   if(wstrOrig && wstrNew)
   {
      std::wstring::size_type pos = 0;
   
      std::wstring::size_type lenOrig = wcslen(wstrOrig);
      std::wstring::size_type lenNew  = wcslen(wstrNew);


      while((pos = ws.find(wstrOrig, pos)) != std::wstring::npos)
      {
         ws.replace(pos, lenOrig, wstrNew); 
         pos += lenNew;
      }
   }
}


void StdStr::Replace(std::string &s, const char * strOrig,  const char * strNew)
{
   if(strOrig && strNew)
   {
      std::string::size_type pos = 0;
   
      std::string::size_type lenOrig = strlen(strOrig);
      std::string::size_type lenNew  = strlen(strNew);


      while((pos = s.find(strOrig, pos)) != std::string::npos)
      {
         s.replace(pos, lenOrig, strNew); 
         pos += lenNew;
      }
   }
}

void StdStr::Replace(std::wstring &ws, const std::wstring & wstrOrig, const std::wstring & wstrNew)
{
   Replace(ws, wstrOrig.c_str(), wstrNew.c_str());
}
   
void StdStr::Replace(std::string  &s,  const std::string  & strOrig,  const std::string  & strNew)
{
   Replace(s, strOrig.c_str(), strNew.c_str());
}


void StdStr::AddBackSlash(std::wstring & ws)
{
   if(ws[ws.length()-1] != L'\\')
   {
      ws += L'\\';
   }
}


void StdStr::AddBackSlash(std::string & s)
{
   if(s[s.length()-1] != '\\')
   {
      s += '\\';
   }
}