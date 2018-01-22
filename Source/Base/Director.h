#pragma once

#include "Base/Ref.h"
#include "Base/Ptr.h"

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
	inline Device* getDevice() const { return _ptrDevice.get(); }
protected:
	ref_ptr<Device> _ptrDevice;
};

NS_END