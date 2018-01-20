#pragma once

#include "Platform/Device.h"

#if GLFW_SUPPORT

#	include "Framework/Win32/glfw3.h"

#	if PLATFORM_WIN32
#		ifndef GLFW_EXPOSE_NATIVE_WIN32
#			define GLFW_EXPOSE_NATIVE_WIN32
#		endif
#		ifndef GLFW_EXPOSE_NATIVE_WGL
#			define GLFW_EXPOSE_NATIVE_WGL
#		endif
#		include "Framework/Win32/glfw3native.h"
#	endif

#	if _MSC_VER > 1800
#		pragma comment(lib,"glfw3-2015.lib")
#	else
#		pragma comment(lib,"glfw3.lib")
#	endif

NS_BEGIN

class DeviceGLFW : public DeviceProtocol
{
public:
	virtual ~DeviceGLFW();

	static DeviceGLFW* create(Device::Parameters& para);

	bool init(Device::Parameters& para);

public:
	inline Device::Parameters getDeviceParameters() const { return _sParameters; }

	inline GLFWwindow* getGLFWWindow() const { return _pWindow; }

#if PLATFORM_WIN32
	inline HWND getWin32Window() const { return glfwGetWin32Window(_pWindow); }
#endif

public:
	//! Runs the device.
	virtual bool run() override;

	//! Notifies the device that it should close itself.
	virtual void closeDevice() override;
protected:
	DeviceGLFW();

	bool makeGLEW();

	// GLFW callbacks
	static void onGLFWError(int errorID, const char* errorDesc);
	static void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify);
	static void onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y);
	static void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y);
	static void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void onGLFWCharCallback(GLFWwindow* window, unsigned int character);
	static void onGLFWWindowPosCallback(GLFWwindow* windows, int x, int y);
	static void onGLFWframebuffersize(GLFWwindow* window, int w, int h);
	static void onGLFWWindowSizeFunCallback(GLFWwindow *window, int width, int height);
	static void onGLFWWindowIconifyCallback(GLFWwindow* window, int iconified);

protected:
	GLFWwindow* _pWindow;

	GLFWmonitor* _pMonitor;

	Device::Parameters _sParameters;
};

NS_END

#endif