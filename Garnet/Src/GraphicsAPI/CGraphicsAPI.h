#pragma once

#include "../../Interface/IGraphicsAPI.h"
#include "../Graphics/STextureSamplerParam.h"

namespace api
{
	class CGraphicsAPI : public IGraphicsAPI
	{
		const int m_MaxBoneCount;

		std::shared_ptr<animation::CBoneNameProvider> m_BoneNameProvider;
		std::shared_ptr<animation::CBlendShapeNameProvider> m_BlendShapeNameProvider;
	protected:
		std::map<std::string, std::shared_ptr<graphics::IRenderPass>> m_OffScreenRenderPassMap;
		std::string m_CurrentRenderPassName;
	public:
		CGraphicsAPI();
		virtual ~CGraphicsAPI();

		virtual bool Initialize(window::IWindowAPI* pWindowAPI) = 0;

		virtual void Release() = 0;

		virtual bool CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width = -1, int Height = -1, 
			const graphics::SRenderPassState& PassState = {}) = 0;
		virtual std::shared_ptr<graphics::CVertexBuffer> CreateVertexBuffer() = 0;
		virtual std::shared_ptr<graphics::CIndexBuffer> CreateIndexBuffer() = 0;
		virtual std::shared_ptr<graphics::IRenderer> CreateRenderer() = 0;
		virtual std::shared_ptr<graphics::CMaterial> CreateMaterial(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, graphics::ECullMode CullMode) = 0;
		virtual std::shared_ptr<graphics::CTexture> CreateTexture(bool UseMipMap = false, graphics::STextureSamplerParam SamplerParam = {}) = 0;
#ifdef USE_GPGPU
		virtual std::shared_ptr<api::IGPGPUHandler> CreateGPGPUHandler(const std::shared_ptr<graphics::CMaterial>& ComputeMaterial) = 0;
#endif // USE_GPGPU
		virtual bool Resize(int Width, int Height) = 0;

		virtual bool PrepareRender() = 0;
		virtual bool BeginRender(const std::string& PassName = "") = 0;
		virtual bool EndRender() = 0;
		virtual bool SubmitRender() = 0;

		virtual int GetMaxBoneCount() override;

		virtual std::string GetVertexShaderExtension() const = 0;
		virtual std::string GetFragmentShaderExtension() const = 0;
		virtual std::string GetGeometryShaderExtension() const = 0;
		virtual std::string GetHullShaderExtension() const = 0;
		virtual std::string GetDomainShaderExtension() const = 0;
		virtual std::string GetComputeShaderExtension() const = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		virtual const std::map<std::string, std::shared_ptr<graphics::IRenderPass>>& GetOffScreenRenderPassMap() const override;
		virtual std::shared_ptr<graphics::IRenderPass> FindOffScreenRenderPass(const std::string& PassName) = 0;
		virtual const std::string& GetCurrentRenderPassName() const override;
		virtual bool CopyRenderPass(const std::string& SrcPassName, const std::string& DstPassName, bool Color, bool Depth) override;
		virtual bool CopyColorBuffer(const std::string& SrcPassName, const std::string& DstPassName) override;
		virtual bool CopyDepthBuffer(const std::string& SrcPassName, const std::string& DstPassName) override;

		virtual bool IsEnabledRuntimeShaderEditing() const = 0;

		virtual bool CheckValidShader(std::string& ErrorMsg, const std::vector<unsigned char>& ShaderCode, graphics::EShaderStage ShaderStage) = 0;

		virtual const std::shared_ptr<animation::CBoneNameProvider>& GetBoneNameProvider() const override;
		virtual const std::shared_ptr<animation::CBlendShapeNameProvider>& GetBlendShapeNameProvider() const override;
	};
}