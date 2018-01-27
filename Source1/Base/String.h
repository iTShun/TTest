#pragma once

#include "Base/Allocator.h"

#if CRT_MSVC && !defined(va_copy)
#	define va_copy(_a, _b) (_a) = (_b)
#endif // CRT_MSVC && !defined(va_copy)

NS_BEGIN

int32_t strLen(const char* _str, int32_t _max);

struct Units
{
	enum Enum
	{
		Kilo,
		Kibi,
	};
};

/// Non-zero-terminated string view.
class StringView
{
public:
	///
	inline StringView() { clear(); }

	///
	inline StringView(const StringView& _rhs) { set(_rhs.m_ptr, _rhs.m_len); }

	///
	inline StringView& operator=(const char* _rhs)
	{
		set(_rhs);
		return *this;
	}

	///
	inline StringView& operator=(const StringView& _rhs)
	{
		set(_rhs.m_ptr, _rhs.m_len);
		return *this;
	}

	///
	inline StringView(const char* _ptr, int32_t _len = INT32_MAX)
	{
		set(_ptr, _len);
	}

	///
	inline StringView(const char* _ptr, const char* _term)
	{
		set(_ptr, _term);
	}

	///
	inline void set(const char* _ptr, int32_t _len = INT32_MAX)
	{
		clear();

		if (NULL != _ptr)
		{
			int32_t len = strLen(_ptr, _len);
			if (0 != len)
			{
				m_len = len;
				m_ptr = _ptr;
			}
		}
	}

	///
	inline void set(const char* _ptr, const char* _term)
	{
		set(_ptr, int32_t(_term - _ptr));
	}

	///
	inline void set(const StringView& _str)
	{
		set(_str.m_ptr, _str.m_len);
	}

	///
	inline void clear()
	{
		m_ptr = "";
		m_len = 0;
	}

	///
	inline const char* getPtr() const
	{
		return m_ptr;
	}

	///
	inline const char* getTerm() const
	{
		return m_ptr + m_len;
	}

	///
	inline bool isEmpty() const
	{
		return 0 == m_len;
	}

	///
	inline int32_t getLength() const
	{
		return m_len;
	}

protected:
	const char* m_ptr;
	int32_t     m_len;
};

/// ASCII string
template<NS_::AllocatorI** AllocatorT>
class StringT : public StringView
{
public:
	///
	inline StringT()
	: StringView()
	{
	}

	///
	inline StringT(const StringT<AllocatorT>& _rhs)
	: StringView()
	{
		set(_rhs);
	}

	///
	inline StringT<AllocatorT>& operator=(const StringT<AllocatorT>& _rhs)
	{
		set(_rhs);
		return *this;
	}

	///
	inline StringT(const StringView& _rhs)
	{
		set(_rhs);
	}

	///
	inline ~StringT()
	{
		clear();
	}

	///
	inline void set(const StringView& _str)
	{
		clear();
		append(_str);
	}

	///
	inline void append(const StringView& _str)
	{
		if (0 != _str.getLength())
		{
			int32_t old = m_len;
			int32_t len = m_len + strLen(_str);
			char* ptr = (char*)REALLOC(*AllocatorT, 0 != m_len ? const_cast<char*>(m_ptr) : NULL, len + 1);
			m_len = len;
			strCopy(ptr + old, len - old + 1, _str);

			*const_cast<char**>(&m_ptr) = ptr;
		}
	}

	///
	inline void clear()
	{
		if (0 != m_len)
		{
			FREE(*AllocatorT, const_cast<char*>(m_ptr));

			StringView::clear();
		}
	}
};

/// Retruns true if character is part of space set.
bool isSpace(char _ch);

/// Returns true if string view contains only space characters.
bool isSpace(const StringView& _str);

/// Retruns true if character is uppercase.
bool isUpper(char _ch);

/// Returns true if string view contains only uppercase characters.
bool isUpper(const StringView& _str);

/// Retruns true if character is lowercase.
bool isLower(char _ch);

/// Returns true if string view contains only lowercase characters.
bool isLower(const StringView& _str);

/// Returns true if character is part of alphabet set.
bool isAlpha(char _ch);

/// Retruns true if string view contains only alphabet characters.
bool isAlpha(const StringView& _str);

/// Returns true if character is part of numeric set.
bool isNumeric(char _ch);

/// Retruns true if string view contains only numeric characters.
bool isNumeric(const StringView& _str);

/// Returns true if character is part of alpha numeric set.
bool isAlphaNum(char _ch);

/// Returns true if string view contains only alpha-numeric characters.
bool isAlphaNum(const StringView& _str);

/// Returns true if character is part of hexadecimal set.
bool isHexNum(char _ch);

/// Returns true if string view contains only hexadecimal characters.
bool isHexNum(const StringView& _str);

/// Returns true if character is printable.
bool isPrint(char _ch);

/// Returns true if string vieww contains only printable characters.
bool isPrint(const StringView& _str);

///
char toLower(char _ch);

///
void toLowerUnsafe(char* _inOutStr, int32_t _len);

///
void toLower(char* _inOutStr, int32_t _max = INT32_MAX);

///
char toUpper(char _ch);

///
void toUpperUnsafe(char* _inOutStr, int32_t _len);

///
void toUpper(char* _inOutStr, int32_t _max = INT32_MAX);

/// String compare.
int32_t strCmp(const StringView& _lhs, const StringView& _rhs, int32_t _max = INT32_MAX);

/// Case insensitive string compare.
int32_t strCmpI(const StringView& _lhs, const StringView& _rhs, int32_t _max = INT32_MAX);

// Compare as strings holding indices/version numbers.
int32_t strCmpV(const StringView& _lhs, const StringView& _rhs, int32_t _max = INT32_MAX);

/// Get string length.
int32_t strLen(const char* _str, int32_t _max = INT32_MAX);

/// Get string length.
int32_t strLen(const StringView& _str, int32_t _max = INT32_MAX);

/// Copy _num characters from string _src to _dst buffer of maximum _dstSize capacity
/// including zero terminator. Copy will be terminated with '\0'.
int32_t strCopy(char* _dst, int32_t _dstSize, const StringView& _str, int32_t _num = INT32_MAX);

/// Concatinate string.
int32_t strCat(char* _dst, int32_t _dstSize, const StringView& _str, int32_t _num = INT32_MAX);

/// Find character in string. Limit search to _max characters.
const char* strFind(const StringView& _str, char _ch);

/// Find character in string in reverse. Limit search to _max characters.
const char* strRFind(const StringView& _str, char _ch);

/// Find substring in string. Limit search to _max characters.
const char* strFind(const StringView& _str, const StringView& _find, int32_t _num = INT32_MAX);

/// Find substring in string. Case insensitive. Limit search to _max characters.
const char* strFindI(const StringView& _str, const StringView& _find, int32_t _num = INT32_MAX);

///
StringView strLTrim(const StringView& _str, const StringView& _chars);

///
StringView strRTrim(const StringView& _str, const StringView& _chars);

///
StringView strTrim(const StringView& _str, const StringView& _chars);

/// Find new line. Returns pointer after new line terminator.
const char* strnl(const char* _str);

/// Find end of line. Retuns pointer to new line terminator.
const char* streol(const char* _str);

/// Skip whitespace.
const char* strws(const char* _str);

/// Skip non-whitespace.
const char* strnws(const char* _str);

/// Returns pointer to first character after word.
const char* strSkipWord(const char* _str, int32_t _max = INT32_MAX);

/// Returns StringView of word or empty.
StringView strWord(const StringView& _str);

/// Find matching block.
const char* strmb(const char* _str, char _open, char _close);

// Normalize string to sane line endings.
void eolLF(char* _out, int32_t _size, const char* _str);

// Finds identifier.
const char* findIdentifierMatch(const char* _str, const char* _word);

/// Finds any identifier from NULL terminated array of identifiers.
const char* findIdentifierMatch(const char* _str, const char* _words[]);

/// Cross platform implementation of vsnprintf that returns number of
/// characters which would have been written to the final string if
/// enough space had been available.
int32_t vsnprintf(char* _out, int32_t _max, const char* _format, va_list _argList);

/// Cross platform implementation of vsnwprintf that returns number of
/// characters which would have been written to the final string if
/// enough space had been available.
int32_t vsnwprintf(wchar_t* _out, int32_t _max, const wchar_t* _format, va_list _argList);

///
int32_t snprintf(char* _out, int32_t _max, const char* _format, ...);

///
int32_t swnprintf(wchar_t* _out, int32_t _max, const wchar_t* _format, ...);


///
template <typename Ty>
inline void stringPrintfVargs(Ty& _out, const char* _format, va_list _argList)
{
	char temp[2048];

	char* out = temp;
	int32_t len = NS_::vsnprintf(out, sizeof(temp), _format, _argList);
	if ((int32_t)sizeof(temp) < len)
	{
		out = (char*)alloca(len + 1);
		len = NS_::vsnprintf(out, len, _format, _argList);
	}
	out[len] = '\0';
	_out.append(out);
}

///
template <typename Ty>
inline void stringPrintf(Ty& _out, const char* _format, ...)
{
	va_list argList;
	va_start(argList, _format);
	stringPrintfVargs(_out, _format, argList);
	va_end(argList);
}

/// Replace all instances of substring.
template <typename Ty>
inline Ty replaceAll(const Ty& _str, const char* _from, const char* _to)
{
	Ty str = _str;
	typename Ty::size_type startPos = 0;
	const typename Ty::size_type fromLen = strLen(_from);
	const typename Ty::size_type toLen = strLen(_to);
	while ((startPos = str.find(_from, startPos)) != Ty::npos)
	{
		str.replace(startPos, fromLen, _to);
		startPos += toLen;
	}

	return str;
}

/// Convert size in bytes to human readable string kibi units.
int32_t prettify(char* _out, int32_t _count, uint64_t _value, Units::Enum _units = Units::Kibi);

///
int32_t toString(char* _out, int32_t _max, bool _value);

///
int32_t toString(char* _out, int32_t _max, double _value);

///
int32_t toString(char* _out, int32_t _max, int32_t _value, uint32_t _base = 10);

///
int32_t toString(char* _out, int32_t _max, int64_t _value, uint32_t _base = 10);

///
int32_t toString(char* _out, int32_t _max, uint32_t _value, uint32_t _base = 10);

///
int32_t toString(char* _out, int32_t _max, uint64_t _value, uint32_t _base = 10);

///
bool fromString(bool* _out, const StringView& _str);

///
bool fromString(float* _out, const StringView& _str);

///
bool fromString(double* _out, const StringView& _str);

///
bool fromString(int32_t* _out, const StringView& _str);

///
bool fromString(uint32_t* _out, const StringView& _str);


NS_END