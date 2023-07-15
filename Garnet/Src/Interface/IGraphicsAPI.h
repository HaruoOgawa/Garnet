#pragma once
#include <memory>
#include <string>
#include <map>

#include "IRenderer.h"
#include "IRenderPass.h"

#include "../GraphicsAPI/ERenderPassFormat.h"

#ifndef __EMSCRIPTEN__

#ifndef __DAWN__ 
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <glfw3.h>
#include <glfw3native.h>
#include <glm/glm.hpp>

#endif // !__EMSCRIPTEN__

namespace graphics
{
	class CMaterial;
	class CTexture;
	enum class ETextureType;
}

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
		
		virtual bool CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor) = 0;
		virtual std::shared_ptr<renderer::IRenderer> CreateRenderer(const std::string& PassName) = 0;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial() = 0;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false) = 0;

		virtual bool Resize(int Width, int Height) = 0;

		virtual bool PrepareRender() = 0;
		virtual bool BeginRender(const std::string& PassName = "") = 0;
		virtual bool EndRender() = 0;
		virtual bool SubmitRender() = 0;

		virtual const std::string& GetShaderExtension() const = 0;
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		virtual const std::map<std::string, std::shared_ptr<graphics::IRenderPass>>& GetOffScreenRenderPassMap() const = 0;
	};
}