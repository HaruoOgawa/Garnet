#pragma once
#include <memory>
#include <string>
#include <map>
#include <glm/glm.hpp>

#include "IRenderer.h"
#include "IRenderPass.h"
#include "../GraphicsAPI/ERenderPassFormat.h"

#ifndef __EMSCRIPTEN__

#ifdef USE_VULKAN 
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#ifndef USE_OPENGL
#include <glfw3.h>
#include <glfw3native.h>
#endif // !USE_OPENGL

#endif // !__EMSCRIPTEN__

namespace graphics
{
	class CMaterial;
	class CTexture;
	enum class ETextureType;
	class CMaterialCreateInfo;
}

namespace api
{
	class IGraphicsAPI
	{
	public:
#if defined(__EMSCRIPTEN__) || defined(USE_OPENGL) 
		virtual bool Initialize() = 0;
#else
		virtual bool InitializeWithGLFW(GLFWwindow* pWindow) = 0;
#endif // __EMSCRIPTEN__
		
		virtual bool CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width = -1, int Height = -1) = 0;
		virtual std::shared_ptr<renderer::IRenderer> CreateRenderer(const std::string& PassName) = 0;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo) = 0;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false) = 0;

		virtual bool Resize(int Width, int Height) = 0;

		virtual bool PrepareRender() = 0;
		virtual bool BeginRender(const std::string& PassName = "") = 0;
		virtual bool EndRender() = 0;
		virtual bool SubmitRender() = 0;

		virtual const std::string& GetVertexShaderExtension() const = 0;
		virtual const std::string& GetFragmentShaderExtension() const = 0;
		virtual const std::string& GetGeometryShaderExtension() const = 0;
		virtual const std::string& GetHullShaderExtension() const = 0;
		virtual const std::string& GetDomainShaderExtension() const = 0;
		virtual const std::string& GetComputeShaderExtension() const = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		virtual const std::map<std::string, std::shared_ptr<graphics::IRenderPass>>& GetOffScreenRenderPassMap() const = 0;
	};
}