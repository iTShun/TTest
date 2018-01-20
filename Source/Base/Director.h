#pragma once

#include "Base/Ref.h"
#include "Base/Console.h"

NS_BEGIN

class Device;

class Director : public Ref
{
public:
	Director();
	virtual ~Director();

	/**
	* Returns a shared instance of the director.
	*/
	static Director* getInstance();

	void setDevice(Device* device);
public:
	inline Device* getDevice() const { return _pDevice; }
protected:
	Device* _pDevice;
};

NS_END