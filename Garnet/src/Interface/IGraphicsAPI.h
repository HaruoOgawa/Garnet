#pragma once
#include <memory>
#include "IRenderer.h"
#include "../GraphicsAPI/ERenderPassType.h"

#ifndef __EMSCRIPTEN__

#ifndef __DAWN__ 
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <glfw3.h>
#include <glfw3native.h>

#endif // !__EMSCRIPTEN__

namespace renderer { class IRenderer; }

namespace api
{
	class IGraphicsAPI
	{
	public:
#ifdef __EMSCRIPTEN__
		virtual bool Initialize() = 0;
#else
		virtual bool InitializeWithGLFW(GLFWwindow* pWindow) = 0;
#endif // __EMSCRIPTEN__
		
		virtual std::shared_ptr<renderer::IRenderer> CreateRenderer() = 0;

		virtual bool BeginRender(ERenderPassType RenderPassType) = 0;
		virtual bool EndRender() = 0;
		virtual bool IsWaitting() = 0;
	};
}