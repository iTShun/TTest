#include "Ref.h"
#include "Base/Console.h"
#include "Base/PoolManager.h"
#include "Base/MemoryTracker.h"

#if REF_LEAK_DETECTION
#include <algorithm>    // std::find
#include <thread>
#include <mutex>
#include <vector>
#endif

NS_BEGIN

#if REF_LEAK_DETECTION
static void trackRef(Ref* ref);
static void untrackRef(Ref* ref);
#endif

Ref::Ref()
: _nReferenceCount(1) // when the Ref is created, the reference count of it is 1
{
#if REF_LEAK_DETECTION
	trackRef(this);
#endif

#if DEBUG_MODE
	MemoryTracker::getInstance().recordAlloc(this, sizeof(this));
#endif
}

Ref::Ref(unsigned int pool, const char* file, unsigned int ln, const char* func, const char* describe /*= 0*/)
: _nReferenceCount(1) // when the Ref is created, the reference count of it is 1
{
#if REF_LEAK_DETECTION
	trackRef(this);
#endif

#if DEBUG_MODE
	MemoryTracker::getInstance().recordAlloc(this, sizeof(this), pool, file, ln, func, describe);
#endif
}

Ref::~Ref()
{
#if REF_LEAK_DETECTION
	if (_nReferenceCount != 0)
		untrackRef(this);
#endif

#if DEBUG_MODE
	MemoryTracker::getInstance().recordDealloc(this);
#endif
}

void Ref::retain()
{
	Logger_Assert(_nReferenceCount > 0, "reference count should be greater than 0");
	++_nReferenceCount;
}

void Ref::release()
{
	Logger_Assert(_nReferenceCount > 0, "reference count should be greater than 0");
	--_nReferenceCount;

	if (_nReferenceCount == 0)
	{
#if DEBUG_MODE > 0
		PoolManager* poolMgr = PoolManager::getInstance();
		if (!poolMgr->getCurrentPool()->isClearing() && poolMgr->isObjectInPools(this))
		{
			Logger_Assert(false, "The reference shouldn't be 0 because it is still in autorelease pool.");
		}
#endif

#if REF_LEAK_DETECTION
		untrackRef(this);
#endif
		delete this;
	}
}

Ref* Ref::autorelease()
{
	PoolManager::getInstance()->getCurrentPool()->addObject(this);
	return this;
}

unsigned int Ref::getReferenceCount() const
{
	return _nReferenceCount;
}

#if REF_LEAK_DETECTION

static std::vector<Ref*> __refAllocationList;
static std::mutex __refMutex;

void Ref::printLeaks()
{
	std::lock_guard<std::mutex> refLockGuard(__refMutex);
	// Dump Ref object memory leaks
	if (__refAllocationList.empty())
	{
		Logger_Log("[memory] All Ref objects successfully cleaned up (no leaks detected).\n");
	}
	else
	{
		Logger_Log("[memory] WARNING: %d Ref objects still active in memory.\n", (int)__refAllocationList.size());

		for (const auto& ref : __refAllocationList)
		{
			assert(ref);
			const char* type = typeid(*ref).name();
			Logger_Log("[memory] LEAK: Ref object '%s' still active with reference count %d.\n", (type ? type : ""), ref->getReferenceCount());
		}
	}
}

static void trackRef(Ref* ref)
{
	std::lock_guard<std::mutex> refLockGuard(__refMutex);
	Logger_Assert(ref, "Invalid parameter, ref should not be null!");

	// Create memory allocation record.
	__refAllocationList.push_back(ref);
}

static void untrackRef(Ref* ref)
{
	std::lock_guard<std::mutex> refLockGuard(__refMutex);
	auto iter = std::find(__refAllocationList.begin(), __refAllocationList.end(), ref);
	if (iter == __refAllocationList.end())
	{
		Logger_Log("[memory] CORRUPTION: Attempting to free (%s) with invalid ref tracking record.\n", typeid(*ref).name());
		return;
	}

	__refAllocationList.erase(iter);
}

#endif // #if REF_LEAK_DETECTION

NS_END