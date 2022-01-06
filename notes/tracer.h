#pragma once
#include <windows.h>
#include <stdarg.h>
#include <time.h>
#include <string>

//---------------------------------------------
// class Tracer :-
//		Tracer()						: set log file path
//		start(bool i_traceOn = true)	: turn on/off
//		trace(LPCWSTR first, ...)		: print debug string
//		ftrace(LPCWSTR first, ...)		: print to log file
//		err(DWORD i_dwErr = -1)			: print last error as debug string
//		ferr(DWORD i_dwErr = -1)		: print last error in log file
//      str(LPCWSTR i_str)              : dump a string bigger than DBG_BUFFER_SIZE, no timestamp
//      fstr(LPCWSTR i_str)             : dump a string bigger than DBG_BUFFER_SIZE to log file, no timestamp

//---------------------------------------------
#define DBG_BUFFER_SIZE 1024

class Tracer
{
private:
	CRITICAL_SECTION	m_cs;
	bool				m_bOn;
	bool				m_bTimeStamp;
	std::wstring		m_szLogFilePath;
	HANDLE				m_hLogFile;
	WCHAR				wstrTimeStamp[256];
	WCHAR				wstrDbgBuffer[DBG_BUFFER_SIZE];
    CHAR                strDbgBuffer[DBG_BUFFER_SIZE];
	std::wstring		m_szPrefix;
    std::wstring		m_szDbgMsg;

public:
	Tracer(LPCWSTR i_wstrPrefix = NULL, LPCWSTR i_wstrLogFile = NULL, bool i_bTimeStamp = true)
		:m_bOn(false),
		m_bTimeStamp(i_bTimeStamp),
		m_hLogFile(INVALID_HANDLE_VALUE)
	{
		if(i_wstrLogFile)
      { 
			m_szLogFilePath = i_wstrLogFile;
      }
        //wchar_t filenamebuf[1024] = {0};
        //GetModuleFileNameW(NULL, filenamebuf, 1024);
        //std::wstring wsFileName = filenamebuf;
        //wsFileName = wsFileName.substr(wsFileName.find_last_of(L'\\')+1);
        //m_szLogFilePath = L"d:\\";
        //m_szLogFilePath += wsFileName;
        //m_szLogFilePath += L".log";

		if(i_wstrPrefix)
			m_szPrefix = i_wstrPrefix;
//#ifdef _DEBUG
		start();
//#endif
	}

	~Tracer()
	{
	    start(false);
	}

    std::wstring time_t_to_string(time_t input)
    {
		tm ltm = {0};

#if defined (__BORLANDC__)
   ::gmtime_s(&input, &ltm);
#else
   ::gmtime_s(&ltm, &input);
#endif

        wchar_t buf[128] = {0};
        ::wsprintfW(buf, L"%d/%d/%d %02d:%02d:%02d", ltm.tm_mday, ltm.tm_mon+1, ltm.tm_year+1900, ltm.tm_hour, ltm.tm_min, ltm.tm_sec);

        return std::wstring(buf);
    }

	void start(bool i_traceOn = true)
	{
		if(m_bOn == i_traceOn)
			return;
		m_bOn = i_traceOn;
		if(i_traceOn)
		{
			InitializeCriticalSection(&m_cs);
			if(! m_szLogFilePath.empty())
			{
				m_hLogFile = CreateFileW(m_szLogFilePath.c_str(),
										GENERIC_WRITE,
										FILE_SHARE_READ,
										NULL,
										OPEN_ALWAYS,
										FILE_FLAG_WRITE_THROUGH,0
										);
				if(m_hLogFile != INVALID_HANDLE_VALUE)
				{
					if(0 == GetFileSize(m_hLogFile, NULL) )
					{
						SetFilePointer(m_hLogFile, 0, 0, SEEK_SET);
						wchar_t BOM = 0xfeff;
						DWORD dwWritten = 0;
						::WriteFile(m_hLogFile, &BOM, (DWORD)sizeof(wchar_t),&dwWritten, NULL);
					}
					SetFilePointer(m_hLogFile, 0,0,SEEK_END);
				}
			}
		}
		else
		{
			if(m_hLogFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(m_hLogFile);
				m_hLogFile = INVALID_HANDLE_VALUE;
			}
			DeleteCriticalSection(&m_cs);
		}
	}

	void trace(LPCWSTR first, ...)
	{
		if(!m_bOn)
			return;
        EnterCriticalSection(&m_cs);
		va_list list;
		va_start(list, first);

        write_message_string(false, first, list);
		OutputDebugStringW(m_szDbgMsg.c_str());

		va_end(list);
		LeaveCriticalSection(&m_cs);
	}

    void traceA(LPCSTR first, ...)
    {
        if(!m_bOn)
			return;
        EnterCriticalSection(&m_cs);
		va_list list;
		va_start(list, first);

        write_message_stringA(false, first, list);
        OutputDebugStringW(m_szDbgMsg.c_str());

        va_end(list);
		LeaveCriticalSection(&m_cs);
    }

    void time(time_t t)
    {
        std::wstring strTime = time_t_to_string(t);
        trace(strTime.c_str());
    }

	void ftrace(LPCWSTR first, ...)
	{
		if(!m_bOn || (m_hLogFile==INVALID_HANDLE_VALUE))
			return;

		EnterCriticalSection(&m_cs);
		va_list list;
		va_start(list, first);

        write_message_string(true, first, list);
		DWORD dwWritten = 0;
		::WriteFile(m_hLogFile, m_szDbgMsg.c_str(), (DWORD)(m_szDbgMsg.length()*sizeof(wchar_t)), &dwWritten, NULL);

		va_end(list);
		LeaveCriticalSection(&m_cs);
	}

    void ftraceA(LPCSTR first, ...)
	{
		if(!m_bOn || (m_hLogFile==INVALID_HANDLE_VALUE))
			return;

		EnterCriticalSection(&m_cs);
		va_list list;
		va_start(list, first);

        write_message_stringA(true, first, list);
		DWORD dwWritten = 0;
		::WriteFile(m_hLogFile, m_szDbgMsg.c_str(), (DWORD)(m_szDbgMsg.length()*sizeof(wchar_t)), &dwWritten, NULL);

		va_end(list);
		LeaveCriticalSection(&m_cs);
	}

    void fdtrace(LPCWSTR first, ...)
	{
		if(!m_bOn)
			return;

		EnterCriticalSection(&m_cs);
		va_list list;
		va_start(list, first);

        write_message_string(true, first, list);
        OutputDebugStringW(m_szDbgMsg.c_str() + 2);
        if(m_hLogFile!=INVALID_HANDLE_VALUE)
        {
            DWORD dwWritten = 0;
            ::WriteFile(m_hLogFile, m_szDbgMsg.c_str(), (DWORD)(m_szDbgMsg.length()*sizeof(wchar_t)), &dwWritten, NULL);
        }

		va_end(list);
		LeaveCriticalSection(&m_cs);
	}

    void fdtraceA(LPCSTR first, ...)
	{
		if(!m_bOn)
			return;

		EnterCriticalSection(&m_cs);
		va_list list;
		va_start(list, first);

        write_message_stringA(true, first, list);
        OutputDebugStringW(m_szDbgMsg.c_str() + 2);
        if(m_hLogFile!=INVALID_HANDLE_VALUE)
        {
            DWORD dwWritten = 0;
            ::WriteFile(m_hLogFile, m_szDbgMsg.c_str(), (DWORD)(m_szDbgMsg.length()*sizeof(wchar_t)), &dwWritten, NULL);
        }

		va_end(list);
		LeaveCriticalSection(&m_cs);
	}

    void str(LPCWSTR i_str)
    {
        if(!m_bOn  ||  !i_str)
			return;
        EnterCriticalSection(&m_cs);

        OutputDebugStringW(i_str);

        LeaveCriticalSection(&m_cs);
    }

    void str(LPCSTR i_str)
    {
        if(!m_bOn  ||  !i_str)
			return;
        EnterCriticalSection(&m_cs);

        OutputDebugStringA(i_str);

        LeaveCriticalSection(&m_cs);
    }

    void fstr(LPCWSTR i_str)
    {
        if(!m_bOn  ||  !i_str)
			return;
        EnterCriticalSection(&m_cs);

        DWORD dwWritten = 0;
		::WriteFile(m_hLogFile, i_str, (DWORD)(wcslen(i_str)*sizeof(wchar_t)), &dwWritten, NULL);

        LeaveCriticalSection(&m_cs);
    }

	void err(DWORD i_dwErr = -1)
	{
		if(!m_bOn)
			return;
		
		EnterCriticalSection(&m_cs);
		show_error(i_dwErr, false);
		LeaveCriticalSection(&m_cs);
	}

	void ferr(DWORD i_dwErr = -1)
	{
		if(!m_bOn || (m_hLogFile==INVALID_HANDLE_VALUE))
			return;

		EnterCriticalSection(&m_cs);
		show_error(i_dwErr, true);
		LeaveCriticalSection(&m_cs);
	}

private:
	void TimeStamp()
	{
		SYSTEMTIME systime = {0};
		GetLocalTime(&systime);
		DWORD dwPID = GetCurrentProcessId();

		wchar_t wstrTimeStamp[256];
		wsprintfW(wstrTimeStamp, L"[%2d:%2d:%2d:%3d][P%4d,T%4d] ",systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds, dwPID, GetCurrentThreadId());

        m_szDbgMsg += wstrTimeStamp;
	}

    wchar_t * ConvertToWChar(const char * input)
    {
        wchar_t * pRetVal = NULL;
        int nSize = MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
        if(nSize > 0)
        {
            pRetVal = new wchar_t[nSize];
            if(pRetVal)
            {
                int nWritten = MultiByteToWideChar(CP_UTF8, 0, input, -1, pRetVal, nSize);
                if(nWritten != nSize)
                {
                    delete [] pRetVal;
                    pRetVal = NULL;
                }
            }
        }
        return pRetVal;
    }

    void write_message_string(bool bNewLine, LPCWSTR first, va_list list)
    {
        m_szDbgMsg = bNewLine ? L"\r\n" : L"";

        if(m_bTimeStamp)
			TimeStamp();

        if(!m_szPrefix.empty())
			m_szDbgMsg += m_szPrefix;

        vswprintf_s	(wstrDbgBuffer, DBG_BUFFER_SIZE, first, list);
        m_szDbgMsg += wstrDbgBuffer;
        m_szDbgMsg += L"\r\n";
    }

    void write_message_stringA(bool bNewLine, LPCSTR first, va_list list)
    {
        m_szDbgMsg = bNewLine ? L"\r\n" : L"";

        if(m_bTimeStamp)
			TimeStamp();

        if(!m_szPrefix.empty())
			m_szDbgMsg += m_szPrefix;

        vsprintf_s	(strDbgBuffer, DBG_BUFFER_SIZE, first, list);
        wchar_t * pWcharBuffer = ConvertToWChar(strDbgBuffer);
        if(pWcharBuffer)
        {
            m_szDbgMsg += pWcharBuffer;
            delete [] pWcharBuffer;
        }
        m_szDbgMsg += L"\r\n";
    }

	void show_error(DWORD dwError, bool bFile)
	{
		// Get the error code
		if(dwError == -1)
			dwError = GetLastError();

		HLOCAL hlocal = NULL;   // Buffer that gets the error message string

		// Use the default system locale since we look for Windows messages
		// Note: this MAKELANGID combination has a value of 0
		DWORD systemLocale = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL);

		// Get the error code's textual description
		BOOL fOk = FormatMessage(
						FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
						FORMAT_MESSAGE_ALLOCATE_BUFFER,
						NULL, dwError, systemLocale,
						(PTSTR) &hlocal, 0, NULL
						);

		if(!fOk)
		{
			// Is it a network-related error?
			HMODULE hDll = LoadLibraryEx(TEXT("netmsg.dll"), NULL, DONT_RESOLVE_DLL_REFERENCES);
			if (hDll != NULL)
			{
				fOk = FormatMessage(
							FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS |
							FORMAT_MESSAGE_ALLOCATE_BUFFER,
							hDll, dwError, systemLocale,
							(PTSTR) &hlocal, 0, NULL
							);
				FreeLibrary(hDll);
			}
		}

		if (fOk && (hlocal != NULL))
		{
			std::wstring szMsg;
			WCHAR wstrErrNo[64] = {0};
			wsprintfW(wstrErrNo, L"0x%x", dwError);
			szMsg += wstrErrNo;
			szMsg += L": ";
			szMsg += (LPCWSTR) LocalLock(hlocal);
			LocalFree(hlocal);

			if(bFile && (m_hLogFile!=INVALID_HANDLE_VALUE))
			{
				DWORD dwWritten = 0;
				::WriteFile(m_hLogFile, szMsg.c_str(), (DWORD)(szMsg.length()*sizeof(wchar_t)), &dwWritten, NULL);
			}
			else
				OutputDebugStringW(szMsg.c_str());
		}
	}
};

