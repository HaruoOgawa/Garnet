#include "CPresetPrimitive.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include <glm/glm.hpp>
#include "../Interface/IGraphicsAPI.h"

namespace graphics
{
	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreateFromType(api::IGraphicsAPI* pGraphicsAPI, graphics::EPresetPrimitiveType PresetType)
	{
		switch (PresetType)
		{
		case graphics::EPresetPrimitiveType::None:
			return std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());
		case graphics::EPresetPrimitiveType::BOARD:
			return CreateBoard(pGraphicsAPI);
		case graphics::EPresetPrimitiveType::CUBE:
			return CreateBox(pGraphicsAPI);
		case graphics::EPresetPrimitiveType::POINT:
			return CreatePoint(pGraphicsAPI);
		case graphics::EPresetPrimitiveType::SPHERE:
			return CreateSphere(pGraphicsAPI);
		case graphics::EPresetPrimitiveType::Max:
		default:
			return std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());
		}
	}

	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreateBoard(api::IGraphicsAPI* pGraphicsAPI)
	{
		std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo = std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());

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
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
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
		createInfo.first->SetVertices(Vertices);
		createInfo.second->SetIndices(Indices);
		createInfo.first->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo.first->SetAttribDataTypes(std::vector<graphics::EDataType>({ graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT, graphics::EDataType::TYPE_UNSIGNED_INT, graphics::EDataType::TYPE_FLOAT }));
		createInfo.first->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}

	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreateBox(api::IGraphicsAPI* pGraphicsAPI)
	{
		std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo = std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());

		// Vertex Buffer
		std::vector<float> Pos = {
			//0 +Z
			-0.5f,-0.5f,0.5f, // 0
			0.5f,0.5f,0.5f,   // 1
			-0.5f,0.5f,0.5f,  // 2
			0.5f,-0.5f,0.5f,  // 3
			//1 -Z
			0.5f,-0.5f,-0.5f, // 4 // 裏(-) に行くときは左右反転
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
		createInfo.first->SetVertices(Vertices);
		createInfo.second->SetIndices(Indices);
		createInfo.first->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo.first->SetAttribDataTypes(std::vector<graphics::EDataType>({ graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT, graphics::EDataType::TYPE_UNSIGNED_INT, graphics::EDataType::TYPE_FLOAT }));
		createInfo.first->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}

	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreatePoint(api::IGraphicsAPI* pGraphicsAPI)
	{
		std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo = std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());

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
		createInfo.first->SetVertices(Vertices);
		createInfo.second->SetIndices(Indices);
		createInfo.first->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo.first->SetAttribDataTypes(std::vector<graphics::EDataType>({ graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT, graphics::EDataType::TYPE_UNSIGNED_INT, graphics::EDataType::TYPE_FLOAT }));
		createInfo.first->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}
	
	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreateSphere(api::IGraphicsAPI* pGraphicsAPI)
	{
		std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo = std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());

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
		float pi = 3.14159265f, row = 32.0f, column = 32.0f, rad = 1.0f;

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

		createInfo.first->SetVertices(Vertices);
		createInfo.second->SetIndices(Indices);
		createInfo.first->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo.first->SetAttribDataTypes(std::vector<graphics::EDataType>({ graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT, graphics::EDataType::TYPE_UNSIGNED_INT, graphics::EDataType::TYPE_FLOAT }));
		createInfo.first->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}

	std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> CPresetPrimitive::CreateCylinder(api::IGraphicsAPI* pGraphicsAPI)
	{
		// 半径1.0f, 高さ1.0f, 分割数32の円柱を生成
		std::pair<std::shared_ptr<graphics::CVertexBuffer>, std::shared_ptr<graphics::CIndexBuffer>> createInfo = std::make_pair(pGraphicsAPI->CreateVertexBuffer(), pGraphicsAPI->CreateIndexBuffer());

		// Vertex Buffer
		std::vector<float> Pos;
		std::vector<float> Normal;
		std::vector<float> UV;
		std::vector<float> Tangent;
		std::vector<float> Joints;
		std::vector<float> Weights;
		std::vector<unsigned short> Indices;
		float radius = 1.0f;
		float height = 1.0f;
		int segments = 32;
		float angleStep = 2.0f * 3.14159265f / segments;

		// 下端を中心に合わせる
		float offset = height * 0.5f;

		// 蓋をしめるかどうか
		bool FillCap = true;

		// 円柱の上面と下面の頂点を生成
		for (int i = 0; i < segments; ++i) {
			float angle = i * angleStep;
			float x = radius * cos(angle);
			float z = radius * sin(angle);
			// 上面の頂点
			Pos.push_back(x); Pos.push_back(height / 2.0f + offset); Pos.push_back(z);
			Normal.push_back(0.0f); Normal.push_back(1.0f); Normal.push_back(0.0f);
			UV.push_back(static_cast<float>(i) / segments); UV.push_back(1.0f);
			
			// 下面の頂点
			Pos.push_back(x); Pos.push_back(-height / 2.0f + offset); Pos.push_back(z);
			Normal.push_back(0.0f); Normal.push_back(-1.0f); Normal.push_back(0.0f);
			UV.push_back(static_cast<float>(i) / segments); UV.push_back(0.0f);
		}
		// 円柱の側面の頂点を生成
		for (int i = 0; i < segments; ++i) {
			float angle = i * angleStep;
			float x = radius * cos(angle);
			float z = radius * sin(angle);
			// 側面の頂点
			Pos.push_back(x); Pos.push_back(height / 2.0f + offset); Pos.push_back(z);
			Normal.push_back(cos(angle)); Normal.push_back(0.0f); Normal.push_back(sin(angle));
			UV.push_back(static_cast<float>(i) / segments); UV.push_back(1.0f);
			Pos.push_back(x); Pos.push_back(-height / 2.0f + offset); Pos.push_back(z);
			Normal.push_back(cos(angle)); Normal.push_back(0.0f); Normal.push_back(sin(angle));
			UV.push_back(static_cast<float>(i) / segments); UV.push_back(0.0f);
		}

		// 蓋をしめるように上下されぞれの円の中心を追加
		if (FillCap)
		{
			// 上面の中心頂点
			Pos.push_back(0.0f); Pos.push_back(height / 2.0f + offset); Pos.push_back(0.0);
			Normal.push_back(0.0f); Normal.push_back(1.0f); Normal.push_back(0.0f);
			UV.push_back(0.0f); UV.push_back(1.0f);

			// 下面の中心頂点
			Pos.push_back(0.0f); Pos.push_back(-height / 2.0f + offset); Pos.push_back(0.0f);
			Normal.push_back(0.0f); Normal.push_back(-1.0f); Normal.push_back(0.0f);
			UV.push_back(0.0f); UV.push_back(0.0f);
		}

		// 頂点数
		const int NumOfVertex = static_cast<int>(Pos.size()) / 3;

		// インデックスバッファの生成
		for (int i = 0; i < segments; ++i) {
			// 側面のインデックスを生成
			int topIndex = i * 2;
			int bottomIndex = topIndex + 1;
			int nextTopIndex = ((i + 1) % segments) * 2;
			int nextBottomIndex = nextTopIndex + 1;

			// 側面の三角形
			Indices.push_back(topIndex);
			Indices.push_back(nextTopIndex);
			Indices.push_back(bottomIndex);

			Indices.push_back(bottomIndex);
			Indices.push_back(nextTopIndex);
			Indices.push_back(nextBottomIndex);
			
			// 蓋
			if (FillCap)
			{
				// 上下中心のインデックス
				int topCenterIndex = NumOfVertex - 2;
				int bottomCenterIndex = NumOfVertex - 1;

				// 上面の三角形
				Indices.push_back(topCenterIndex);
				Indices.push_back(nextTopIndex);
				Indices.push_back(topIndex);

				// 下面の三角形
				Indices.push_back(bottomCenterIndex);
				Indices.push_back(nextBottomIndex);
				Indices.push_back(bottomIndex);
			}
		}

		// Tangent, Joints, Weightsの初期化
		Tangent.resize(Pos.size() / 3 * 4, 0.0f);
		Joints.resize(Pos.size() / 3 * 4, 0.0f);
		Weights.resize(Pos.size() / 3 * 4, 0.0f);

		std::vector<std::vector<float>> Vertices = {
			Pos, Normal, UV, Tangent, Joints, Weights
		};

		createInfo.first->SetVertices(Vertices);
		createInfo.second->SetIndices(Indices);
		createInfo.first->SetAttributeDimensions(std::vector<int>({ 3 , 3 , 2, 4, 4, 4 }));
		createInfo.first->SetAttribDataTypes(std::vector<graphics::EDataType>({ graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT , graphics::EDataType::TYPE_FLOAT, graphics::EDataType::TYPE_UNSIGNED_INT, graphics::EDataType::TYPE_FLOAT }));
		createInfo.first->SetAttribByteStrides(std::vector<int>({ 0, 0, 0, 0, 0, 0 }));

		return createInfo;
	}
}