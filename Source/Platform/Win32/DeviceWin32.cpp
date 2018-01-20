#include "DeviceWin32.h"
#include "Base/PoolManager.h"

#if PLATFORM_WIN32

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

Device* Device::create(GraphicsType graphics, Device::Parameters& para)
{
	Device* pRet = new Device();
	if (pRet && pRet->init(graphics, para))
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

bool Device::init(GraphicsType graphics, Device::Parameters& para)
{
	do 
	{
		if (!isGraphicsSupported(graphics))
			break;

		if (graphics == GraphicsType::kGTOGL)
		{
#if GLFW_SUPPORT
			_pDeviceGLFW = DeviceGLFW::create(para);
			if (!_pDeviceGLFW)
				break;
			_pGraphics = GraphicsOGL::create();
			if (!_pGraphics)
				break;
#endif
		}
		else
		{

		}

		_sParameters = para;
		retain();

		return true;
	} while (0);

	return false;
}

HWND Device::getWin32Window() const
{
#if GLFW_SUPPORT
	if (_pDeviceGLFW)
		return _pDeviceGLFW->getWin32Window();
#endif

	return 0;
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