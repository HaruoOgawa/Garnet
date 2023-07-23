#pragma once

#ifdef USE_OPENGL

#include <memory>
#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "../../Interface/IGraphicsAPI.h"

namespace api
{
	class COpenGLAPI : public IGraphicsAPI
	{
		const std::string m_ShaderExtension;
		int m_Width;
		int m_Height;

		std::map<std::string, std::shared_ptr<graphics::IRenderPass>> m_OffScreenRenderPassMap;
	public:
		COpenGLAPI(int Width, int Height);
		virtual ~COpenGLAPI();

		virtual bool InitializeWithGLFW(GLFWwindow* pWindow) override;
		void Release();

		virtual bool CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width = -1, int Height = -1) override;
		virtual std::shared_ptr<renderer::IRenderer> CreateRenderer(const std::string& PassName) override;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial() override;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false) override;

		virtual bool Resize(int Width, int Height) override;

		virtual bool PrepareRender() override;
		virtual bool BeginRender(const std::string& PassName = "") override;
		virtual bool EndRender() override;
		virtual bool SubmitRender() override;

		virtual const std::string& GetShaderExtension() const override;
		virtual int GetWidth() const override;
		virtual int GetHeight() const override;

		virtual const std::map<std::string, std::shared_ptr<graphics::IRenderPass>>& GetOffScreenRenderPassMap() const override;
	};
}

#endif // USE_OPENGL