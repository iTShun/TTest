#pragma once

#include "Base/Console.h"

NS_BEGIN

///
class Mutex
{
	CLASS(Mutex
		, NO_COPY
		, NO_ASSIGNMENT
	);

public:
	///
	Mutex();

	///
	~Mutex();

	///
	void lock();

	///
	void unlock();

private:
	ALIGN_DECL(16, uint8_t) m_internal[64];
};

///
class MutexScope
{
	CLASS(MutexScope
		, NO_DEFAULT_CTOR
		, NO_COPY
		, NO_ASSIGNMENT
	);

public:
	///
	MutexScope(Mutex& _mutex)
	: m_mutex(_mutex)
	{
		m_mutex.lock();
	}

	///
	~MutexScope()
	{
		m_mutex.unlock();
	}

private:
	Mutex& m_mutex;
};

NS_END