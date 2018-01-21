#pragma once

#include "Platform/Platform.h"

#if PLATFORM_WIN32
#	include "Platform/Win32/DeviceWin32.h"
#elif PLATFORM_MAC
#   include "Platform/Mac/DeviceMac.h"
#endif