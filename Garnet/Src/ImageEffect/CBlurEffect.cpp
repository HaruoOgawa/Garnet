#include "CBlurEffect.h"
#include "../Debug/Message/Console.h"
#include "../File/CFileReader.h"
#include "../Interface/IGraphicsAPI.h"

namespace imageeffect
{
	CBlurEffect::CBlurEffect(api::IGraphicsAPI* pGraphicsAPI):
		m_pGraphicsAPI(pGraphicsAPI),

		m_IsLoaded(false),
		m_KernelSize(0),
		m_BlurVertex(std::make_shared<file::CFileReader>()),
		m_BlurFrag(std::make_shared<file::CFileReader>()),
		m_ScreenObjX(std::make_shared<object::C3DObject>("BlurX", "")),
		m_ScreenObjY(std::make_shared<object::C3DObject>("BlurY", ""))
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

	bool CBlurEffect::Create()
	{
		if (!CalcGaussianKernel()) return false;

		m_BlurVertex->ReadFile("Resources\\Shaders\\blur" + m_pGraphicsAPI->GetVertexShaderExtension());
		m_BlurFrag->ReadFile("Resources\\Shaders\\blur" + m_pGraphicsAPI->GetFragmentShaderExtension());

		if (!m_pGraphicsAPI->CreateRenderPass("BlurX", api::ERenderPassFormat::COLOR_RENDERPASS, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 512, 512)) return false;
		if (!m_pGraphicsAPI->CreateRenderPass("BlurY", api::ERenderPassFormat::COLOR_RENDERPASS, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 512, 512)) return false;

		return true;
	}

	bool CBlurEffect::Update()
	{
		if (!m_IsLoaded)
		{
			if (!m_BlurVertex->IsLoaded()) return true;
			if (!m_BlurFrag->IsLoaded()) return true;

			if (!Load()) return false;

			m_IsLoaded = true;
		}

		return true;
	}

	bool CBlurEffect::Draw(float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		if (!m_IsLoaded) return true;

		const auto& Tex = m_ScreenObjX->GetTextureList()[0];
		if (!Tex) return true;

		float w = static_cast<float>(Tex->GetWidth());
		float h = static_cast<float>(Tex->GetHeight());

		/*{
			m_ScreenObjX->GetMaterialList()[0]->SetUniformValue("UseBlur", (GetKeyState(VK_SPACE)? &glm::ivec1(0)[0] : &glm::ivec1(1)[0]));
			m_ScreenObjY->GetMaterialList()[0]->SetUniformValue("UseBlur", (GetKeyState(VK_SPACE)? &glm::ivec1(0)[0] : &glm::ivec1(1)[0]));
		}*/

		{
			if (!m_pGraphicsAPI->BeginRender("BlurX")) return false;

			glm::vec2 OffsetV = glm::vec2(1.0f / w, 0.0f);
			m_ScreenObjX->GetMaterialList()[0]->SetUniformValue("Direction", &OffsetV[0]);

			if (!m_ScreenObjX->Draw(false, SecondsTime, Camera, Projection, DrawInfo)) return false;
			if (!m_pGraphicsAPI->EndRender()) return false;
		}
		
		{
			if (!m_pGraphicsAPI->BeginRender("BlurY")) return false;

			glm::vec2 OffsetV = glm::vec2(0.0f, 1.0f / h);
			m_ScreenObjY->GetMaterialList()[0]->SetUniformValue("Direction", &OffsetV[0]);

			if (!m_ScreenObjY->Draw(false, SecondsTime, Camera, Projection, DrawInfo)) return false;
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

		auto MaterialX = m_pGraphicsAPI->CreateMaterial();
		MaterialX->SetCreateInfo(createInfo);
		MaterialX->SetEnabledZTest(false);
		MaterialX->SetCullMode(graphics::ECullMode::CULL_NONE);
		
		auto MaterialY = m_pGraphicsAPI->CreateMaterial();
		MaterialY->SetCreateInfo(createInfo);
		MaterialY->SetEnabledZTest(false);
		MaterialY->SetCullMode(graphics::ECullMode::CULL_NONE);

		// UBO0
		{
			auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ 0 });
			
			UniformBuffer->AddData("UseBlur", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
			UniformBuffer->AddData("KernelSize", &glm::ivec1(m_KernelSize)[0], sizeof(glm::ivec1), 0);
			UniformBuffer->AddData("Direction", &glm::vec2(0.0f)[0], sizeof(glm::vec2), 0);

			UniformBuffer->RecalculateBindingLayoutOffset();

			MaterialX->AddUniformBuffer(UniformBuffer);
		}
		
		/*{
			auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ 1 });
			UniformBuffer->AddData("kernel", &m_GaussianKernel[0], sizeof(float) * static_cast<int>(m_GaussianKernel.size()), 1);

			UniformBuffer->RecalculateBindingLayoutOffset();

			MaterialX->AddUniformBuffer(UniformBuffer);
		}*/
		
		{
			auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ 0 });

			UniformBuffer->AddData("UseBlur", &glm::ivec1(1)[0], sizeof(glm::ivec1), 0);
			UniformBuffer->AddData("KernelSize", &glm::ivec1(m_KernelSize)[0], sizeof(glm::ivec1), 0);
			UniformBuffer->AddData("Direction", &glm::vec2(0.0f)[0], sizeof(glm::vec2), 0);

			UniformBuffer->RecalculateBindingLayoutOffset();

			MaterialY->AddUniformBuffer(UniformBuffer);
		}

		/*{
			auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ 1 });
			UniformBuffer->AddData("kernel", &m_GaussianKernel[0], sizeof(float) * static_cast<int>(m_GaussianKernel.size()), 1);

			UniformBuffer->RecalculateBindingLayoutOffset();

			MaterialY->AddUniformBuffer(UniformBuffer);
		}*/
		
		// UBO1
		/*{
			auto UniformBuffer = graphics::CMaterialCreateInfo::CreateUniformBuffer({ 1 });
			UniformBuffer->AddData("kernel", &m_GaussianKernel[0], sizeof(float) * static_cast<int>(m_GaussianKernel.size()), 1);

			UniformBuffer->RecalculateBindingLayoutOffset();

			Material->AddUniformBuffer(UniformBuffer);
		}*/

		// Bind Texture
		{
			const auto& RenderPass = m_pGraphicsAPI->GetOffScreenRenderPassMap().find("ShadowPass");
			if (RenderPass != m_pGraphicsAPI->GetOffScreenRenderPassMap().end()) m_ScreenObjX->AddTexture(RenderPass->second->GetFrameTexture());
			MaterialX->AddTextureBindingLayout({ 2, 3, 0, graphics::ETextureType::TEXTURE_2D });

			m_ScreenObjX->AddMaterial(MaterialX);
		}

		{
			const auto& RenderPass = m_pGraphicsAPI->GetOffScreenRenderPassMap().find("BlurX");
			if (RenderPass != m_pGraphicsAPI->GetOffScreenRenderPassMap().end()) m_ScreenObjY->AddTexture(RenderPass->second->GetFrameTexture());
			MaterialY->AddTextureBindingLayout({ 2, 3, 0, graphics::ETextureType::TEXTURE_2D });

			m_ScreenObjY->AddMaterial(MaterialY);
		}

		// Mesh
		std::shared_ptr<graphics::CMesh> Mesh = std::make_shared<graphics::CMesh>();
		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>(nullptr, 0, graphics::EPresetPrimitiveType::BOARD);
		Mesh->AddPrimitive(Primitive);
		m_ScreenObjX->AddMesh(Mesh);
		m_ScreenObjY->AddMesh(Mesh);

		// Node
		{
			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, m_ScreenObjX->GetMeshList(), m_ScreenObjX->GetMaterialList());
			Node->SetMeshIndex(0);
			m_ScreenObjX->AddNode(Node);
		}

		{
			std::shared_ptr<object::CNode> Node = std::make_shared<object::CNode>(0, m_ScreenObjY->GetMeshList(), m_ScreenObjY->GetMaterialList());
			Node->SetMeshIndex(0);
			m_ScreenObjY->AddNode(Node);
		}

		// Create
		if (!m_ScreenObjX->Create(m_pGraphicsAPI, nullptr, nullptr)) return false;
		if (!m_ScreenObjY->Create(m_pGraphicsAPI, nullptr, nullptr)) return false;

		return true;
	}
}