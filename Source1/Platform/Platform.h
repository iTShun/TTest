#pragma once

#include "Platform/PlatformConfig.h"
#include "Platform/PlatformDefine.h"

#include <stdarg.h> // va_list
#include <stdint.h> // uint32_t
#include <stdlib.h> // size_t
#include <stddef.h> // ptrdiff_t
#include <assert.h>
#include <type_traits>


#if defined(DEBUG) || defined(_DEBUG)
#	define DEBUG_MODE				1
#else
#	define DEBUG_MODE				0
#endif

#ifndef CONFIG_SUPPORTS_THREADING
#	define CONFIG_SUPPORTS_THREADING !(0 \
			|| PLATFORM_EMSCRIPTEN       \
			|| CRT_NONE                  \
			)
#endif // CONFIG_SUPPORTS_THREADING

///
#if COMPILER_MSVC
// Workaround MSVS bug...
#	define VA_ARGS_PASS(...) VA_ARGS_PASS_1_ __VA_ARGS__ VA_ARGS_PASS_2_
#	define VA_ARGS_PASS_1_ (
#	define VA_ARGS_PASS_2_ )
#else
#	define VA_ARGS_PASS(...) (__VA_ARGS__)
#endif // COMPILER_MSVC

#define VA_ARGS_COUNT(...) VA_ARGS_COUNT_ VA_ARGS_PASS(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define VA_ARGS_COUNT_(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11, _a12, _a13, _a14, _a15, _a16, _last, ...) _last

///
#define MACRO_DISPATCHER(_func, ...) MACRO_DISPATCHER_1_(_func, VA_ARGS_COUNT(__VA_ARGS__) )
#define MACRO_DISPATCHER_1_(_func, _argCount) MACRO_DISPATCHER_2_(_func, _argCount)
#define MACRO_DISPATCHER_2_(_func, _argCount) CONCATENATE(_func, _argCount)

///
#define MAKEFOURCC(_a, _b, _c, _d) ( ( (uint32_t)(_a) | ( (uint32_t)(_b) << 8) | ( (uint32_t)(_c) << 16) | ( (uint32_t)(_d) << 24) ) )

///
#define STRINGIZE(_x) STRINGIZE_(_x)
#define STRINGIZE_(_x) #_x

///
#define CONCATENATE(_x, _y) CONCATENATE_(_x, _y)
#define CONCATENATE_(_x, _y) _x ## _y

///
#define FILE_LINE_LITERAL "" __FILE__ "(" STRINGIZE(__LINE__) "): "

///
#define ALIGN_MASK(_value, _mask) ( ( (_value)+(_mask) ) & ( (~0)&(~(_mask) ) ) )
#define ALIGN_16(_value) ALIGN_MASK(_value, 0xf)
#define ALIGN_256(_value) ALIGN_MASK(_value, 0xff)
#define ALIGN_4096(_value) ALIGN_MASK(_value, 0xfff)

#define ALIGNOF(_type) __alignof(_type)

#if defined(__has_feature)
#	define CLANG_HAS_FEATURE(_x) __has_feature(_x)
#else
#	define CLANG_HAS_FEATURE(_x) 0
#endif // defined(__has_feature)

#if defined(__has_extension)
#	define CLANG_HAS_EXTENSION(_x) __has_extension(_x)
#else
#	define CLANG_HAS_EXTENSION(_x) 0
#endif // defined(__has_extension)

#if COMPILER_GCC || COMPILER_CLANG
#	define ALIGN_DECL(_align, _decl) _decl __attribute__( (aligned(_align) ) )
#	define ALLOW_UNUSED __attribute__( (unused) )
#	define FORCE_INLINE inline __attribute__( (__always_inline__) )
#	define FUNCTION __PRETTY_FUNCTION__
#	define LIKELY(_x)   __builtin_expect(!!(_x), 1)
#	define UNLIKELY(_x) __builtin_expect(!!(_x), 0)
#	define NO_INLINE   __attribute__( (noinline) )
#	define NO_RETURN   __attribute__( (noreturn) )
#	if COMPILER_GCC >= 70000
#		define FALLTHROUGH __attribute__( (fallthrough) )
#	else
#		define FALLTHROUGH BX_NOOP()
#	endif // COMPILER_GCC >= 70000
#	define NO_VTABLE
#	define PRINTF_ARGS(_format, _args) __attribute__( (format(__printf__, _format, _args) ) )
#	if CLANG_HAS_FEATURE(cxx_thread_local)
#		define THREAD_LOCAL __thread
#	endif // COMPILER_CLANG
#	if (!PLATFORM_OSX && (COMPILER_GCC >= 40200)) || (COMPILER_GCC >= 40500)
#		define THREAD_LOCAL __thread
#	endif // COMPILER_GCC
#	define ATTRIBUTE(_x) __attribute__( (_x) )
#	if CRT_MSVC
#		define __stdcall
#	endif // CRT_MSVC
#elif COMPILER_MSVC
#	define ALIGN_DECL(_align, _decl) __declspec(align(_align) ) _decl
#	define ALLOW_UNUSED
#	define FORCE_INLINE __forceinline
#	define FUNCTION __FUNCTION__
#	define LIKELY(_x)   (_x)
#	define UNLIKELY(_x) (_x)
#	define NO_INLINE __declspec(noinline)
#	define NO_RETURN
#	define FALLTHROUGH BX_NOOP()
#	define NO_VTABLE __declspec(novtable)
#	define PRINTF_ARGS(_format, _args)
#	define THREAD_LOCAL __declspec(thread)
#	define ATTRIBUTE(_x)
#else
#	error "Unknown COMPILER_?"
#endif

#define STATIC_ASSERT(_condition, ...) static_assert(_condition, "" __VA_ARGS__)

///
#define ALIGN_DECL_16(_decl) ALIGN_DECL(16, _decl)
#define ALIGN_DECL_256(_decl) ALIGN_DECL(256, _decl)
#define ALIGN_DECL_CACHE_LINE(_decl) ALIGN_DECL(CACHE_LINE_SIZE, _decl)

///
#define MACRO_BLOCK_BEGIN for(;;) {
#define MACRO_BLOCK_END break; }
#define NOOP(...) MACRO_BLOCK_BEGIN MACRO_BLOCK_END

///
#define UNUSED_1(_a1)                                                 \
	MACRO_BLOCK_BEGIN                                                 \
		PRAGMA_DIAGNOSTIC_PUSH();                                     \
		/*PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wuseless-cast");*/	  \
		(void)(true ? (void)0 : ( (void)(_a1) ) );                    \
		PRAGMA_DIAGNOSTIC_POP();                                      \
	MACRO_BLOCK_END

#define UNUSED_2(_a1, _a2) UNUSED_1(_a1); UNUSED_1(_a2)
#define UNUSED_3(_a1, _a2, _a3) UNUSED_2(_a1, _a2); UNUSED_1(_a3)
#define UNUSED_4(_a1, _a2, _a3, _a4) UNUSED_3(_a1, _a2, _a3); UNUSED_1(_a4)
#define UNUSED_5(_a1, _a2, _a3, _a4, _a5) UNUSED_4(_a1, _a2, _a3, _a4); UNUSED_1(_a5)
#define UNUSED_6(_a1, _a2, _a3, _a4, _a5, _a6) UNUSED_5(_a1, _a2, _a3, _a4, _a5); UNUSED_1(_a6)
#define UNUSED_7(_a1, _a2, _a3, _a4, _a5, _a6, _a7) UNUSED_6(_a1, _a2, _a3, _a4, _a5, _a6); UNUSED_1(_a7)
#define UNUSED_8(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8) UNUSED_7(_a1, _a2, _a3, _a4, _a5, _a6, _a7); UNUSED_1(_a8)
#define UNUSED_9(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9) UNUSED_8(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8); UNUSED_1(_a9)
#define UNUSED_10(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10) UNUSED_9(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9); UNUSED_1(_a10)
#define UNUSED_11(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11) UNUSED_10(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10); UNUSED_1(_a11)
#define UNUSED_12(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11, _a12) UNUSED_11(_a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9, _a10, _a11); UNUSED_1(_a12)

#if COMPILER_MSVC
// Workaround MSVS bug...
#	define UNUSED(...) MACRO_DISPATCHER(UNUSED_, __VA_ARGS__) VA_ARGS_PASS(__VA_ARGS__)
#else
#	define UNUSED(...) MACRO_DISPATCHER(UNUSED_, __VA_ARGS__)(__VA_ARGS__)
#endif // COMPILER_MSVC

///
#if COMPILER_CLANG
#	define PRAGMA_DIAGNOSTIC_PUSH_CLANG_()     _Pragma("clang diagnostic push")
#	define PRAGMA_DIAGNOSTIC_POP_CLANG_()      _Pragma("clang diagnostic pop")
#	define PRAGMA_DIAGNOSTIC_IGNORED_CLANG(_x) _Pragma(STRINGIZE(clang diagnostic ignored _x) )
#else
#	define PRAGMA_DIAGNOSTIC_PUSH_CLANG_()
#	define PRAGMA_DIAGNOSTIC_POP_CLANG_()
#	define PRAGMA_DIAGNOSTIC_IGNORED_CLANG(_x)
#endif // COMPILER_CLANG

#if COMPILER_GCC && COMPILER_GCC >= 40600
#	define PRAGMA_DIAGNOSTIC_PUSH_GCC_()       _Pragma("GCC diagnostic push")
#	define PRAGMA_DIAGNOSTIC_POP_GCC_()        _Pragma("GCC diagnostic pop")
#	define PRAGMA_DIAGNOSTIC_IGNORED_GCC(_x)   _Pragma(STRINGIZE(GCC diagnostic ignored _x) )
#else
#	define PRAGMA_DIAGNOSTIC_PUSH_GCC_()
#	define PRAGMA_DIAGNOSTIC_POP_GCC_()
#	define PRAGMA_DIAGNOSTIC_IGNORED_GCC(_x)
#endif // COMPILER_GCC

#if COMPILER_MSVC
#	define PRAGMA_DIAGNOSTIC_PUSH_MSVC_()     __pragma(warning(push) )
#	define PRAGMA_DIAGNOSTIC_POP_MSVC_()      __pragma(warning(pop) )
#	define PRAGMA_DIAGNOSTIC_IGNORED_MSVC(_x) __pragma(warning(disable:_x) )
#else
#	define PRAGMA_DIAGNOSTIC_PUSH_MSVC_()
#	define PRAGMA_DIAGNOSTIC_POP_MSVC_()
#	define PRAGMA_DIAGNOSTIC_IGNORED_MSVC(_x)
#endif // COMPILER_CLANG

#if COMPILER_CLANG
#	define PRAGMA_DIAGNOSTIC_PUSH              PRAGMA_DIAGNOSTIC_PUSH_CLANG_
#	define PRAGMA_DIAGNOSTIC_POP               PRAGMA_DIAGNOSTIC_POP_CLANG_
#	define PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC PRAGMA_DIAGNOSTIC_IGNORED_CLANG
#elif COMPILER_GCC
#	define PRAGMA_DIAGNOSTIC_PUSH              PRAGMA_DIAGNOSTIC_PUSH_GCC_
#	define PRAGMA_DIAGNOSTIC_POP               PRAGMA_DIAGNOSTIC_POP_GCC_
#	define PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC PRAGMA_DIAGNOSTIC_IGNORED_GCC
#elif COMPILER_MSVC
#	define PRAGMA_DIAGNOSTIC_PUSH              PRAGMA_DIAGNOSTIC_PUSH_MSVC_
#	define PRAGMA_DIAGNOSTIC_POP               PRAGMA_DIAGNOSTIC_POP_MSVC_
#	define PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC(_x)
#endif // COMPILER_

///
#if COMPILER_MSVC
#	define TYPE_IS_POD(t) (!__is_class(t) || __is_pod(t))
#else
#	define TYPE_IS_POD(t) std::is_pod<t>::value
#endif
///
#define CLASS_NO_DEFAULT_CTOR(_class) \
			private: _class()

#define CLASS_NO_COPY(_class) \
			private: _class(const _class& _rhs)

#define CLASS_NO_ASSIGNMENT(_class) \
			private: _class& operator=(const _class& _rhs)

#define CLASS_ALLOCATOR(_class) \
			public: void* operator new(size_t _size); \
			public: void  operator delete(void* _ptr); \
			public: void* operator new[](size_t _size); \
			public: void  operator delete[](void* _ptr)

#define CLASS_1(_class, _a1) CONCATENATE(CLASS_, _a1)(_class)
#define CLASS_2(_class, _a1, _a2) CLASS_1(_class, _a1); CLASS_1(_class, _a2)
#define CLASS_3(_class, _a1, _a2, _a3) CLASS_2(_class, _a1, _a2); CLASS_1(_class, _a3)
#define CLASS_4(_class, _a1, _a2, _a3, _a4) CLASS_3(_class, _a1, _a2, _a3); CLASS_1(_class, _a4)

#if COMPILER_MSVC
#	define CLASS(_class, ...) MACRO_DISPATCHER(CLASS_, __VA_ARGS__) VA_ARGS_PASS(_class, __VA_ARGS__)
#else
#	define CLASS(_class, ...) MACRO_DISPATCHER(CLASS_, __VA_ARGS__)(_class, __VA_ARGS__)
#endif // COMPILER_MSVC

#ifndef CHECK
#	define CHECK(_condition, ...) NOOP()
#endif // CHECK

#ifndef TRACE
#	define TRACE(...) NOOP()
#endif // TRACE

#ifndef WARN
#	define WARN(_condition, ...) NOOP()
#endif // CHECK

// static_assert sometimes causes unused-local-typedef...
PRAGMA_DIAGNOSTIC_IGNORED_CLANG("-Wunused-local-typedef")
