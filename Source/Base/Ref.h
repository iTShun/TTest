#pragma once

#include "Platform/Platform.h"

NS_BEGIN

class Ref;

/**
* Interface that defines how to clone an Ref.
*/
class Clonable
{
public:
	/** Returns a copy of the Ref. */
	virtual Clonable* clone() const = 0;

	virtual ~Clonable() {};

	/** Returns a copy of the Ref.
	* @deprecated Use clone() instead.
	*/
	DEPRECATED_ATTRIBUTE Ref* copy() const
	{
		// use "clone" instead
		assert(false);
		return nullptr;
	}
};

/**
* Ref is used for reference count management. If a class inherits from Ref,
* then it is easy to be shared in different places.
*/
class Ref
{
public:
	virtual ~Ref();

	/**
	* Retains the ownership.
	*
	* This increases the Ref's reference count.
	*
	* @see release, autorelease
	*/
	void retain();

	/**
	* Releases the ownership immediately.
	*
	* This decrements the Ref's reference count.
	*
	* If the reference count reaches 0 after the decrement, this Ref is
	* destructed.
	*
	* @see retain, autorelease
	*/
	void release();

	/**
	* Releases the ownership sometime soon automatically.
	*
	* This decrements the Ref's reference count at the end of current
	* autorelease pool block.
	*
	* If the reference count reaches 0 after the decrement, this Ref is
	* destructed.
	*
	* @returns The Ref itself.
	*
	* @see AutoreleasePool, retain, release
	*/
	Ref* autorelease();

	/**
	* Returns the Ref's current reference count.
	*
	* @returns The Ref's reference count.
	*/
	unsigned int getReferenceCount() const;

protected:
	Ref();
	Ref(unsigned int pool, const char* file, unsigned int ln, const char* func, const char* describe = 0);

protected:
	//! The reference counter. Mutable to do reference counting on const objects.
	mutable unsigned int _nReferenceCount;

};

NS_END