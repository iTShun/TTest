#include "Console.h"
#include "Base/Director.h"
#include "Platform/Device.h"

NS_BEGIN

//
// Free functions to log
//

#if (PLATFORM_WIN32)
void SendLogToWindow(const char *log)
{
	static const int LOG_STRING_TAG = 1;
	// Send data as a message
	COPYDATASTRUCT myCDS;
	myCDS.dwData = LOG_STRING_TAG;
	myCDS.cbData = (DWORD)strlen(log) + 1;
	myCDS.lpData = (PVOID)log;
	if (Director::getInstance()->getDevice())
	{
		HWND hwnd = Director::getInstance()->getDevice()->getWin32Window();
		SendMessage(hwnd,
			WM_COPYDATA,
			(WPARAM)(HWND)hwnd,
			(LPARAM)(LPVOID)&myCDS);
	}
}
#elif PLATFORM_WINRT
void SendLogToWindow(const char *log)
{
}
#endif

void _log(const char *format, va_list args)
{
	int bufferSize = MAX_LOG_LENGTH;
	char* buf = nullptr;
	int nret = 0;
	do
	{
		buf = new (std::nothrow) char[bufferSize];
		if (buf == nullptr)
			return;
		/*
		pitfall: The behavior of vsnprintf between VS2013 and VS2015/2017 is different
		VS2013 or Unix-Like System will return -1 when buffer not enough, but VS2015/2017 will return the actural needed length for buffer at this station
		The _vsnprintf behavior is compatible API which always return -1 when buffer isn't enough at VS2013/2015/2017
		Yes, The vsnprintf is more efficient implemented by MSVC 19.0 or later, AND it's also standard-compliant, see reference: http://www.cplusplus.com/reference/cstdio/vsnprintf/
		*/
		nret = vsnprintf(buf, bufferSize - 3, format, args);
		if (nret >= 0)
		{ // VS2015/2017
			if (nret <= bufferSize - 3)
			{// success, so don't need to realloc
				break;
			}
			else
			{
				bufferSize = nret + 3;
				delete[] buf;
			}
		}
		else // < 0
		{	// VS2013 or Unix-like System(GCC)
			bufferSize *= 2;
			delete[] buf;
		}
	} while (true);
	buf[nret] = '\n';
	buf[++nret] = '\0';

#if PLATFORM_ANDROID
	__android_log_print(ANDROID_LOG_DEBUG, "cocos2d-x debug info", "%s", buf);

#elif PLATFORM_WIN32 || PLATFORM_WINRT

	int pos = 0;
	int len = nret;
	char tempBuf[MAX_LOG_LENGTH + 1] = { 0 };
	WCHAR wszBuf[MAX_LOG_LENGTH + 1] = { 0 };

	do
	{
		std::copy(buf + pos, buf + pos + MAX_LOG_LENGTH, tempBuf);

		tempBuf[MAX_LOG_LENGTH] = 0;

		MultiByteToWideChar(CP_UTF8, 0, tempBuf, -1, wszBuf, sizeof(wszBuf));
		OutputDebugStringW(wszBuf);
		WideCharToMultiByte(CP_ACP, 0, wszBuf, -1, tempBuf, sizeof(tempBuf), nullptr, FALSE);
		printf("%s", tempBuf);

		pos += MAX_LOG_LENGTH;

	} while (pos < len);
	SendLogToWindow(buf);
	fflush(stdout);
#else
	// Linux, Mac, iOS, etc
	fprintf(stdout, "%s", buf);
	fflush(stdout);
#endif

	//Director::getInstance()->getConsole()->log(buf);Todo
	delete[] buf;
}

void log(const char * format, ...)
{
	va_list args;
	va_start(args, format);
	_log(format, args);
	va_end(args);
}

Console::Console()
{
}


Console::~Console()
{
}

NS_END