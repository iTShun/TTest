#pragma once

#include "Base/Console.h"

NS_BEGIN

typedef uint16_t WindowHandle;

class DeviceProtocol
{
public:
	virtual ~DeviceProtocol() {}

	virtual WindowHandle createWindow(int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, const char* _title = "") = 0;
	virtual void destroyWindow(WindowHandle _handle) = 0;
	virtual void setWindowPos(WindowHandle _handle, int32_t _x, int32_t _y) = 0;
	virtual void setWindowSize(WindowHandle _handle, uint32_t _width, uint32_t _height) = 0;
	virtual void setWindowTitle(WindowHandle _handle, const char* _title) = 0;
	virtual void setWindowFlags(WindowHandle _handle, uint32_t _flags, bool _enabled) = 0;
	virtual void toggleFullscreen(WindowHandle _handle) = 0;
	virtual void setMouseLock(WindowHandle _handle, bool _lock) = 0;
	virtual void setCurrentDir(const char* _dir) = 0;

	inline const char* getPlatformName() const { return PLATFORM_NAME; }
	inline bool isWindowValid(WindowHandle _handle) const { return UINT16_MAX != _handle; }
};

NS_END