#include "Device-windows.h"

#if PLATFORM_WINDOWS && DEVICE_CONFIG_USE_NATIVE

NS_BEGIN

enum
{
	WM_USER_WINDOW_CREATE = WM_USER,
	WM_USER_WINDOW_DESTROY,
	WM_USER_WINDOW_SET_TITLE,
	WM_USER_WINDOW_SET_FLAGS,
	WM_USER_WINDOW_SET_POS,
	WM_USER_WINDOW_SET_SIZE,
	WM_USER_WINDOW_TOGGLE_FRAME,
	WM_USER_WINDOW_MOUSE_LOCK,
};

struct Msg
{
	Msg()
		: _x(0)
		, _y(0)
		, _width(0)
		, _height(0)
		, _flagsEnabled(false)
	{
	}

	int32_t  _x;
	int32_t  _y;
	uint32_t _width;
	uint32_t _height;
	std::string _title;
	bool _flagsEnabled;
};

LRESULT CALLBACK wndProc(HWND _hwnd, UINT _id, WPARAM _wparam, LPARAM _lparam);

static Device* s_pDevice = nullptr;
static const char* s_szClassName = "DeviceWindows";
static uint32_t s_nWidth = 1280;
static uint32_t s_nHeight = 720;

Device::Device()
:  _nWidth(0)
, _nHeight(0)
, _fAspectRatio(0.0f)
, _bFrame(true)
, _nOldWidth(0)
, _nOldHeight(0)
, _nFrameWidth(0)
, _nFrameHeight(0)
, _nX(0)
, _nY(0)
{
	s_pDevice = static_cast<Device*>(this);

	for (size_t i = 0; i < DEVICE_CONFIG_MAX_WINDOWS; i++)
	{
		_hwnd[i] = 0;
		_nWindowID[i] = UINT16_MAX;
	}
}

Device::~Device()
{
	s_pDevice = nullptr;
}

Device* Device::create()
{
	Device* pRet = new (std::nothrow)Device();
	if (pRet && pRet->init())
	{

	}
	else
	{
		delete pRet;
		pRet = nullptr;
	}

	return pRet;
}

bool Device::init()
{
	do 
	{
		HINSTANCE instance = (HINSTANCE)GetModuleHandle(NULL);

		WNDCLASSEXA wnd;
		::memset(&wnd, 0, sizeof(wnd));
		wnd.cbSize = sizeof(wnd);
		wnd.style = CS_HREDRAW | CS_VREDRAW;
		wnd.lpfnWndProc = wndProc;
		wnd.hInstance = instance;
		wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
		wnd.lpszClassName = s_szClassName;
		wnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		RegisterClassExA(&wnd);

		getNewWindowHandle();
		_hwnd[0] = CreateWindowExA(
			WS_EX_ACCEPTFILES
			, s_szClassName
			, ""
			, WS_OVERLAPPEDWINDOW | WS_VISIBLE
			, 0
			, 0
			, s_nWidth
			, s_nHeight
			, NULL
			, NULL
			, instance
			, 0
		);

		adjustWindow(_hwnd[0], s_nWidth, s_nHeight, true);
		clearWindow(_hwnd[0]);

		_nWidth = s_nWidth;
		_nHeight = s_nHeight;
		_nOldWidth = s_nWidth;
		_nOldHeight = s_nHeight;

		return true;
	} while (0);

	return false;
}

void Device::clearWindow(HWND _hwnd)
{
	RECT rect;
	GetWindowRect(_hwnd, &rect);
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	HDC hdc = GetDC(_hwnd);
	SelectObject(hdc, brush);
	FillRect(hdc, &rect, brush);
}

void Device::adjustWindow(HWND _hwnd, uint32_t _width, uint32_t _height, bool _windowFrame)
{
	_nWidth = _width;
	_nHeight = _height;
	_fAspectRatio = float(_width) / float(_height);

	ShowWindow(_hwnd, SW_SHOWNORMAL);
	RECT rect;
	RECT newrect = { 0, 0, (LONG)_width, (LONG)_height };
	DWORD style = WS_POPUP | WS_SYSMENU;

	if (_bFrame)
	{
		GetWindowRect(_hwnd, &_sRect);
		_nStyle = GetWindowLong(_hwnd, GWL_STYLE);
	}

	if (_windowFrame)
	{
		rect = _sRect;
		style = _nStyle;
	}
	else
	{
		HMONITOR monitor = MonitorFromWindow(_hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(monitor, &mi);
		newrect = mi.rcMonitor;
		rect = mi.rcMonitor;
		_fAspectRatio = float(newrect.right - newrect.left) / float(newrect.bottom - newrect.top);
	}

	SetWindowLong(_hwnd, GWL_STYLE, style);
	uint32_t prewidth = newrect.right - newrect.left;
	uint32_t preheight = newrect.bottom - newrect.top;
	AdjustWindowRect(&newrect, style, FALSE);
	_nFrameWidth = (newrect.right - newrect.left) - prewidth;
	_nFrameHeight = (newrect.bottom - newrect.top) - preheight;
	UpdateWindow(_hwnd);

	if (rect.left == -32000
		|| rect.top == -32000)
	{
		rect.left = 0;
		rect.top = 0;
	}

	int32_t left = rect.left;
	int32_t top = rect.top;
	int32_t width = (newrect.right - newrect.left);
	int32_t height = (newrect.bottom - newrect.top);

	if (!_windowFrame)
	{
		float aspectRatio = 1.0f / _fAspectRatio;
		width = MAX(s_nWidth / 4, width);
		height = uint32_t(float(width) * aspectRatio);

		left = newrect.left + (newrect.right - newrect.left - width) / 2;
		top = newrect.top + (newrect.bottom - newrect.top - height) / 2;
	}

	SetWindowPos(_hwnd
		, HWND_TOP
		, left
		, top
		, width
		, height
		, SWP_SHOWWINDOW
	);

	ShowWindow(_hwnd, SW_RESTORE);

	_bFrame = _windowFrame;
}

void Device::setMousePos(HWND _hwnd, int32_t _x, int32_t _y)
{
	POINT pt = { _x, _y };
	ClientToScreen(_hwnd, &pt);
	SetCursorPos(pt.x, pt.y);
}

WindowHandle Device::getNewWindowHandle()
{
	WindowHandle _nNew = UINT16_MAX;
	for (size_t i = 0; i < DEVICE_CONFIG_MAX_WINDOWS; i++)
	{
		if (_hwnd[i] == 0)
		{
			_nWindowID[i] = i;
			_nNew = i;
		}
	}

	return _nNew;
}

WindowHandle Device::findHandle(HWND hwnd)
{
	for (size_t i = 0; i < DEVICE_CONFIG_MAX_WINDOWS; i++)
	{
		if (_hwnd[i] == hwnd)
			return _nWindowID[i];
	}

	return UINT16_MAX;
}

HWND Device::findHWND(WindowHandle _handle)
{
	return _hwnd[_handle];
}

void Device::setHWND(WindowHandle _handle, HWND hwnd)
{
	_hwnd[_handle] = hwnd;
}

WindowHandle Device::createWindow(int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, const char* _title /*= ""*/)
{
	do 
	{
		WindowHandle handle = getNewWindowHandle();

		if (!isWindowValid(handle))
			break;

		Msg* msg = new (std::nothrow)Msg;
		msg->_x = _x;
		msg->_y = _y;
		msg->_width = _width;
		msg->_height = _height;
		msg->_title = _title;

		PostMessage(_hwnd[0], WM_USER_WINDOW_CREATE, handle, (LPARAM)msg);

		return true;
	} while (0);

	return false;
}

void Device::destroyWindow(WindowHandle _handle)
{
	if (isWindowValid(_handle))
	{
		PostMessage(_hwnd[0], WM_USER_WINDOW_DESTROY, _handle, 0);
	}
}

void Device::setWindowPos(WindowHandle _handle, int32_t _x, int32_t _y)
{
	Msg* msg = new (std::nothrow)Msg;
	msg->_x = _x;
	msg->_y = _y;
	PostMessage(_hwnd[0], WM_USER_WINDOW_SET_POS, _handle, (LPARAM)msg);
}

void Device::setWindowSize(WindowHandle _handle, uint32_t _width, uint32_t _height)
{
	PostMessage(_hwnd[0], WM_USER_WINDOW_SET_SIZE, _handle, (_height << 16) | (_width & 0xffff));
}

void Device::setWindowTitle(WindowHandle _handle, const char* _title)
{
	Msg* msg = new (std::nothrow)Msg;
	msg->_title = _title;
	PostMessage(_hwnd[0], WM_USER_WINDOW_SET_TITLE, _handle, (LPARAM)msg);
}

void Device::setWindowFlags(WindowHandle _handle, uint32_t _flags, bool _enabled)
{

}

void Device::toggleFullscreen(WindowHandle _handle)
{
	PostMessage(_hwnd[0], WM_USER_WINDOW_TOGGLE_FRAME, _handle, 0);
}

void Device::setMouseLock(WindowHandle _handle, bool _lock)
{
	Msg* msg = new (std::nothrow)Msg;
	msg->_x = _nWidth / 2;
	msg->_y = _nHeight / 2;
	msg->_flagsEnabled = _lock;

	PostMessage(_hwnd[0], WM_USER_WINDOW_MOUSE_LOCK, _handle, (LPARAM)msg);
}

void Device::setCurrentDir(const char* _dir)
{

}

LRESULT CALLBACK wndProc(HWND _hwnd, UINT _id, WPARAM _wparam, LPARAM _lparam)
{
	if (!s_pDevice)
		return DefWindowProc(_hwnd, _id, _wparam, _lparam);

	switch (_id)
	{
		case WM_USER_WINDOW_CREATE:
			{
				Msg* msg = (Msg*)_lparam;
				HWND hwnd = CreateWindowA(
					s_szClassName
					, msg->_title.c_str()
					, WS_OVERLAPPEDWINDOW | WS_VISIBLE
					, msg->_x
					, msg->_y
					, msg->_width
					, msg->_height
					, NULL
					, NULL
					, (HINSTANCE)GetModuleHandle(NULL)
					, 0
				);

				s_pDevice->clearWindow(hwnd);
				s_pDevice->setHWND((WindowHandle)_wparam, hwnd);

				delete msg;
				msg = nullptr;
			}
			break;

		case WM_USER_WINDOW_DESTROY:
			{
				WindowHandle handle = (WindowHandle)_wparam;
				HWND hwnd = s_pDevice->findHWND(handle);
				DestroyWindow(hwnd);
				hwnd = 0;

				if (0 == handle)
				{

				}
			}
			break;

		case WM_USER_WINDOW_SET_TITLE:
			{
				WindowHandle handle = (WindowHandle)_wparam;
				HWND hwnd = s_pDevice->findHWND(handle);
				Msg* msg = (Msg*)_lparam;
				SetWindowTextA(hwnd, msg->_title.c_str());
				delete msg;
				msg = nullptr;
			}
			break;

		case WM_USER_WINDOW_SET_FLAGS:
			{

			}
			break;

		case WM_USER_WINDOW_SET_POS:
			{
				WindowHandle handle = (WindowHandle)_wparam;
				HWND hwnd = s_pDevice->findHWND(handle);
				Msg* msg = (Msg*)_lparam;
				SetWindowPos(hwnd, 0, msg->_x, msg->_y, 0, 0
					, SWP_NOACTIVATE
					| SWP_NOOWNERZORDER
					| SWP_NOSIZE
				);
				delete msg;
				msg = nullptr;
			}
			break;

		case WM_USER_WINDOW_SET_SIZE:
			{
				WindowHandle handle = (WindowHandle)_wparam;
				HWND hwnd = s_pDevice->findHWND(handle);
				uint32_t width = GET_X_LPARAM(_lparam);
				uint32_t height = GET_Y_LPARAM(_lparam);
				s_pDevice->adjustWindow(hwnd, width, height, true);
			}
			break;

		case WM_USER_WINDOW_TOGGLE_FRAME:
			{
				WindowHandle handle = (WindowHandle)_wparam;
				HWND hwnd = s_pDevice->findHWND(handle);
				if (s_pDevice->isFrame())
				{
					uint32_t width, height;
					s_pDevice->getWindowSize(width, height);
					s_pDevice->setWindowOldSize(width, height);
				}
				uint32_t width, height;
				s_pDevice->getWindowOldSize(width, height);
				s_pDevice->adjustWindow(hwnd, width, height, !s_pDevice->isFrame());
			}
			break;

		case WM_USER_WINDOW_MOUSE_LOCK:
			{
				WindowHandle handle = (WindowHandle)_wparam;
				HWND hwnd = s_pDevice->findHWND(handle);
				Msg* msg = (Msg*)_lparam;
				if (msg->_flagsEnabled)
				{
					ShowCursor(false);
					s_pDevice->setMousePos(hwnd, msg->_x, msg->_y);
				}
				else
				{
					s_pDevice->setMousePos(hwnd, msg->_x, msg->_y);
					ShowCursor(true);
				}

				delete msg;
				msg = nullptr;
			}
			break;

		case WM_DESTROY:
			break;

		case WM_QUIT:
		case WM_CLOSE:
			s_pDevice->destroyWindow(s_pDevice->findHandle(_hwnd));
			// Don't process message. Window will be destroyed later.
			return 0;

		case WM_SIZING:
			{

			}
			break;

		case WM_SIZE:
			{

			}
			break;

		case WM_SYSCOMMAND:
			switch (_wparam)
			{
			case SC_MINIMIZE:
			case SC_RESTORE:
			{
				HWND parent = GetWindow(_hwnd, GW_OWNER);
				if (NULL != parent)
				{
					PostMessage(parent, _id, _wparam, _lparam);
				}
			}
			}
			break;

		case WM_MOUSEMOVE:
			{

			}
			break;

		case WM_MOUSEWHEEL:
			{

			}
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_LBUTTONDBLCLK:
			{

			}
			break;

		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
			{

			}
			break;

		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
			{

			}
			break;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			{

			}
			break;

		case WM_CHAR:
			{

			}
			break;

		case WM_DROPFILES:
			{

			}
			break;

	default:
		break;
	}

	return DefWindowProc(_hwnd, _id, _wparam, _lparam);
}

NS_END

#endif