#pragma once

#include "Platform/Platform.h"

#if PLATFORM_ANDROID

#elif PLATFORM_BSD

#elif PLATFORM_EMSCRIPTEN

#elif PLATFORM_HURD

#elif PLATFORM_IOS

#elif PLATFORM_LINUX

#elif PLATFORM_NX

#elif PLATFORM_OSX

#elif PLATFORM_PS4

#elif PLATFORM_RPI

#elif PLATFORM_STEAMLINK

#elif PLATFORM_WINDOWS
#	include "Platform/Windows/Device-windows.h"
#elif PLATFORM_WINRT

#elif PLATFORM_XBOXONE

#else
#	error "Unknown PLATFORM DEVICE!"
#endif // PLATFORM_