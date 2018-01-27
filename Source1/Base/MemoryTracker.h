#pragma once

#include "Base/Console.h"
#include "Base/Mutex.h"

#if CONFIG_USE_TINYSTL
NS_BEGIN

struct TinyStlAllocator
{
	static void* static_allocate(size_t _bytes);
	static void static_deallocate(void* _ptr, size_t /*_bytes*/);
};

NS_END
#	define TINYSTL_ALLOCATOR NS_::TinyStlAllocator
#	include <tinystl/string.h>
#	include <tinystl/unordered_map.h>
#	include <tinystl/unordered_set.h>
#	include <tinystl/vector.h>
namespace stl = tinystl;
#else
#	include <string>
#	include <unordered_map>
#	include <unordered_set>
#	include <vector>
namespace stl = std;
#endif // CONFIG_USE_TINYSTL

#ifndef CONFIG_MEMORY_TRACKING
#	define CONFIG_MEMORY_TRACKING (DEBUG_MODE && CONFIG_SUPPORTS_THREADING)
#endif // CONFIG_MEMORY_TRACKING

#if CONFIG_MEMORY_TRACKING

NS_BEGIN

class MemoryTracker
{
protected:
	// Allocation record
	struct Alloc
	{
		unsigned int	bytes;
		unsigned int	pool;
		const char*		filename;
		unsigned int	line;
		const char*		function;
        const char*     desc;

		Alloc() : bytes(0), line(0) {}
		Alloc(unsigned int _sz, unsigned int _pool, const char* _file, unsigned int _line, const char* _func, const char* _describe)
			:bytes(_sz), pool(_pool), line(_line), filename(_file), function(_func), desc(_describe)
		{
		}
	};

	typedef stl::unordered_map<void*, Alloc> AllocationMap;
	typedef stl::vector<unsigned int> AllocationsByPool;
public:
	~MemoryTracker();

	static MemoryTracker& getInstance();

	void recordAlloc(void* _ptr, unsigned int _sz, unsigned int _pool = 0, const char* _file = 0, unsigned int _line = 0, const char* _func = 0, const char* _describe = 0);

	void recordDealloc(void* _ptr);

	inline unsigned int getTotalMemoryAllocated() const { return m_nTotalAllocations; }

	unsigned int getMemoryAllocatedForPool(int _pool) const { return m_vAllocationsByPool[_pool]; }

	inline void setReportFileName(const char* _name) { m_pLeakFileName = _name; }

	inline const char* getReportFileName() const {	return m_pLeakFileName;	}

	inline void setReportToStdOut(bool _rep){ m_bDumpToStdOut = _rep; }

	inline bool getReportToStdOut() const { return m_bDumpToStdOut; }
	
	inline void setRecordEnable(bool _recordEnable) { m_bRecordEnable = _recordEnable; }

	inline bool getRecordEnable() const { return m_bRecordEnable; }

protected:
	MemoryTracker();

	void reportLeaks();
protected:
	const char*			m_pLeakFileName;
	bool				m_bDumpToStdOut;
	AllocationMap		m_vAllocations;
	unsigned int		m_nTotalAllocations;
	AllocationsByPool	m_vAllocationsByPool;
	bool				m_bRecordEnable;
};

NS_END

#endif

NS_BEGIN

class AllocatorStub : public NS_::AllocatorI
{
public:
	AllocatorStub();

	virtual void* realloc(void* _ptr, size_t _size, size_t _align, const char* _file, uint32_t _line) override;

	void checkLeaks();

protected:
#if CONFIG_MEMORY_TRACKING
	NS_::Mutex m_mutex;
	uint32_t m_numBlocks;
	uint32_t m_maxBlocks;
#endif // CONFIG_MEMORY_TRACKING
};

NS_END