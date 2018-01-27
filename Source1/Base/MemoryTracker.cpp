#include "MemoryTracker.h"

#if CONFIG_MEMORY_TRACKING

#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>

#if PLATFORM_WINDOWS || PLATFORM_WINRT
#	include <windows.h>
#   define OutputCString(str) ::OutputDebugStringA(str)
#   define OutputWString(str) ::OutputDebugStringW(str)
#else
#   define OutputCString(str) std::cerr << str
#   define OutputWString(str) std::cerr << str
#endif

NS_BEGIN

MemoryTracker::MemoryTracker()
: m_pLeakFileName("MemLeaks.log")
, m_bDumpToStdOut(true)
, m_nTotalAllocations(0)
, m_bRecordEnable(true)
{
}

MemoryTracker::~MemoryTracker()
{
	reportLeaks();

	m_bRecordEnable = false;
}

MemoryTracker & MemoryTracker::getInstance()
{
	static MemoryTracker tracker;
	return tracker;
}

void MemoryTracker::recordAlloc(void * ptr, unsigned int sz, unsigned int pool, const char * file, unsigned int ln, const char * func, const char* describe)
{
	if (m_bRecordEnable)
	{
		assert(m_vAllocations.find(ptr) == m_vAllocations.end() && "Double allocation with same address.");
		
		m_vAllocations[ptr] = Alloc(sz, pool, file, ln, func, describe);
		if (pool >= m_vAllocationsByPool.size())
			m_vAllocationsByPool.resize(pool + 1, 0);
		m_vAllocationsByPool[pool] += sz;
		m_nTotalAllocations += sz;
	}
}

void MemoryTracker::recordDealloc(void * ptr)
{
	if (m_bRecordEnable)
	{
		if (!ptr) return;

		AllocationMap::iterator i = m_vAllocations.find(ptr);
		assert(i != m_vAllocations.end() && "Unable to locate allocation unit.");
		// update category stats
		m_vAllocationsByPool[i->second.pool] -= i->second.bytes;
		// global stats
		m_nTotalAllocations -= i->second.bytes;
		m_vAllocations.erase(i);
	}
}

void MemoryTracker::reportLeaks()
{
	if (m_bRecordEnable)
	{
		std::stringstream os;

		if (m_vAllocations.empty())
		{
			os << NS_NAME << " Memory: No memory leaks" << std::endl;
		}
		else
		{
			os << NS_NAME << " Memory: Detected memory leaks !!! " << std::endl;
			os << NS_NAME << " Memory: (" << m_vAllocations.size() << ") Allocation(s) with total " << m_nTotalAllocations << " bytes." << std::endl;
			os << NS_NAME << " Memory: Dumping allocations -> " << std::endl;

			for (AllocationMap::const_iterator i = m_vAllocations.begin(); i != m_vAllocations.end(); ++i)
			{
				const Alloc& alloc = i->second;
				if (strlen(alloc.filename) > 0)
					os << alloc.filename;
				else
					os << "(unknown source):";
                
				if (alloc.desc != 0 && alloc.desc[0] != '\0')
					os << "(" << alloc.line << ") : {" << alloc.bytes << " bytes}" << " function: " << alloc.function << " describe: " << alloc.desc << std::endl;
				else
					os << "(" << alloc.line << ") : {" << alloc.bytes << " bytes}" << " function: " << alloc.function << std::endl;
			}
			os << std::endl;
		}

		if (m_bDumpToStdOut)
			OutputCString(os.str().c_str());

		std::ofstream of;
		of.open(m_pLeakFileName);
		of << os.str();
		of.close();
	}
}

NS_END

#endif

#if CONFIG_USE_TINYSTL
NS_BEGIN
void* TinyStlAllocator::static_allocate(size_t _bytes)
{
	return ALLOC_(g_allocator, _bytes);
}

void TinyStlAllocator::static_deallocate(void* _ptr, size_t /*_bytes*/)
{
	if (NULL != _ptr)
	{
		FREE_(g_allocator, _ptr);
	}
}
NS_END
#endif // CONFIG_USE_TINYSTL

NS_BEGIN

const size_t kNaturalAlignment = 8;

AllocatorStub::AllocatorStub()
#if CONFIG_MEMORY_TRACKING
	: m_numBlocks(0)
	, m_maxBlocks(0)
#endif // CONFIG_MEMORY_TRACKING
{
}

void* AllocatorStub::realloc(void* _ptr, size_t _size, size_t _align, const char* _file, uint32_t _line) override
{
	if (0 == _size)
	{
		if (NULL != _ptr)
		{
			if (kNaturalAlignment >= _align)
			{
#if CONFIG_MEMORY_TRACKING
				{
					NS_::MutexScope scope(m_mutex);
					Logger_Assert(m_numBlocks > 0, "Number of blocks is 0. Possible alloc/free mismatch?");
					--m_numBlocks;
				}
#endif // CONFIG_MEMORY_TRACKING

				::free(_ptr);
			}
			else
			{
				NS_::alignedFree(this, _ptr, _align, _file, _line);
			}
		}

		return NULL;
	}
	else if (NULL == _ptr)
	{
		if (kNaturalAlignment >= _align)
		{
#if BGFX_CONFIG_MEMORY_TRACKING
			{
				bx::MutexScope scope(m_mutex);
				++m_numBlocks;
				m_maxBlocks = bx::uint32_max(m_maxBlocks, m_numBlocks);
			}
#endif // BGFX_CONFIG_MEMORY_TRACKING

			return ::malloc(_size);
		}

		return bx::alignedAlloc(this, _size, _align, _file, _line);
	}

	if (kNaturalAlignment >= _align)
	{
#if BGFX_CONFIG_MEMORY_TRACKING
		if (NULL == _ptr)
		{
			bx::MutexScope scope(m_mutex);
			++m_numBlocks;
			m_maxBlocks = bx::uint32_max(m_maxBlocks, m_numBlocks);
		}
#endif // BGFX_CONFIG_MEMORY_TRACKING

		return ::realloc(_ptr, _size);
	}

	return NS_::alignedRealloc(this, _ptr, _size, _align, _file, _line);
}

void AllocatorStub::checkLeaks()
{
#if CONFIG_MEMORY_TRACKING
	// BK - CallbackStub will be deleted after printing this info, so there is always one
	// leak if CallbackStub is used.
	Logger_Warn(uint32_t(NULL != s_callbackStub ? 1 : 0) == m_numBlocks
		, "MEMORY LEAK: %d (max: %d)"
		, m_numBlocks
		, m_maxBlocks
	);
#endif // BGFX_CONFIG_MEMORY_TRACKING
}

NS_END