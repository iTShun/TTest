#include "DeviceGLFW.h"
#include "Base/Director.h"

#if GLFW_SUPPORT

NS_BEGIN

DeviceGLFW::DeviceGLFW()
: _pWindow(nullptr)
, _pMonitor(nullptr)
{
	glfwSetErrorCallback(DeviceGLFW::onGLFWError);
	glfwInit();
}

DeviceGLFW::~DeviceGLFW()
{
	glfwTerminate();
}

DeviceGLFW* DeviceGLFW::create(Device::Parameters& para)
{
	DeviceGLFW* pRet = new DeviceGLFW();
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

bool DeviceGLFW::init(Device::Parameters& para)
{
	do 
	{
		_pMonitor = glfwGetPrimaryMonitor();

		const GLFWvidmode* videoMode = glfwGetVideoMode(_pMonitor);

		if (para._bFullscreen)
		{
			if (!_pMonitor)
				break;

			para._nXDesiredPositionOnScreen = 0;
			para._nYDesiredPositionOnScreen = 0;
			para._nWidthDesiredOnScreen = videoMode->width;
			para._nHeightDesiredOnScreen = videoMode->height;
		}
		else
		{
			glfwGetMonitorPos(_pMonitor, (int*)para._nXDesiredPositionOnScreen, (int*)para._nYDesiredPositionOnScreen);
			para._nXDesiredPositionOnScreen += (videoMode->width - para._nWidthDesiredOnScreen) * 0.5f;
			para._nYDesiredPositionOnScreen += (videoMode->height - para._nHeightDesiredOnScreen) * 0.5f;
			_pMonitor = nullptr;
		}

		glfwWindowHint(GLFW_RESIZABLE, para._bResizable ? GL_TRUE : GL_FALSE);
		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);

		char* title = nullptr;
#if PLATFORM_WIN32
		int len = WideCharToMultiByte(CP_ACP, 0, para._szTitle, wcslen(para._szTitle), NULL, 0, NULL, NULL);
		title = new char[len + 1];
		WideCharToMultiByte(CP_ACP, 0, para._szTitle, wcslen(para._szTitle), title, len, NULL, NULL);
		title[len] = '\0';
#else
		title = para._szTitle;
#endif
		_pWindow = glfwCreateWindow(para._nWidthDesiredOnScreen, para._nHeightDesiredOnScreen, title, _pMonitor, nullptr);
		
		glfwSetWindowPos(_pWindow, para._nXDesiredPositionOnScreen, para._nYDesiredPositionOnScreen);

		glfwMakeContextCurrent(_pWindow);

		glfwSetMouseButtonCallback(_pWindow, DeviceGLFW::onGLFWMouseCallBack);
		glfwSetCursorPosCallback(_pWindow, DeviceGLFW::onGLFWMouseMoveCallBack);
		glfwSetScrollCallback(_pWindow, DeviceGLFW::onGLFWMouseScrollCallback);
		glfwSetCharCallback(_pWindow, DeviceGLFW::onGLFWCharCallback);
		glfwSetKeyCallback(_pWindow, DeviceGLFW::onGLFWKeyCallback);
		glfwSetWindowPosCallback(_pWindow, DeviceGLFW::onGLFWWindowPosCallback);
		glfwSetFramebufferSizeCallback(_pWindow, DeviceGLFW::onGLFWframebuffersize);
		glfwSetWindowSizeCallback(_pWindow, DeviceGLFW::onGLFWWindowSizeFunCallback);
		glfwSetWindowIconifyCallback(_pWindow, DeviceGLFW::onGLFWWindowIconifyCallback);

		if (!makeGLEW())
			break;

		// Enable point size by default.
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		_sParameters = para;

		retain();

		return true;
	} while (0);

	return false;
}

bool DeviceGLFW::run()
{
	glfwPollEvents();

	glfwSwapBuffers(_pWindow);

	if (_pWindow)
		return glfwWindowShouldClose(_pWindow) ? false : true;

	return false;
}

void DeviceGLFW::closeDevice()
{
	if (_pWindow)
	{
		glfwSetWindowShouldClose(_pWindow, 1);
		_pWindow = nullptr;
	}
}

#if PLATFORM_WIN32
static bool glew_dynamic_binding()
{
	const char *gl_extensions = (const char*)glGetString(GL_EXTENSIONS);

	// If the current opengl driver doesn't have framebuffers methods, check if an extension exists
	if (glGenFramebuffers == nullptr)
	{
		Logger_Log("OpenGL: glGenFramebuffers is nullptr, try to detect an extension");
		if (strstr(gl_extensions, "ARB_framebuffer_object"))
		{
			Logger_Log("OpenGL: ARB_framebuffer_object is supported");

			glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbuffer");
			glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
			glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffers");
			glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
			glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
			glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameteriv");
			glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebuffer");
			glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
			glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
			glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
			glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
			glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1D");
			glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
			glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3D");
			glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
			glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameteriv");
			glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
		}
		else
			if (strstr(gl_extensions, "EXT_framebuffer_object"))
			{
				Logger_Log("OpenGL: EXT_framebuffer_object is supported");
				glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)wglGetProcAddress("glIsRenderbufferEXT");
				glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbufferEXT");
				glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
				glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffersEXT");
				glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorageEXT");
				glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
				glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)wglGetProcAddress("glIsFramebufferEXT");
				glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebufferEXT");
				glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
				glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffersEXT");
				glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
				glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
				glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
				glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
				glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
				glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
				glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmapEXT");
			}
			else
			{
				Logger_Log("OpenGL: No framebuffers extension is supported");
				Logger_Log("OpenGL: Any call to Fbo will crash!");
				return false;
			}
	}
	return true;
}
#endif

bool DeviceGLFW::makeGLEW()
{
#if !PLATFORM_MAC
	GLenum GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
		Logger_Log("OpenGL error: %s", (char *)glewGetErrorString(GlewInitResult));
		return false;
	}

	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
	{
		Logger_Log("Ready for GLSL");
	}
	else
	{
		Logger_Log("Not totally ready :(");
	}

	if (glewIsSupported("GL_VERSION_2_0"))
	{
		Logger_Log("Ready for OpenGL 2.0");
	}
	else
	{
		Logger_Log("OpenGL 2.0 not supported");
	}

#	if PLATFORM_WIN32
	if (glew_dynamic_binding() == false)
	{
		MessageBox(NULL, __TEXT("No OpenGL framebuffer support. Please upgrade the driver of your video card."), __TEXT("OpenGL error"), MB_OK);
		return false;
	}
#	endif

#endif
	return true;
}

void DeviceGLFW::onGLFWError(int errorID, const char* errorDesc)
{
	Logger_Log("Error: GLFWError #%d Happen, %s", errorID, errorDesc);
}

void DeviceGLFW::onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify)
{

}

void DeviceGLFW::onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
{

}

void DeviceGLFW::onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
{

}

void DeviceGLFW::onGLFWKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (GLFW_RELEASE == action)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			Director::getInstance()->getDevice()->closeDevice();
		}
	}
}

void DeviceGLFW::onGLFWCharCallback(GLFWwindow *window, unsigned int character)
{
	char16_t wcharString[2] = { (char16_t)character, 0 };
	std::string utf8String;

}

void DeviceGLFW::onGLFWWindowPosCallback(GLFWwindow *windows, int x, int y)
{
}

void DeviceGLFW::onGLFWframebuffersize(GLFWwindow* window, int w, int h)
{

}

void DeviceGLFW::onGLFWWindowSizeFunCallback(GLFWwindow *window, int width, int height)
{

}

void DeviceGLFW::onGLFWWindowIconifyCallback(GLFWwindow* window, int iconified)
{
	if (iconified == GL_TRUE)
	{
	}
	else
	{
	}
}

NS_END

#endif