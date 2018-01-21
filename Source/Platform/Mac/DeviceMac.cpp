#include "DeviceMac.h"
#include "Base/PoolManager.h"

#if PLATFORM_MAC

#include "Platform/GLFW/DeviceGLFW.h"

NS_BEGIN

Device::Device()
: _pGraphics(nullptr)
#if GLFW_SUPPORT
, _pDeviceGLFW(nullptr)
#endif
{
}

Device::~Device()
{
}

Device* Device::makeDevice()
{
	Device* pRet = new Device();
	if (pRet)
	{
		pRet->autorelease();
	}
	else
	{
		SAFE_DELETE(pRet);
	}

	return pRet;
}

Device* Device::create(Device::Parameters& para)
{
	Device* pRet = new Device();
	if (pRet && pRet->init(para))
	{
		pRet->autorelease();
	}
	else
	{
		SAFE_DELETE(pRet);
	}

	return pRet;
}

bool Device::initEmbeddedInDevice(DeviceEmbeddedType type, DeviceProtocol* device)
{
	do 
	{
		if (!device || !isDeviceEmbeddedSupported(type))
			break;

		switch (type)
		{
		case soldier::kDETGLFW:
#if GLFW_SUPPORT
		{
			if (_pDeviceGLFW)
				_pDeviceGLFW->closeDevice();

			_pDeviceGLFW = (DeviceGLFW*)device;
			_sParameters = _pDeviceGLFW->getDeviceParameters();
			retain();
			return true;
		}
#endif
			break;
		}

	} while (0);

	return false;
}

bool Device::init(Device::Parameters& para)
{
	do 
	{
        if (!isGraphicsSupported(GraphicsType::kGTOGL))
			break;

#if GLFW_SUPPORT
        _pDeviceGLFW = DeviceGLFW::create(para);
        if (!_pDeviceGLFW)
            break;
        _pGraphics = GraphicsOGL::create();
        if (!_pGraphics)
            break;
#endif

		_sParameters = para;
		retain();

		return true;
	} while (0);

	return false;
}

void* Device::getCocoaWindow() const
{
#if GLFW_SUPPORT
	if (_pDeviceGLFW)
		return _pDeviceGLFW->getCocoaWindow();
#endif

	return nullptr;
}

bool Device::run()
{
	// release the objects
	PoolManager::getInstance()->getCurrentPool()->clear();

#if GLFW_SUPPORT
	if (_pDeviceGLFW)
		return _pDeviceGLFW->run();
#endif
	return false;
}

void Device::closeDevice()
{
#if GLFW_SUPPORT
	if (_pDeviceGLFW)
	{
		_pDeviceGLFW->closeDevice();
		SAFE_RELEASE_NULL(_pDeviceGLFW);
	}
#endif
}


NS_END

#endif