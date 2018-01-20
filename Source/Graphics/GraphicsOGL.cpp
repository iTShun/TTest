#include "GraphicsOGL.h"
#include "Graphics/Graphics.h"

#if GRAPHICS_OGL

NS_BEGIN

GraphicsOGL::GraphicsOGL()
{
}

GraphicsOGL::~GraphicsOGL()
{
}

Graphics* GraphicsOGL::create()
{
	GraphicsOGL* pRet = new (std::nothrow) GraphicsOGL();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
	}
	else
	{
		SAFE_DELETE(pRet);
	}

	return pRet;
}

bool GraphicsOGL::init()
{
	do 
	{
		

		retain();

		return true;
	} while (0);

	return false;
}

bool GraphicsOGL::beginScene()
{
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glClearDepth(1.0f);
	glClearStencil(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	return true;
}

bool GraphicsOGL::endScene()
{
	return true;
}

void GraphicsOGL::setViewPort(int x, int y, int width, int height)
{

}

NS_END

#endif