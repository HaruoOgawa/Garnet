#pragma once

#ifdef USE_OPENGL

#include <memory>
#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "glDef.h"
#include "../CGraphicsAPI.h"

namespace api
{
	class COpenGLAPI : public CGraphicsAPI
	{
		const std::string m_VertexShaderExtension;
		const std::string m_FragmentShaderExtension;
		const std::string m_GeometryShaderExtension;
		const std::string m_HullShaderExtension;
		const std::string m_DomainShaderExtension;
		const std::string m_ComputeShaderExtension;
		int m_Width;
		int m_Height;
	private:
	public:
		COpenGLAPI(int Width, int Height);
		virtual ~COpenGLAPI();

		virtual bool Initialize(window::IWindowAPI* pWindowAPI) override;
		
		virtual void Release() override;

		virtual bool CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width = -1, int Height = -1, int RenderTargetCount = 1) override;
		virtual std::shared_ptr<graphics::CVertexBuffer> CreateVertexBuffer() override;
		virtual std::shared_ptr<graphics::CIndexBuffer> CreateIndexBuffer() override;
		virtual std::shared_ptr<graphics::IRenderer> CreateRenderer() override;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, graphics::ECullMode CullMode) override;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false) override;
#ifdef USE_GPGPU
		virtual std::shared_ptr<api::IGPGPUHandler> CreateGPGPUHandler(const std::shared_ptr<graphics::CMaterial>& ComputeMaterial) override;
#endif // USE_GPGPU

		virtual bool Resize(int Width, int Height) override;

		virtual bool PrepareRender() override;
		virtual bool BeginRender(const std::string& PassName = std::string()) override;
		virtual bool EndRender() override;
		virtual bool SubmitRender() override;

		virtual std::string GetVertexShaderExtension() const override;
		virtual std::string GetFragmentShaderExtension() const override;
		virtual std::string GetGeometryShaderExtension() const override;
		virtual std::string GetHullShaderExtension() const override;
		virtual std::string GetDomainShaderExtension() const override;
		virtual std::string GetComputeShaderExtension() const override;

		virtual int GetWidth() const override;
		virtual int GetHeight() const override;

		virtual std::shared_ptr<graphics::IRenderPass> FindOffScreenRenderPass(const std::string& PassName) override;
		virtual bool CopyColorBuffer(const std::string& SrcPassName, const std::string& DstPassName) override;
		virtual bool CopyDepthBuffer(const std::string& SrcPassName, const std::string& DstPassName) override;

		virtual bool IsEnabledRuntimeShaderEditing() const override;

		virtual bool CheckValidShader(std::string& ErrorMsg, const std::vector<unsigned char>& ShaderCode, graphics::EShaderStage ShaderStage) override;

		GLuint GetFrameBuffer(const std::string& PassName);
	};
}

#endif // USE_OPENGL