#include "DeviceWin32.h"
#include "Base/PoolManager.h"

#if PLATFORM_WIN32

#include "Platform/GLFW/DeviceGLFW.h"

NS_BEGIN

Device::Device()
: Ref(sizeof(Device), 0, __FILE__, __LINE__, __FUNCTION__, "DeviceWin32")
, _ptrGraphics()
#if GLFW_SUPPORT
, _ptrDeviceGLFW()
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
			if (_ptrDeviceGLFW)
				_ptrDeviceGLFW->closeDevice();

			_ptrDeviceGLFW = (DeviceGLFW*)device;
			_sParameters = _ptrDeviceGLFW->getDeviceParameters();
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
			_ptrDeviceGLFW = DeviceGLFW::create(para);
			if (!_ptrDeviceGLFW)
				break;
			_ptrGraphics = GraphicsOGL::create();
			if (!_ptrGraphics)
				break;
#endif
		}
		else
		{

		}

		_sParameters = para;

		return true;
	} while (0);

	return false;
}

HWND Device::getWin32Window() const
{
#if GLFW_SUPPORT
	if (_ptrDeviceGLFW)
		return _ptrDeviceGLFW->getWin32Window();
#endif

	return 0;
}

bool Device::run()
{
	// release the objects
	PoolManager::getInstance()->getCurrentPool()->clear();

#if GLFW_SUPPORT
	if (_ptrDeviceGLFW)
		return _ptrDeviceGLFW->run();
#endif
	return false;
}

void Device::closeDevice()
{
#if GLFW_SUPPORT
	if (_ptrDeviceGLFW)
	{
		_ptrDeviceGLFW->closeDevice();
	}
#endif
}


NS_END

#endif