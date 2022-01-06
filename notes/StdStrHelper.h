#include <string>
#include <vector>

class StdStr
{
public:

   static void ToUpper(std::wstring & ws);   // ascii only
   static void ToUpper(std::string  & s);    // ascii only

   static void ToLower(std::wstring & ws);
   static void ToLower(std::string  & s);

   static std::string  ToUtf8(const std::wstring & ws);
   static std::wstring ToWideChar(const std::string & s);

   static std::vector<std::wstring> Tokanize(const std::wstring & ws, const wchar_t * wsSeperators);
   static std::vector<std::string>  Tokanize(const std::string  & s,  const char *    sSeperators);

   static std::vector<std::wstring> TokanizeByWord(const std::wstring & ws, const std::wstring & wsSeparator);
   static std::vector<std::string>  TokanizeByWord(const std::string  & s,  const std::string  & sSeperator);

   static void Replace(std::wstring &ws, const wchar_t * wstrOrig, const wchar_t * wstrNew);
   static void Replace(std::string  &s,  const char *    strOrig,  const char *    strNew);

   static void Replace(std::wstring &ws, const std::wstring & wstrOrig, const std::wstring & wstrNew);
   static void Replace(std::string  &s,  const std::string  & strOrig,  const std::string  & strNew);

   static void AddBackSlash(std::wstring & ws);
   static void AddBackSlash(std::string  & s);
};