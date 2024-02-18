#include "CPresetPrimitive.h"
#include "../GraphicsAPI/CRendererCreateInfo.h"
#include <glm/glm.hpp>

namespace graphics
{
	std::shared_ptr<renderer::CRendererCreateInfo> CPresetPrimitive::CreateBoard()
	{
		std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();

		// Vertex Buffer
		std::vector<float> Pos = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f
		};

		std::vector<float> Normal = {
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
		};

		std::vector<float> UV = {
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};

		std::vector<float> Tangent(16, 0.0f);
		std::vector<float> Joints(16, 0.0f);
		std::vector<float> Weights(16, 0.0f);

		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, Joints, Weights
		};

		// Indices
		std::vector<unsigned short> Indices = {
			0, 1, 2, 2, 3, 0,
		};

		//
		createInfo->SetVertices(Vertices);
		createInfo->SetIndices(Indices);
		createInfo->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo->SetAttribDataTypes(std::vector<renderer::EDataType>({ renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT, renderer::EDataType::TYPE_UNSIGNED_INT, renderer::EDataType::TYPE_FLOAT }));
		createInfo->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}

	std::shared_ptr<renderer::CRendererCreateInfo> CPresetPrimitive::CreateBox()
	{
		std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();

		// Vertex Buffer
		std::vector<float> Pos = {
			//0 +Z
			-0.5f,-0.5f,0.5f, // 0
			0.5f,0.5f,0.5f,   // 1
			-0.5f,0.5f,0.5f,  // 2
			0.5f,-0.5f,0.5f,  // 3
			//1 -Z
			0.5f,-0.5f,-0.5f, // 4 // ó†(-) Ç…çsÇ≠Ç∆Ç´ÇÕç∂âEîΩì]
			-0.5f,0.5f,-0.5f, // 5
			0.5f,0.5f,-0.5f,  // 6
			-0.5f,-0.5f,-0.5f,// 7
			//2 -X
			-0.5f,0.5f,-0.5f,
			-0.5f,-0.5f,0.5f,
			-0.5f,0.5f,0.5f,
			-0.5f,-0.5f,-0.5f,
			//3 +X
			0.5f,-0.5f,-0.5f,
			0.5f,0.5f,0.5f,
			0.5f,-0.5f,0.5f,
			0.5f,0.5f,-0.5f,
			//4  -Y
			-0.5f,-0.5f,-0.5f,
			0.5f,-0.5f,0.5f,
			-0.5f,-0.5f,0.5f,
			0.5f,-0.5f,-0.5f,
			//5 +Y
			0.5f,0.5f,-0.5f,
			-0.5f,0.5f,0.5f,
			0.5f,0.5f,0.5f,
			-0.5f,0.5f,-0.5f,
		};

		std::vector<float> Normal = {
			//0 +Z
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,
			0.0f,0.0f,1.0f,
			//1 -Z
			0.0f,0.0f,-1.0f,
			0.0f,0.0f,-1.0f,
			0.0f,0.0f,-1.0f,
			0.0f,0.0f,-1.0f,
			//2 -X
			-1.0f,0.0f,0.0f,
			-1.0f,0.0f,0.0f,
			-1.0f,0.0f,0.0f,
			-1.0f,0.0f,0.0f,
			//3 +X
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,
			1.0f,0.0f,0.0f,
			//4 -Y
			0.0f,-1.0f,0.0f,
			0.0f,-1.0f,0.0f,
			0.0f,-1.0f,0.0f,
			0.0f,-1.0f,0.0f,
			//5 +Y
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,
			0.0f,1.0f,0.0f,
		};

		std::vector<float> UV = {
			//0 +Z
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
			//1 -Z
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
			//2 -X
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
			//3 +X
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
			//4 -Y
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
			//5 +Y
			0.0f,0.0f,
			1.0f,1.0f,
			0.0f,1.0f,
			1.0f,0.0f,
		};

		std::vector<float> Tangent(96, 0.0f);
		std::vector<float> Joints(96, 0.0f);
		std::vector<float> Weights(96, 0.0f);

		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, Joints, Weights
		};

		// Indices
		std::vector<unsigned short> Indices = {
			//0 +Z
			0,1,2,
			0,3,1,
			//1 -Z
			4,5,6,
			4,7,5,
			//2 -X
			8,9,10,
			8,11,9,
			//3 +X
			12,13,14,
			12,15,13,
			//4 -Y
			16,17,18,
			16,19,17,
			//5 +Y
			20,21,22,
			20,23,21,
		};

		//
		createInfo->SetVertices(Vertices);
		createInfo->SetIndices(Indices);
		createInfo->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo->SetAttribDataTypes(std::vector<renderer::EDataType>({ renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT, renderer::EDataType::TYPE_UNSIGNED_INT, renderer::EDataType::TYPE_FLOAT }));
		createInfo->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}

	std::shared_ptr<renderer::CRendererCreateInfo> CPresetPrimitive::CreatePoint()
	{
		std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();

		// Vertex Buffer
		std::vector<float> Pos = {
			0.0f, 0.0f, 0.0f,
		};

		std::vector<float> Normal = {
			0.0f, 0.0f, 0.0f,
		};

		std::vector<float> UV = {
			0.0f, 0.0f,
		};

		std::vector<float> Tangent(4, 0.0f);
		std::vector<float> Joints(4, 0.0f);
		std::vector<float> Weights(4, 0.0f);

		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, Joints, Weights
		};

		// Indices
		std::vector<unsigned short> Indices = {
			0
		};

		//
		createInfo->SetVertices(Vertices);
		createInfo->SetIndices(Indices);
		createInfo->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo->SetAttribDataTypes(std::vector<renderer::EDataType>({ renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT, renderer::EDataType::TYPE_UNSIGNED_INT, renderer::EDataType::TYPE_FLOAT }));
		createInfo->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}
	
	std::shared_ptr<renderer::CRendererCreateInfo> CPresetPrimitive::CreateSphere()
	{
		std::shared_ptr<renderer::CRendererCreateInfo> createInfo = std::make_shared<renderer::CRendererCreateInfo>();

		// Vertex Buffer
		std::vector<float> Pos;
		std::vector<float> Normal;
		std::vector<float> UV;
		std::vector<float> Tangent;
		std::vector<float> Joints;
		std::vector<float> Weights;

		// Indices
		std::vector<unsigned short> Indices;

		//
		float pi = 3.14159265f, row = 16.0f, column = 16.0f, rad = 1.0f;

		for (auto i = 0; i <= row; i++) {
			auto r = pi / row * i;
			auto ry = glm::cos(r);
			auto rr = glm::sin(r);
			for (auto ii = 0; ii <= column; ii++) {
				float tr = pi * 2 / column * ii;
				float tx = rr * rad * glm::cos(tr);
				float ty = ry * rad;
				float tz = rr * rad * glm::sin(tr);
				float rx = rr * glm::cos(tr);
				float rz = rr * glm::sin(tr);

				Pos.push_back(tx); Pos.push_back(ty); Pos.push_back(tz);
				Normal.push_back(rx); Normal.push_back(ry); Normal.push_back(rz);
				UV.push_back(static_cast<float>(1 - 1 / column * ii)); UV.push_back(static_cast<float>(1 / row * i));
			}
		}

		for (int i = 0; i < row; i++) {
			for (int ii = 0; ii < static_cast<int>(column); ii++) {
				int r = (static_cast<int>(column) + 1) * i + ii;

				Indices.push_back(static_cast<int>(r));
				Indices.push_back(static_cast<int>(r + 1));
				Indices.push_back(static_cast<int>(r + static_cast<int>(column) + 2));
				Indices.push_back(static_cast<int>(r));
				Indices.push_back(static_cast<int>(r + static_cast<int>(column) + 2));
				Indices.push_back(static_cast<int>(r + static_cast<int>(column) + 1));
			}
		}

		Tangent.resize(Pos.size() / 3 * 4, 0.0f);
		Joints.resize(Pos.size() / 3 * 4, 0.0f);
		Weights.resize(Pos.size() / 3 * 4, 0.0f);

		//
		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, Joints, Weights
		};

		createInfo->SetVertices(Vertices);
		createInfo->SetIndices(Indices);
		createInfo->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4}));
		createInfo->SetAttribDataTypes(std::vector<renderer::EDataType>({ renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT , renderer::EDataType::TYPE_FLOAT, renderer::EDataType::TYPE_UNSIGNED_INT, renderer::EDataType::TYPE_FLOAT }));
		createInfo->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}
}