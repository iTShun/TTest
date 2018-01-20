#include "PoolManager.h"
#include "Base/Console.h"

NS_BEGIN

AutoreleasePool::AutoreleasePool()
: _strName("")
#if DEBUG_MODE > 0
, _bClearing(false)
#endif
{
	_vManagedObjectArray.reserve(150);
	PoolManager::getInstance()->push(this);
}

AutoreleasePool::AutoreleasePool(const std::string &name)
: _strName(name)
#if DEBUG_MODE > 0
, _bClearing(false)
#endif
{
	_vManagedObjectArray.reserve(150);
	PoolManager::getInstance()->push(this);
}

AutoreleasePool::~AutoreleasePool()
{
	Logger_Log("deallocing AutoreleasePool: %p", this);
	clear();

	PoolManager::getInstance()->pop();
}

void AutoreleasePool::addObject(Ref* object)
{
	_vManagedObjectArray.push_back(object);
}

void AutoreleasePool::clear()
{
#if DEBUG_MODE > 0
	_bClearing = true;
#endif
	int count = 0;
	std::vector<Ref*> releasings;
	releasings.swap(_vManagedObjectArray);
	for (const auto &obj : releasings)
	{
		obj->release();
	}
#if DEBUG_MODE > 0
	_bClearing = false;
#endif
}

bool AutoreleasePool::contains(Ref* object) const
{
	for (const auto& obj : _vManagedObjectArray)
	{
		if (obj == object)
			return true;
	}
	return false;
}

void AutoreleasePool::dump()
{
	Logger_Log("autorelease pool: %s, number of managed object %d\n", _strName.c_str(), static_cast<int>(_vManagedObjectArray.size()));
	Logger_Log("%20s%20s%20s", "Object pointer", "Object id", "reference count");
	for (const auto &obj : _vManagedObjectArray)
	{
		Logger_Log("%20p%20u\n", obj, obj->getReferenceCount());
	}
}


//--------------------------------------------------------------------
//
// PoolManager
//
//--------------------------------------------------------------------

PoolManager* PoolManager::s_pSingleInstance = nullptr;

PoolManager* PoolManager::getInstance()
{
	if (!s_pSingleInstance)
	{
		s_pSingleInstance = new (std::nothrow) PoolManager();
		// Add the first auto release pool
		new AutoreleasePool("self autorelease pool");
	}
	return s_pSingleInstance;
}

void PoolManager::destroyInstance()
{
	delete s_pSingleInstance;
	s_pSingleInstance = nullptr;
}

PoolManager::PoolManager()
{
	_vReleasePoolStack.reserve(10);
}

PoolManager::~PoolManager()
{
	Logger_Log("deallocing PoolManager: %p", this);

	while (!_vReleasePoolStack.empty())
	{
		AutoreleasePool* pool = _vReleasePoolStack.back();

		delete pool;
	}
}

AutoreleasePool* PoolManager::getCurrentPool() const
{
	return _vReleasePoolStack.back();
}

bool PoolManager::isObjectInPools(Ref* obj) const
{
	for (const auto& pool : _vReleasePoolStack)
	{
		if (pool->contains(obj))
			return true;
	}
	return false;
}

void PoolManager::push(AutoreleasePool *pool)
{
	_vReleasePoolStack.push_back(pool);
}

void PoolManager::pop()
{
	assert(!_vReleasePoolStack.empty());
	_vReleasePoolStack.pop_back();
}

NS_END