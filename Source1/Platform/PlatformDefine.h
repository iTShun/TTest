#pragma once

#ifdef __cplusplus
#	define NS_BEGIN                     namespace soldier {
#	define NS_END                       }
#	define USING_NS                     using namespace soldier
#	define NS_                          soldier
#	define NS_NAME						"Soldier"
#else
#	define NS_BEGIN
#	define NS_END
#	define USING_NS
#	define NS_
#	define NS_NAME						""
#endif

/// Enable use of tinystl.
#ifndef CONFIG_USE_TINYSTL
#	define CONFIG_USE_TINYSTL	1
#endif // CONFIG_USE_TINYSTL


