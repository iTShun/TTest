#pragma once

#define PLATFORM_WIN32				0

// win32
#if defined(_WIN32) && defined(_WINDOWS)
#	undef  PLATFORM_WIN32
#	define PLATFORM_WIN32			1
#endif