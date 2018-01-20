#pragma once

#include <assert.h>

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

#if defined(DEBUG) || defined(_DEBUG)
#	define DEBUG_MODE				1
#else
#	define DEBUG_MODE				0
#endif

#if DEBUG_MODE > 0

#	define Logger_Log(format, ...)			NS_::log(format, ##__VA_ARGS__)
#	define Logger_Assert(condition, msg)	do {	\
	if (!(condition)) {								\
		NS_::log("Assert failed: %s", msg);			\
		assert(condition);							\
	}												\
} while (0)

#else

#	define Logger_Log(format, ...)					do {} while (0)
#	define Logger_Assert(condition, format, ...)	do {} while (0)

#endif

#define SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)

/** @def DEPRECATED_ATTRIBUTE
* Only certain compilers support __attribute__((deprecated)).
*/
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#	define DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#elif _MSC_VER >= 1400 //vs 2005 or higher
#	define DEPRECATED_ATTRIBUTE __declspec(deprecated) 
#else
#	define DEPRECATED_ATTRIBUTE
#endif

/** @def FORMAT_PRINTF(formatPos, argPos)
* Only certain compiler support __attribute__((format))
*
* @param formatPos 1-based position of format string argument.
* @param argPos    1-based position of first format-dependent argument.
*/
#if defined(__GNUC__) && (__GNUC__ >= 4)
#	define FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#elif defined(__has_attribute)
#	if __has_attribute(format)
#		define FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#	endif // __has_attribute(format)
#else
#	define FORMAT_PRINTF(formatPos, argPos)
#endif
