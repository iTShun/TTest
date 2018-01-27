#pragma once

#include "Base/Console.h"
#include "Base/Cpu.h"
#include "Base/Semaphore.h"

NS_BEGIN

///
class SpScUnboundedQueue
{
	CLASS(SpScUnboundedQueue
		, NO_COPY
		, NO_ASSIGNMENT
	);

public:
	///
	SpScUnboundedQueue(AllocatorI* _allocator);

	///
	~SpScUnboundedQueue();

	///
	void push(void* _ptr);

	///
	void* peek();

	///
	void* pop();

private:
	struct Node
	{
		///
		Node(void* _ptr);

		void* m_ptr;
		Node* m_next;
	};

	AllocatorI* m_allocator;
	Node* m_first;
	Node* m_divider;
	Node* m_last;
};

///
template<typename Ty>
class SpScUnboundedQueueT
{
	CLASS(SpScUnboundedQueueT
		, NO_COPY
		, NO_ASSIGNMENT
	);

public:
	///
	SpScUnboundedQueueT(AllocatorI* _allocator);

	///
	~SpScUnboundedQueueT();

	///
	void push(Ty* _ptr);

	///
	Ty* peek();

	///
	Ty* pop();

private:
	SpScUnboundedQueue m_queue;
};

#if CONFIG_SUPPORTS_THREADING
///
class SpScBlockingUnboundedQueue
{
	CLASS(SpScBlockingUnboundedQueue
		, NO_COPY
		, NO_ASSIGNMENT
	);

public:
	///
	SpScBlockingUnboundedQueue(AllocatorI* _allocator);

	///
	~SpScBlockingUnboundedQueue();

	///
	void push(void* _ptr); // producer only

						   ///
	void* peek(); // consumer only

				  ///
	void* pop(int32_t _msecs = -1); // consumer only

private:
	Semaphore m_count;
	SpScUnboundedQueue m_queue;
};

///
template<typename Ty>
class SpScBlockingUnboundedQueueT
{
	CLASS(SpScBlockingUnboundedQueueT
		, NO_COPY
		, NO_ASSIGNMENT
	);

public:
	///
	SpScBlockingUnboundedQueueT(AllocatorI* _allocator);

	///
	~SpScBlockingUnboundedQueueT();

	///
	void push(Ty* _ptr); // producer only

						 ///
	Ty* peek(); // consumer only

				///
	Ty* pop(int32_t _msecs = -1); // consumer only

private:
	SpScBlockingUnboundedQueue m_queue;
};
#endif // CONFIG_SUPPORTS_THREADING

// http://drdobbs.com/article/print?articleId=210604448&siteSectionName=
inline SpScUnboundedQueue::SpScUnboundedQueue(AllocatorI* _allocator)
	: m_allocator(_allocator)
	, m_first(NEW(m_allocator, Node)(NULL))
	, m_divider(m_first)
	, m_last(m_first)
{
}

inline SpScUnboundedQueue::~SpScUnboundedQueue()
{
	while (NULL != m_first)
	{
		Node* node = m_first;
		m_first = node->m_next;
		DELETE(m_allocator, node);
	}
}

inline void SpScUnboundedQueue::push(void* _ptr)
{
	m_last->m_next = NEW(m_allocator, Node)(_ptr);
	atomicExchangePtr((void**)&m_last, m_last->m_next);
	while (m_first != m_divider)
	{
		Node* node = m_first;
		m_first = m_first->m_next;
		DELETE(m_allocator, node);
	}
}

inline void* SpScUnboundedQueue::peek()
{
	if (m_divider != m_last)
	{
		return m_divider->m_next->m_ptr;
	}

	return NULL;
}

inline void* SpScUnboundedQueue::pop()
{
	if (m_divider != m_last)
	{
		void* ptr = m_divider->m_next->m_ptr;
		atomicExchangePtr((void**)&m_divider, m_divider->m_next);
		return ptr;
	}

	return NULL;
}

inline SpScUnboundedQueue::Node::Node(void* _ptr)
	: m_ptr(_ptr)
	, m_next(NULL)
{
}

template<typename Ty>
inline SpScUnboundedQueueT<Ty>::SpScUnboundedQueueT(AllocatorI* _allocator)
	: m_queue(_allocator)
{
}

template<typename Ty>
inline SpScUnboundedQueueT<Ty>::~SpScUnboundedQueueT()
{
}

template<typename Ty>
inline void SpScUnboundedQueueT<Ty>::push(Ty* _ptr)
{
	m_queue.push(_ptr);
}

template<typename Ty>
inline Ty* SpScUnboundedQueueT<Ty>::peek()
{
	return (Ty*)m_queue.peek();
}

template<typename Ty>
inline Ty* SpScUnboundedQueueT<Ty>::pop()
{
	return (Ty*)m_queue.pop();
}

#if CONFIG_SUPPORTS_THREADING
inline SpScBlockingUnboundedQueue::SpScBlockingUnboundedQueue(AllocatorI* _allocator)
	: m_queue(_allocator)
{
}

inline SpScBlockingUnboundedQueue::~SpScBlockingUnboundedQueue()
{
}

inline void SpScBlockingUnboundedQueue::push(void* _ptr)
{
	m_queue.push(_ptr);
	m_count.post();
}

inline void* SpScBlockingUnboundedQueue::peek()
{
	return m_queue.peek();
}

inline void* SpScBlockingUnboundedQueue::pop(int32_t _msecs)
{
	if (m_count.wait(_msecs))
	{
		return m_queue.pop();
	}

	return NULL;
}

template<typename Ty>
inline SpScBlockingUnboundedQueueT<Ty>::SpScBlockingUnboundedQueueT(AllocatorI* _allocator)
	: m_queue(_allocator)
{
}

template<typename Ty>
inline SpScBlockingUnboundedQueueT<Ty>::~SpScBlockingUnboundedQueueT()
{
}

template<typename Ty>
inline void SpScBlockingUnboundedQueueT<Ty>::push(Ty* _ptr)
{
	m_queue.push(_ptr);
}

template<typename Ty>
inline Ty* SpScBlockingUnboundedQueueT<Ty>::peek()
{
	return (Ty*)m_queue.peek();
}

template<typename Ty>
inline Ty* SpScBlockingUnboundedQueueT<Ty>::pop(int32_t _msecs)
{
	return (Ty*)m_queue.pop(_msecs);
}

#endif // CONFIG_SUPPORTS_THREADING

NS_END