#include "Timer.h"

#if PLATFORM_ANDROID
#	include <time.h> // clock, clock_gettime
#elif PLATFORM_EMSCRIPTEN
#	include <emscripten.h>
#elif PLATFORM_WINDOWS || PLATFORM_XBOXONE || PLATFORM_WINRT
#	include <windows.h>
#else
#	include <sys/time.h> // gettimeofday
#endif // PLATFORM_

NS_BEGIN

int64_t getHPCounter()
{
#if    PLATFORM_WINDOWS \
	|| PLATFORM_XBOXONE \
	|| PLATFORM_WINRT
	LARGE_INTEGER li;
	// Performance counter value may unexpectedly leap forward
	// http://support.microsoft.com/kb/274323
	QueryPerformanceCounter(&li);
	int64_t i64 = li.QuadPart;
#elif PLATFORM_ANDROID
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	int64_t i64 = now.tv_sec*INT64_C(1000000000) + now.tv_nsec;
#elif PLATFORM_EMSCRIPTEN
	int64_t i64 = int64_t(1000.0f * emscripten_get_now());
#elif !PLATFORM_NONE
	struct timeval now;
	gettimeofday(&now, 0);
	int64_t i64 = now.tv_sec*INT64_C(1000000) + now.tv_usec;
#else
	Logger_Assert(false, "Not implemented!");
	int64_t i64 = UINT64_MAX;
#endif // PLATFORM_
	return i64;
}

int64_t getHPFrequency()
{
#if    PLATFORM_WINDOWS \
	|| PLATFORM_XBOXONE \
	|| PLATFORM_WINRT
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	return li.QuadPart;
#elif PLATFORM_ANDROID
	return INT64_C(1000000000);
#elif PLATFORM_EMSCRIPTEN
	return INT64_C(1000000);
#else
	return INT64_C(1000000);
#endif // PLATFORM_
}

NS_END