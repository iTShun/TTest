#include "Director.h"
#include "Base/PoolManager.h"
#include "Platform/Device.h"

NS_BEGIN

// singleton stuff
static Director *s_pSharedDirector = nullptr;

Director::Director()
: _pDevice(nullptr)
{
}

Director::~Director()
{
	PoolManager::getInstance()->getCurrentPool()->clear();

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
	if (_pDevice)
	{
		_pDevice->closeDevice();
		SAFE_RELEASE_NULL(_pDevice);
	}

	_pDevice = device;
}

NS_END