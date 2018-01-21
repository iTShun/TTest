#pragma once

#include "Graphics/GraphicsConfig.h"
#include "Graphics/GraphicsDefine.h"

// check user set graphics
#if !GRAPHICS_OGL && !GRAPHICS_D3D9 && !GRAPHICS_D3D11
#	error  "Cannot recognize the target graphics; are you targeting an unsupported graphics?"
#endif

#define GLFW_SUPPORT			0

#if GRAPHICS_OGL

#   define GRAPHICS_OGL_MAJOR   2
#   define GRAPHICS_OGL_MINOR   3

#	include "Graphics/GraphicsOGL.h"

#	if PLATFORM_WIN32
#		include "Framework/Win32/glew.h"
#		pragma comment(lib,"opengl32.lib")
#		pragma comment(lib,"glew32.lib")

#	elif PLATFORM_MAC
#       include <OpenGL/gl.h>
#       include <OpenGL/glu.h>
#       include <OpenGL/glext.h>
#	endif

#	undef GLFW_SUPPORT
#	define GLFW_SUPPORT			1

#	pragma message("GRAPHICS_OGL")
#endif

#if GRAPHICS_D3D9

#	if PLATFORM_WIN32
#		include <d3d9.h>
#endif

#	pragma message("GRAPHICS_D3D9")
#endif

#if GRAPHICS_D3D11

#	if PLATFORM_WIN32
#		include <d3d11.h>
#endif

#	pragma message("GRAPHICS_D3D11")
#endif
