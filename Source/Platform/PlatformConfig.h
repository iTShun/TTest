#pragma once

#define PLATFORM_WIN32				0
#define PLATFORM_MAC				0
#define PLATFORM_IOS				0

// win32
#if defined(_WIN32) && defined(_WINDOWS)
#	undef  PLATFORM_WIN32
#	define PLATFORM_WIN32			1
#endif

// Apple: Mac and iOS
#if defined(__APPLE__) && !defined(ANDROID) // exclude android for binding generator.
#   include <TargetConditionals.h>
#   if TARGET_OS_IPHONE // TARGET_OS_IPHONE includes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
#       undef  PLATFORM_IOS
#       define PLATFORM_IOS        1
#   elif TARGET_OS_MAC
#       undef  PLATFORM_MAC
#       define PLATFORM_MAC        1
#   endif
#endif