#pragma once

#include "Base/Console.h"

NS_BEGIN

///
class Semaphore
{
	CLASS(Semaphore
		, NO_COPY
		, NO_ASSIGNMENT
	);

public:
	///
	Semaphore();

	///
	~Semaphore();

	///
	void post(uint32_t _count = 1);

	///
	bool wait(int32_t _msecs = -1);

private:
	ALIGN_DECL(16, uint8_t) m_internal[128];
};

NS_END