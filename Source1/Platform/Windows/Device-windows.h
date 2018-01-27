#pragma once

#include "Platform/DeviceConfig.h"

#if PLATFORM_WINDOWS && DEVICE_CONFIG_USE_NATIVE

#include <windows.h>
#include <windowsx.h>
#include <xinput.h>

#if COMPILER_MSVC
#	pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif

NS_BEGIN

class Device : public DeviceProtocol
{
public:
	virtual ~Device();

	static Device* create();

	bool init();

	void clearWindow(HWND _hwnd);

	void adjustWindow(HWND _hwnd, uint32_t _width, uint32_t _height, bool _windowFrame);

	void setMousePos(HWND _hwnd, int32_t _x, int32_t _y);
public:
	WindowHandle getNewWindowHandle();
	WindowHandle findHandle(HWND _hwnd);
	HWND findHWND(WindowHandle _handle);
	void setHWND(WindowHandle _handle, HWND _hwnd);

	inline bool isFrame() const { return _bFrame; }
	inline void getWindowSize(uint32_t& _width, uint32_t& _height) const { _width = _nWidth; _height = _nHeight; }
	inline void setWindowOldSize(uint32_t _width, uint32_t _height) { _nOldWidth = _width; _nOldHeight = _height; }
	inline void getWindowOldSize(uint32_t& _width, uint32_t& _height) const { _width = _nOldWidth; _height = _nOldHeight; }
public:
	virtual WindowHandle createWindow(int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, const char* _title = "") override;
	virtual void destroyWindow(WindowHandle _handle) override;
	virtual void setWindowPos(WindowHandle _handle, int32_t _x, int32_t _y) override;
	virtual void setWindowSize(WindowHandle _handle, uint32_t _width, uint32_t _height) override;
	virtual void setWindowTitle(WindowHandle _handle, const char* _title) override;
	virtual void setWindowFlags(WindowHandle _handle, uint32_t _flags, bool _enabled) override;
	virtual void toggleFullscreen(WindowHandle _handle) override;
	virtual void setMouseLock(WindowHandle _handle, bool _lock) override;
	virtual void setCurrentDir(const char* _dir) override;

protected:
	Device();
	
protected:
	WindowHandle _nWindowID[DEVICE_CONFIG_MAX_WINDOWS];
	uint32_t     _nWidth;
	uint32_t     _nHeight;
	float		 _fAspectRatio;
	HWND		_hwnd[DEVICE_CONFIG_MAX_WINDOWS];
	RECT		_sRect;
	DWORD		_nStyle;
	bool		_bFrame;
	uint32_t	_nOldWidth;
	uint32_t	_nOldHeight;
	uint32_t	_nFrameWidth;
	uint32_t	_nFrameHeight;
	int32_t		_nX;
	int32_t		_nY;
};

NS_END

#endif