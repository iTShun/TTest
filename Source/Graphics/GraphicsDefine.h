#pragma once

#include "Platform/PlatformConfig.h"

#define GRAPHICS_OGL                (0 \
									| PLATFORM_WIN32 \
                                    )

#define GRAPHICS_D3D9               (0 \
									| PLATFORM_WIN32)

#define GRAPHICS_D3D11              (0 \
									| PLATFORM_WIN32)