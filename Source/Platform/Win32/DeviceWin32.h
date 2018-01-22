#pragma once

#include "Platform/DeviceProtocol.h"
#include "Base/Ptr.h"

#if PLATFORM_WIN32

#include <windows.h>

NS_BEGIN

#if GLFW_SUPPORT
class DeviceGLFW;
#endif

class Graphics;

class Device : public DeviceProtocol
{
public:
	struct Parameters
	{
		Parameters() 
			: _nXDesiredPositionOnScreen(0)
			, _nYDesiredPositionOnScreen(0)
			, _nWidthDesiredOnScreen(1024)
			, _nHeightDesiredOnScreen(768)
			, _szTitle(L"")
			, _bFullscreen(false)
			, _bResizable(false)
			, _bDecorated(true)
		{}

		int _nXDesiredPositionOnScreen;
		int _nYDesiredPositionOnScreen;

		int _nWidthDesiredOnScreen;
		int _nHeightDesiredOnScreen;

		wchar_t* _szTitle;

		bool _bFullscreen;
		bool _bResizable;
		bool _bDecorated;
	};

public:
	static Device* makeDevice();
	static Device* create(GraphicsType graphics, Device::Parameters& para);

	bool initEmbeddedInDevice(DeviceEmbeddedType type, DeviceProtocol* device);
	bool init(GraphicsType graphics, Device::Parameters& para);

	HWND getWin32Window() const;

public:
	inline Graphics* getGraphicsDriver() const { return _ptrGraphics.get(); }
public:
	//! Runs the device.
	virtual bool run() override;

	//! Notifies the device that it should close itself.
	virtual void closeDevice() override;
public:
	virtual ~Device();

protected:
	Device();

protected:
	Device::Parameters _sParameters;
#if GLFW_SUPPORT
	ref_ptr<DeviceGLFW> _ptrDeviceGLFW;
#endif
	ref_ptr<Graphics> _ptrGraphics;
};

NS_END

#endif