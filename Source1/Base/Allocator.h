#pragma once

#include "Platform/Platform.h"
#include "Platform/PlatformUtils.h"

#if DEBUG_MODE
#define _FILE_STRING_  __FILE__
#define _LINE_STRING_  __LINE__
#else
#	define _FILE_STRING_   0
#	define _LINE_STRING_   0
#endif

#define ALLOC_(_allocator, _size)                         NS_::alloc(_allocator, _size, 0, _FILE_STRING_, _LINE_STRING_)
#define REALLOC_(_allocator, _ptr, _size)                 NS_::realloc(_allocator, _ptr, _size, 0, _FILE_STRING_, _LINE_STRING_)
#define FREE_(_allocator, _ptr)                           NS_::free(_allocator, _ptr, 0, _FILE_STRING_, _LINE_STRING_)
#define ALIGNED_ALLOC_(_allocator, _size, _align)         NS_::alloc(_allocator, _size, _align, _FILE_STRING_, _LINE_STRING_)
#define ALIGNED_REALLOC_(_allocator, _ptr, _size, _align) NS_::realloc(_allocator, _ptr, _size, _align, _FILE_STRING_, _LINE_STRING_)
#define ALIGNED_FREE_(_allocator, _ptr, _align)           NS_::free(_allocator, _ptr, _align, _FILE_STRING_, _LINE_STRING_)
#define DELETE(_allocator, _ptr)                          NS_::deleteObject(_allocator, _ptr, 0, _FILE_STRING_, _LINE_STRING_)
#define ALIGNED_DELETE(_allocator, _ptr, _align)          NS_::deleteObject(_allocator, _ptr, _align, _FILE_STRING_, _LINE_STRING_)

#define NEW(_allocator, _type)                 PLACEMENT_NEW(ALLOC_(_allocator, sizeof(_type) ), _type)
#define ALIGNED_NEW(_allocator, _type, _align) PLACEMENT_NEW(ALIGNED_ALLOC_(_allocator, sizeof(_type), _align), _type)
#define PLACEMENT_NEW(_ptr, _type)             ::new(NS_::PlacementNewTag(), _ptr) _type

NS_BEGIN struct PlacementNewTag {}; NS_END

inline void* operator new(size_t, NS_::PlacementNewTag, void* _ptr)
{
	return _ptr;
}

inline void  operator delete(void*, NS_::PlacementNewTag, void*) throw()
{

}

NS_BEGIN

/// Abstract allocator interface.
///
struct NO_VTABLE AllocatorI
{
	///
	virtual ~AllocatorI() = 0;

	/// Allocates, resizes memory block, or frees memory.
	///
	/// @param[in] _ptr If _ptr is NULL new block will be allocated.
	/// @param[in] _size If _ptr is set, and _size is 0, memory will be freed.
	/// @param[in] _align Alignment.
	/// @param[in] _file Debug file path info.
	/// @param[in] _line Debug file line info.
	virtual void* realloc(
		void* _ptr
		, size_t _size
		, size_t _align
		, const char* _file
		, uint32_t _line
	) = 0;
};

inline AllocatorI::~AllocatorI()
{
}

///
class DefaultAllocator : public AllocatorI
{
public:
	///
	DefaultAllocator();

	///
	virtual ~DefaultAllocator();

	///
	virtual void* realloc(
		void* _ptr
		, size_t _size
		, size_t _align
		, const char* _file
		, uint32_t _line
	) override;
};

/// Check if pointer is aligned. _align must be power of two.
inline bool isAligned(const void* _ptr, size_t _align)
{
	union { const void* ptr; uintptr_t addr; } un;
	un.ptr = _ptr;
	return 0 == (un.addr & (_align - 1));
}

/// Aligns pointer to nearest next aligned address. _align must be power of two.
inline void* alignPtr(
	void* _ptr
	, size_t _extra
	, size_t _align = 0
)
{
	union { void* ptr; uintptr_t addr; } un;
	un.ptr = _ptr;
	uintptr_t unaligned = un.addr + _extra; // space for header
	uintptr_t mask = _align - 1;
	uintptr_t aligned = ALIGN_MASK(unaligned, mask);
	un.addr = aligned;
	return un.ptr;
}

/// Allocate memory.
inline void* alloc(
	AllocatorI* _allocator
	, size_t _size
	, size_t _align = 0
	, const char* _file = NULL
	, uint32_t _line = 0
)
{
	return _allocator->realloc(NULL, _size, _align, _file, _line);
}

/// Free memory.
inline void free(
	AllocatorI* _allocator
	, void* _ptr
	, size_t _align = 0
	, const char* _file = NULL
	, uint32_t _line = 0
)
{
	_allocator->realloc(_ptr, 0, _align, _file, _line);
}

/// Resize memory block.
inline void* realloc(
	AllocatorI* _allocator
	, void* _ptr
	, size_t _size
	, size_t _align = 0
	, const char* _file = NULL
	, uint32_t _line = 0
)
{
	return _allocator->realloc(_ptr, _size, _align, _file, _line);
}

/// Allocate memory with specific alignment.
inline void* alignedAlloc(
	AllocatorI* _allocator
	, size_t _size
	, size_t _align
	, const char* _file = NULL
	, uint32_t _line = 0
)
{
	size_t total = _size + _align;
	uint8_t* ptr = (uint8_t*)alloc(_allocator, total, 0, _file, _line);
	uint8_t* aligned = (uint8_t*)alignPtr(ptr, sizeof(uint32_t), _align);
	uint32_t* header = (uint32_t*)aligned - 1;
	*header = uint32_t(aligned - ptr);
	return aligned;
}

/// Free memory that was allocated with aligned allocator.
inline void alignedFree(
	AllocatorI* _allocator
	, void* _ptr
	, size_t _align
	, const char* _file = NULL
	, uint32_t _line = 0
)
{
	UNUSED(_align);
	uint8_t* aligned = (uint8_t*)_ptr;
	uint32_t* header = (uint32_t*)aligned - 1;
	uint8_t* ptr = aligned - *header;
	free(_allocator, ptr, 0, _file, _line);
}

/// Resize memory block that was allocated with aligned allocator.
inline void* alignedRealloc(
	AllocatorI* _allocator
	, void* _ptr
	, size_t _size
	, size_t _align
	, const char* _file = NULL
	, uint32_t _line = 0
)
{
	if (NULL == _ptr)
	{
		return alignedAlloc(_allocator, _size, _align, _file, _line);
	}

	uint8_t* aligned = (uint8_t*)_ptr;
	uint32_t offset = *((uint32_t*)aligned - 1);
	uint8_t* ptr = aligned - offset;
	size_t total = _size + _align;
	ptr = (uint8_t*)realloc(_allocator, ptr, total, 0, _file, _line);
	uint8_t* newAligned = (uint8_t*)alignPtr(ptr, sizeof(uint32_t), _align);

	if (newAligned == aligned)
	{
		return aligned;
	}

	aligned = ptr + offset;
	memMove(newAligned, aligned, _size);
	uint32_t* header = (uint32_t*)newAligned - 1;
	*header = uint32_t(newAligned - ptr);
	return newAligned;
}

/// Delete object with specific allocator.
template <typename ObjectT>
inline void deleteObject(
	AllocatorI* _allocator
	, ObjectT* _object
	, size_t _align = 0
	, const char* _file = NULL
	, uint32_t _line = 0
)
{
	if (NULL != _object)
	{
		_object->~ObjectT();
		free(_allocator, _object, _align, _file, _line);
	}
}

NS_END