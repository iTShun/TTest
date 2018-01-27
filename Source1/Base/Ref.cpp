#include "Ref.h"
//#include "Base/PoolManager.h"
//#include "Base/MemoryTracker.h"

NS_BEGIN

Ref::Ref()
: m_nReferenceCount(1) // when the Ref is created, the reference count of it is 1
{
}

Ref::Ref(unsigned int sz, unsigned int pool, const char* file, unsigned int ln, const char* func, const char* describe /*= 0*/)
: m_nReferenceCount(1) // when the Ref is created, the reference count of it is 1
{
/*
#if DEBUG_MODE
	MemoryTracker::getInstance().recordAlloc(this, sz, pool, file, ln, func, describe);
#endif*/
}

Ref::~Ref()
{
/*
#if DEBUG_MODE
	MemoryTracker::getInstance().recordDealloc(this);
#endif*/
}

void Ref::retain()
{
	Logger_Assert(m_nReferenceCount > 0, "reference count should be greater than 0");
	++m_nReferenceCount;
}

void Ref::release()
{
	Logger_Assert(m_nReferenceCount > 0, "reference count should be greater than 0");
	--m_nReferenceCount;

	if (m_nReferenceCount == 0)
	{
/*
#if DEBUG_MODE > 0
		PoolManager* poolMgr = PoolManager::getInstance();
		if (!poolMgr->getCurrentPool()->isClearing() && poolMgr->isObjectInPools(this))
		{
			Logger_Assert(false, "The reference shouldn't be 0 because it is still in autorelease pool.");
		}
#endif*/

		delete this;
	}
}

Ref* Ref::autorelease()
{
	//PoolManager::getInstance()->getCurrentPool()->addObject(this);
	return this;
}

unsigned int Ref::getReferenceCount() const
{
	return m_nReferenceCount;
}

NS_END