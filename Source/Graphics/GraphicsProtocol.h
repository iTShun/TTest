#pragma once

#include "Base/Console.h"

NS_BEGIN

class Graphics : public virtual Ref
{
public:
	//! Applications must call this method before performing any rendering.
	virtual bool beginScene() = 0;

	//! Presents the rendered image to the screen.
	virtual bool endScene() = 0;

	//! Sets a new viewport.
	virtual void setViewPort(int x, int y, int width, int height) = 0;
};

NS_END