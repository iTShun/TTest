#pragma once

#include "Base/Mutex.h"
#include "Base/Spscqueue.h"

NS_BEGIN

///
template <typename Ty>
class MpScUnboundedQueueT
{
	CLASS(MpScUnboundedQueueT
		, NO_COPY
		, NO_ASSIGNMENT
	);

public:
	///
	MpScUnboundedQueueT(AllocatorI* _allocator);

	///
	~MpScUnboundedQueueT();

	///
	void push(Ty* _ptr); // producer only

						 ///
	Ty* peek(); // consumer only

				///
	Ty* pop(); // consumer only

private:
	Mutex m_write;
	SpScUnboundedQueueT<Ty> m_queue;
};

///
template <typename Ty>
class MpScUnboundedBlockingQueue
{
	CLASS(MpScUnboundedBlockingQueue
		, NO_COPY
		, NO_ASSIGNMENT
	);

public:
	///
	MpScUnboundedBlockingQueue(AllocatorI* _allocator);

	///
	~MpScUnboundedBlockingQueue();

	///
	void push(Ty* _ptr); // producer only

						 ///
	Ty* pop(); // consumer only

private:
	MpScUnboundedQueueT<Ty> m_queue;
	Semaphore m_sem;
};

template <typename Ty>
inline MpScUnboundedQueueT<Ty>::MpScUnboundedQueueT(AllocatorI* _allocator)
	: m_queue(_allocator)
{
}

template <typename Ty>
inline MpScUnboundedQueueT<Ty>::~MpScUnboundedQueueT()
{
}

template <typename Ty>
inline void MpScUnboundedQueueT<Ty>::push(Ty* _ptr)
{
	MutexScope lock(m_write);
	m_queue.push(_ptr);
}

template <typename Ty>
inline Ty* MpScUnboundedQueueT<Ty>::peek()
{
	return m_queue.peek();
}

template <typename Ty>
inline Ty* MpScUnboundedQueueT<Ty>::pop()
{
	return m_queue.pop();
}

template <typename Ty>
inline MpScUnboundedBlockingQueue<Ty>::MpScUnboundedBlockingQueue(AllocatorI* _allocator)
	: m_queue(_allocator)
{
}

template <typename Ty>
inline MpScUnboundedBlockingQueue<Ty>::~MpScUnboundedBlockingQueue()
{
}

template <typename Ty>
inline void MpScUnboundedBlockingQueue<Ty>::push(Ty* _ptr)
{
	m_queue.push(_ptr);
	m_sem.post();
}

template <typename Ty>
inline Ty* MpScUnboundedBlockingQueue<Ty>::pop()
{
	m_sem.wait();
	return m_queue.pop();
}

NS_END