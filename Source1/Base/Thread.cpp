#include "Thread.h"

#if    PLATFORM_ANDROID \
	|| PLATFORM_LINUX   \
	|| PLATFORM_IOS     \
	|| PLATFORM_OSX     \
	|| PLATFORM_PS4     \
	|| PLATFORM_RPI
#	include <pthread.h>
#	if defined(__FreeBSD__)
#		include <pthread_np.h>
#	endif
#	if PLATFORM_LINUX && (CRT_GLIBC < 21200)
#		include <sys/prctl.h>
#	endif // PLATFORM_
#elif  PLATFORM_WINDOWS \
	|| PLATFORM_WINRT   \
	|| PLATFORM_XBOXONE
#	include <windows.h>
#	include <limits.h>
#	include <errno.h>
#	if PLATFORM_WINRT
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
#	endif // PLATFORM_WINRT
#endif // PLATFORM_

#if CONFIG_SUPPORTS_THREADING

NS_BEGIN

static AllocatorI* getAllocator()
{
	static DefaultAllocator s_allocator;
	return &s_allocator;
}

struct ThreadInternal
{
#if    PLATFORM_WINDOWS \
	|| PLATFORM_WINRT   \
	|| PLATFORM_XBOXONE
	static DWORD WINAPI threadFunc(LPVOID _arg);
	HANDLE m_handle;
	DWORD  m_threadId;
#elif PLATFORM_POSIX
	static void* threadFunc(void* _arg);
	pthread_t m_handle;
#endif // PLATFORM_
};

#if    PLATFORM_WINDOWS \
	|| PLATFORM_XBOXONE \
	|| PLATFORM_WINRT
DWORD WINAPI ThreadInternal::threadFunc(LPVOID _arg)
{
	Thread* thread = (Thread*)_arg;
	int32_t result = thread->entry();
	return result;
}
#else
void* ThreadInternal::threadFunc(void* _arg)
{
	Thread* thread = (Thread*)_arg;
	union
	{
		void* ptr;
		int32_t i;
	} cast;
	cast.i = thread->entry();
	return cast.ptr;
}
#endif // PLATFORM_

Thread::Thread()
	: m_fn(NULL)
	, m_userData(NULL)
	, m_queue(getAllocator())
	, m_stackSize(0)
	, m_exitCode(kExitSuccess)
	, m_running(false)
{
	STATIC_ASSERT(sizeof(ThreadInternal) <= sizeof(m_internal));

	ThreadInternal* ti = (ThreadInternal*)m_internal;
#if    PLATFORM_WINDOWS \
	|| PLATFORM_WINRT   \
	|| PLATFORM_XBOXONE
	ti->m_handle = INVALID_HANDLE_VALUE;
	ti->m_threadId = UINT32_MAX;
#elif PLATFORM_POSIX
	ti->m_handle = 0;
#endif // PLATFORM_
}

Thread::~Thread()
{
	if (m_running)
	{
		shutdown();
	}
}

void Thread::init(ThreadFn _fn, void* _userData, uint32_t _stackSize, const char* _name)
{
	Logger_Assert(!m_running, "Already running!");

	m_fn = _fn;
	m_userData = _userData;
	m_stackSize = _stackSize;
	m_running = true;

	ThreadInternal* ti = (ThreadInternal*)m_internal;
#if    PLATFORM_WINDOWS \
	|| PLATFORM_XBOXONE
	ti->m_handle = ::CreateThread(NULL
		, m_stackSize
		, (LPTHREAD_START_ROUTINE)ti->threadFunc
		, this
		, 0
		, NULL
	);
#elif PLATFORM_WINRT
	ti->m_handle = CreateEventEx(nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
	auto workItemHandler = ref new WorkItemHandler([=](IAsyncAction^)
	{
		m_exitCode = ti->threadFunc(this);
		SetEvent(ti->m_handle);
	}
		, CallbackContext::Any
		);

	ThreadPool::RunAsync(workItemHandler, WorkItemPriority::Normal, WorkItemOptions::TimeSliced);
#elif PLATFORM_POSIX
	int result;
	UNUSED(result);

	pthread_attr_t attr;
	result = pthread_attr_init(&attr);
	Logger_Assert(0 == result, "pthread_attr_init failed! %d", result);

	if (0 != m_stackSize)
	{
		result = pthread_attr_setstacksize(&attr, m_stackSize);
		Logger_Assert(0 == result, "pthread_attr_setstacksize failed! %d", result);
	}

	result = pthread_create(&ti->m_handle, &attr, &ti->threadFunc, this);
	Logger_Assert(0 == result, "pthread_attr_setschedparam failed! %d", result);
#else
#	error "Not implemented!"
#endif // PLATFORM_

	m_sem.wait();

	if (NULL != _name)
	{
		setThreadName(_name);
	}
}

void Thread::shutdown()
{
	Logger_Assert(m_running, "Not running!");
	ThreadInternal* ti = (ThreadInternal*)m_internal;
#if PLATFORM_WINDOWS
	WaitForSingleObject(ti->m_handle, INFINITE);
	GetExitCodeThread(ti->m_handle, (DWORD*)&m_exitCode);
	CloseHandle(ti->m_handle);
	ti->m_handle = INVALID_HANDLE_VALUE;
#elif PLATFORM_WINRT || PLATFORM_XBOXONE
	WaitForSingleObjectEx(ti->m_handle, INFINITE, FALSE);
	CloseHandle(ti->m_handle);
	ti->m_handle = INVALID_HANDLE_VALUE;
#elif PLATFORM_POSIX
	union
	{
		void* ptr;
		int32_t i;
	} cast;
	pthread_join(ti->m_handle, &cast.ptr);
	m_exitCode = cast.i;
	ti->m_handle = 0;
#endif // PLATFORM_

	m_running = false;
}

bool Thread::isRunning() const
{
	return m_running;
}

int32_t Thread::getExitCode() const
{
	return m_exitCode;
}

void Thread::setThreadName(const char* _name)
{
	ThreadInternal* ti = (ThreadInternal*)m_internal;
	UNUSED(ti);
#if PLATFORM_OSX || PLATFORM_IOS
	pthread_setname_np(_name);
#elif (CRT_GLIBC >= 21200) && ! PLATFORM_HURD
	pthread_setname_np(ti->m_handle, _name);
#elif PLATFORM_LINUX
	prctl(PR_SET_NAME, _name, 0, 0, 0);
#elif PLATFORM_BSD
#	ifdef __NetBSD__
	pthread_setname_np(ti->m_handle, "%s", (void*)_name);
#	else
	pthread_set_name_np(ti->m_handle, _name);
#	endif // __NetBSD__
#elif PLATFORM_WINDOWS && COMPILER_MSVC
#	pragma pack(push, 8)
	struct ThreadName
	{
		DWORD  type;
		LPCSTR name;
		DWORD  id;
		DWORD  flags;
	};
#	pragma pack(pop)
	ThreadName tn;
	tn.type = 0x1000;
	tn.name = _name;
	tn.id = ti->m_threadId;
	tn.flags = 0;

	__try
	{
		RaiseException(0x406d1388
			, 0
			, sizeof(tn) / 4
			, reinterpret_cast<ULONG_PTR*>(&tn)
		);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
#else
	UNUSED(_name);
#endif // PLATFORM_
}

void Thread::push(void* _ptr)
{
	m_queue.push(_ptr);
}

void* Thread::pop()
{
	void* ptr = m_queue.pop();
	return ptr;
}

int32_t Thread::entry()
{
#if PLATFORM_WINDOWS
	ThreadInternal* ti = (ThreadInternal*)m_internal;
	ti->m_threadId = ::GetCurrentThreadId();
#endif // PLATFORM_WINDOWS

	m_sem.post();
	int32_t result = m_fn(this, m_userData);
	return result;
}

struct TlsDataInternal
{
#if PLATFORM_WINDOWS
	uint32_t m_id;
#elif !(PLATFORM_XBOXONE || PLATFORM_WINRT)
	pthread_key_t m_id;
#endif // PLATFORM_*
};

#if PLATFORM_WINDOWS
TlsData::TlsData()
{
	STATIC_ASSERT(sizeof(TlsDataInternal) <= sizeof(m_internal));

	TlsDataInternal* ti = (TlsDataInternal*)m_internal;
	ti->m_id = TlsAlloc();
	Logger_Assert(TLS_OUT_OF_INDEXES != ti->m_id, "Failed to allocated TLS index (err: 0x%08x).", GetLastError());
}

TlsData::~TlsData()
{
	TlsDataInternal* ti = (TlsDataInternal*)m_internal;
	BOOL result = TlsFree(ti->m_id);
	Logger_Assert(0 != result, "Failed to free TLS index (err: 0x%08x).", GetLastError()); UNUSED(result);
}

void* TlsData::get() const
{
	TlsDataInternal* ti = (TlsDataInternal*)m_internal;
	return TlsGetValue(ti->m_id);
}

void TlsData::set(void* _ptr)
{
	TlsDataInternal* ti = (TlsDataInternal*)m_internal;
	TlsSetValue(ti->m_id, _ptr);
}

#elif !(PLATFORM_XBOXONE || PLATFORM_WINRT)

TlsData::TlsData()
{
	STATIC_ASSERT(sizeof(TlsDataInternal) <= sizeof(m_internal));

	TlsDataInternal* ti = (TlsDataInternal*)m_internal;
	int result = pthread_key_create(&ti->m_id, NULL);
	Logger_Assert(0 == result, "pthread_key_create failed %d.", result); UNUSED(result);
}

TlsData::~TlsData()
{
	TlsDataInternal* ti = (TlsDataInternal*)m_internal;
	int result = pthread_key_delete(ti->m_id);
	Logger_Assert(0 == result, "pthread_key_delete failed %d.", result); UNUSED(result);
}

void* TlsData::get() const
{
	TlsDataInternal* ti = (TlsDataInternal*)m_internal;
	return pthread_getspecific(ti->m_id);
}

void TlsData::set(void* _ptr)
{
	TlsDataInternal* ti = (TlsDataInternal*)m_internal;
	int result = pthread_setspecific(ti->m_id, _ptr);
	Logger_Assert(0 == result, "pthread_setspecific failed %d.", result); UNUSED(result);
}
#endif // PLATFORM_*

NS_END

#endif // CONFIG_SUPPORTS_THREADING