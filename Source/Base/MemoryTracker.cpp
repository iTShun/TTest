#include "MemoryTracker.h"

#if DEBUG_MODE

#include <assert.h>
#include <iostream>
#include <sstream>
#include <fstream>

#if PLATFORM_WIN32 || PLATFORM_WINRT
#	include <windows.h>
#   define OutputCString(str) ::OutputDebugStringA(str)
#   define OutputWString(str) ::OutputDebugStringW(str)
#else
#   define OutputCString(str) std::cerr << str
#   define OutputWString(str) std::cerr << str
#endif

NS_BEGIN

MemoryTracker::MemoryTracker()
: _pLeakFileName("MemLeaks.log")
, _bDumpToStdOut(true)
, _nTotalAllocations(0)
, _bRecordEnable(true)
{
}

MemoryTracker::~MemoryTracker()
{
	reportLeaks();

	_bRecordEnable = false;
}

MemoryTracker & MemoryTracker::getInstance()
{
	static MemoryTracker tracker;
	return tracker;
}

void MemoryTracker::recordAlloc(void * ptr, unsigned int sz, unsigned int pool, const char * file, unsigned int ln, const char * func, const char* describe)
{
	if (_bRecordEnable)
	{
		assert(_vAllocations.find(ptr) == _vAllocations.end() && "Double allocation with same address.");
		
		_vAllocations[ptr] = Alloc(sz, pool, file, ln, func, describe);
		if (pool >= _vAllocationsByPool.size())
			_vAllocationsByPool.resize(pool + 1, 0);
		_vAllocationsByPool[pool] += sz;
		_nTotalAllocations += sz;
	}
}

void MemoryTracker::recordDealloc(void * ptr)
{
	if (_bRecordEnable)
	{
		if (!ptr) return;

		AllocationMap::iterator i = _vAllocations.find(ptr);
		assert(i != _vAllocations.end() && "Unable to locate allocation unit.");
		// update category stats
		_vAllocationsByPool[i->second.pool] -= i->second.bytes;
		// global stats
		_nTotalAllocations -= i->second.bytes;
		_vAllocations.erase(i);
	}
}

void MemoryTracker::reportLeaks()
{
	if (_bRecordEnable)
	{
		std::stringstream os;

		if (_vAllocations.empty())
		{
			os << NS_NAME << " Memory: No memory leaks" << std::endl;
		}
		else
		{
			os << NS_NAME << " Memory: Detected memory leaks !!! " << std::endl;
			os << NS_NAME << " Memory: (" << _vAllocations.size() << ") Allocation(s) with total " << _nTotalAllocations << " bytes." << std::endl;
			os << NS_NAME << " Memory: Dumping allocations -> " << std::endl;

			for (AllocationMap::const_iterator i = _vAllocations.begin(); i != _vAllocations.end(); ++i)
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

		if (_bDumpToStdOut)
			OutputCString(os.str().c_str());

		std::ofstream of;
		of.open(_pLeakFileName);
		of << os.str();
		of.close();
	}
}

NS_END

#endif