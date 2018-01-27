#pragma once

#include <alloca.h> // alloca
#include <stdarg.h> // va_list
#include <stdint.h> // uint32_t
#include <stdlib.h> // size_t
#include <stddef.h> // ptrdiff_t

#include "Platform/Platform.h"

///
#define COUNTOF(_x) sizeof(NS_::COUNTOF_REQUIRES_ARRAY_ARGUMENT(_x) )

///
#define IGNORE_C4127(_x) NS_::ignoreC4127(!!(_x) )

///
#define ENABLED(_x) NS_::isEnabled<!!(_x)>()

NS_BEGIN

const int32_t kExitSuccess = 0;
const int32_t kExitFailure = 1;

/// Template for avoiding MSVC: C4127: conditional expression is constant
template<bool>
constexpr bool isEnabled();

/// Exchange two values.
template<typename Ty>
void xchg(Ty& _a, Ty& _b);

/// Exchange memory.
void xchg(void* _a, void* _b, size_t _numBytes);

/// Returns minimum of two values.
template<typename Ty>
constexpr Ty min(const Ty& _a, const Ty& _b);

/// Returns maximum of two values.
template<typename Ty>
constexpr Ty max(const Ty& _a, const Ty& _b);

/// Returns minimum of three values.
template<typename Ty>
constexpr Ty min(const Ty& _a, const Ty& _b, const Ty& _c);

/// Returns maximum of three values.
template<typename Ty>
constexpr Ty max(const Ty& _a, const Ty& _b, const Ty& _c);

/// Returns middle of three values.
template<typename Ty>
constexpr Ty mid(const Ty& _a, const Ty& _b, const Ty& _c);

/// Returns clamped value between min/max.
template<typename Ty>
constexpr Ty clamp(const Ty& _a, const Ty& _min, const Ty& _max);

/// Returns true if value is power of 2.
template<typename Ty>
constexpr bool isPowerOf2(Ty _a);

// http://cnicholson.net/2011/01/stupid-c-tricks-a-better-sizeof_array/
template<typename T, size_t N>
char(&COUNTOF_REQUIRES_ARRAY_ARGUMENT(const T(&)[N]))[N];

///
void memCopy(void* _dst, const void* _src, size_t _numBytes);

///
void memCopy(void* _dst, const void* _src, uint32_t _size, uint32_t _num, uint32_t _srcPitch, uint32_t _dstPitch);

///
void gather(void* _dst, const void* _src, uint32_t _size, uint32_t _num, uint32_t _srcPitch);

///
void scatter(void* _dst, const void* _src, uint32_t _size, uint32_t _num, uint32_t _dstPitch);

///
void memMove(void* _dst, const void* _src, size_t _numBytes);

///
void memSet(void* _dst, uint8_t _ch, size_t _numBytes);

///
int32_t memCmp(const void* _lhs, const void* _rhs, size_t _numBytes);

template<bool>
inline constexpr bool isEnabled()
{
	return true;
}

template<>
inline constexpr bool isEnabled<false>()
{
	return false;
}

inline constexpr bool ignoreC4127(bool _x)
{
	return _x;
}

template<typename Ty>
inline void xchg(Ty& _a, Ty& _b)
{
	Ty tmp = _a; _a = _b; _b = tmp;
}

template<typename Ty>
inline constexpr Ty min(const Ty& _a, const Ty& _b)
{
	return _a < _b ? _a : _b;
}

template<typename Ty>
inline constexpr Ty max(const Ty& _a, const Ty& _b)
{
	return _a > _b ? _a : _b;
}

template<typename Ty>
inline constexpr Ty min(const Ty& _a, const Ty& _b, const Ty& _c)
{
	return min(min(_a, _b), _c);
}

template<typename Ty>
inline constexpr Ty max(const Ty& _a, const Ty& _b, const Ty& _c)
{
	return max(max(_a, _b), _c);
}

template<typename Ty>
inline constexpr Ty mid(const Ty& _a, const Ty& _b, const Ty& _c)
{
	return max(min(_a, _b), min(max(_a, _b), _c));
}

template<typename Ty>
inline constexpr Ty clamp(const Ty& _a, const Ty& _min, const Ty& _max)
{
	return max(min(_a, _max), _min);
}

template<typename Ty>
inline constexpr bool isPowerOf2(Ty _a)
{
	return _a && !(_a & (_a - 1));
}

NS_END