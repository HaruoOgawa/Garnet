#include "CBlurEffect.h"
#include "../../LoadWorker/CLoadWorker.h"
#include "../Debug/Message/Console.h"
#include "../LoadWorker/CFile.h"
#include "../Interface/IGraphicsAPI.h"
#include "../Graphics/CTextureSet.h"

namespace imageeffect
{
	CBlurEffect::CBlurEffect(api::IGraphicsAPI* pGraphicsAPI):
		m_pGraphicsAPI(pGraphicsAPI),

		m_IsLoaded(false),
		m_KernelSize(0),
		m_BlurVertex(std::make_shared<resource::CFile>("Resources\\Shaders\\blur" + m_pGraphicsAPI->GetVertexShaderExtension())),
		m_BlurFrag(std::make_shared<resource::CFile>("Resources\\Shaders\\blur" + m_pGraphicsAPI->GetFragmentShaderExtension())),
		m_ScreenObjX(std::make_shared<object::C3DObject>("BlurX", "")),
		m_ScreenObjY(std::make_shared<object::C3DObject>("BlurY", "")),

		m_TextureSet(std::make_shared<graphics::CTextureSet>())
	{
	}

	bool CBlurEffect::IsLoaded()
	{
		return m_IsLoaded;
	}

	std::shared_ptr<graphics::CTexture> CBlurEffect::GetFrameTexture()
	{
		std::shared_ptr<graphics::CTexture> Tex = nullptr;

		const auto& RenderPass = m_pGraphicsAPI->GetOffScreenRenderPassMap().find("BlurY");
		if (RenderPass != m_pGraphicsAPI->GetOffScreenRenderPassMap().end()) Tex = RenderPass->second->GetFrameTexture();

		return Tex;
	}

	bool CBlurEffect::Create(resource::CLoadWorker* pLoadWorker)
	{
		if (!CalcGaussianKernel()) return false;

		pLoadWorker->AddFirstLoadResource(m_BlurVertex);
		pLoadWorker->AddFirstLoadResource(m_BlurFrag);

		if (!m_pGraphicsAPI->CreateRenderPass("BlurX", api::ERenderPassFormat::COLOR_RENDERPASS, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 512, 512)) return false;
		if (!m_pGraphicsAPI->CreateRenderPass("BlurY", api::ERenderPassFormat::COLOR_RENDERPASS, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 512, 512)) return false;

		return true;
	}

	bool CBlurEffect::Update(resource::CLoadWorker* pLoadWorker)
	{
		if (!m_IsLoaded)
		{
			if (!pLoadWorker->IsLoaded()) return true;

			if (!Load()) return false;

			m_IsLoaded = true;
		}

		return true;
	}

	bool CBlurEffect::Draw(const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		if (!m_TextureSet) return true;

		const auto& Tex = m_TextureSet->GetFrameTextureList()[0];
		if (!Tex) return true;

		float w = static_cast<float>(Tex->GetWidth());
		float h = static_cast<float>(Tex->GetHeight());

		{
			if (!m_pGraphicsAPI->BeginRender("BlurX")) return false;

			glm::vec2 OffsetV = glm::vec2(1.0f / w, 0.0f);
			m_ScreenObjX->GetMaterialList()[0]->SetUniformValue("Direction", &OffsetV[0]);

			if (!m_ScreenObjX->Draw(false, Camera, Projection, DrawInfo)) return false;
			if (!m_pGraphicsAPI->EndRender()) return false;
		}
		
		{
			if (!m_pGraphicsAPI->BeginRender("BlurY")) return false;

			glm::vec2 OffsetV = glm::vec2(0.0f, 1.0f / h);
			m_ScreenObjY->GetMaterialList()[0]->SetUniformValue("Direction", &OffsetV[0]);

			if (!m_ScreenObjY->Draw(false, Camera, Projection, DrawInfo)) return false;
			if (!m_pGraphicsAPI->EndRender()) return false;
		}

		return true;
	}

	bool CBlurEffect::CalcGaussianKernel()
	{
		m_GaussianKernel.clear();

		float Sum = 0.0f;
		float sigma = 3.0f;
		float pi = 3.1415f;
		float support = 0.995f;

		std::vector<float> Kernel;

		// radiusはこのように求めると結構適切な値が得られる
		// http://demofox.org/gauss.html
		float radius = glm::ceil(glm::sqrt(-2.0f * sigma * sigma * glm::log(1.0f - support)));

		// ガウスカーネルを計算
		// https://stackoverflow.com/questions/74230190/how-to-calculate-normal-distribution-kernal-for-1d-gaussian-filter
		// https://github.com/lchop/Gaussian_filter_1D_cpp/blob/master/gaussian_filter_1D.cpp#L22
		for (float x = -radius; x <= radius; x++)
		{
			float v = glm::exp(-(x * x) / (2.0f * sigma * sigma)) / (sigma * glm::sqrt(2.0f * pi));
			Sum += v;

			Kernel.push_back(v);
		}

		//
		m_KernelSize = static_cast<int>(Kernel.size());

		// 正規化
		for (auto& v : Kernel)
		{
			v /= Sum;
		}
		
		// 空いた分を0詰めする
		Kernel.resize(32, 0.0f);

		//Console::Log("[CalcGaussianKernel] radius: %f ___________________________________\n", radius);
		//for (int i = 0; i < Kernel.size(); i++){ Console::Log("[%d] %f\n", i, Kernel[i]); }
		//Console::Log("___________________________________________________________________\n");

		// floatのuniform array(float test[32] のような値)は、なぜか一つの要素辺り、16バイトでオフセットされてしまうのでパディングを入れる
		for (const float v : Kernel)
		{
			m_GaussianKernel.push_back(v);
			m_GaussianKernel.push_back(0.0f);
			m_GaussianKernel.push_back(0.0f);
			m_GaussianKernel.push_back(0.0f);
		}

		return true;
	}

	bool CBlurEffect::Load()
	{
		// Material
		std::shared_ptr<graphics::CMaterialCreateInfo> createInfo = std::make_shared<graphics::CMaterialCreateInfo>();
		createInfo->SetVertexShaderCode(m_BlurVertex->GetData());
		createInfo->SetFragmentShaderCode(m_BlurFrag->GetData());

		auto MaterialX = m_pGraphicsAPI->CreateMaterial(createInfo);
		MaterialX->SetEnabledZTest(false);
		MaterialX->SetCullMode(graphics::ECullMode::CULL_NONE);
		
		auto MaterialY = m_pGraphicsAPI->CreateMaterial(createInfo);
		MaterialY->SetEnabledZTest(false);
		MaterialY->SetCullMode(graphics::ECullMode::CULL_NONE);

		// UBO0
		{
			auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0, false) });
			
			UniformBuffer->AddData("UseBlur", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
			UniformBuffer->AddData("KernelSize", &glm::ivec1(m_KernelSize)[0], sizeof(glm::ivec1), 0);
			UniformBuffer->AddData("Direction", &glm::vec2(0.0f)[0], sizeof(glm::vec2), 0);

			MaterialX->AddShaderBuffer(UniformBuffer);
		}
		
		{
			auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ graphics::SBindingLayout("UniformBufferObject", 0, false) });

			UniformBuffer->AddData("UseBlur", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
			UniformBuffer->AddData("KernelSize", &glm::ivec1(m_KernelSize)[0], sizeof(glm::ivec1), 0);
			UniformBuffer->AddData("Direction", &glm::vec2(0.0f)[0], sizeof(glm::vec2), 0);

			MaterialY->AddShaderBuffer(UniformBuffer);
		}

		// Bind Texture
		{
			const auto& RenderPass = m_pGraphicsAPI->GetOffScreenRenderPassMap().find("ShadowPass");
			if (RenderPass != m_pGraphicsAPI->GetOffScreenRenderPassMap().end()) m_TextureSet->AddFrameTexture(RenderPass->second->GetFrameTexture());
			MaterialX->AddTextureBindingLayout({ "SrcTex", 2, 3, 0, graphics::ETextureUsage::TEXTURE_USAGE_FRAME});

			m_ScreenObjX->AddMaterial(MaterialX);
		}

		{
			const auto& RenderPass = m_pGraphicsAPI->GetOffScreenRenderPassMap().find("BlurX");
			if (RenderPass != m_pGraphicsAPI->GetOffScreenRenderPassMap().end()) m_TextureSet->AddFrameTexture(RenderPass->second->GetFrameTexture());
			MaterialY->AddTextureBindingLayout({ "SrcTex", 2, 3, 1, graphics::ETextureUsage::TEXTURE_USAGE_FRAME });

			m_ScreenObjY->AddMaterial(MaterialY);
		}

		// Mesh
		{
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			std::shared_ptr<renderer::CRendererCreateInfo> rendererCreateInfo = graphics::CPresetPrimitive::CreateBoard();

			std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(rendererCreateInfo, 0);
			Mesh->AddPrimitive(Primitive);
			m_ScreenObjX->AddMesh(Mesh);
		}

		{
			std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();

			std::shared_ptr<renderer::CRendererCreateInfo> rendererCreateInfo = graphics::CPresetPrimitive::CreateBoard();

			std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(rendererCreateInfo, 0);
			Mesh->AddPrimitive(Primitive);

			m_ScreenObjY->AddMesh(Mesh);
		}

		// Node
		{
			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, m_ScreenObjX->GetMeshList(), m_ScreenObjX->GetMaterialList());
			m_ScreenObjX->AddNode(Node);
		}

		{
			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, m_ScreenObjY->GetMeshList(), m_ScreenObjY->GetMaterialList());
			m_ScreenObjY->AddNode(Node);
		}

		// Create
		if (!m_ScreenObjX->Create(m_pGraphicsAPI, nullptr, m_TextureSet)) return false;
		if (!m_ScreenObjY->Create(m_pGraphicsAPI, nullptr, m_TextureSet)) return false;

		return true;
	}
}