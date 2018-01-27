#pragma once

#include "Base/Allocator.h"

#if DEBUG_MODE
#define Logger_Log(format, ...)                                                                       \
				MACRO_BLOCK_BEGIN																	   \
					NS_::Console::debugPrintf(__FILE__ "(" STRINGIZE(__LINE__) "): " format "\n", ##__VA_ARGS__); \
				MACRO_BLOCK_END

#define Logger_Warn(condition, format, ...)                        \
				MACRO_BLOCK_BEGIN								   \
					if (!IGNORE_C4127(condition) )                 \
					{                                              \
						Logger_Log("WARN " format, ##__VA_ARGS__); \
					}                                              \
				MACRO_BLOCK_END

#define Logger_Assert(condition, format, ...)                        \
				MACRO_BLOCK_BEGIN									 \
					if (!IGNORE_C4127(condition) )					 \
					{												 \
						Logger_Log("ASSERT " format, ##__VA_ARGS__);  \
						NS_::Console::debugBreak();                  \
					}												 \
				MACRO_BLOCK_END
#else
#	define Logger_Log(_format, ...)					do {} while (0);
#	define Logger_Warn(condition, format, ...)		do {} while (0);
#	define Logger_Assert(condition, format, ...)	do {} while (0);
#endif

NS_BEGIN

class Console
{
public:
	static void debugBreak();

	static void debugOutput(const char* _out);

	static void debugPrintfVargs(const char* _format, va_list _argList);

	static void debugPrintf(const char* _format, ...);
};

NS_END