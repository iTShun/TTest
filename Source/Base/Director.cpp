#include "Director.h"
#include "Base/PoolManager.h"
#include "Platform/Device.h"

NS_BEGIN

// singleton stuff
static Director *s_pSharedDirector = nullptr;

Director::Director()
: Ref(0, __FILE__, __LINE__, __FUNCTION__, "Director")
, _ptrDevice()
{
}

Director::~Director()
{
	PoolManager::getInstance()->getCurrentPool()->clear();

#if REF_LEAK_DETECTION
	printLeaks();
#endif
	s_pSharedDirector = nullptr;
}

Director* Director::getInstance()
{
	if (!s_pSharedDirector)
	{
		s_pSharedDirector = new (std::nothrow) Director();
		Logger_Assert(s_pSharedDirector, "FATAL: Not enough memory.");
	}
	return s_pSharedDirector;
}

void Director::setDevice(Device* device)
{
	if (_ptrDevice)
	{
		_ptrDevice->closeDevice();
		//_ptrDevice.~ref_ptr();
	}
	
	_ptrDevice = device;
}

NS_END