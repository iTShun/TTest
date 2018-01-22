#pragma once

#include "Base/Console.h"

NS_BEGIN

/** Smart pointer for handling referenced counted objects.*/
template<class T>
class ref_ptr
{
public:
	typedef T element_type;

	ref_ptr() : _pPtr(0) {}
	ref_ptr(T* ptr) : _pPtr(ptr) { if (_pPtr) _pPtr->retain(); }
	ref_ptr(const ref_ptr& rp) : _pPtr(rp._pPtr) { if (_pPtr) _pPtr->retain(); }
	~ref_ptr() { if (_pPtr) _pPtr->release();  _pPtr = nullptr; }

	inline ref_ptr& operator = (T* ptr)
	{
		if (_pPtr == ptr) 
			return *this;
		T* tmp_ptr = _pPtr;
		_pPtr = ptr;
		if (_pPtr) 
			_pPtr->retain();
		// release to prevent any deletion of any object which might
		// be referenced by the other object. i.e rp is child of the
		// original _pPtr.
		if (tmp_ptr) 
			tmp_ptr->release();
		return *this;
	}

	operator T*() const { return _pPtr; }

	// comparison operators for ref_ptr.
	bool operator == (const ref_ptr& rp) const { return (_pPtr == rp._pPtr); }
	bool operator == (const T* ptr) const { return (_pPtr == ptr); }
	friend bool operator == (const T* ptr, const ref_ptr& rp) { return (ptr == rp._pPtr); }

	bool operator != (const ref_ptr& rp) const { return (_pPtr != rp._pPtr); }
	bool operator != (const T* ptr) const { return (_pPtr != ptr); }
	friend bool operator != (const T* ptr, const ref_ptr& rp) { return (ptr != rp._pPtr); }

	bool operator < (const ref_ptr& rp) const { return (_pPtr < rp._pPtr); }

	T& operator*() const { return *_pPtr; }
	T* operator->() const { return _pPtr; }
	T* get() const { return _pPtr; }

	bool operator!() const { return _pPtr == nullptr; } // not required
	bool valid() const { return _pPtr != nullptr; }



protected:
	T* _pPtr;
};

NS_END