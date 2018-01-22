#pragma once

#include "platform/Platform.h"

#if DEBUG_MODE

#include <unordered_map>

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
		Alloc(unsigned int sz, unsigned int p, const char* file, unsigned int ln, const char* func, const char* describe)
			:bytes(sz), pool(p), line(ln), filename(file), function(func), desc(describe)
		{
		}
	};

	typedef std::unordered_map<void*, Alloc> AllocationMap;
	typedef std::vector<unsigned int> AllocationsByPool;
public:
	~MemoryTracker();

	static MemoryTracker& getInstance();

	void recordAlloc(void* ptr, unsigned int sz, unsigned int pool = 0, const char* file = 0, unsigned int ln = 0, const char* func = 0, const char* describe = 0);

	void recordDealloc(void* ptr);

	inline unsigned int getTotalMemoryAllocated() const { return _nTotalAllocations; }

	unsigned int getMemoryAllocatedForPool(int pool) const { return _vAllocationsByPool[pool]; }

	inline void setReportFileName(const char* name) { _pLeakFileName = name; }

	inline const char* getReportFileName() const {	return _pLeakFileName;	}

	inline void setReportToStdOut(bool rep){ _bDumpToStdOut = rep; }

	inline bool getReportToStdOut() const { return _bDumpToStdOut; }
	
	inline void setRecordEnable(bool recordEnable) { _bRecordEnable = recordEnable; }

	inline bool getRecordEnable() const { return _bRecordEnable; }

protected:
	MemoryTracker();

	void reportLeaks();
protected:
	const char*			_pLeakFileName;
	bool				_bDumpToStdOut;
	AllocationMap		_vAllocations;
	unsigned int		_nTotalAllocations;
	AllocationsByPool	_vAllocationsByPool;
	bool				_bRecordEnable;
};

NS_END

#endif