#include "Allocator.h"

#include <malloc.h>

#ifndef CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
#	define CONFIG_ALLOCATOR_NATURAL_ALIGNMENT 8
#endif // CONFIG_ALLOCATOR_NATURAL_ALIGNMENT

NS_BEGIN

DefaultAllocator::DefaultAllocator()
{
}

DefaultAllocator::~DefaultAllocator()
{
}

void* DefaultAllocator::realloc(void* _ptr, size_t _size, size_t _align, const char* _file, uint32_t _line)
{
	if (0 == _size)
	{
		if (NULL != _ptr)
		{
			if (CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
			{
				::free(_ptr);
				return NULL;
			}

#	if COMPILER_MSVC
			UNUSED(_file, _line);
			_aligned_free(_ptr);
#	else
			NS_::alignedFree(this, _ptr, _align, _file, _line);
#	endif
		}

		return NULL;
	}
	else if (NULL == _ptr)
	{
		if (CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
		{
			return ::malloc(_size);
		}

#	if COMPILER_MSVC
		UNUSED(_file, _line);
		return _aligned_malloc(_size, _align);
#	else
		return NS_::alignedAlloc(this, _size, _align, _file, _line);
#	endif
	}

	if (CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
	{
		return ::realloc(_ptr, _size);
	}

#	if COMPILER_MSVC
	UNUSED(_file, _line);
	return _aligned_realloc(_ptr, _size, _align);
#	else
	return NS_::alignedRealloc(this, _ptr, _size, _align, _file, _line);
#	endif
}

NS_END