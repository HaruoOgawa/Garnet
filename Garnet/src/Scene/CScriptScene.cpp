#include "CScriptScene.h"
#include "../Graphics/CMesh.h"
#include "../Graphics/CPrimitive.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"
#include "../File/CFile.h"

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
		std::vector<float> Pos = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f
		};

		std::vector<float> Color = {
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f
		};

		std::vector<float> UV = {
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};

		std::vector<std::vector<float>> Vertices = {
			Pos, Color, UV
		};

		std::vector<uint16_t> Indices = {
			0, 1, 2, 2, 3, 0,
		};

#ifdef __DAWN__
		//
		std::string VertexShaderCode = R"(
			struct VertexInput {
				@location(0) position: vec3<f32>,
				@location(1) color: vec3<f32>,
				@location(2) uv: vec2<f32>,
			};	

			struct VertexOutput {
				@builtin(position) position: vec4<f32>,
				@location(0) color: vec3<f32>,
				@location(1) uv: vec2<f32>,
			};	
	
			@group(0) @binding(0) var<uniform> uTime: f32;

			@vertex
			fn main(in: VertexInput) -> VertexOutput {
				var offset = vec3<f32>(0.0, sin(uTime), 0.0);

				var out: VertexOutput;
				out.position = vec4<f32>(in.position + offset, 1.0);			
				out.color = in.color;			
				out.uv = in.uv;			

				return out;
			}
		)";
		
		std::string FragmentShaderCode = R"(
			struct VertexOutput {
				@builtin(position) position: vec4<f32>,
				@location(0) color: vec3<f32>,
				@location(1) uv: vec2<f32>,
			};	

			@fragment
			fn main(in: VertexOutput) -> @location(0) vec4<f32> {
				return vec4<f32>(in.color, 1.0);
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
		createInfo.SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2 }));

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