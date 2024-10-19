#include "CGraphicsAPI.h"

namespace api
{
	CGraphicsAPI::CGraphicsAPI():
		m_MaxBoneCount(1024)
	{
	}

	CGraphicsAPI::~CGraphicsAPI()
	{
	}

	bool CGraphicsAPI::Initialize(window::IWindowAPI* pWindowAPI)
	{
		return true;
	}

	void CGraphicsAPI::Release()
	{
	}

	bool CGraphicsAPI::CreateRenderPass(const std::string& PassName, ERenderPassFormat RenderPassFormat, const glm::vec4& InitColor, int Width, int Height, int RenderTargetCount)
	{
		return true;
	}

	std::shared_ptr<graphics::CVertexBuffer> CGraphicsAPI::CreateVertexBuffer()
	{
		return nullptr;
	}

	std::shared_ptr<graphics::CIndexBuffer> CGraphicsAPI::CreateIndexBuffer()
	{
		return nullptr;
	}

	std::shared_ptr<graphics::IRenderer> CGraphicsAPI::CreateRenderer(const std::string& PassName)
	{
		return nullptr;
	}

	std::shared_ptr<graphics::CMaterial> CGraphicsAPI::CreateMaterial(const std::shared_ptr<graphics::CMaterialCreateInfo>& createInfo, int RefCount, graphics::ECullMode CullMode)
	{
		return nullptr;
	}

	std::shared_ptr<graphics::CTexture> CGraphicsAPI::CreateTexture(bool UseMipMap)
	{
		return nullptr;
	}

#ifdef USE_GPGPU
	std::shared_ptr<api::IGPGPUHandler> CGraphicsAPI::CreateGPGPUHandler(const std::shared_ptr<graphics::CMaterial>& ComputeMaterial)
	{
		return nullptr;
	}

#endif // USE_GPGPU
	bool CGraphicsAPI::Resize(int Width, int Height)
	{
		return true;
	}

	bool CGraphicsAPI::PrepareRender()
	{
		return true;
	}

	bool CGraphicsAPI::BeginRender(const std::string& PassName)
	{
		return true;
	}

	bool CGraphicsAPI::EndRender()
	{
		return true;
	}

	bool CGraphicsAPI::SubmitRender()
	{
		return true;
	}

	int CGraphicsAPI::GetMaxBoneCount()
	{
		return m_MaxBoneCount;
	}

	const std::string& CGraphicsAPI::GetVertexShaderExtension() const
	{
		return std::string();
	}

	const std::string& CGraphicsAPI::GetFragmentShaderExtension() const
	{
		return std::string();
	}

	const std::string& CGraphicsAPI::GetGeometryShaderExtension() const
	{
		return std::string();
	}

	const std::string& CGraphicsAPI::GetHullShaderExtension() const
	{
		return std::string();
	}

	const std::string& CGraphicsAPI::GetDomainShaderExtension() const
	{
		return std::string();
	}

	const std::string& CGraphicsAPI::GetComputeShaderExtension() const
	{
		return std::string();
	}

	int CGraphicsAPI::GetWidth() const
	{
		return 0;
	}

	int CGraphicsAPI::GetHeight() const
	{
		return 0;
	}

	const std::map<std::string, std::shared_ptr<graphics::IRenderPass>>& CGraphicsAPI::GetOffScreenRenderPassMap() const
	{
		return {};
	}

	std::shared_ptr<graphics::IRenderPass> CGraphicsAPI::FindOffScreenRenderPass(const std::string& PassName)
	{
		return nullptr;
	}

	const std::string& CGraphicsAPI::GetCurrentRenderPassName() const
	{
		return std::string();
	}

	bool CGraphicsAPI::CopyColorBuffer(const std::string& SrcPassName, const std::string& DstPassName)
	{
		return true;
	}

	bool CGraphicsAPI::CopyDepthBuffer(const std::string& SrcPassName, const std::string& DstPassName)
	{
		return true;
	}

	bool CGraphicsAPI::IsEnabledRuntimeShaderEditing() const
	{
		return true;
	}

	bool CGraphicsAPI::CheckValidShader(std::string& ErrorMsg, const std::vector<unsigned char>& ShaderCode, graphics::EShaderStage ShaderStage)
	{
		return true;
	}
}