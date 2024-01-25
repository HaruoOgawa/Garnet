#pragma once

#ifdef USE_OPENGL

#include <memory>
#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "glDef.h"
#include "../../Interface/IGraphicsAPI.h"

namespace api
{
	class COpenGLAPI : public IGraphicsAPI
	{
		const std::string m_VertexShaderExtension;
		const std::string m_FragmentShaderExtension;
		const std::string m_GeometryShaderExtension;
		const std::string m_HullShaderExtension;
		const std::string m_DomainShaderExtension;
		const std::string m_ComputeShaderExtension;
		int m_Width;
		int m_Height;

		std::map<std::string, std::shared_ptr<graphics::IRenderPass>> m_OffScreenRenderPassMap;
	private:
	public:
		COpenGLAPI(int Width, int Height);
		virtual ~COpenGLAPI();

		virtual bool Initialize() override;
		void Release();

		virtual bool CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width = -1, int Height = -1) override;
		virtual std::shared_ptr<renderer::IRenderer> CreateRenderer(const std::string& PassName) override;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, int RefCount, graphics::ECullMode CullMode) override;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false) override;
#ifdef USE_GPGPU
		virtual std::shared_ptr<api::IGPGPUHandler> CreateGPGPUHandler(const std::shared_ptr<graphics::CMaterial>& ComputeMaterial) override;
#endif // USE_GPGPU

		virtual bool Resize(int Width, int Height) override;

		virtual bool PrepareRender() override;
		virtual bool BeginRender(const std::string& PassName = "") override;
		virtual bool EndRender() override;
		virtual bool SubmitRender() override;

		virtual const std::string& GetVertexShaderExtension() const override;
		virtual const std::string& GetFragmentShaderExtension() const override;
		virtual const std::string& GetGeometryShaderExtension() const override;
		virtual const std::string& GetHullShaderExtension() const override;
		virtual const std::string& GetDomainShaderExtension() const override;
		virtual const std::string& GetComputeShaderExtension() const override;

		virtual int GetWidth() const override;
		virtual int GetHeight() const override;

		virtual const std::map<std::string, std::shared_ptr<graphics::IRenderPass>>& GetOffScreenRenderPassMap() const override;
	};
}

#endif // USE_OPENGL