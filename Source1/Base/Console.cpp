#include "Console.h"
#include "Base/String.h"

#if PLATFORM_ANDROID
#	include <android/log.h>
#elif  PLATFORM_WINDOWS \
	|| PLATFORM_WINRT \
	|| PLATFORM_XBOXONE
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA(const char* _str);
#elif PLATFORM_IOS || PLATFORM_OSX
#	if defined(__OBJC__)
#		import <Foundation/NSObjCRuntime.h>
#	else
#		include <CoreFoundation/CFString.h>
extern "C" void NSLog(CFStringRef _format, ...);
#	endif // defined(__OBJC__)
#elif 0 // PLATFORM_EMSCRIPTEN
#	include <emscripten.h>
#else
#	include <stdio.h> // fputs, fflush
#endif // PLATFORM_WINDOWS

NS_BEGIN

void Console::debugBreak()
{
#if DEBUG_MODE

#if COMPILER_MSVC
	__debugbreak();
#elif CPU_ARM
	__builtin_trap();
	//		asm("bkpt 0");
#elif CPU_X86 && (COMPILER_GCC || COMPILER_CLANG)
	// NaCl doesn't like int 3:
	// NativeClient: NaCl module load failed: Validation failure. File violates Native Client safety rules.
	__asm__("int $3");
#else // cross platform implementation
	int* int3 = (int*)3L;
	*int3 = 3;
#endif

#endif // DEBUG_MODE
}

void Console::debugOutput(const char* _out)
{
#if DEBUG_MODE

#if PLATFORM_ANDROID
#	ifndef ANDROID_LOG_TAG
#		define ANDROID_LOG_TAG ""
#	endif // ANDROID_LOG_TAG
	__android_log_write(ANDROID_LOG_DEBUG, BX_ANDROID_LOG_TAG, _out);
#elif  PLATFORM_WINDOWS \
	|| PLATFORM_WINRT \
	|| PLATFORM_XBOXONE
	OutputDebugStringA(_out);
#elif PLATFORM_IOS || PLATFORM_OSX
#	if defined(__OBJC__)
	NSLog(@"%s", _out);
#	else
	NSLog(__CFStringMakeConstantString("%s"), _out);
#	endif // defined(__OBJC__)
#elif 0 // PLATFORM_EMSCRIPTEN
	emscripten_log(EM_LOG_CONSOLE, "%s", _out);
#else
	fputs(_out, stdout);
	fflush(stdout);
#endif // PLATFORM_

#endif // DEBUG_MODE
}

void Console::debugPrintfVargs(const char* _format, va_list _argList)
{
#if DEBUG_MODE
	char temp[8192];
	char* out = temp;
	int32_t len = NS_::vsnprintf(out, sizeof(temp), _format, _argList);
	if ((int32_t)sizeof(temp) < len)
	{
		out = (char*)alloca(len + 1);
		len = NS_::vsnprintf(out, len, _format, _argList);
	}
	out[len] = '\0';
	Console::debugOutput(out);
#endif // DEBUG_MODE
}

void Console::debugPrintf(const char* _format, ...)
{
#if DEBUG_MODE
	va_list argList;
	va_start(argList, _format);
	Console::debugPrintfVargs(_format, argList);
	va_end(argList);
#endif // DEBUG_MODE
}

NS_END