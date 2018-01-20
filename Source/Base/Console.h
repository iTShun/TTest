#pragma once

#if defined(_MSC_VER) || defined(__MINGW32__)
#	include <BaseTsd.h>
#	include <WinSock2.h>

#ifndef __SSIZE_T
#define __SSIZE_T
	typedef SSIZE_T ssize_t;
#endif // __SSIZE_T

#else
#	include <sys/select.h>
#endif

#include "Base/Ref.h"

NS_BEGIN

/// The max length of Logger_Log message.
static const int MAX_LOG_LENGTH = 16 * 1024;

/**
@brief Output Debug message.
*/
void log(const char * format, ...) FORMAT_PRINTF(1, 2);

class Console
{
public:
	Console();
	~Console();
};

NS_END