#pragma once

#include "Base/Console.h"

NS_BEGIN

/**
* Ref is used for reference count management. If a class inherits from Ref,
* then it is easy to be shared in different places.
*/
class Ref
{
public:
	virtual ~Ref();

	virtual Ref* copy() const { return nullptr; }

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
	Ref(unsigned int sz, unsigned int pool, const char* file, unsigned int ln, const char* func, const char* describe = 0);

protected:
	//! The reference counter. Mutable to do reference counting on const objects.
	mutable unsigned int m_nReferenceCount;

};

NS_END