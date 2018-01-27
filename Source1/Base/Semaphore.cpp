#include "Semaphore.h"

#if CONFIG_SUPPORTS_THREADING

#if PLATFORM_OSX \
||  PLATFORM_IOS
#	include <dispatch/dispatch.h>
#elif PLATFORM_POSIX
#	include <errno.h>
#	include <pthread.h>
#	include <semaphore.h>
#	include <time.h>
#elif  PLATFORM_WINDOWS \
	|| PLATFORM_WINRT   \
	|| PLATFORM_XBOXONE
#	include <windows.h>
#	include <limits.h>
#	if PLATFORM_XBOXONE
#		include <synchapi.h>
#	endif // PLATFORM_XBOXONE
#endif // PLATFORM_

NS_BEGIN

struct SemaphoreInternal
{
#if PLATFORM_OSX \
||  PLATFORM_IOS
	dispatch_semaphore_t m_handle;
#elif PLATFORM_POSIX
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
	int32_t m_count;
#elif  PLATFORM_WINDOWS \
	|| PLATFORM_WINRT   \
	|| PLATFORM_XBOXONE
	HANDLE m_handle;
#endif // PLATFORM_
};


#if PLATFORM_OSX \
||  PLATFORM_IOS

Semaphore::Semaphore()
{
	STATIC_ASSERT(sizeof(SemaphoreInternal) <= sizeof(m_internal));

	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;
	si->m_handle = dispatch_semaphore_create(0);
	Logger_Assert(NULL != si->m_handle, "dispatch_semaphore_create failed.");
}

Semaphore::~Semaphore()
{
	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;
	dispatch_release(si->m_handle);
}

void Semaphore::post(uint32_t _count)
{
	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

	for (uint32_t ii = 0; ii < _count; ++ii)
	{
		dispatch_semaphore_signal(si->m_handle);
	}
}

bool Semaphore::wait(int32_t _msecs)
{
	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

	dispatch_time_t dt = 0 > _msecs
		? DISPATCH_TIME_FOREVER
		: dispatch_time(DISPATCH_TIME_NOW, _msecs * 1000000)
		;
	return !dispatch_semaphore_wait(si->m_handle, dt);
}

#elif PLATFORM_POSIX

uint64_t toNs(const timespec& _ts)
{
	return _ts.tv_sec*UINT64_C(1000000000) + _ts.tv_nsec;
}

void toTimespecNs(timespec& _ts, uint64_t _nsecs)
{
	_ts.tv_sec = _nsecs / UINT64_C(1000000000);
	_ts.tv_nsec = _nsecs%UINT64_C(1000000000);
}

void toTimespecMs(timespec& _ts, int32_t _msecs)
{
	toTimespecNs(_ts, _msecs * 1000000);
}

void add(timespec& _ts, int32_t _msecs)
{
	uint64_t ns = toNs(_ts);
	toTimespecNs(_ts, ns + uint64_t(_msecs) * 1000000);
}

Semaphore::Semaphore()
{
	STATIC_ASSERT(sizeof(SemaphoreInternal) <= sizeof(m_internal));

	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;
	si->m_count = 0;

	int result;

	result = pthread_mutex_init(&si->m_mutex, NULL);
	Logger_Assert(0 == result, "pthread_mutex_init %d", result);

	result = pthread_cond_init(&si->m_cond, NULL);
	Logger_Assert(0 == result, "pthread_cond_init %d", result);

	UNUSED(result);
}

Semaphore::~Semaphore()
{
	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

	int result;
	result = pthread_cond_destroy(&si->m_cond);
	Logger_Assert(0 == result, "pthread_cond_destroy %d", result);

	result = pthread_mutex_destroy(&si->m_mutex);
	Logger_Assert(0 == result, "pthread_mutex_destroy %d", result);

	UNUSED(result);
}

void Semaphore::post(uint32_t _count)
{
	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

	int result = pthread_mutex_lock(&si->m_mutex);
	Logger_Assert(0 == result, "pthread_mutex_lock %d", result);

	for (uint32_t ii = 0; ii < _count; ++ii)
	{
		result = pthread_cond_signal(&si->m_cond);
		Logger_Assert(0 == result, "pthread_cond_signal %d", result);
	}

	si->m_count += _count;

	result = pthread_mutex_unlock(&si->m_mutex);
	Logger_Assert(0 == result, "pthread_mutex_unlock %d", result);

	UNUSED(result);
}

bool Semaphore::wait(int32_t _msecs)
{
	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

	int result = pthread_mutex_lock(&si->m_mutex);
	Logger_Assert(0 == result, "pthread_mutex_lock %d", result);

	if (-1 == _msecs)
	{
		while (0 == result
			&& 0 >= si->m_count)
		{
			result = pthread_cond_wait(&si->m_cond, &si->m_mutex);
		}
	}
	else
	{
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		add(ts, _msecs);

		while (0 == result
			&& 0 >= si->m_count)
		{
			result = pthread_cond_timedwait(&si->m_cond, &si->m_mutex, &ts);
		}
	}

	bool ok = 0 == result;

	if (ok)
	{
		--si->m_count;
	}

	result = pthread_mutex_unlock(&si->m_mutex);
	Logger_Assert(0 == result, "pthread_mutex_unlock %d", result);

	UNUSED(result);

	return ok;
}

#elif  PLATFORM_WINDOWS \
	|| PLATFORM_WINRT   \
	|| PLATFORM_XBOXONE

Semaphore::Semaphore()
{
	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

#if PLATFORM_WINRT \
||  PLATFORM_XBOXONE
	si->m_handle = CreateSemaphoreExW(NULL, 0, LONG_MAX, NULL, 0, SEMAPHORE_ALL_ACCESS);
#else
	si->m_handle = CreateSemaphoreA(NULL, 0, LONG_MAX, NULL);
#endif
	Logger_Assert(NULL != si->m_handle, "Failed to create Semaphore!");
}

Semaphore::~Semaphore()
{
	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

	CloseHandle(si->m_handle);
}

void Semaphore::post(uint32_t _count)
{
	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

	ReleaseSemaphore(si->m_handle, _count, NULL);
}

bool Semaphore::wait(int32_t _msecs)
{
	SemaphoreInternal* si = (SemaphoreInternal*)m_internal;

	DWORD milliseconds = (0 > _msecs) ? INFINITE : _msecs;
#if PLATFORM_WINRT \
||  PLATFORM_XBOXONE
	return WAIT_OBJECT_0 == WaitForSingleObjectEx(si->m_handle, milliseconds, FALSE);
#else
	return WAIT_OBJECT_0 == WaitForSingleObject(si->m_handle, milliseconds);
#endif
}
#endif // PLATFORM_

NS_END

#endif