#pragma once

#include "Graphics/GraphicsConfig.h"
#include "Graphics/GraphicsDefine.h"

#include "Graphics/GraphicsProtocol.h"

#if GRAPHICS_OGL

NS_BEGIN

class GraphicsOGL : public Graphics
{
public:
	virtual ~GraphicsOGL();

	static Graphics* create();

	bool init();
public:
	//! Applications must call this method before performing any rendering.
	virtual bool beginScene() override;

	//! Presents the rendered image to the screen.
	virtual bool endScene() override;

	//! Sets a new viewport.
	virtual void setViewPort(int x, int y, int width, int height) override;

protected:
	GraphicsOGL();

protected:
};

NS_END

#endif