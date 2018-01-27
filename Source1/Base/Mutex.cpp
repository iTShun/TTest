#include "Mutex.h"

#if CONFIG_SUPPORTS_THREADING

#if    PLATFORM_ANDROID \
	|| PLATFORM_LINUX   \
	|| PLATFORM_IOS     \
	|| PLATFORM_OSX     \
	|| PLATFORM_PS4     \
	|| PLATFORM_RPI
#	include <pthread.h>
#elif  PLATFORM_WINDOWS \
	|| PLATFORM_WINRT   \
	|| PLATFORM_XBOXONE
#	include <windows.h>
#	include <errno.h>
#endif // PLATFORM_

NS_BEGIN

#if    PLATFORM_WINDOWS \
	|| PLATFORM_XBOXONE \
	|| PLATFORM_WINRT
	typedef CRITICAL_SECTION pthread_mutex_t;
	typedef unsigned pthread_mutexattr_t;

	inline int pthread_mutex_lock(pthread_mutex_t* _mutex)
	{
		EnterCriticalSection(_mutex);
		return 0;
	}

	inline int pthread_mutex_unlock(pthread_mutex_t* _mutex)
	{
		LeaveCriticalSection(_mutex);
		return 0;
	}

	inline int pthread_mutex_trylock(pthread_mutex_t* _mutex)
	{
		return TryEnterCriticalSection(_mutex) ? 0 : EBUSY;
	}

	inline int pthread_mutex_init(pthread_mutex_t* _mutex, pthread_mutexattr_t* /*_attr*/)
	{
#if PLATFORM_WINRT
		InitializeCriticalSectionEx(_mutex, 4000, 0);   // docs recommend 4000 spincount as sane default
#else
		InitializeCriticalSection(_mutex);
#endif // PLATFORM_
		return 0;
	}

	inline int pthread_mutex_destroy(pthread_mutex_t* _mutex)
	{
		DeleteCriticalSection(_mutex);
		return 0;
	}
#endif // PLATFORM_

	Mutex::Mutex()
	{
		STATIC_ASSERT(sizeof(pthread_mutex_t) <= sizeof(m_internal));

		pthread_mutexattr_t attr;

#if    PLATFORM_WINDOWS \
	|| PLATFORM_XBOXONE \
	|| PLATFORM_WINRT
#else
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
#endif // PLATFORM_

		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_init(handle, &attr);
	}

	Mutex::~Mutex()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_destroy(handle);
	}

	void Mutex::lock()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_lock(handle);
	}

	void Mutex::unlock()
	{
		pthread_mutex_t* handle = (pthread_mutex_t*)m_internal;
		pthread_mutex_unlock(handle);
	}

NS_END

#endif // CONFIG_SUPPORTS_THREADING