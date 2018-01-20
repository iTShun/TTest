#pragma once

#include "Base/Console.h"
#include "Graphics/Graphics.h"

NS_BEGIN

enum DeviceEmbeddedType
{
	kDETGLFW = 1,
};

class DeviceProtocol : public virtual Ref
{
public:
	//! Runs the device.
	virtual bool run() = 0;

	//! Notifies the device that it should close itself.
	virtual void closeDevice() = 0;

	//! Check if a Embedded type is supported by the engine.
	static bool isDeviceEmbeddedSupported(DeviceEmbeddedType device)
	{
		switch (device)
		{
		case DeviceEmbeddedType::kDETGLFW:
#if GLFW_SUPPORT
			return true;
#else
			return false;
#endif
		}

		return false;
	}

	//! Check if a Graphics type is supported by the engine.
	static bool isGraphicsSupported(GraphicsType graphics)
	{
		switch (graphics)
		{
			case GraphicsType::kGTD3D9:
#if GRAPHICS_D3D9
				return true;
#else
				return false;
#endif
			case GraphicsType::kGTD3D11:
#if GRAPHICS_D3D11
				return true;
#else
				return false;
#endif
			case GraphicsType::kGTOGL:
#if GRAPHICS_OGL
				return true;
#else
				return false;
#endif
		}

		return false;
	}
};

NS_END