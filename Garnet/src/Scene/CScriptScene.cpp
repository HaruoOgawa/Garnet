#include "CScriptScene.h"
#include "../Graphics/CMesh.h"
#include "../Graphics/CPrimitive.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"
#include "../File/CFile.h"
#include "../Graphics/SVertex.h"

namespace scene
{
	CScriptScene::CScriptScene():
		m_TestMesh(nullptr)
	{
	}

	bool CScriptScene::Release(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (m_TestMesh)
		{
			m_TestMesh->Release();
			m_TestMesh.reset();
			m_TestMesh = nullptr;
		}

		return true;
	}

	bool CScriptScene::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		//
		std::string ShaderPath = "Resources\\Shaders\\";

		// Vertex Buffer
		std::vector<renderer::SVertex> Vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f},  {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f},   {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f},	 {1.0f, 1.0f}},
		};

		std::vector<uint16_t> Indices = {
			0, 1, 2, 2, 3, 0,
		};

#ifdef __DAWN__
		//
		std::string VertexShaderCode = R"(
			@vertex
			fn main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4<f32> {
				var p = vec2<f32>(0.0, 0.0);
				if (in_vertex_index == 0u) {
					p = vec2<f32>(-0.5, -0.5);
				} else if (in_vertex_index == 1u) {
					p = vec2<f32>(0.5, -0.5);
				} else {
					p = vec2<f32>(0.0, 0.5);
				}
				return vec4<f32>(p, 0.0, 1.0);
			}
		)";
		
		std::string FragmentShaderCode = R"(
			@fragment
			fn main() -> @location(0) vec4<f32> {
				return vec4<f32>(0.0, 0.4, 1.0, 1.0);
			}
		)";
#endif
		
		//
		renderer::CRendererCreateInfo createInfo;
#ifdef __DAWN__
		createInfo.SetVertexShaderCode(std::string(VertexShaderCode.data(), VertexShaderCode.data() + VertexShaderCode.size()));
		createInfo.SetFragmentShaderCode(std::string(FragmentShaderCode.data(), FragmentShaderCode.data() + FragmentShaderCode.size()));
#else
		createInfo.SetVertexShaderCode(file::CFile::ReadFileAsString(ShaderPath + "vert.spv"));
		createInfo.SetFragmentShaderCode(file::CFile::ReadFileAsString(ShaderPath + "frag.spv"));
#endif
		
		createInfo.SetVertices(Vertices);
		createInfo.SetIndices(Indices);

		//
		m_TestMesh = std::make_shared<graphics::CMesh>();
		std::shared_ptr<graphics::CPrimitive> Primitive = std::make_shared<graphics::CPrimitive>();
		if (!Primitive->Create(pGraphicsAPI, createInfo)) return false;

		//
		m_TestMesh->AddPrimitive(Primitive);

		return true;
	}
	bool CScriptScene::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_TestMesh->Update()) return false;

		return true;
	}

	bool CScriptScene::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!m_TestMesh->Draw()) return false;

		return true;
	}
}